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

#include <QMessageBox>
#include <QProgressDialog>
#include "bitmapcoloring.h"
#include "ui_bitmapcoloringwidget.h"
#include "layermanager.h"
#include "toolmanager.h"
#include "app_util.h"


BitmapColoring::BitmapColoring(Editor* editor, QWidget *parent) :
    BaseDockWidget(parent)
{
    QWidget* innerWidget = new QWidget;
    setWindowTitle(tr("Bitmap Coloring"));

    ui = new Ui::BitmapColoringWidget;
    ui->setupUi(innerWidget);
    setWidget(innerWidget);

    mEditor = editor;
    mScribblearea = mEditor->getScribbleArea();
    if (mEditor->layers()->currentLayer()->type() == Layer::BITMAP)
        mLayerBitmap = static_cast<LayerBitmap*>(mEditor->layers()->currentLayer());
    mBitmapImage = mLayerBitmap->getBitmapImageAtFrame(mEditor->currentFrame());
    ui->btnSelectAreas->setIcon(QIcon(":/icons/select.png"));
    checkBlackBoxes();
    checkRedBoxes();
    checkGreenBoxes();
    checkBlueBoxes();

    connect(ui->cbMethodSelector, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &BitmapColoring::colorMethodChanged);
    connect(ui->cb2TraceBlack, &QCheckBox::stateChanged, this, &BitmapColoring::checkBlackBoxes);
    connect(ui->cb2TraceRed, &QCheckBox::stateChanged, this, &BitmapColoring::checkRedBoxes);
    connect(ui->cb2TraceGreen, &QCheckBox::stateChanged, this, &BitmapColoring::checkGreenBoxes);
    connect(ui->cb2TraceBlue, &QCheckBox::stateChanged, this, &BitmapColoring::checkBlueBoxes);
    connect(ui->cb3TraceAllKeyframes, &QCheckBox::stateChanged, this, &BitmapColoring::checkAllKeyframes);

    // Prepare
    connect(ui->tabWidget, &QTabWidget::tabBarClicked, this, &BitmapColoring::tabClicked);
    connect(ui->cb1Threshold, &QCheckBox::stateChanged, this, &BitmapColoring::updateBtnSelect);
    connect(ui->sb1Threshold, QOverload<int>::of(&QSpinBox::valueChanged), this, &BitmapColoring::setThreshold);
    connect(ui->btnSelectAreas, &QPushButton::clicked, this, &BitmapColoring::activateSelectTool);
    connect(ui->btnApplyPrepare, &QPushButton::clicked, this, &BitmapColoring::traceLines);
    // Thin
    connect(ui->sbSpotAreas, QOverload<int>::of(&QSpinBox::valueChanged), this, &BitmapColoring::setSpotArea);
    connect(ui->btnApplyThin, &QPushButton::clicked, this, &BitmapColoring::thinLines);
    // Finish
    connect(ui->btnApplyFinish, &QPushButton::clicked, this, &BitmapColoring::blendLines);

    updateUI();
}

BitmapColoring::~BitmapColoring()
{
    delete ui;
}

void BitmapColoring::initUI()
{
    updateUI();
}

void BitmapColoring::updateUI()
{
    if (mEditor->layers()->currentLayer()->type() == Layer::BITMAP)
        mLayerBitmap = static_cast<LayerBitmap*>(mEditor->layers()->currentLayer());
    if (mLayerBitmap == nullptr) { return; }

    if (mEditor->layers()->currentLayer()->type() == Layer::BITMAP)
    {
        if (mLayerBitmap->getHasColorLayer())
        {
            ui->tab1->setEnabled(true);
            ui->tab2->setEnabled(false);
            ui->tab3->setEnabled(false);
        }
        else if (mLayerBitmap->getIsColorLayer())
        {
            ui->tab1->setEnabled(false);
            ui->tab2->setEnabled(true);
            ui->tab3->setEnabled(true);
        }
        else
        {
            ui->tab1->setEnabled(true);
            ui->tab2->setEnabled(true);
            ui->tab3->setEnabled(true);
        }
    }
    else
    {   // If it is not a Bitmap Layer - disable
        ui->tab1->setEnabled(false);
        ui->tab2->setEnabled(false);
        ui->tab3->setEnabled(false);
    }
}

