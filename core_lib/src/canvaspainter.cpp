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

#include "object.h"
#include "layerbitmap.h"
#include "layervector.h"
#include "bitmapimage.h"
#include "tile.h"
#include "tiledbuffer.h"
#include "vectorimage.h"

#include "painterutils.h"

CanvasPainter::CanvasPainter(QPixmap& canvas) : mCanvas(canvas)
{
    reset();
}

CanvasPainter::~CanvasPainter()
{
}

void CanvasPainter::reset()
{
    mPostLayersPixmap = QPixmap(mCanvas.size());
    mPreLayersPixmap = QPixmap(mCanvas.size());
    mCurrentLayerPixmap = QPixmap(mCanvas.size());
    mOnionSkinPixmap = QPixmap(mCanvas.size());
    mPreLayersPixmap.fill(Qt::transparent);
    mCanvas.fill(Qt::transparent);
    mCurrentLayerPixmap.fill(Qt::transparent);
    mPostLayersPixmap.fill(Qt::transparent);
    mOnionSkinPixmap.fill(Qt::transparent);
    mCurrentLayerPixmap.setDevicePixelRatio(mCanvas.devicePixelRatioF());
    mPreLayersPixmap.setDevicePixelRatio(mCanvas.devicePixelRatioF());
    mPostLayersPixmap.setDevicePixelRatio(mCanvas.devicePixelRatioF());
    mOnionSkinPixmap.setDevicePixelRatio(mCanvas.devicePixelRatioF());
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
    if (!mPreLayersPixmapCacheValid)
    {
        QPainter preLayerPainter;
        initializePainter(preLayerPainter, mPreLayersPixmap, blitRect);
        renderPreLayers(preLayerPainter, blitRect);
        preLayerPainter.end();
        mPreLayersPixmapCacheValid = true;
    }

    QPainter mainPainter;
    initializePainter(mainPainter, mCanvas, blitRect);
    mainPainter.setWorldMatrixEnabled(false);
    mainPainter.drawPixmap(mPointZero, mPreLayersPixmap);
    mainPainter.setWorldMatrixEnabled(true);

    paintCurrentFrame(mainPainter, blitRect, mCurrentLayerIndex, mCurrentLayerIndex);

    if (!mPostLayersPixmapCacheValid)
    {
        QPainter postLayerPainter;
        initializePainter(postLayerPainter, mPostLayersPixmap, blitRect);
        renderPostLayers(postLayerPainter, blitRect);
        postLayerPainter.end();
        mPostLayersPixmapCacheValid = true;
    }

    mainPainter.setWorldMatrixEnabled(false);
    mainPainter.drawPixmap(mPointZero, mPostLayersPixmap);
    mainPainter.setWorldMatrixEnabled(true);
}

void CanvasPainter::resetLayerCache()
{
    mPreLayersPixmapCacheValid = false;
    mPostLayersPixmapCacheValid = false;
}

