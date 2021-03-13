/*

Pencil - Traditional Animation Software
Copyright (C) 2012-2021 Matthew Chiawen Chang

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
#include "selectionmanager.h"
#include "app_util.h"


BitmapColoring::BitmapColoring(Editor* editor, QWidget *parent) :
    BaseDockWidget(parent)
{
    QWidget* innerWidget = new QWidget;
    setWindowTitle(tr("Anime Coloring"));

    ui = new Ui::BitmapColoringWidget;
    ui->setupUi(innerWidget);
    setWidget(innerWidget);

    mEditor = editor;
    mScribblearea = mEditor->getScribbleArea();
    if (mEditor->layers()->currentLayer()->type() == Layer::BITMAP)
        mLayerBitmap = static_cast<LayerBitmap*>(mEditor->layers()->currentLayer());
    mBitmapImage = mLayerBitmap->getBitmapImageAtFrame(mEditor->currentFrame());
    checkRedBoxes();
    checkGreenBoxes();
    checkBlueBoxes();

    connect(ui->cb2TraceRed, &QCheckBox::stateChanged, this, &BitmapColoring::checkRedBoxes);
    connect(ui->cb2TraceGreen, &QCheckBox::stateChanged, this, &BitmapColoring::checkGreenBoxes);
    connect(ui->cb2TraceBlue, &QCheckBox::stateChanged, this, &BitmapColoring::checkBlueBoxes);
    connect(ui->cb3TraceAllKeyframes, &QCheckBox::stateChanged, this, &BitmapColoring::checkAllKeyframesBoxes);
    connect(ui->btnResetTrace, &QPushButton::clicked, this, &BitmapColoring::resetColoringDock);
    connect(ui->cbMethodSelector, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &BitmapColoring::enableTabs);

    // Prepare
    connect(ui->tabWidget, &QTabWidget::tabBarClicked, this, &BitmapColoring::tabWidgetClicked);
    connect(ui->btnApplyTrace, &QPushButton::clicked, this, &BitmapColoring::traceLines);
    // Thin
    connect(ui->sbSpotAreas, QOverload<int>::of(&QSpinBox::valueChanged), this, &BitmapColoring::setSpotArea);
    connect(ui->cbSpotAreas, &QCheckBox::stateChanged, this, &BitmapColoring::updateFillSpotsButton);
    connect(ui->btnFillAreas, &QPushButton::clicked, this, &BitmapColoring::fillSpotAreas);
    connect(ui->btnApplyThin, &QPushButton::clicked, this, &BitmapColoring::thinLines);
    // Finish
    connect(ui->btnApplyBlend, &QPushButton::clicked, this, &BitmapColoring::blendLines);

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

    if (mEditor->layers()->currentLayer()->type() == Layer::BITMAP &&
            ui->cbMethodSelector->currentIndex() > 0)
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
    {   // If it is not a Bitmap Layer OR method not chosen - disable
        ui->tab1->setEnabled(false);
        ui->tab2->setEnabled(false);
        ui->tab3->setEnabled(false);
    }
    mScribblearea->updateCurrentFrame();
}

void BitmapColoring::visibilityChanged(bool visibility)
{
    if (visibility)
        updateUI();
}

void BitmapColoring::checkRedBoxes()
{
    if (ui->cb2TraceRed->isChecked())
    {
        ui->cb2ThinRed->setChecked(ui->cb2TraceRed->isChecked());
        ui->cb2BlendRed->setChecked(ui->cb2TraceRed->isChecked());
    }
}

void BitmapColoring::checkGreenBoxes()
{
    if (ui->cb2TraceGreen->isChecked())
    {
        ui->cb2ThinGreen->setChecked(ui->cb2TraceGreen->isChecked());
        ui->cb2BlendGreen->setChecked(ui->cb2TraceGreen->isChecked());
    }
}

void BitmapColoring::checkBlueBoxes()
{
    if (ui->cb2TraceBlue->isChecked())
    {
        ui->cb2ThinBlue->setChecked(ui->cb2TraceBlue->isChecked());
        ui->cb2BlendBlue->setChecked(ui->cb2TraceBlue->isChecked());
    }
}

void BitmapColoring::checkAllKeyframesBoxes()
{
    ui->cbThinAllKeyframes->setChecked(ui->cb3TraceAllKeyframes->isChecked());
    ui->cb3BlendAllKeyframes->setChecked(ui->cb3TraceAllKeyframes->isChecked());
}

void BitmapColoring::tabWidgetClicked(int index)
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
        updateBlendBoxes();
        break;
    default:
        updateTraceBoxes();
    }
}

void BitmapColoring::resetColoringDock()
{
    ui->cbMethodSelector->setCurrentIndex(0);
    ui->cbSpotAreas->setChecked(false);
    ui->sbSpotAreas->setValue(6);
    ui->cb2TraceRed->setChecked(false);
    ui->cb2TraceGreen->setChecked(false);
    ui->cb2TraceBlue->setChecked(false);
    ui->cb3TraceAllKeyframes->setChecked(false);
}

void BitmapColoring::enableTabs(int index)
{
    Q_UNUSED(index)
    updateUI();
}

// public Trace funtions
void BitmapColoring::updateTraceBoxes()
{
    if (mLayerBitmap->getIsColorLayer())
    {
        ui->tab1->setEnabled(false);
    }
    else if (ui->cbMethodSelector->currentIndex() > 0)
    {
        ui->tab1->setEnabled(true);
        ui->gb2Trace->setEnabled(true);
    }
}

void BitmapColoring::setThreshold(int threshold)
{
    mBitmapImage->setThreshold(threshold);
}

void BitmapColoring::traceLines()
{
    if (mLayerBitmap == nullptr || mLayerBitmap->type() != Layer::BITMAP) { return; }

    if (ui->cb3TraceAllKeyframes->isChecked())
    {
        mEditor->setIsDoingRepeatColoring(true);
        int count = mEditor->getAutoSaveCounter();
        QProgressDialog mProgress(tr("Tracing lines in bitmaps..."), tr("Abort"), 0, 100, this);
        mProgress.setWindowModality(Qt::WindowModal);
        mProgress.show();
        int keysToTrace = mLayerBitmap->keyFrameCount();
        mProgress.setMaximum(keysToTrace);
        mProgress.setValue(0);
        int keysTraced = 0;
        QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);

        for (int i = mLayerBitmap->firstKeyFramePosition(); i <= mLayerBitmap->getMaxKeyFramePosition(); i++)
        {
            if (mLayerBitmap->keyExists(i))
            {
                mProgress.setValue(keysTraced++);
                mEditor->scrubTo(i);
                trace();
                count++;
                if (mProgress.wasCanceled())
                {
                    break;
                }
            }
        }
        mProgress.close();

        // move colorLayer beneath Animation layer
        while (mColLayer > mAnimLayer)
        {
            mEditor->object()->swapLayers(mColLayer, mColLayer - 1);
            mColLayer--;
        }
        mAnimLayer++;

        mEditor->setIsDoingRepeatColoring(false);
        mEditor->setAutoSaveCounter(count);
    }
    else if (mLayerBitmap->keyExists(mEditor->currentFrame()))
    {
        trace();
    }
    mEditor->deselectAll();
    if (ui->cb3TraceAllKeyframes->isChecked())
    {
        ui->tabWidget->setCurrentIndex(1);
        QMessageBox msgBox;
        msgBox.setText(tr("Ready for thinning lines!"));
        msgBox.exec();
    }
}

void BitmapColoring::updateFillSpotsButton()
{
    if (ui->cbSpotAreas->isChecked())
    {
        ui->btnFillAreas->setEnabled(true);
        ui->btnApplyThin->setEnabled(false);
        ui->labReminder->setText(tr("Fill areas blocks Thin button!"));
    }
    else
    {
        ui->btnFillAreas->setEnabled(false);
        ui->btnApplyThin->setEnabled(true);
        ui->labReminder->setText("");
    }
}

void BitmapColoring::fillSpotAreas()
{
    if (mEditor->layers()->currentLayer()->type() != Layer::BITMAP) { return; }

    mLayerBitmap = static_cast<LayerBitmap*>(mEditor->layers()->currentLayer());

    if (!ui->cbThinAllKeyframes->isChecked() && mLayerBitmap->keyExists(mEditor->currentFrame()))
    {
        mBitmapImage = mLayerBitmap->getBitmapImageAtFrame(mEditor->currentFrame());
        mBitmapImage->setSpotArea(ui->sbSpotAreas->value());
        mBitmapImage->fillSpotAreas(mBitmapImage);
        mBitmapImage->modification();
        mEditor->scrubTo(mEditor->currentFrame());
    }
    else
    {
        mEditor->setIsDoingRepeatColoring(true);
        int count = mEditor->getAutoSaveCounter();
        QProgressDialog mProgress(tr("Fill small areas in bitmaps..."), tr("Abort"), 0, 100, this);
        mProgress.setWindowModality(Qt::WindowModal);
        mProgress.show();
        mProgress.setMaximum(mLayerBitmap->keyFrameCount());
        mProgress.setValue(0);
        int keysThinned = 0;
        QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
        for (int i = mLayerBitmap->firstKeyFramePosition(); i <= mLayerBitmap->getMaxKeyFramePosition(); i++)
        {
            if (mLayerBitmap->keyExists(i))
            {
                mEditor->scrubTo(i);
                mProgress.setValue(keysThinned++);
                mBitmapImage = mLayerBitmap->getBitmapImageAtFrame(mEditor->currentFrame());
                mBitmapImage->setSpotArea(ui->sbSpotAreas->value());
                qDebug() << "Frame: " << i << ", Spot area: " << ui->sbSpotAreas->value();
                mBitmapImage->fillSpotAreas(mBitmapImage);
                mBitmapImage->modification();
                count++;
            }
            if (mProgress.wasCanceled())
            {
                break;
            }
        }
        mProgress.close();
        mEditor->setIsDoingRepeatColoring(false);
        mEditor->setAutoSaveCounter(count);
        ui->cbSpotAreas->setChecked(false);
    }
    updateUI();
}

// public Thin functions
void BitmapColoring::updateThinBoxes()
{
    if (mLayerBitmap->getHasColorLayer())
    {
        ui->tab2->setEnabled(false);
    }
    else if (ui->cbMethodSelector->currentIndex() > 0)
    {
        ui->tab2->setEnabled(true);
        ui->gb2Thin->setEnabled(true);
    }
}

void BitmapColoring::setSpotArea(int size)
{
    mBitmapImage->setSpotArea(size);
}

void BitmapColoring::thinLines()
{
    if (mLayerBitmap == nullptr) { return; }

    if (!ui->cbThinAllKeyframes->isChecked() && mLayerBitmap->keyExists(mEditor->currentFrame()))
    {
        thin();
    }
    else
    {
        mEditor->setIsDoingRepeatColoring(true);
        int count = mEditor->getAutoSaveCounter();
        QProgressDialog mProgress(tr("Thinning lines in bitmaps..."), tr("Abort"), 0, 100, this);
        mProgress.setWindowModality(Qt::WindowModal);
        mProgress.show();
        mProgress.setMaximum(mLayerBitmap->keyFrameCount());
        mProgress.setValue(0);
        int keysThinned = 0;
        QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
        for (int i = mLayerBitmap->firstKeyFramePosition(); i <= mLayerBitmap->getMaxKeyFramePosition(); i++)
        {
            if (mLayerBitmap->keyExists(i))
            {
                mProgress.setValue(keysThinned++);
                mEditor->scrubTo(i);
                thin();
                count++;
            }
            if (mProgress.wasCanceled())
            {
                break;
            }
        }
        mProgress.close();
        mEditor->setIsDoingRepeatColoring(false);
        mEditor->setAutoSaveCounter(count);
    }
    ui->cbSpotAreas->setChecked(false);
    if (ui->cbThinAllKeyframes->isChecked())
    {
        ui->tabWidget->setCurrentIndex(2);
        QMessageBox msgBox;
        msgBox.setText(tr("Ready for coloring!"));
        msgBox.exec();
    }
}

// public Blend functions
void BitmapColoring::updateBlendBoxes()
{
    if (mLayerBitmap->getHasColorLayer())
    {
        ui->tab3->setEnabled(false);
    }
    else if (ui->cbMethodSelector->currentIndex() > 0)
    {
        ui->tab3->setEnabled(true);
    }
}

void BitmapColoring::blendLines()
{
    if (mLayerBitmap == nullptr) { return; }

    QString orgName = mLayerBitmap->name();
    if (ui->cbMethodSelector->currentIndex() == 2)
        orgName.chop(2);
    qDebug() << "artlayer: " << orgName;
    LayerBitmap* artLayer = static_cast<LayerBitmap*>(mEditor->layers()->findLayerByName(orgName));
    if (artLayer == nullptr) { return; }

    if (!ui->cb3BlendAllKeyframes->isChecked() && mLayerBitmap->keyExists(mEditor->currentFrame()))
    {
        blend(artLayer);
    }
    else
    {
        mEditor->setIsDoingRepeatColoring(true);
        int count = mEditor->getAutoSaveCounter();
        QProgressDialog progress(tr("Blending lines in bitmaps..."), tr("Abort"), 0, 100, this);
        hideQuestionMark(progress);
        progress.setWindowModality(Qt::WindowModal);
        progress.show();
        int keysToBlend = mLayerBitmap->keyFrameCount();
        progress.setMaximum(keysToBlend);
        int keysBlended = 0;
        QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);

        for (int i = mLayerBitmap->firstKeyFramePosition(); i <= mLayerBitmap->getMaxKeyFramePosition(); i++)
        {
            if (mLayerBitmap->keyExists(i))
            {
                mEditor->scrubTo(i);
                count++;
                progress.setValue(keysBlended++);
                blend(artLayer);
                if (progress.wasCanceled())
                {
                    break;
                }
            }
        }
        progress.close();
        mEditor->setIsDoingRepeatColoring(false);
        mEditor->setAutoSaveCounter(count);
        ui->tabWidget->setCurrentIndex(0);
        resetColoringDock();
        QMessageBox msgBox;
        msgBox.setText(tr("Coloring finished!\nDialog reset..."));
        msgBox.exec();
    }
}

// protected functions
void BitmapColoring::prepareLines()
{
    // Method selector can be 0, 1 or 2.  0 = No method selected
    if (ui->cbMethodSelector->currentIndex() < 1)
    {
        return;
    }
    LayerBitmap* colorLayer = nullptr;
    bool black;
    ui->cbMethodSelector->currentIndex() == 1 ? black = false: black = true;
    // Method selector 1 = Coloring on same layer
    if (ui->cbMethodSelector->currentIndex() == 1)
    {
        colorLayer = mLayerBitmap;
    }
    // Method selector 2 = Coloring on separate layer
    else
    {
        if (!mLayerBitmap->getHasColorLayer())
        {
            mAnimLayer = mEditor->currentLayerIndex(); // necessary since new layer becomes currentlayer
            colorLayer = mEditor->layers()->createBitmapLayer(mLayerBitmap->name() + "_C");
            mColLayer = mEditor->object()->getLayerCount() - 1;
            mEditor->layers()->setCurrentLayer(mAnimLayer);
            mLayerBitmap->setHasColorLayer(true);
            colorLayer->setIsColorLayer(true);
        }
        else
        {
            colorLayer = static_cast<LayerBitmap*>(mEditor->layers()->findLayerByName(mLayerBitmap->name() + "_C"));
        }
    }
    Q_ASSERT(colorLayer);

    if (ui->cbMethodSelector->currentIndex() == 2)
    {
        mLayerBitmap->copyFrame(mLayerBitmap, colorLayer, mEditor->currentFrame());
    }
    mBitmapImage = colorLayer->getBitmapImageAtFrame(mEditor->currentFrame());
    if (mBitmapImage == nullptr)
    {
        nonValidBitmap(mEditor->currentFrame());
        return;
    }

    mBitmapImage->traceLine(colorLayer->getBitmapImageAtFrame(mEditor->currentFrame()),
                                                                          black,
                                                                          ui->cb2TraceRed->isChecked(),
                                                                          ui->cb2TraceGreen->isChecked(),
                                                                          ui->cb2TraceBlue->isChecked());
}

void BitmapColoring::trace()
{
    if (mSelectAreas)
    {
        mEditor->copy();
        mLayerBitmap->removeKeyFrame(mEditor->currentFrame());
        mLayerBitmap->addNewKeyFrameAt(mEditor->currentFrame());
        mEditor->paste();
    }
    mBitmapImage = mLayerBitmap->getBitmapImageAtFrame(mEditor->currentFrame());
    if (mBitmapImage == nullptr)
    {
        nonValidBitmap(mEditor->currentFrame());
        return;
    }

    prepareLines();
    mEditor->backup("Trace lines");
}

void BitmapColoring::thin()
{
    bool black;
    ui->cbMethodSelector->currentIndex() == 1 ? black = false: black = true;
    mBitmapImage = mLayerBitmap->getBitmapImageAtFrame(mEditor->currentFrame());
    if (mBitmapImage == nullptr)
    {
        nonValidBitmap(mEditor->currentFrame());
        return;
    }

    mBitmapImage->toThinLine(mBitmapImage,
                             black,
                             ui->cb2ThinRed->isChecked(),
                             ui->cb2ThinGreen->isChecked(),
                             ui->cb2ThinBlue->isChecked());
    mEditor->backup("Thin lines");
    updateUI();
}

void BitmapColoring::blend(LayerBitmap *artLayer)
{
    bool black;
    ui->cbMethodSelector->currentIndex() == 1 ? black = false: black = true;
    mBitmapImage = mLayerBitmap->getBitmapImageAtFrame(mEditor->currentFrame());
    if (mBitmapImage == nullptr)
    {
        nonValidBitmap(mEditor->currentFrame());
        return;
    }

    mBitmapImage->blendLines(mLayerBitmap->getBitmapImageAtFrame(mEditor->currentFrame()),
                                                       black,
                                                       ui->cb2BlendRed->isChecked(),
                                                       ui->cb2BlendGreen->isChecked(),
                                                       ui->cb2BlendBlue->isChecked());
    mEditor->backup("Blend lines");
    if (ui->cbMethodSelector->currentIndex() == 2 && artLayer != nullptr)
    {
        mBitmapImage = artLayer->getBitmapImageAtFrame(mEditor->currentFrame());
        if (mBitmapImage == nullptr)
        {
            nonValidBitmap(mEditor->currentFrame());
            return;
        }

        mBitmapImage->eraseRedGreenBlueLines(mBitmapImage);
        mEditor->backup("Blend lines");
    }
    updateUI();
}

void BitmapColoring::nonValidBitmap(int frame)
{
    QMessageBox msgBox;
    msgBox.setText(tr("Frame %1 is not valid!\nAborting frame...").arg(frame));
    msgBox.exec();
}
