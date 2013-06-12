
#include <QDebug>
#include <QMap>
#include <QSettings>
#include <QVBoxLayout>
#include <QTableView>
#include <QStandardItemModel>

#include "shortcutspage.h"

ShortcutsPage::ShortcutsPage(QWidget *parent) :
    QWidget(parent)
{
    /*
    QSettings pKeySettings("shortcuts.ini", QSettings::IniFormat);

    pKeySettings.setValue("MoveTool", "Q");
    pKeySettings.setValue("HandTool", "H");
    pKeySettings.setValue("ClearTool", "L");
    pKeySettings.setValue("SelectTool", "V");
    pKeySettings.setValue("BrushTool", "B");

    //pKeySettings
    pKeySettings.sync();
    QStringList allKeys = pKeySettings.allKeys();

    QStandardItemModel* pTableModel = new QStandardItemModel(allKeys.size(), 2, this);
    connect(pTableModel, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(tableItemChangs(QStandardItem*)));

    for (int i = 0; i < allKeys.size();  ++i )
    {
        QString strKey = allKeys[ i ];
        QString strKeyName = pKeySettings.value(strKey).toString();
        pTableModel->setItem(i, 0, new QStandardItem(strKey));
        pTableModel->setItem(i, 1, new QStandardItem(strKeyName));

        pTableModel->item(i, 0)->setEditable(false);
    }

    QTableView* pShortcutsTable = new QTableView(this);
    pShortcutsTable->setModel(pTableModel);

    QVBoxLayout* layout = new QVBoxLayout();
    layout->add(pShortcutsTable);

    setLayout(layout);
    */
}

void ShortcutsPage::tableItemChangs(QStandardItem* pItem)
{
    qDebug("Item Changes! at(%d, %d) = %s", pItem->row(), pItem->column(), pItem->text().toStdString().c_str());
}