void BitmapColoring::visibilityChanged(bool visibility)
{
    if (visibility)
        updateUI();
}

void BitmapColoring::colorMethodChanged()
{
    if (ui->cbMethodSelector->currentIndex() == 0)
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
        ui->cb2TraceBlack->setChecked(true);
    }
}

void BitmapColoring::activateSelectTool()
{
    ToolManager* tool = mEditor->tools();
    tool->setCurrentTool(SELECT);
}

void BitmapColoring::checkBlackBoxes()
{
    if (ui->cb2TraceBlack->isChecked())
    {
        ui->cb2ThinBlack->show();
        ui->cb2FinishBlack->show();
        ui->cb2ThinBlack->setChecked(ui->cb2TraceBlack->isChecked());
        ui->cb2FinishBlack->setChecked(ui->cb2TraceBlack->isChecked());
    }
    else
    {
        ui->cb2ThinBlack->hide();
        ui->cb2FinishBlack->hide();
    }
}

void BitmapColoring::checkRedBoxes()
{
    if (ui->cb2TraceRed->isChecked())
    {
        ui->cb2ThinRed->show();
        ui->cb2FinishRed->show();
        ui->cb2ThinRed->setChecked(ui->cb2TraceRed->isChecked());
        ui->cb2FinishRed->setChecked(ui->cb2TraceRed->isChecked());
    }
    else
    {
        ui->cb2ThinRed->hide();
        ui->cb2FinishRed->hide();
    }
}

void BitmapColoring::checkGreenBoxes()
{
    if (ui->cb2TraceGreen->isChecked())
    {
        ui->cb2ThinGreen->show();
        ui->cb2FinishGreen->show();
        ui->cb2ThinGreen->setChecked(ui->cb2TraceGreen->isChecked());
        ui->cb2FinishGreen->setChecked(ui->cb2TraceGreen->isChecked());
    }
    else
    {
        ui->cb2ThinGreen->hide();
        ui->cb2FinishGreen->hide();
    }
}

void BitmapColoring::checkBlueBoxes()
{
    if (ui->cb2TraceBlue->isChecked())
    {
        ui->cb2ThinBlue->show();
        ui->cb2FinishBlue->show();
        ui->cb2ThinBlue->setChecked(ui->cb2TraceBlue->isChecked());
        ui->cb2FinishBlue->setChecked(ui->cb2TraceBlue->isChecked());
    }
    else
    {
        ui->cb2ThinBlue->hide();
        ui->cb2FinishBlue->hide();
    }
}

void BitmapColoring::checkAllKeyframes()
{
    ui->cb3ThinAllKeyframes->setChecked(ui->cb3TraceAllKeyframes->isChecked());
    ui->cb3FinishAllKeyframes->setChecked(ui->cb3TraceAllKeyframes->isChecked());
}

void BitmapColoring::tabClicked(int index)
{
    switch (index)
    {
    case 0:
        updateTraceBoxes();
        break;
    case 1:
        updateThinBoxes();
        break;
    case 2:
        updateFinishBoxes();
        break;
    default:
        updateTraceBoxes();
    }
}

void BitmapColoring::updateTraceBoxes()
{
    if (mLayerBitmap->getIsColorLayer())
    {
        ui->tab1->setEnabled(false);
    }
    else
    {
        ui->tab1->setEnabled(true);
        ui->gb2Prepare->setEnabled(true);
        if (ui->cbMethodSelector->currentIndex() == 0)
            ui->cb2TraceBlack->setEnabled(false);
    }
}

void BitmapColoring::updateBtnSelect()
{
    if (ui->cb1Threshold->isChecked())
    {
        ui->btnSelectAreas->setEnabled(true);
    }
    else
    {
        mSelectAreas = false;
        ui->btnSelectAreas->setIcon(QIcon(":/icons/select.png"));
        ui->btnSelectAreas->setEnabled(false);
    }
}

void BitmapColoring::setThreshold(int threshold)
{
    mBitmapImage->setThreshold(threshold);
}

