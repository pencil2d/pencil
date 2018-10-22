#include "xsheet.h"
#include "ui_xsheet.h"
#include <QSettings>
#include <QStringList>
#include "pencildef.h"
#include "layer.h"

Xsheet::Xsheet(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Xsheet)
{
    ui->setupUi(this);
    mTableWidget = ui->tableXsheet;
    connect(mTableWidget, SIGNAL(cellClicked(int, int)), this, SLOT(selectLayerFrame(int, int)));
//    connect(this, SIGNAL(ui->tableXsheet->cellClicked(1,2)), this, SLOT(selectLayerFrame()));
}

Xsheet::~Xsheet()
{
    delete ui;
}

void Xsheet::updateUi(LayerManager &lMgr, Editor *&editor)
{
    mLayerMgr = &lMgr;
    mEditor = editor;
    QSettings settings(PENCIL2D, PENCIL2D);
    mTimeLineLength = settings.value(SETTING_TIMELINE_SIZE,240).toInt();
    qDebug() << "Width: " << ui->tableXsheet->width();
    mLayerCount = 0;
    QStringList sl;
    sl.clear();
    for (int i = 0; i < lMgr.count(); i++)
    {   // count Bitmap and Vector layers
        if (lMgr.getLayer(i)->type() == 1 || lMgr.getLayer(i)->type() == 2)
        {
            mLayerCount++;
            sl.append(lMgr.getLayer(i)->name());
        }
    }
    this->setMinimumWidth(mLayerCount * 40 + 140);
    ui->tableXsheet->setRowCount(mTimeLineLength + 1);
    ui->tableXsheet->setColumnCount(mLayerCount + 2);
//    ui->tableXsheet->setColumnWidth(0, 40);
//    ui->tableXsheet->setColumnWidth(mLayerCount + 1, 40);
    qDebug() << "Width: " << ui->tableXsheet->width();
    // set column width for layers
    for (int i = 0; i < ui->tableXsheet->columnCount(); i++)
    {
        ui->tableXsheet->setColumnWidth(i, 40);
    }

    // set headers of Xsheet
    ui->tableXsheet->setRowHeight(0,16);
    mTableItem = new QTableWidgetItem("#");
    mTableItem->setBackgroundColor(QColor(Qt::lightGray));
    ui->tableXsheet->setItem(0, 0, mTableItem);
    for (int i = 0; i < sl.size(); i++)
    {
        mTableItem = new QTableWidgetItem(sl.at(i));
        mTableItem->setBackgroundColor(QColor(Qt::lightGray));
        ui->tableXsheet->setItem(0, i + 1, mTableItem);
    }
    mTableItem = new QTableWidgetItem("DIAL");
    mTableItem->setBackgroundColor(QColor(Qt::lightGray));
    ui->tableXsheet->setItem(0, sl.size() + 1, mTableItem);

    // fill Xsheet
    for (int i = 1; i <= mTimeLineLength; i++)
    {
        ui->tableXsheet->setRowHeight(i,16);
        mTableItem = new QTableWidgetItem(QString::number(i));
        ui->tableXsheet->setItem(i, 0, mTableItem);
        for (int j = 1; j <= mLayerCount; j++)
        {
            if (lMgr.getLayer(j)->keyExists(i))
            {
                mTableItem = new QTableWidgetItem(QString::number(i));
                ui->tableXsheet->setItem(i, j, mTableItem);
            }
            else
            {
                ui->tableXsheet->setItem(i, j, new QTableWidgetItem(" "));
            }
        }
    }
}

void Xsheet::selectLayerFrame(int row, int column)
{
    if (column > 0 && column <= mLayerCount)
    {
        mTableItem = new QTableWidgetItem();
        mTableItem = mTableWidget->item(0, column);
        Layer* layer = mLayerMgr->findLayerByName(mTableItem->text());
        if (layer == nullptr) { return; }
        mLayerMgr->setCurrentLayer(layer);
        mEditor->scrubTo(row);
    }
}
