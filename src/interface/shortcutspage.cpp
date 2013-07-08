
#include <QDebug>
#include <QMap>
#include <QSettings>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QTreeView>
#include <QStandardItemModel>
#include <QKeyEvent>
#include <QKeySequence>
#include "pencilsettings.h"
#include "shortcutspage.h"
#include "keycapturelineedit.h"
#include "ui_shortcutspage.h"


ShortcutsPage::ShortcutsPage(QWidget *parent) :
    QWidget(parent),
    m_treeModel( nullptr ),
    m_currentActionItem( nullptr ),
    m_currentKeySeqItem( nullptr ),
    ui( new Ui::ShortcutsPage )
{
    ui->setupUi(this);
    m_treeModel = new QStandardItemModel(this);

    loadShortcutsFromSetting();

    ui->treeView->setModel(m_treeModel);

    connect(ui->treeView, SIGNAL(clicked(const QModelIndex&)),
            this, SLOT(tableItemClicked(const QModelIndex&)));

    connect(ui->keySeqLineEdit, SIGNAL(keyCaptured(QKeySequence)),
            this, SLOT(keyCapLineEditTextChanged(QKeySequence)));

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
    QString strCmdName = m_currentActionItem->text();
    QString strKeySeq  = keySeqence.toString( QKeySequence::PortableText );

    pencilSettings()->beginGroup("shortcuts");
    pencilSettings()->setValue(strCmdName, strKeySeq);
    pencilSettings()->endGroup();
    pencilSettings()->sync();

    m_currentKeySeqItem->setText( keySeqence.toString(QKeySequence::NativeText) );

    qDebug() << "Shortcut " << strCmdName << " = " << strKeySeq;
}

void ShortcutsPage::loadShortcutsFromSetting()
{
    // Load shortcuts from settings
    QSettings* pSettings = pencilSettings();
    pSettings->beginGroup("shortcuts");

    m_treeModel->setRowCount( pSettings->allKeys().size());
    m_treeModel->setColumnCount( 2 );

    int i = 0;
    foreach (QString strCmdName, pSettings->allKeys())
    {
        QString strKeySequence = pSettings->value(strCmdName).toString();

        //convert to native format
        strKeySequence = QKeySequence(strKeySequence).toString( QKeySequence::NativeText );

        m_treeModel->setItem(i, 0, new QStandardItem(strCmdName));
        m_treeModel->setItem(i, 1, new QStandardItem(strKeySequence));

        m_treeModel->item(i, 0)->setEditable(false);
        m_treeModel->item(i, 1)->setEditable(true);

        i++;
    }
    pSettings->endGroup();
}

