/*

Pencil2D - Traditional Animation Software
Copyright (C) 2012-2020 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/

#include "canvaspainter.h"

#include <QtMath>
#include <QSettings>

#include "object.h"
#include "layerbitmap.h"
#include "layervector.h"
#include "bitmapimage.h"
#include "vectorimage.h"

#include "painterutils.h"

CanvasPainter::CanvasPainter()
{
}

CanvasPainter::~CanvasPainter()
{
}

void CanvasPainter::setCanvas(QPixmap* canvas)
{
    Q_ASSERT(canvas);
    if (mCanvas == nullptr || mCanvasSize != canvas->size()) {

        mCanvas = canvas;
        mPostLayersPixmap = QPixmap(mCanvas->size());
        mPreLayersPixmap = QPixmap(mCanvas->size());
        mPreLayersPixmap.fill(Qt::transparent);
        mCanvas->fill(Qt::transparent);
        mCurrentLayerPixmap = QPixmap(mCanvas->size());
        mCurrentLayerPixmap.fill(Qt::transparent);
        mPostLayersPixmap.fill(Qt::transparent);
        mOnionSkinPixmap = QPixmap(mCanvas->size());
        mOnionSkinPixmap.fill(Qt::transparent);
        mCanvasSize = canvas->size();
    }
}

void CanvasPainter::setViewTransform(const QTransform view, const QTransform viewInverse)
{
    if (mViewTransform != view || mViewInverse != viewInverse) {
        mViewTransform = view;
        mViewInverse = viewInverse;
    }
}

void CanvasPainter::setTransformedSelection(QRect selection, QTransform transform)
{
    // Make sure that the selection is not empty
    if (selection.width() > 0 && selection.height() > 0)
    {
        mSelection = selection;
        mSelectionTransform = transform;
        mRenderTransform = true;
    }
    else
    {
        // Otherwise we shouldn't be in transformation mode
        ignoreTransformedSelection();
    }
}

void CanvasPainter::ignoreTransformedSelection()
{
    mRenderTransform = false;
}

void CanvasPainter::paintCached(const QRect& blitRect)
{
    QPainter preLayerPainter;
    QPainter mainPainter;
    QPainter postLayerPainter;

    initializePainter(mainPainter, *mCanvas, blitRect);

    if (!mPreLayersPixmapCacheValid)
    {
        initializePainter(preLayerPainter, mPreLayersPixmap, blitRect);
        renderPreLayers(preLayerPainter, blitRect);
        preLayerPainter.end();
        mPreLayersPixmapCacheValid = true;
    }

    mainPainter.setWorldMatrixEnabled(false);
    mainPainter.drawPixmap(blitRect, mPreLayersPixmap, blitRect);
    mainPainter.setWorldMatrixEnabled(true);

    renderCurrentFrame(mainPainter, blitRect);

    if (!mPostLayersPixmapCacheValid)
    {
        renderPostLayers(postLayerPainter, blitRect);
        mPostLayersPixmapCacheValid = true;
    }

    mainPainter.setWorldMatrixEnabled(false);
    mainPainter.drawPixmap(blitRect, mPostLayersPixmap, blitRect);
    mainPainter.setWorldMatrixEnabled(true);
}
void CanvasPainter::resetLayerCache()
{
    mPreLayersPixmapCacheValid = false;
    mPreLayersPixmapCacheValid = false;
}

void CanvasPainter::initializePainter(QPainter& painter, QPaintDevice& device, const QRect& blitRect)
{
    painter.begin(&device);

    // Clear the area that's about to be painted again, to avoid painting on top of existing pixels
    // causing artifacts.
    painter.setCompositionMode(QPainter::CompositionMode_Clear);
    painter.fillRect(blitRect, Qt::transparent);

    // Surface has been cleared and is ready to be painted on
    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
    painter.setWorldMatrixEnabled(true);
    painter.setWorldTransform(mViewTransform);
}

void CanvasPainter::renderPreLayers(QPainter& painter, const QRect& blitRect)
{
    if (mOptions.eLayerVisibility != LayerVisibility::CURRENTONLY || mObject->getLayer(mCurrentLayerIndex)->type() == Layer::CAMERA)
    {
        paintCurrentFrame(painter, blitRect, 0, mCurrentLayerIndex - 1);
    }

    paintOnionSkin(painter, blitRect);
    painter.setOpacity(1.0);
}

void CanvasPainter::renderCurrentFrame(QPainter& painter, const QRect& blitRect)
{
    int currentLayerIndex = mCurrentLayerIndex;
    Layer* layer = mObject->getLayer(currentLayerIndex);

    painter.setOpacity(1.0);

    bool isCameraLayer = layer->type() == Layer::CAMERA;
    if (layer->visible() == false)
        return;

    if (mOptions.eLayerVisibility == LayerVisibility::RELATED && !isCameraLayer) {
        painter.setOpacity(calculateRelativeOpacityForLayer(currentLayerIndex, currentLayerIndex, mOptions.fLayerVisibilityThreshold));
    }

    switch (layer->type())
    {
    case Layer::BITMAP: { paintCurrentBitmapFrame(painter, blitRect, layer, true); break; }
    case Layer::VECTOR: { paintCurrentVectorFrame(painter, blitRect, layer, true); break; }
    default: break;
    }
}

void CanvasPainter::renderPostLayers(QPainter& painter, const QRect& blitRect)
{
    if (mOptions.eLayerVisibility != LayerVisibility::CURRENTONLY || mObject->getLayer(mCurrentLayerIndex)->type() == Layer::CAMERA)
    {
        paintCurrentFrame(painter, blitRect, mCurrentLayerIndex + 1, mObject->getLayerCount() - 1);
    }
}

void CanvasPainter::setPaintSettings(const Object* object, int currentLayer, int frame, QRect rect, BitmapImage* buffer)
{
    Q_UNUSED(rect)
    Q_ASSERT(object);
    mObject = object;

    CANVASPAINTER_LOG("Set CurrentLayerIndex = %d", currentLayer);
    mCurrentLayerIndex = currentLayer;
    mFrameNumber = frame;
    mBuffer = buffer;
}

void CanvasPainter::paint(const QRect& blitRect)
{
    QPainter preLayerPainter;
    QPainter mainPainter;
    QPainter postLayerPainter;

    initializePainter(mainPainter, *mCanvas, blitRect);

    initializePainter(preLayerPainter, mPreLayersPixmap, blitRect);
    renderPreLayers(preLayerPainter, blitRect);
    preLayerPainter.end();

    mainPainter.setWorldMatrixEnabled(false);
    mainPainter.drawPixmap(blitRect, mPreLayersPixmap, blitRect);
    mainPainter.setWorldMatrixEnabled(true);

    renderCurrentFrame(mainPainter, blitRect);

    initializePainter(postLayerPainter, mPostLayersPixmap, blitRect);
    renderPostLayers(postLayerPainter, blitRect);
    postLayerPainter.end();

    mainPainter.setWorldMatrixEnabled(false);
    mainPainter.drawPixmap(blitRect, mPostLayersPixmap, blitRect);
    mainPainter.setWorldMatrixEnabled(true);

    mPreLayersPixmapCacheValid = true;
    mPostLayersPixmapCacheValid = true;
}

void CanvasPainter::paintOnionSkin(QPainter& painter, const QRect& blitRect)
{
    Layer* layer = mObject->getLayer(mCurrentLayerIndex);

    mOnionSkinSubPainter.paint(painter, layer, mOnionSkinPainterOptions, mFrameNumber, [&] (OnionSkinPaintState state, int onionFrameNumber) {
        if (state == OnionSkinPaintState::PREV) {
            switch (layer->type())
            {
            case Layer::BITMAP: { paintBitmapOnionSkinFrame(painter, blitRect, layer, onionFrameNumber, mOnionSkinPainterOptions.colorizePrevFrames); break; }
            case Layer::VECTOR: { paintVectorOnionSkinFrame(painter, blitRect, layer, onionFrameNumber, mOnionSkinPainterOptions.colorizePrevFrames); break; }
            default: break;
            }
        }
        if (state == OnionSkinPaintState::NEXT) {
            switch (layer->type())
            {
            case Layer::BITMAP: { paintBitmapOnionSkinFrame(painter, blitRect, layer, onionFrameNumber, mOnionSkinPainterOptions.colorizeNextFrames); break; }
            case Layer::VECTOR: { paintVectorOnionSkinFrame(painter, blitRect, layer, onionFrameNumber, mOnionSkinPainterOptions.colorizeNextFrames); break; }
            default: break;
            }
        }
    });
}

void CanvasPainter::paintBitmapOnionSkinFrame(QPainter& painter, const QRect& blitRect, Layer* layer, int nFrame, bool colorize)
{
    LayerBitmap* bitmapLayer = static_cast<LayerBitmap*>(layer);

    BitmapImage* bitmapImage = bitmapLayer->getLastBitmapImageAtFrame(nFrame, 0);

    if (bitmapImage == nullptr) { return; }
    QPainter onionSkinPainter;
    initializePainter(onionSkinPainter, mOnionSkinPixmap, blitRect);

    onionSkinPainter.drawImage(bitmapImage->topLeft(), *bitmapImage->image());
    paintOnionSkinFrame(painter, onionSkinPainter, blitRect, nFrame, colorize, bitmapImage->getOpacity());
}

void CanvasPainter::paintVectorOnionSkinFrame(QPainter& painter, const QRect& blitRect, Layer* layer, int nFrame, bool colorize)
{
    LayerVector* vectorLayer = static_cast<LayerVector*>(layer);

    CANVASPAINTER_LOG("Paint Onion skin vector, Frame = %d", nFrame);
    VectorImage* vectorImage = vectorLayer->getLastVectorImageAtFrame(nFrame, 0);
    if (vectorImage == nullptr)
    {
        return;
    }
    QPainter onionSkinPainter;
    initializePainter(onionSkinPainter, mOnionSkinPixmap, blitRect);

    vectorImage->paintImage(onionSkinPainter, mOptions.bOutlines, mOptions.bThinLines, mOptions.bAntiAlias);
    paintOnionSkinFrame(painter, onionSkinPainter, blitRect, nFrame, colorize, vectorImage->getOpacity());
}

void CanvasPainter::paintOnionSkinFrame(QPainter& painter, QPainter& onionSkinPainter, const QRect& blitRect, int nFrame, bool colorize, qreal frameOpacity)
{
    // Don't transform the image here as we used the viewTransform in the image output
    painter.setWorldMatrixEnabled(false);
    // Remember to adjust overall opacity based on opacity value from image
    painter.setOpacity(frameOpacity - (1.0-painter.opacity()));
    if (colorize)
    {
        QColor colorBrush = Qt::transparent; //no color for the current frame

        if (nFrame < mFrameNumber)
        {
            colorBrush = Qt::red;
        }
        else if (nFrame > mFrameNumber)
        {
            colorBrush = Qt::blue;
        }
        onionSkinPainter.setWorldMatrixEnabled(false);

        onionSkinPainter.setCompositionMode(QPainter::CompositionMode_SourceIn);
        onionSkinPainter.setBrush(colorBrush);
        onionSkinPainter.drawRect(painter.viewport());
    }
    painter.drawPixmap(blitRect, mOnionSkinPixmap, blitRect);
}

void CanvasPainter::paintCurrentBitmapFrame(QPainter& painter, const QRect& blitRect, Layer* layer, bool isCurrentLayer)
{
    LayerBitmap* bitmapLayer = static_cast<LayerBitmap*>(layer);
    BitmapImage* paintedImage = bitmapLayer->getLastBitmapImageAtFrame(mFrameNumber);;

    if (paintedImage == nullptr) { return; }
    paintedImage->loadFile(); // Critical! force the BitmapImage to load the image

    const bool isDrawing = mBuffer && !mBuffer->bounds().isEmpty();

    QPainter currentBitmapPainter;
    initializePainter(currentBitmapPainter, mCurrentLayerPixmap, blitRect);

    painter.setOpacity(paintedImage->getOpacity() - (1.0-painter.opacity()));
    painter.setWorldMatrixEnabled(false);

    currentBitmapPainter.drawImage(paintedImage->topLeft(), *paintedImage->image());

    if (isCurrentLayer) {
        if (isDrawing) {
            // paint the current stroke data which hasn't been applied to a bitmapImage yet
            currentBitmapPainter.setCompositionMode(mOptions.cmBufferBlendMode);
            currentBitmapPainter.drawImage(mBuffer->topLeft(), *mBuffer->image());
        } else if (mRenderTransform) {
            paintTransformedSelection(currentBitmapPainter, paintedImage, mSelection);
        }
    }

    painter.drawPixmap(blitRect, mCurrentLayerPixmap, blitRect);
}

void CanvasPainter::paintCurrentVectorFrame(QPainter& painter, const QRect& blitRect, Layer* layer, bool isCurrentLayer)
{
    LayerVector* vectorLayer = static_cast<LayerVector*>(layer);
    VectorImage* vectorImage = vectorLayer->getLastVectorImageAtFrame(mFrameNumber, 0);
    if (vectorImage == nullptr)
    {
        return;
    }

    QPainter currentVectorPainter;
    initializePainter(currentVectorPainter, mCurrentLayerPixmap, blitRect);

    const bool isDrawing = mBuffer && !mBuffer->bounds().isEmpty();

    // Paint existing vector image to the painter
    vectorImage->paintImage(currentVectorPainter, mOptions.bOutlines, mOptions.bThinLines, mOptions.bAntiAlias);

    if (isCurrentLayer) {
        if (isDrawing) {
            currentVectorPainter.setCompositionMode(mOptions.cmBufferBlendMode);
            currentVectorPainter.setTransform(QTransform());
            currentVectorPainter.drawImage(mBuffer->topLeft(), *mBuffer->image());
        } else if (mRenderTransform) {
            vectorImage->setSelectionTransformation(mSelectionTransform);
        }
    }

    // Don't transform the image here as we used the viewTransform in the image output
    painter.setWorldMatrixEnabled(false);
    painter.setTransform(QTransform());

    // Remember to adjust opacity based on addition opacity value from image
    painter.setOpacity(vectorImage->getOpacity() - (1.0-painter.opacity()));
    painter.drawPixmap(blitRect, mCurrentLayerPixmap, blitRect);
}


void CanvasPainter::prescale(BitmapImage* bitmapImage)
{
    QImage origImage = bitmapImage->image()->copy();

    // copy content of our unmodified qimage
    // to our (not yet) scaled bitmap
    mScaledBitmap = origImage.copy();

    if (mOptions.scaling >= 1.0f)
    {
        // TODO: Qt doesn't handle huge upscaled qimages well...
        // possible solution, myPaintLib canvas renderer splits its canvas up in chunks.
    }
    else
    {
        // map to correct matrix
        QRect mappedOrigImage = mViewTransform.mapRect(bitmapImage->bounds());
        mScaledBitmap = mScaledBitmap.scaled(mappedOrigImage.size(),
                                             Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    }
}

void CanvasPainter::paintTransformedSelection(QPainter& painter, BitmapImage* bitmapImage, const QRect& selection) const
{
    // Make sure there is something selected
    if (selection.width() == 0 && selection.height() == 0)
        return;

    QPixmap transformedPixmap = QPixmap(mSelection.size());
    transformedPixmap.fill(Qt::transparent);

    QPainter imagePainter(&transformedPixmap);
    imagePainter.translate(-selection.topLeft());
    imagePainter.drawImage(bitmapImage->topLeft(), *bitmapImage->image());
    imagePainter.end();

    painter.save();

    painter.setTransform(mViewTransform);

    // Clear the painted area to make it look like the content has been erased
    painter.save();
    painter.setCompositionMode(QPainter::CompositionMode_Clear);
    painter.fillRect(selection, QColor(255,255,255,255));
    painter.restore();

    // Multiply the selection and view matrix to get proper rotation and scale values
    // Now the image origin will be topleft
    painter.setTransform(mSelectionTransform*mViewTransform);

    // Draw the selection image separately and on top
    painter.drawPixmap(selection, transformedPixmap);
    painter.restore();
}

/** Paints layers within the specified range for the current frame.
 *
 *  @param painter The painter to paint to
 *  @param startLayer The first layer to paint (inclusive)
 *  @param endLayer The last layer to paint (inclusive)
 */
void CanvasPainter::paintCurrentFrame(QPainter& painter, const QRect& blitRect, int startLayer, int endLayer)
{
    painter.setOpacity(1.0);

    bool isCameraLayer = mObject->getLayer(mCurrentLayerIndex)->type() == Layer::CAMERA;

    for (int i = startLayer; i <= endLayer; ++i)
    {
        Layer* layer = mObject->getLayer(i);

        if (layer->visible() == false)
            continue;

        if (mOptions.eLayerVisibility == LayerVisibility::RELATED && !isCameraLayer)
        {
            painter.setOpacity(calculateRelativeOpacityForLayer(mCurrentLayerIndex, i, mOptions.fLayerVisibilityThreshold));
        }
        bool isCurrentLayer = mCurrentLayerIndex == i;

        CANVASPAINTER_LOG("  Render Layer[%d] %s", i, layer->name());
        switch (layer->type())
        {
        case Layer::BITMAP: { paintCurrentBitmapFrame(painter, blitRect, layer, isCurrentLayer); break; }
        case Layer::VECTOR: { paintCurrentVectorFrame(painter, blitRect, layer, isCurrentLayer); break; }
        default: break;
        }
    }
}