void BitmapColoring::traceLines()
{
    if (ui->cb3TraceAllKeyframes->isChecked())
    {
        QProgressDialog progress(tr("Tracing lines in bitmaps..."), tr("Abort"), 0, 100, this);
        hideQuestionMark(progress);
        progress.setWindowModality(Qt::WindowModal);
        progress.show();
        int keysToTrace = mLayerBitmap->keyFrameCount();
        progress.setMaximum(keysToTrace);
        int keysTraced = 0;

        for (int i = mLayerBitmap->firstKeyFramePosition(); i <= mLayerBitmap->getMaxKeyFramePosition(); i++)
        {
            if (mLayerBitmap->keyExists(i))
            {
                progress.setValue(keysTraced++);
                QApplication::processEvents();
                mEditor->scrubTo(i);
                if (ui->cb1Threshold->isChecked())
                    traceScansToTransparent();
                prepareLines();
                if (progress.wasCanceled())
                {
                    break;
                }
            }
        }
        progress.close();
    }
    else if (mLayerBitmap->keyExists(mEditor->currentFrame()))
    {
        if (ui->cb1Threshold->isChecked())
            traceScansToTransparent();
        prepareLines();
    }
}

void BitmapColoring::traceScansToTransparent()
{
    if (ui->cb1Threshold->isChecked())
    {
        mEditor->copy();
        mEditor->layers()->currentLayer()->removeKeyFrame(mEditor->currentFrame());
        mEditor->layers()->currentLayer()->addNewKeyFrameAt(mEditor->currentFrame());
        mEditor->paste();
    }
    mBitmapImage = mLayerBitmap->getBitmapImageAtFrame(mEditor->currentFrame());
    mBitmapImage = mBitmapImage->scanToTransparent(mBitmapImage,
                                                   true,
                                                   ui->cb2TraceRed->isChecked(),
                                                   ui->cb2TraceGreen->isChecked(),
                                                   ui->cb2TraceBlue->isChecked());
}

void BitmapColoring::prepareLines()
{
    if (mLayerBitmap == nullptr) { return; }
    if (!mLayerBitmap->keyExists(mEditor->currentFrame())) { return; }

    LayerBitmap* colorLayer = nullptr;
    if (ui->cbMethodSelector->currentIndex() == 0)
    {           // if coloring is on same layer...
        colorLayer = mLayerBitmap;
    }
    else
    {           // if coloring is on separate layer...
        if (!mLayerBitmap->getHasColorLayer())
        {
            colorLayer = mEditor->layers()->createBitmapLayer(mLayerBitmap->name() + "_C");
            mLayerBitmap->setHasColorLayer(true);
            colorLayer->setIsColorLayer(true);
        }
        else {
            colorLayer = static_cast<LayerBitmap*>(mEditor->layers()->findLayerByName(mLayerBitmap->name() + "_C"));
        }
    }

    if (ui->cbMethodSelector->currentIndex() == 1)
    {
        colorLayer->setVisible(false);
        mLayerBitmap->copyFrame(mLayerBitmap, colorLayer, mEditor->currentFrame());
    }
    colorLayer->getBitmapImageAtFrame(mEditor->currentFrame())->traceLine(colorLayer->getBitmapImageAtFrame(mEditor->currentFrame()),
                                                                          ui->cb2TraceBlack->isChecked(),
                                                                          ui->cb2TraceRed->isChecked(),
                                                                          ui->cb2TraceGreen->isChecked(),
                                                                          ui->cb2TraceBlue->isChecked());
}

void BitmapColoring::updateThinBoxes()
{
    if (mLayerBitmap->getHasColorLayer())
    {
        ui->tab2->setEnabled(false);
    }
    else
    {
        ui->tab2->setEnabled(true);
        ui->gb2Thin->setEnabled(true);
        if (ui->cbMethodSelector->currentIndex() == 0)
            ui->cb2ThinBlack->setEnabled(false);
    }
}

void BitmapColoring::setSpotArea(int size)
{
    mBitmapImage->setSpotArea(size);
}

