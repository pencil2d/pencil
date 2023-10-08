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
#include "app_util.h"


BitmapColoring::BitmapColoring(Editor* editor, QWidget *parent) :
    BaseDockWidget(parent)
{
    QWidget* innerWidget = new QWidget;
    setWindowTitle(tr("Advanced Coloring"));

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
    connect(ui->btnInfo, &QPushButton::clicked, this, &BitmapColoring::infoBox);

    // Prepare
    connect(ui->tabWidget, &QTabWidget::tabBarClicked, this, &BitmapColoring::tabWidgetClicked);
    connect(ui->btnPrepareLines, &QPushButton::clicked, this, &BitmapColoring::prepareAndTraceLines);
    connect(ui->btnApplyTrace, &QPushButton::clicked, this, &BitmapColoring::traceLines);
    // Thin
    connect(ui->sbSpotAreas, QOverload<int>::of(&QSpinBox::valueChanged), this, &BitmapColoring::setSpotArea);
    connect(ui->cbSpotAreas, &QCheckBox::stateChanged, this, &BitmapColoring::updateFillSpotsButton);
    connect(ui->btnFillAreas, &QPushButton::clicked, this, &BitmapColoring::fillSpotAreas);
    connect(ui->btnApplyThin, &QPushButton::clicked, this, &BitmapColoring::thinLines);
    // Finish
    connect(ui->btnApplyBlend, &QPushButton::clicked, this, &BitmapColoring::blendLines);
    updateTraceButtons();

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
    mScribblearea->updateFrame();
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
        mRedChecked = true;
    }
    else
    {
        mRedChecked = false;
    }
    updateTraceButtons();
}

void BitmapColoring::checkGreenBoxes()
{
    if (ui->cb2TraceGreen->isChecked())
    {
        ui->cb2ThinGreen->setChecked(ui->cb2TraceGreen->isChecked());
        ui->cb2BlendGreen->setChecked(ui->cb2TraceGreen->isChecked());
        mGreenChecked = true;
    }
    else
    {
        mGreenChecked = false;
    }
    updateTraceButtons();
}

void BitmapColoring::checkBlueBoxes()
{
    if (ui->cb2TraceBlue->isChecked())
    {
        ui->cb2ThinBlue->setChecked(ui->cb2TraceBlue->isChecked());
        ui->cb2BlendBlue->setChecked(ui->cb2TraceBlue->isChecked());
        mBlueChecked = true;
    }
    else
    {
        mBlueChecked = false;
    }
    updateTraceButtons();
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

void BitmapColoring::infoBox()
{
    QMessageBox::information(this, tr("Basics in Advanced Coloring")
                             ,tr("Advanced coloring is developed for users that use color-separation to achieve a shade-effect.\n"
                                "It is assumed that the animation is done with black/gray pencil,\n"
                                "and you can use red, blue and/or green for the color-separation.\n\n"
                                "In this example, we have a active layer called 'Bird'.\n"
                                "The Advanced coloring creates two new layers, called 'Bird_L' and 'Bird_C'.\n"
                                "'Bird_L' is a copy of the Line-art layer, and 'Bird_C' is the Coloring layer.\n"
                                "STEP 1: Trace:\n"
                                "Make sure that you are on the layer 'Bird'!\n"
                                "Check the relevant boxes for color-separation.\n"
                                "Check the 'all drawings' box, if it is all drawings on layer. It most times is.\n"
                                "Press 'Trace'\n"
                                "STEP 2: Thin:\n"
                                "Make sure that you are on the layer 'Bird_C'!\n"
                                "Here the traced lines are thinned to 1 pixel thickness.\n"
                                "To avoid 'holes' in your thinned line, you can run the 'Fill small areas' first.\n"
                                "Your original layer 'Bird' is un-altered through the process, and should be made hidden.\n"
                                "Press 'Thin'\n"
                                "STEP 3: Colorize:\n"
                                "Choose the Bucket-tool. Reference 'current layer', and method 'Replace'\n"
                                "NB! Uncheck 'Color tolerance' and 'Expand fill'!\n"
                                "STEP 4: Blend:\n"
                                "Press 'Blend and Finish', and the thinned lines will dissappear,\n"
                                "and be replaced with blending af neighboring colors. You're done!\n\n"));
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
        qDebug() << "TRACE one " << mEditor->currentFrame();
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

        ui->tabWidget->setCurrentIndex(2);
        QMessageBox msgBox;
        msgBox.setText(tr("Ready for coloring!"));
        msgBox.exec();
    }

    mEditor->scrubTo(mEditor->currentFrame());
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
    LayerBitmap* artLayer = nullptr;
    orgName.chop(2);
    QString artLayerName = orgName + "_L";
    artLayer = static_cast<LayerBitmap*>(mEditor->layers()->findLayerByName(artLayerName));
    if (!artLayer)
    {
        artLayerName.chop(2);
        artLayer = static_cast<LayerBitmap*>(mEditor->layers()->findLayerByName(artLayerName));
    }
    if (artLayer == nullptr) { return; }

    artLayer->setVisible(false);

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
    mEditor->scrubTo(mEditor->currentFrame());
}

