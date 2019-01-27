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

    connect(ui->btn1Select, &QPushButton::clicked, mEditor, &Editor::copyFromScan);
    connect(ui->btn1Next, &QPushButton::clicked, mEditor, &Editor::scrubNextKeyFrame);
    connect(ui->sb2_Threshold, QOverload<int>::of(&QSpinBox::valueChanged), mLayerBitmap, &LayerBitmap::setThreshold);
    connect(ui->btn3_apply, &QPushButton::clicked, mEditor, &Editor::scanToTransparent);
    connect(ui->btn3_Next, &QPushButton::clicked, mEditor, &Editor::scrubNextKeyFrame);
    connect(ui->btn3_applyRest, &QPushButton::clicked, mEditor, &Editor::scanToTransparentRest);
    connect(ui->sb4_fillArea, QOverload<int>::of(&QSpinBox::valueChanged), mLayerBitmap, &LayerBitmap::setWhiteArea);
    connect(ui->btn4_apply, &QPushButton::clicked, mEditor, &Editor::fillWhiteAreas);
    connect(ui->btn4_next, &QPushButton::clicked, mEditor, &Editor::scrubNextKeyFrame);
    connect(ui->btn4_applyRest, &QPushButton::clicked, mEditor, &Editor::fillWhiteAreasRest);
    connect(ui->btn5_thin, &QPushButton::clicked, mEditor, &Editor::toThinBlackLine);
    connect(ui->btn5_next, &QPushButton::clicked, mEditor, &Editor::scrubNextKeyFrame);
    connect(ui->btn5_thinRest, &QPushButton::clicked, mEditor, &Editor::toThinBlackLineRest);
}

BitmapColoring::~BitmapColoring()
{
    delete ui;
}

void BitmapColoring::initUI()
{
    if (!isVisible()) { return; }

    updateUI();
}

void BitmapColoring::updateUI()
{
    Layer* layer = mEditor->layers()->currentLayer();
    if (layer->type() == Layer::BITMAP)
    {
        setEnabled(true);
    }
    else
    {
        setEnabled(false);
    }
}