void BitmapColoring::thinLines()
{
    if (mLayerBitmap == nullptr) { return; }
    mBitmapImage = mLayerBitmap->getBitmapImageAtFrame(mEditor->layers()->currentLayer()->getMaxKeyFramePosition());

    int firstFrame = 1, lastFrame = 1;
    if (ui->cb3ThinAllKeyframes->isChecked())
    {
        lastFrame = mLayerBitmap->getMaxKeyFramePosition();
    }
    else
    {
        firstFrame = mEditor->currentFrame();
        lastFrame = firstFrame;
    }

    QProgressDialog progress(tr("Thinning lines in bitmaps..."), tr("Abort"), 0, 100, this);
    hideQuestionMark(progress);
    progress.setWindowModality(Qt::WindowModal);
    progress.show();
    int keysToThin = mLayerBitmap->keyFrameCount();
    progress.setMaximum(keysToThin);
    int keysThinned = 0;

    for (int i = firstFrame; i <= lastFrame; i++)
    {
        if (mLayerBitmap->keyExists(i))
        {
            progress.setValue(keysThinned++);
            QApplication::processEvents();
            mEditor->scrubTo(i);
            mBitmapImage = mLayerBitmap->getBitmapImageAtFrame(i);
            if (ui->cbSpotAreas->isChecked())
            {
                mBitmapImage->fillSpotAreas(mLayerBitmap->getBitmapImageAtFrame(i));
            }

            mBitmapImage->toThinLine(mBitmapImage,
                                     ui->cb2ThinBlack->isChecked(),
                                     ui->cb2ThinRed->isChecked(),
                                     ui->cb2ThinGreen->isChecked(),
                                     ui->cb2ThinBlue->isChecked());
            mEditor->backup(mEditor->layers()->currentLayerIndex(), mEditor->currentFrame(), tr("Thin lines"));
            if (progress.wasCanceled())
            {
                break;
            }
        }
    }
    updateUI();
}

void BitmapColoring::updateFinishBoxes()
{
    if (mLayerBitmap->getHasColorLayer())
    {
        ui->gb2Finish->setEnabled(false);
    }
    else
    {
        ui->gb2Finish->setEnabled(true);
        if (ui->cbMethodSelector->currentIndex() == 0)
            ui->cb2FinishBlack->setEnabled(false);
    }
}

void BitmapColoring::blendLines()
{
    if (mLayerBitmap == nullptr) { return; }
    mBitmapImage = mLayerBitmap->getBitmapImageAtFrame(mEditor->layers()->currentLayer()->getMaxKeyFramePosition());

    QString orgName = mLayerBitmap->name();
    orgName.chop(2);
    LayerBitmap* artLayer = static_cast<LayerBitmap*>(mEditor->layers()->findLayerByName(orgName));

    int firstFrame = 1, lastFrame = 1;
    if (ui->cb3FinishAllKeyframes->isChecked())
    {
        lastFrame = mLayerBitmap->getMaxKeyFramePosition();
    }
    else
    {
        firstFrame = mEditor->currentFrame();
        lastFrame = firstFrame;
    }

    QProgressDialog progress(tr("Blending lines in bitmaps..."), tr("Abort"), 0, 100, this);
    hideQuestionMark(progress);
    progress.setWindowModality(Qt::WindowModal);
    progress.show();
    int keysToBlend = mLayerBitmap->keyFrameCount();
    progress.setMaximum(keysToBlend);
    int keysBlended = 0;

    for (int i = firstFrame; i <= lastFrame; i++)
    {
        if (mLayerBitmap->keyExists(i))
        {
            progress.setValue(keysBlended++);
            QApplication::processEvents();
            mEditor->scrubTo(i);
            mLayerBitmap->getBitmapImageAtFrame(i)->blendLines(mLayerBitmap->getBitmapImageAtFrame(i),
                                                               ui->cb2FinishBlack->isChecked(),
                                                               ui->cb2FinishRed->isChecked(),
                                                               ui->cb2FinishGreen->isChecked(),
                                                               ui->cb2FinishBlue->isChecked());
            if (ui->cbMethodSelector->currentIndex() == 1 && artLayer != nullptr)
            {
                artLayer->getBitmapImageAtFrame(i)->blendLines(artLayer->getBitmapImageAtFrame(i),
                                                               false, // don't mess with the original
                                                               ui->cb2FinishRed->isChecked(),
                                                               ui->cb2FinishGreen->isChecked(),
                                                               ui->cb2FinishBlue->isChecked());
            }
            mEditor->backup(mEditor->layers()->currentLayerIndex(), mEditor->currentFrame(), tr("Blend lines"));
            if (progress.wasCanceled())
            {
                break;
            }
        }
    }
}

