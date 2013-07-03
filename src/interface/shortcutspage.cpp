
#include <functional>
#include <QDebug>
#include <QDialog>
#include <QMap>
#include <QSettings>
#include <QVBoxLayout>
#include <QTableView>
#include <QStandardItemModel>
#include "pencilsettings.h"
#include "shortcutspage.h"

ShortcutsPage::ShortcutsPage(QWidget *parent) :
    QWidget(parent)
{    
    QSettings* pSettings = pencilSettings();

    pSettings->beginGroup("shortcuts");            

    m_pTableModel = new QStandardItemModel(pSettings->allKeys().size(), 2, this);

    int i = 0;
    foreach (QString strCmdName, pSettings->allKeys())
    {        
        QString strKeySequence = pSettings->value(strCmdName).toString();
        
        m_pTableModel->setItem(i, 0, new QStandardItem(strCmdName));
        m_pTableModel->setItem(i, 1, new QStandardItem(strKeySequence));

        m_pTableModel->item(i, 0)->setEditable(false);
        m_pTableModel->item(i, 1)->setEditable(false);
        
        i++;
    }
    pSettings->endGroup();

    QTableView* pShortcutsTable = new QTableView(this);
    pShortcutsTable->setModel(m_pTableModel);

    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget(pShortcutsTable);

    setLayout(layout);

    connect(m_pTableModel, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(tableItemChangs(QStandardItem*)));
    connect(pShortcutsTable, SIGNAL(doubleClicked(const QModelIndex&)), this, SLOT(tableItemDoubleClicked(const QModelIndex&)));
}

void ShortcutsPage::tableItemChangs(QStandardItem* pItem)
{
    qDebug("Item Changes! at(%d, %d) = %s", pItem->row(), pItem->column(), pItem->text().toStdString().c_str());
}

void ShortcutsPage::tableItemDoubleClicked( const QModelIndex& modelIndex )
{
    qDebug("Double Clicked!");
    
    int rowIndex = modelIndex.row();

    QString strCmdName = m_pTableModel->item(rowIndex, 0)->text();

    m_pChooseKeyDialog = new QDialog(this);
    m_pChooseKeyDialog->setModal(true);
    m_pChooseKeyDialog->show();

        
    std::function<void(void)> test = [] () { qDebug("Lambda Test"); };
    test();
}

