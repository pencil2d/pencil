/*

Pencil - Traditional Animation Software
Copyright (C) 2012-2018 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/

#include "xsheet.h"
#include "ui_xsheet.h"
#include <QSettings>
#include <QStringList>
#include <QMessageBox>
#include "pencildef.h"
#include "layer.h"
#include <QFile>
#include <QFileDialog>

Xsheet::Xsheet(QWidget *parent) :
    BaseDockWidget(parent),
    ui(new Ui::Xsheet)
{
    ui->setupUi(this);
    initUI();
}

Xsheet::~Xsheet()
{
    delete ui;
}

void Xsheet::initUI()
{
    mLayerNames = new QStringList;
    mLayerCount = 0;
    mPapaLines = new QStringList;
    mTableWidget = ui->tableXsheet;
    connect(mTableWidget, SIGNAL(cellClicked(int, int)), this, SLOT(selectLayerFrame(int, int)));
    connect(mTableWidget, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(addLayerFrame(int,int)));
    connect(ui->btnPapa, SIGNAL(clicked(bool)), this, SLOT(loadPapa()));
    connect(ui->btnNoPapa, SIGNAL(clicked(bool)), this, SLOT(erasePapa()));
}

void Xsheet::updateUI()
{

}

void Xsheet::updateUi(Editor* editor)
{
    qDebug() << "in UpdateUi. papaSize: " << mPapaLines->size();
    mEditor = editor;
    QSettings settings(PENCIL2D, PENCIL2D);
    mTimeLineLength = settings.value(SETTING_TIMELINE_SIZE,240).toInt();
    initXsheet();
    fillXsheet();
    showScrub(mEditor->currentFrame());
    writePapa();
}

void Xsheet::upNew()
{
    qDebug() << "Up new";
}

void Xsheet::showScrub(int frame)
{
//    qDebug() << "in showScrub. Frame: " << frame;
    mTableItem = new QTableWidgetItem(QString::number(frame));
    mTableItem->setBackgroundColor(QColor(250, 150, 150));
    mTableWidget->setItem(frame, 0, mTableItem);
}

void Xsheet::updateXsheet()
{
    initXsheet();
    fillXsheet();
    writePapa();
}

void Xsheet::selectLayerFrame(int row, int column)
{
    initXsheet();
    fillXsheet();
    selectItem(row, column);
    showScrub(row);
    writePapa();
}

void Xsheet::addLayerFrame(int row, int column)
{
    selectItem(row, column);
    if (column > 0 && column <= mLayerCount)
        mEditor->layers()->currentLayer()->addNewKeyFrameAt(row);
    initXsheet();
    fillXsheet();
    showScrub(row);
    writePapa();
}

void Xsheet::fillXsheet()
{
    for (int i = 1; i <= mTimeLineLength; i++)
    {
        mTableWidget->setRowHeight(i,16);
        mTableItem = new QTableWidgetItem(QString::number(i));
        mTableItem->setBackgroundColor(QColor(250, 240, 160));
        mTableWidget->setItem(i, 0, mTableItem);
        for (int j = 1; j <= mLayerCount; j++)
        {
            if (mEditor->layers()->findLayerByName(mTableWidget->item(0,j)->text())->keyExists(i))
            {
                int type = getLayerType(mEditor->layers()->findLayerByName(mTableWidget->item(0,j)->text()));
                mTableItem = new QTableWidgetItem(QString::number(i));
                mTableItem->setBackgroundColor(getLayerColor(type));
                mTableWidget->setItem(i, j, mTableItem);
            }
        }
    }
}

void Xsheet::loadPapa()
{
    qDebug() << "Before load: mPapaLines " << mPapaLines->size();
    mPapaLines->clear();
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open *.pgo file"), "", tr("Pgo Files (*.pgo)"));
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;
    QTextStream in(&file);
    while (!in.atEnd()) {
        QString tmp = in.readLine();
        tmp.remove("\t");
        mPapaLines->append(tmp);
    }
    file.close();
    qDebug() << "After load: mPapaLines " << mPapaLines->size();
    writePapa();
}

void Xsheet::erasePapa()
{
    int dial = mTableWidget->columnCount();

    // clear column
    for (int i = 0; i < mTimeLineLength; i++)
    {
        mTableItem = new QTableWidgetItem("");
        mTableItem->setBackgroundColor(Qt::white);
        mTableWidget->setItem(i, dial - 1, mTableItem);
    }
    mTableItem = new QTableWidgetItem("DIAL");
    mTableItem->setBackgroundColor(QColor(244, 167, 167, 150));
    mTableWidget->setItem(0, mLayerNames->size() + 1, mTableItem);
    mPapaLines->clear();
}

void Xsheet::initXsheet()
{
    mLayerCount = 0;
    mLayerNames->clear();
    for (int i = 0; i < mEditor->layers()->count(); i++)
    {   // count Bitmap and Vector layers (duplicate names NOT supported)
        if (mEditor->layers()->getLayer(i)->type() == 1 || mEditor->layers()->getLayer(i)->type() == 2)
        {
            if (!mLayerNames->contains(mEditor->layers()->getLayer(i)->name()))
            {
                mLayerCount++;
                mLayerNames->append(mEditor->layers()->getLayer(i)->name());
            }
            else
            {
                int ret = QMessageBox::critical(new QWidget,
                               tr("Layer name duplicate!"),
                               tr("Identical layer names '%1' not supported in Xsheet").arg(mEditor->layers()->getLayer(i)->name()),
                               QMessageBox::Ok);
                Q_UNUSED(ret);
            }
        }
    }
    for (int i = 1; i <= mLayerNames->size(); i++)
    {
        for (int j = 1; j < mTimeLineLength; j++)
        {
            mTableItem = new QTableWidgetItem("");
            mTableItem->setBackgroundColor(Qt::white);
            mTableWidget->setItem(j, i, mTableItem);
        }
    }
    this->setMinimumWidth(mLayerCount * 40 + 100);
    mTableWidget->setRowCount(mTimeLineLength + 1);
    mTableWidget->setColumnCount(mLayerCount + 2);
    // set column width for layers
    for (int i = 0; i < mTableWidget->columnCount(); i++)
    {
        mTableWidget->setColumnWidth(i, 40);
    }
    // set headers of Xsheet
    mTableWidget->setRowHeight(0,16);
    mTableItem = new QTableWidgetItem("#");
    mTableItem->setBackgroundColor(QColor(250, 240, 160));
    mTableWidget->setItem(0, 0, mTableItem);
    for (int i = 0; i < mLayerNames->size(); i++)
    {
        int type = getLayerType(mEditor->layers()->findLayerByName(mLayerNames->at(i)));
        mTableItem = new QTableWidgetItem(mLayerNames->at(i));
        mTableItem->setBackgroundColor(getLayerColor(type));
        mTableWidget->setItem(0, i + 1, mTableItem);
    }
//    if (!mPapaLines->isEmpty()) { return; }
    mTableItem = new QTableWidgetItem("DIAL");
    mTableItem->setBackgroundColor(QColor(244, 167, 167, 150));
    mTableWidget->setItem(0, mLayerNames->size() + 1, mTableItem);
}

void Xsheet::writePapa()
{
    qDebug() << "Before write: mPapaLines " << mPapaLines->size();

    if (mPapaLines->size() > 11)
    {
        int dial = mTableWidget->columnCount();

        // clear column
        for (int i = 0; i < mTimeLineLength; i++)
        {
            mTableItem = new QTableWidgetItem("");
            mTableItem->setBackgroundColor(Qt::white);
            mTableWidget->setItem(i, dial - 1, mTableItem);
        }

        // write header
        mTableItem = new QTableWidgetItem(mPapaLines->at(5));
        mTableItem->setBackgroundColor(QColor(245, 155, 155, 150));
        mTableWidget->setItem(0, dial - 1, mTableItem);
        QString tmp;
        QStringList lipsync;    // papagayo info on mouths
        for (int i = 12; i < mPapaLines->size(); i++)
        {
            tmp = mPapaLines->at(i);
            lipsync = tmp.split(" ");
            if (lipsync.size() == 2)
            {
                tmp = lipsync.at(0);    // frame number
                int row = tmp.toInt();
                mTableItem = new QTableWidgetItem(lipsync.at(1)); // audio to animate
                mTableItem->setBackgroundColor(QColor(245, 155, 155, 150));
                mTableWidget->setItem(row, dial - 1, mTableItem);
            }
        }
        tmp = mPapaLines->at(10);
        int row = tmp.toInt();
        mTableWidget->setItem(row, dial - 1, new QTableWidgetItem("-"));
    }
    qDebug() << "After write: mPapaLines " << mPapaLines->size();
}

int Xsheet::getLayerType(Layer *layer)
{
    return layer->type();
}

void Xsheet::selectItem(int row, int column)
{
    if (column > 0 && column <= mLayerCount)
    {
        mTableItem = new QTableWidgetItem();
        mTableItem = mTableWidget->item(0, column);
        Layer* layer = mEditor->layers()->findLayerByName(mTableItem->text());
        if (layer == nullptr) { return; }
        mEditor->layers()->setCurrentLayer(layer);
        mEditor->scrubTo(row);
    }
}

QColor Xsheet::getLayerColor(int color)
{
    if (color == 1) { return QColor(151, 176, 244); }
    else            { return QColor(150, 242, 150); }
}