void CanvasPainter::initializePainter(QPainter& painter, QPaintDevice& device, const QRect& blitRect)
{
    painter.begin(&device);

    // Only draw inside the clipped rectangle
    painter.setClipRect(blitRect);

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

void CanvasPainter::renderPostLayers(QPainter& painter, const QRect& blitRect)
{
    if (mOptions.eLayerVisibility != LayerVisibility::CURRENTONLY || mObject->getLayer(mCurrentLayerIndex)->type() == Layer::CAMERA)
    {
        paintCurrentFrame(painter, blitRect, mCurrentLayerIndex + 1, mObject->getLayerCount() - 1);
    }
}

void CanvasPainter::setPaintSettings(const Object* object, int currentLayer, int frame, TiledBuffer* tiledBuffer)
{
    Q_ASSERT(object);
    mObject = object;

    CANVASPAINTER_LOG("Set CurrentLayerIndex = %d", currentLayer);
    mCurrentLayerIndex = currentLayer;
    mFrameNumber = frame;
    mTiledBuffer = tiledBuffer;
}

void CanvasPainter::paint(const QRect& blitRect)
{
    QPainter preLayerPainter;
    QPainter mainPainter;
    QPainter postLayerPainter;

    initializePainter(mainPainter, mCanvas, blitRect);

    initializePainter(preLayerPainter, mPreLayersPixmap, blitRect);
    renderPreLayers(preLayerPainter, blitRect);
    preLayerPainter.end();

    mainPainter.setWorldMatrixEnabled(false);
    mainPainter.drawPixmap(mPointZero, mPreLayersPixmap);
    mainPainter.setWorldMatrixEnabled(true);

    paintCurrentFrame(mainPainter, blitRect, mCurrentLayerIndex, mCurrentLayerIndex);

    initializePainter(postLayerPainter, mPostLayersPixmap, blitRect);
    renderPostLayers(postLayerPainter, blitRect);
    postLayerPainter.end();

    mainPainter.setWorldMatrixEnabled(false);
    mainPainter.drawPixmap(mPointZero, mPostLayersPixmap);
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

    BitmapImage* bitmapImage = bitmapLayer->getBitmapImageAtFrame(nFrame);

    if (bitmapImage == nullptr) { return; }
    bitmapImage->loadFile(); // Critical! force the BitmapImage to load the image

    QPainter onionSkinPainter;
    initializePainter(onionSkinPainter, mOnionSkinPixmap, blitRect);

    onionSkinPainter.drawImage(bitmapImage->topLeft(), *bitmapImage->image());
    paintOnionSkinFrame(painter, onionSkinPainter, nFrame, colorize, bitmapImage->getOpacity());
}

void CanvasPainter::paintVectorOnionSkinFrame(QPainter& painter, const QRect& blitRect, Layer* layer, int nFrame, bool colorize)
{
    LayerVector* vectorLayer = static_cast<LayerVector*>(layer);

    CANVASPAINTER_LOG("Paint Onion skin vector, Frame = %d", nFrame);
    VectorImage* vectorImage = vectorLayer->getVectorImageAtFrame(nFrame);
    if (vectorImage == nullptr) { return; }

    QPainter onionSkinPainter;
    initializePainter(onionSkinPainter, mOnionSkinPixmap, blitRect);

    vectorImage->paintImage(onionSkinPainter, *mObject, mOptions.bOutlines, mOptions.bThinLines, mOptions.bAntiAlias);
    paintOnionSkinFrame(painter, onionSkinPainter, nFrame, colorize, vectorImage->getOpacity());
}

void CanvasPainter::paintOnionSkinFrame(QPainter& painter, QPainter& onionSkinPainter, int nFrame, bool colorize, qreal frameOpacity)
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
    painter.drawPixmap(mPointZero, mOnionSkinPixmap);
}

void CanvasPainter::paintCurrentBitmapFrame(QPainter& painter, const QRect& blitRect, Layer* layer, bool isCurrentLayer)
{
    LayerBitmap* bitmapLayer = static_cast<LayerBitmap*>(layer);
    BitmapImage* paintedImage = bitmapLayer->getLastBitmapImageAtFrame(mFrameNumber);

    if (paintedImage == nullptr) { return; }
    paintedImage->loadFile(); // Critical! force the BitmapImage to load the image

    const bool isDrawing = mTiledBuffer && !mTiledBuffer->bounds().isEmpty();

    QPainter currentBitmapPainter;
    initializePainter(currentBitmapPainter, mCurrentLayerPixmap, blitRect);

    painter.setOpacity(paintedImage->getOpacity() - (1.0-painter.opacity()));
    painter.setWorldMatrixEnabled(false);

    currentBitmapPainter.drawImage(paintedImage->topLeft(), *paintedImage->image());

    if (isCurrentLayer && isDrawing)
    {
        currentBitmapPainter.setCompositionMode(mOptions.cmBufferBlendMode);
        const auto tiles = mTiledBuffer->tiles();
        for (const Tile* tile : tiles) {
            currentBitmapPainter.drawPixmap(tile->posF(), tile->pixmap());
        }
    }

    // We do not wish to draw selection transformations on anything but the current layer
    Q_ASSERT(!isDrawing || mSelectionTransform.isIdentity());
    if (isCurrentLayer && mRenderTransform && !isDrawing) {
        paintTransformedSelection(currentBitmapPainter, paintedImage, mSelection);
    }

    painter.drawPixmap(mPointZero, mCurrentLayerPixmap);
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

    const bool isDrawing = mTiledBuffer->isValid();

    if (mRenderTransform) {
        vectorImage->setSelectionTransformation(mSelectionTransform);
    }

    // Paint existing vector image to the painter
    vectorImage->paintImage(currentVectorPainter, *mObject, mOptions.bOutlines, mOptions.bThinLines, mOptions.bAntiAlias);

    if (isCurrentLayer && isDrawing) {
        currentVectorPainter.setCompositionMode(mOptions.cmBufferBlendMode);

        const auto tiles = mTiledBuffer->tiles();
        for (const Tile* tile : tiles) {
            currentVectorPainter.drawPixmap(tile->posF(), tile->pixmap());
        }
    }

    // Don't transform the image here as we used the viewTransform in the image output
    painter.setWorldMatrixEnabled(false);
    painter.setTransform(QTransform());

    // Remember to adjust opacity based on additional opacity value from the keyframe
    painter.setOpacity(vectorImage->getOpacity() - (1.0-painter.opacity()));
    painter.drawPixmap(mPointZero, mCurrentLayerPixmap);
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

        if (!layer->visible())
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
