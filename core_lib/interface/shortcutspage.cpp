#include <QDebug>
#include <QMap>
#include <QStringRef>
#include <QSettings>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QTreeView>
#include <QStandardItemModel>
#include <QKeyEvent>
#include <QKeySequence>
#include <QMessageBox>
#include "pencilsettings.h"
#include "shortcutspage.h"
#include "keycapturelineedit.h"
#include "ui_shortcutspage.h"

static const int ACT_NAME_COLUMN = 0;
static const int KEY_SEQ_COLUMN  = 1;

ShortcutsPage::ShortcutsPage(QWidget *parent) :
    QWidget(parent),
    m_treeModel( NULL ),
    ui( new Ui::ShortcutsPage )
{
    ui->setupUi(this);
    m_treeModel = new QStandardItemModel(this);

    treeModelLoadShortcutsSetting();

    ui->treeView->setModel(m_treeModel);
    ui->treeView->resizeColumnToContents(0);

    connect( ui->treeView, &QTreeView::clicked, this, &ShortcutsPage::tableItemClicked );
    connect( ui->keySeqLineEdit, &KeyCaptureLineEdit::keyCaptured, this, &ShortcutsPage::keyCapLineEditTextChanged );
    connect( ui->restoreShortcutsButton, &QPushButton::clicked, this, &ShortcutsPage::restoreShortcutsButtonClicked );
    connect( ui->clearButton, &QPushButton::clicked, this, &ShortcutsPage::clearButtonClicked );
}

void ShortcutsPage::tableItemClicked( const QModelIndex& modelIndex )
{
    int row = modelIndex.row();

    // extract action name
    QStandardItem* actionItem = m_treeModel->item(row, ACT_NAME_COLUMN);
    ui->actionNameLabel->setText(actionItem->text());

    // extract key sequence
    QStandardItem* keyseqItem = m_treeModel->item(row, KEY_SEQ_COLUMN);
    ui->keySeqLineEdit->setText(keyseqItem->text());

    qDebug() << "Command Selected:" << actionItem->text();

    m_currentItemIndex = modelIndex;

    ui->keySeqLineEdit->setFocus();
}

void ShortcutsPage::keyCapLineEditTextChanged(QKeySequence keySeqence)
{
    if ( !m_currentItemIndex.isValid() )
    {
        return;
    }

    int row = m_currentItemIndex.row();
    QStandardItem* actionItem = m_treeModel->item(row, ACT_NAME_COLUMN);
    QStandardItem* keyseqItem = m_treeModel->item(row, KEY_SEQ_COLUMN);

    QString strCmdName = QString("Cmd%1").arg( actionItem->text() );
    QString strKeySeq  = keySeqence.toString( QKeySequence::PortableText );

    QSettings setting("Pencil", "Pencil");
    setting.beginGroup("shortcuts");

    if (isKeySequenceExist(setting, strCmdName, keySeqence))
    {
        QMessageBox msgBox;
        msgBox.setText("Shortcut Conflict!");
        msgBox.setInformativeText(keySeqence.toString(QKeySequence::NativeText) + " is already used, overwrite?");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::No);
        msgBox.setIcon( QMessageBox::Warning );

        int result = msgBox.exec();

        if ( result != QMessageBox::Yes )
        {
            ui->keySeqLineEdit->setText( keyseqItem->text() );
            return;
        }
        removeDuplicateKeySequence(&setting, keySeqence);
    }

    setting.setValue(strCmdName, strKeySeq);
    setting.endGroup();
    setting.sync();

    treeModelLoadShortcutsSetting();

    qDebug() << "Shortcut " << strCmdName << " = " << strKeySeq;
}

void ShortcutsPage::restoreShortcutsButtonClicked()
{
    restoreShortcutsToDefault();
    treeModelLoadShortcutsSetting();
}

bool ShortcutsPage::isKeySequenceExist(const QSettings& settings, QString strTargetCmdName, QKeySequence targetkeySeq)
{
    foreach (QString strCmdName, settings.allKeys())
    {
        if (strTargetCmdName == strCmdName)
        {
            continue;
        }

        QString strCmdKeySeq = settings.value(strCmdName).toString();
        /*
        qDebug() << "Compare:"
        << QKeySequence(strCmdKeySeq).toString()
        << "|"
        << targetkeySeq.toString();
        */
        if (QKeySequence(strCmdKeySeq) == targetkeySeq)
        {
            return true;
        }
    }
    return false;
}

void ShortcutsPage::removeDuplicateKeySequence(QSettings* settings, QKeySequence keySeq)
{
    foreach(QString strCmdName, settings->allKeys())
    {
        QString strCmdKeySeq = settings->value(strCmdName).toString();

        if ( strCmdKeySeq == keySeq.toString(QKeySequence::PortableText))
        {
            settings->setValue(strCmdName, "");
        }
    }
}

void ShortcutsPage::treeModelLoadShortcutsSetting()
{
    // Load shortcuts from settings
    QSettings settings("Pencil", "Pencil");
    settings.beginGroup("shortcuts");

    m_treeModel->clear(); // release all existing items.

    m_treeModel->setRowCount( settings.allKeys().size() );
    m_treeModel->setColumnCount( 2 );

    int row = 0;
    foreach (QString strCmdName, settings.allKeys())
    {
        QString strKeySequence = settings.value(strCmdName).toString();

        //convert to native format
        strKeySequence = QKeySequence(strKeySequence).toString( QKeySequence::NativeText );

        // strip the first 3 chars "Cmd"
        QStringRef strHumanReadCmdName (&strCmdName, 3, strCmdName.size() - 3);

        QStandardItem* nameItem = new QStandardItem(strHumanReadCmdName.toString());
        QStandardItem* keyseqItem = new QStandardItem(strKeySequence);

        m_treeModel->setItem(row, 0, nameItem);
        m_treeModel->setItem(row, 1, keyseqItem);

        m_treeModel->item(row, 0)->setEditable(false);
        m_treeModel->item(row, 1)->setEditable(false);

        row++;
    }
    settings.endGroup();

    ui->treeView->resizeColumnToContents( 0 );
}

void ShortcutsPage::clearButtonClicked()
{
    if ( !m_currentItemIndex.isValid() )
    {
        return;
    }

    int row = m_currentItemIndex.row();
    QStandardItem* actionItem = m_treeModel->item(row, ACT_NAME_COLUMN);

    QString strCmdName = QString("shortcuts/Cmd%1").arg( actionItem->text() );

    QSettings setting("Pencil", "Pencil");
    setting.setValue( strCmdName, "" );
    setting.sync();

    ui->keySeqLineEdit->setText("");

    treeModelLoadShortcutsSetting();
}
