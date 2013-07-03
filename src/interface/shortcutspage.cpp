
#include <QDebug>
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

    QStandardItemModel* pTableModel = new QStandardItemModel(pSettings->allKeys().size(), 2, this);    

    int i = 0;
    foreach (QString strCmdName, pSettings->allKeys())
    {        
        QString strKeySequence = pSettings->value(strCmdName).toString();
        
        pTableModel->setItem(i, 0, new QStandardItem(strCmdName));
        pTableModel->setItem(i, 1, new QStandardItem(strKeySequence));

        pTableModel->item(i, 0)->setEditable(false);
        
        i++;
    }
    pSettings->endGroup();

    QTableView* pShortcutsTable = new QTableView(this);
    pShortcutsTable->setModel(pTableModel);

    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget(pShortcutsTable);

    setLayout(layout);

    connect(pTableModel, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(tableItemChangs(QStandardItem*)));
    connect(pShortcutsTable, SIGNAL(doubleClicked(const QModelIndex&)), this, SLOT(tableItemDoubleClicked(const QModelIndex&)));
}

void ShortcutsPage::tableItemChangs(QStandardItem* pItem)
{
    qDebug("Item Changes! at(%d, %d) = %s", pItem->row(), pItem->column(), pItem->text().toStdString().c_str());
}

void ShortcutsPage::keyPressEvent( QKeyEvent * event )
{
    qDebug("kerker!");
}

void ShortcutsPage::tableItemDoubleClicked( const QModelIndex & )
{

}

