
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


ShortcutsPage::ShortcutsPage(QWidget *parent) :
    QWidget(parent),
    m_treeModel( NULL ),
    m_currentActionItem( NULL ),
    m_currentKeySeqItem( NULL ),
    ui( new Ui::ShortcutsPage )
{
    ui->setupUi(this);
    m_treeModel = new QStandardItemModel(this);

    loadShortcutsFromSetting();

    ui->treeView->setModel(m_treeModel);
    ui->treeView->resizeColumnToContents(0);

    connect(ui->treeView, SIGNAL(clicked(const QModelIndex&)),
            this, SLOT(tableItemClicked(const QModelIndex&)));

    connect(ui->keySeqLineEdit, SIGNAL(keyCaptured(QKeySequence)),
            this, SLOT(keyCapLineEditTextChanged(QKeySequence)));

    connect(ui->restoreShortcutsButton, SIGNAL(clicked()),
            this, SLOT(pressRestoreShortcutsButton()));
}

void ShortcutsPage::tableItemClicked( const QModelIndex& modelIndex )
{
    const int ACT_NAME_COLUMN = 0;
    const int KEY_SEQ_COLUMN  = 1;

    int row = modelIndex.row();

    // extract action name
    m_currentActionItem = m_treeModel->item(row, ACT_NAME_COLUMN);
    ui->actionNameLabel->setText(m_currentActionItem->text());

    // extract key sequence
    m_currentKeySeqItem = m_treeModel->item(row, KEY_SEQ_COLUMN);
    ui->keySeqLineEdit->setText(m_currentKeySeqItem->text());

    qDebug() << "You Select Item:" << m_currentKeySeqItem->text();

    ui->keySeqLineEdit->setFocus();
}

void ShortcutsPage::keyCapLineEditTextChanged(QKeySequence keySeqence)
{
    if ( m_currentActionItem == NULL ||
         m_currentKeySeqItem == NULL )
    {
        return;
    }

    QString strCmdName = QString("Cmd") + m_currentActionItem->text();
    QString strKeySeq  = keySeqence.toString( QKeySequence::PortableText );

    QSettings setting("pencil", "pencil");
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
            return;
        }
        removeDuplicateKeySequenc(&setting, keySeqence);
    }

    setting.setValue(strCmdName, strKeySeq);
    setting.endGroup();
    setting.sync();

    //m_currentKeySeqItem->setText( keySeqence.toString(QKeySequence::NativeText) );
    loadShortcutsFromSetting();

    qDebug() << "Shortcut " << strCmdName << " = " << strKeySeq;
}

void ShortcutsPage::pressRestoreShortcutsButton()
{
    restoreShortcutsToDefault();
    loadShortcutsFromSetting();
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

void ShortcutsPage::removeDuplicateKeySequenc(QSettings* settings, QKeySequence keySeq)
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

void ShortcutsPage::loadShortcutsFromSetting()
{
    // Load shortcuts from settings
    QSettings settings("pencil", "pencil");
    settings.beginGroup("shortcuts");

    m_treeModel->setRowCount( settings.allKeys().size());
    m_treeModel->setColumnCount( 2 );

    int i = 0;
    foreach (QString strCmdName, settings.allKeys())
    {
        QString strKeySequence = settings.value(strCmdName).toString();

        //convert to native format
        strKeySequence = QKeySequence(strKeySequence).toString( QKeySequence::NativeText );

        QStringRef strHumanReadCmdName (&strCmdName, 3, strCmdName.size() - 3);

        m_treeModel->setItem(i, 0, new QStandardItem(strHumanReadCmdName.toString()));
        m_treeModel->setItem(i, 1, new QStandardItem(strKeySequence));

        m_treeModel->item(i, 0)->setEditable(false);
        m_treeModel->item(i, 1)->setEditable(true);

        i++;
    }
    settings.endGroup();
}

