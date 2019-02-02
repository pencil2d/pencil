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
    connect(ui->btnx_blackLine, &QPushButton::clicked, mEditor, &Editor::toBlackLine);

    connect(ui->sb4_fillArea, QOverload<int>::of(&QSpinBox::valueChanged), mLayerBitmap, &LayerBitmap::setWhiteArea);
    connect(ui->btn4_apply, &QPushButton::clicked, mEditor, &Editor::fillWhiteAreas);
    connect(ui->btn4_next, &QPushButton::clicked, mEditor, &Editor::scrubNextKeyFrame);
    connect(ui->btn4_applyRest, &QPushButton::clicked, mEditor, &Editor::fillWhiteAreasRest);
    connect(ui->btn5_thin, &QPushButton::clicked, mEditor, &Editor::toThinBlackLine);
    connect(ui->btn5_next, &QPushButton::clicked, mEditor, &Editor::scrubNextKeyFrame);
    connect(ui->btn5_thinRest, &QPushButton::clicked, mEditor, &Editor::toThinBlackLineRest);
    connect(ui->btn7_replaceThinLines, &QPushButton::clicked, mEditor, &Editor::replaceThinLines);
}

BitmapColoring::~BitmapColoring()
{
    delete ui;
}

void BitmapColoring::initUI()
{
//    if (!isVisible()) { return; }

    updateUI();
}

void BitmapColoring::updateUI()
{
    if (!isVisible()) { return; }

    Layer* layer = mEditor->layers()->currentLayer();
    setEnabled(true);
    if (layer->type() == Layer::BITMAP && layer->parentId() == -1)
    {
        ui->tabWidgetColor->setEnabled(false);
        ui->frameWidgetColor->setEnabled(false);
        ui->tabWidgetScans->setEnabled(true);
        ui->frameWidgetScans->setEnabled(true);
        for (int i = 0; i < mEditor->layers()->count(); i++)
        {
            if (mEditor->layers()->getLayer(i)->parentId() == layer->id())
                ui->labx_3->setText(tr("To Layer: %1").arg(mEditor->layers()->getLayer(i)->name()));
        }
    }
    else if (layer->type() == Layer::BITMAP && layer->parentId() > -1)
    {
        ui->tabWidgetColor->setEnabled(true);
        ui->frameWidgetColor->setEnabled(true);
        ui->tabWidgetScans->setEnabled(false);
        ui->frameWidgetScans->setEnabled(false);
    }
    else
    {
        setEnabled(false);
    }
}
