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
#include "bitmapcoloring.h"
#include "ui_bitmapcoloringwidget.h"
#include "layermanager.h"
#include "qdebug.h"


BitmapColoring::BitmapColoring(Editor* editor, QWidget *parent) :
    BaseDockWidget(parent)
{
    QWidget* innerWidget = new QWidget;
    setWindowTitle(tr("Bitmap Coloring"));

    ui = new Ui::BitmapColoringWidget;
    ui->setupUi(innerWidget);
    setWidget(innerWidget);

    mEditor = editor;
    if (mEditor->layers()->currentLayer()->type() == Layer::BITMAP)
        mLayerBitmap = static_cast<LayerBitmap*>(mEditor->layers()->currentLayer());
    mBitmapImage = mLayerBitmap->getBitmapImageAtFrame(mEditor->currentFrame());
    ui->btnSelectAreas->setIcon(QIcon(":/icons/select.png"));
    connect(ui->cb1Trace, &QCheckBox::stateChanged, this, &BitmapColoring::updateTraceBoxes);
    connect(ui->cbLayerSelector, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &BitmapColoring::colorMethodChanged);
    connect(ui->btnSelectAreas, &QPushButton::clicked, this, &BitmapColoring::selectAreasChanged);
    connect(ui->cb1Thin, &QCheckBox::stateChanged, this, &BitmapColoring::updateThinBoxes);
    connect(ui->cb1FinishMethod, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &BitmapColoring::finishMethodChanged);
/*
    connect(ui->btn1Select, &QPushButton::clicked, mEditor, &Editor::copyFromScan);
    connect(ui->btn1Next, &QPushButton::clicked, mEditor, &Editor::scrubNextKeyFrame);
//    connect(ui->sb1_Threshold, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged) , mBitmapImage, &BitmapImage::setThreshold);
    connect(ui->btnx_blackLine, &QPushButton::clicked, mEditor, &Editor::toBlackLine);

//    connect(ui->sb2_fillArea, QOverload<int>::of(&QSpinBox::valueChanged) , mBitmapImage, &BitmapImage::setWhiteArea);
    connect(ui->btn2_fillRest, &QPushButton::clicked, mEditor, &Editor::fillWhiteAreasRest);
    connect(ui->btn2_repairs, &QPushButton::clicked, mEditor, &Editor::fillWhiteAreas);
    connect(ui->btn3_thinRest, &QPushButton::clicked, mEditor, &Editor::toThinBlackLineRest);
    connect(ui->btn3_repairs, &QPushButton::clicked, mEditor, &Editor::toThinBlackLine);
    connect(ui->btn4_replaceThinLines, &QPushButton::clicked, mEditor, &Editor::replaceThinLines);
    */
}

BitmapColoring::~BitmapColoring()
{
    delete ui;
}

void BitmapColoring::initUI()
{
    ui->btn1FinishLineColor->setEnabled(false);
}

void BitmapColoring::updateUI()
{

}

void BitmapColoring::visibilityChanged(bool visibility)
{
    Q_UNUSED(visibility);
    updateUI();
}

void BitmapColoring::colorMethodChanged()
{
    if (ui->cbLayerSelector->currentIndex() == 0)
    {
        ui->cb2TraceBlack->setChecked(false);
        ui->cb2TraceBlack->setEnabled(false);
        ui->cb2ThinBlack->setChecked(false);
        ui->cb2ThinBlack->setEnabled(false);
        ui->cb2FinishBlack->setChecked(false);
        ui->cb2FinishBlack->setEnabled(false);
    }
    else
    {
        ui->cb2TraceBlack->setEnabled(true);
        ui->cb2ThinBlack->setEnabled(true);
        ui->cb2FinishBlack->setEnabled(true);
    }
}

void BitmapColoring::updateTraceBoxes()
{
    if (ui->cb1Trace->isChecked())
    {
        ui->gb2Prepare->setEnabled(true);
        if (ui->cbLayerSelector->currentIndex() == 0)
            ui->cb2TraceBlack->setEnabled(false);
    }
    else
    {
        ui->cb2TraceBlack->setChecked(false);
        ui->cb2TraceRed->setChecked(false);
        ui->cb2TraceGreen->setChecked(false);
        ui->cb2TraceBlue->setChecked(false);
        ui->gb2Prepare->setEnabled(false);
    }
}

void BitmapColoring::selectAreasChanged()
{
    if (mSelectAreas)
    {
        ui->btnSelectAreas->setIcon(QIcon(":/icons/select.png"));
        mSelectAreas = false;
    }
    else {
        ui->btnSelectAreas->setIcon(QIcon(":/icons/select_yes.png"));
        mSelectAreas = true;
    }

}

void BitmapColoring::updateThinBoxes()
{
    if (ui->cb1Thin->isChecked())
    {
        ui->gb2Thin->setEnabled(true);
        if (ui->cbLayerSelector->currentIndex() == 0)
            ui->cb2ThinBlack->setEnabled(false);
    }
    else
    {
        ui->cb2ThinBlack->setChecked(false);
        ui->cb2ThinRed->setChecked(false);
        ui->cb2ThinGreen->setChecked(false);
        ui->cb2ThinBlue->setChecked(false);
        ui->gb2Thin->setEnabled(false);
    }
}

void BitmapColoring::finishMethodChanged()
{
    if (ui->cb1FinishMethod->currentIndex() == 0)
    {
        ui->btn1FinishLineColor->setEnabled(false);
    }
    else
    {
        ui->btn1FinishLineColor->setEnabled(true);
    }
}