/*
 * If drawings are made in Pencil2D, we need to preserve the originals by:
 * - ...making a copy of the original layer, named 'name'_L (L for Line art)
 * - ...making a new coloring layer          named 'name'_C (C for Coloring)
 * (unless copy and color layer already exists)
*/
void BitmapColoring::prepareAndTraceLines()
{
    bool black;
    ui->cbMethodSelector->currentIndex() == 1 ? black = false: black = true;

    LayerManager* lMgr = mEditor->layers();
    LayerBitmap* sourceLayer = mLayerBitmap;
    QString orgName = mLayerBitmap->name();
    LayerBitmap* artLayer = nullptr;
    LayerBitmap* colorLayer = nullptr;

    if (!lMgr->findLayerByName(orgName + "_L"))
    {
        colorLayer = lMgr->createBitmapLayer(orgName + "_C");
        artLayer = lMgr->createBitmapLayer(orgName + "_L");
    }
    else
    {
        colorLayer = static_cast<LayerBitmap*>(lMgr->findLayerByName(orgName + "_C"));
        artLayer = static_cast<LayerBitmap*>(lMgr->findLayerByName(orgName + "_L"));
    }
    Q_ASSERT(artLayer && colorLayer);

    artLayer->setHasColorLayer(true);
    colorLayer->setIsColorLayer(true);

    if (ui->cb3TraceAllKeyframes->isChecked())
    {
        for (int i = sourceLayer->firstKeyFramePosition(); i <= sourceLayer->getMaxKeyFramePosition(); i++)
        {
            if (sourceLayer->keyExists(i))
            {
                mEditor->scrubTo(i);
                lMgr->setCurrentLayer(sourceLayer);
                sourceLayer->copyFrame(sourceLayer, artLayer, i);
                BitmapImage* image = artLayer->getBitmapImageAtFrame(i);
                image->prepDrawing(image,
                                   mRedChecked,
                                   mGreenChecked,
                                   mBlueChecked);
                artLayer->copyFrame(artLayer, colorLayer, i);
                BitmapImage* colorImage = colorLayer->getBitmapImageAtFrame(i);
                colorImage->traceLine(colorImage,
                                      black,
                                      mRedChecked,
                                      mGreenChecked,
                                      mBlueChecked);
            }
        }
        ui->tabWidget->setCurrentIndex(1);
        QMessageBox msgBox;
        msgBox.setText(tr("Ready for thinning lines!"));
        msgBox.exec();
    }
    else
    {
        int i = mEditor->currentFrame();

        if (sourceLayer->keyExists(i))
        {
            lMgr->setCurrentLayer(sourceLayer);
            sourceLayer->copyFrame(sourceLayer, artLayer, i);
            BitmapImage* image = artLayer->getBitmapImageAtFrame(i);
            image->prepDrawing(image,
                               mRedChecked,
                               mGreenChecked,
                               mBlueChecked);
            artLayer->copyFrame(artLayer, colorLayer, i);
            BitmapImage* colorImage = colorLayer->getBitmapImageAtFrame(i);
            colorImage->traceLine(colorImage,
                                  black,
                                  mRedChecked,
                                  mGreenChecked,
                                  mBlueChecked);
        }
    }
    lMgr->setCurrentLayer(colorLayer);

    mEditor->scrubTo(mEditor->currentFrame());
}

// protected functions
void BitmapColoring::prepareLines()
{
    // Method selector can be 0, 1 or 2.  0 = No method selected
    if (ui->cbMethodSelector->currentIndex() < 1)
    {
        return;
    }

    LayerManager* lMgr = mEditor->layers();
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
        QString orgName = mLayerBitmap->name();
        // is it a copy or is it a prepared scanned drawing?
        if (orgName.endsWith("_L"))
            orgName.chop(2);
        if (!mLayerBitmap->getHasColorLayer())
        {
            mAnimLayer = mEditor->currentLayerIndex(); // necessary since new layer becomes currentlayer
            colorLayer = lMgr->createBitmapLayer(orgName + "_C");
            mColLayer = mEditor->object()->getLayerCount() - 1;
            lMgr->setCurrentLayer(mAnimLayer);
            mLayerBitmap->setHasColorLayer(true);
            colorLayer->setIsColorLayer(true);
        }
        else
        {
            colorLayer = static_cast<LayerBitmap*>(lMgr->findLayerByName(orgName + "_C"));
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

    mBitmapImage->traceLine(mBitmapImage,
                            black,
                            mRedChecked,
                            mGreenChecked,
                            mBlueChecked);
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

void BitmapColoring::updateTraceButtons()
{
    if (ui->cb2TraceRed->isChecked() || ui->cb2TraceGreen->isChecked() || ui->cb2TraceBlue->isChecked())
    {
        ui->btnPrepareLines->setEnabled(true);
        ui->btnApplyTrace->setEnabled(false);
    }
    else
    {
        ui->btnPrepareLines->setEnabled(false);
        ui->btnApplyTrace->setEnabled(true);
    }
}
