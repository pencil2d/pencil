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
}

Xsheet::~Xsheet()
{
    delete ui;
}

void Xsheet::newOpenScene()
{
    erasePapa();
}

void Xsheet::initUI()
{
    mLayerNames = new QStringList;
    mLayerCount = 0;
    mPapaLines = new QStringList;
    mTableWidget = ui->tableXsheet;
    connect(mTableWidget, &QTableWidget::cellClicked, this, &Xsheet::selectLayerFrame);
    connect(mTableWidget, &QTableWidget::cellDoubleClicked, this, &Xsheet::addLayerFrame);
    connect(ui->btnPapa, &QPushButton::clicked, this, &Xsheet::loadPapa );
    connect(ui->btnNoPapa, &QPushButton::clicked, this, &Xsheet::erasePapa);
}

void Xsheet::updateUI()
{
}

void Xsheet::updateUi(Editor* editor)
{
    mEditor = editor;
    QSettings settings(PENCIL2D, PENCIL2D);
    mTimeLineLength = settings.value(SETTING_TIMELINE_SIZE,240).toInt();
    initXsheet();
    fillXsheet();
    showScrub(mEditor->currentFrame());
    writePapa();
}

void Xsheet::showScrub(int frame)
{
    mTableItem = new QTableWidgetItem(QString::number(frame));
    mTableItem->setBackgroundColor(QColor(250, 150, 150));
    mTableWidget->setItem(frame, 0, mTableItem);
    mTableWidget->scrollToItem(mTableItem);
}

void Xsheet::updateScrub(int frame)
{
    fillXsheet();
    showScrub(frame);
}

void Xsheet::updateXsheet()
{
    initXsheet();
    fillXsheet();
    writePapa();
    showScrub(mEditor->currentFrame());
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
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open *.pgo file"), "", tr("Pgo Files (*.pgo)"));
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;
    mPapaLines->clear();
    QTextStream in(&file);
    while (!in.atEnd()) {
        QString tmp = in.readLine();
        tmp.remove("\t");
        mPapaLines->append(tmp);
    }
    file.close();
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
        bool visi = mEditor->layers()->getLayer(i)->getVisibility();
        if (visi && (mEditor->layers()->getLayer(i)->type() == 1 || mEditor->layers()->getLayer(i)->type() == 2))
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
    else
    {
        erasePapa();
    }
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
