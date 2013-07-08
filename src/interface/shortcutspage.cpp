
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
    m_currentKeySeqItem( nullptr ),
    m_treeModel( nullptr ),
    ui( new Ui::ShortcutsPage )
{
    ui->setupUi(this);

    QSettings* pSettings = pencilSettings();

    pSettings->beginGroup("shortcuts");

    m_treeModel = new QStandardItemModel(pSettings->allKeys().size(), 2, this);

    int i = 0;
    foreach (QString strCmdName, pSettings->allKeys())
    {
        QString strKeySequence = pSettings->value(strCmdName).toString();

        m_treeModel->setItem(i, 0, new QStandardItem(strCmdName));
        m_treeModel->setItem(i, 1, new QStandardItem(strKeySequence));

        m_treeModel->item(i, 0)->setEditable(false);
        m_treeModel->item(i, 1)->setEditable(true);

        i++;
    }
    pSettings->endGroup();

    ui->treeView->setModel(m_treeModel);

    connect(m_treeModel, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(tableItemChangs(QStandardItem*)));
    connect(ui->treeView, SIGNAL(clicked(const QModelIndex&)), this, SLOT(tableItemClicked(const QModelIndex&)));

    qDebug() << "zz";
    //KeyCaptureLineEdit* k = ui->keySeqLineEdit;
    //ui->keySeqLineEdit->installEventFilter(this);
}

void ShortcutsPage::tableItemChangs(QStandardItem* pItem)
{
    qDebug("Item Changes! at(%d, %d) = %s", pItem->row(), pItem->column(), pItem->text().toStdString().c_str());
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

