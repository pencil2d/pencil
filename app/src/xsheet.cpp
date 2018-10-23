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
    sl = new QStringList;
    mTableWidget = ui->tableXsheet;
    connect(mTableWidget, SIGNAL(cellClicked(int, int)), this, SLOT(selectLayerFrame(int, int)));
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
    sl->clear();
    int lType = mLayerMgr->currentLayer()->type();
    for (int i = 0; i < mLayerMgr->count(); i++)
    {   // count Bitmap and Vector layers
        if (mLayerMgr->getLayer(i)->type() == lType)
//            if (mLayerMgr->getLayer(i)->type() == 1 || mLayerMgr->getLayer(i)->type() == 2)
        {
            mLayerCount++;
            sl->append(mLayerMgr->getLayer(i)->name());
            qDebug() << "id : " << mLayerMgr->getLayer(i)->id();
        }
    }
    this->setMinimumWidth(mLayerCount * 40 + 140);
    ui->tableXsheet->setRowCount(mTimeLineLength + 1);
    ui->tableXsheet->setColumnCount(mLayerCount + 2);
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
    for (int i = 0; i < sl->size(); i++)
    {
        mTableItem = new QTableWidgetItem(sl->at(i));
        mTableItem->setBackgroundColor(QColor(Qt::lightGray));
        ui->tableXsheet->setItem(0, i + 1, mTableItem);
    }
    mTableItem = new QTableWidgetItem("DIAL");
    mTableItem->setBackgroundColor(QColor(Qt::lightGray));
    ui->tableXsheet->setItem(0, sl->size() + 1, mTableItem);

    fillXsheet();
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
    fillXsheet();
}

void Xsheet::fillXsheet()
{
    // fill Xsheet
    for (int i = 1; i <= mTimeLineLength; i++)
    {
        ui->tableXsheet->setRowHeight(i,16);
        mTableItem = new QTableWidgetItem(QString::number(i));
        ui->tableXsheet->setItem(i, 0, mTableItem);
        for (int j = 1; j <= mLayerCount; j++)
        {
            if (mLayerMgr->getLayer(j)->keyExists(i))
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
