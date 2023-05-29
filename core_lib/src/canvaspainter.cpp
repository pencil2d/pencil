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
#include "layercamera.h"
#include "tiledbuffer.h"
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
    qDebug() << canvas->size();
    if (mCanvas == nullptr || mCanvasSize != canvas->size()) {

        mCanvas = canvas;
        mPostLayersPixmap = QPixmap(mCanvas->size());
        mPreLayersPixmap = QPixmap(mCanvas->size());
        mPreLayersPixmap.fill(Qt::transparent);
        mCanvas->fill(Qt::transparent);
        mCurrentLayerPixmap = QPixmap(mCanvas->size());
        mCurrentLayerPixmap.fill(Qt::transparent);
        mPostLayersPixmap.fill(Qt::transparent);
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

    renderCurLayer(mainPainter, blitRect);

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

    paintOnionSkin(painter);
    painter.setOpacity(1.0);
}


void CanvasPainter::renderCurLayer(QPainter& painter, const QRect& blitRect)
{
    paintCurrentFrame(painter, blitRect, mCurrentLayerIndex, mCurrentLayerIndex);
}

void CanvasPainter::renderPostLayers(QPainter& painter, const QRect& blitRect)
{
    if (mOptions.eLayerVisibility != LayerVisibility::CURRENTONLY || mObject->getLayer(mCurrentLayerIndex)->type() == Layer::CAMERA)
    {
        paintCurrentFrame(painter, blitRect, mCurrentLayerIndex + 1, mObject->getLayerCount() - 1);
    }
}

void CanvasPainter::setPaintSettings(const Object* object, int currentLayer, int frame, QRect rect, BitmapImage* buffer, TiledBuffer* tiledBuffer)
{
    Q_UNUSED(rect)
    Q_ASSERT(object);
    mObject = object;

    CANVASPAINTER_LOG("Set CurrentLayerIndex = %d", currentLayer);
    mCurrentLayerIndex = currentLayer;
    mFrameNumber = frame;
    mBuffer = buffer;
    mTiledBuffer = tiledBuffer;
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

    renderCurLayer(mainPainter, blitRect);

    initializePainter(postLayerPainter, mPostLayersPixmap, blitRect);
    renderPostLayers(postLayerPainter, blitRect);
    postLayerPainter.end();

    mainPainter.setWorldMatrixEnabled(false);
    mainPainter.drawPixmap(blitRect, mPostLayersPixmap, blitRect);
    mainPainter.setWorldMatrixEnabled(true);

    mPreLayersPixmapCacheValid = true;
    mPostLayersPixmapCacheValid = true;
}

void CanvasPainter::paintOnionSkin(QPainter& painter)
{
    Layer* layer = mObject->getLayer(mCurrentLayerIndex);

//    mOnionSkinSubPainter.paint(painter, layer, mOnionSkinPainterOptions, mFrameNumber, [&] (OnionSkinPaintState state, int onionFrameNumber) {
//        if (state == OnionSkinPaintState::PREV) {
//            switch (layer->type())
//            {
//            case Layer::BITMAP: { paintBitmapFrame(painter, layer, onionFrameNumber, mOnionSkinPainterOptions.colorizePrevFrames, false, false); break; }
//            case Layer::VECTOR: { paintVectorFrame(painter, layer, onionFrameNumber, mOnionSkinPainterOptions.colorizePrevFrames, false, false); break; }
//            default: break;
//            }
//        }
//        if (state == OnionSkinPaintState::NEXT) {
//            switch (layer->type())
//            {
//            case Layer::BITMAP: { paintBitmapFrame(painter, layer, onionFrameNumber, mOnionSkinPainterOptions.colorizeNextFrames, false, false); break; }
//            case Layer::VECTOR: { paintVectorFrame(painter, layer, onionFrameNumber, mOnionSkinPainterOptions.colorizeNextFrames, false, false); break; }
//            default: break;
//            }
//        }
//    });
}

void CanvasPainter::paintBitmapFrame(QPainter& painter,
                                     const QRect& blitRect,
                                     Layer* layer,
                                     int nFrame,
                                     bool colorize,
                                     bool useLastKeyFrame,
                                     bool isCurrentFrame)
{
#ifdef _DEBUG
    LayerBitmap* bitmapLayer = dynamic_cast<LayerBitmap*>(layer);
    Q_ASSERT(bitmapLayer);
#else
    LayerBitmap* bitmapLayer = static_cast<LayerBitmap*>(layer);
#endif

    CANVASPAINTER_LOG("    Paint Bitmap Frame = %d, UseLastKeyFrame = %d", nFrame, useLastKeyFrame);
    BitmapImage* paintedImage = nullptr;
    if (useLastKeyFrame)
    {
        paintedImage = bitmapLayer->getLastBitmapImageAtFrame(nFrame, 0);
        CANVASPAINTER_LOG("      Actual frame = %d", paintedImage->pos());
    }
    else
    {
        paintedImage = bitmapLayer->getBitmapImageAtFrame(nFrame);
    }

    if (paintedImage == nullptr) { return; }
    paintedImage->loadFile(); // Critical! force the BitmapImage to load the image
    CANVASPAINTER_LOG("        Paint Image Size: %dx%d", paintedImage->image()->width(), paintedImage->image()->height());

    const bool frameIsEmpty = (paintedImage == nullptr || paintedImage->bounds().isEmpty());
    const bool isDrawing = isCurrentFrame && mBuffer && !mBuffer->bounds().isEmpty();
//    if (frameIsEmpty && !isDrawing)
//    {
//        CANVASPAINTER_LOG("        Early return frame %d, %d", frameIsEmpty, isDrawing);
//        return;
//    }

//    BitmapImage paintToImage = BitmapImage(paintedImage->bounds(), Qt::transparent);
//    paintToImage.paste(paintedImage);

    QPainter currentBitmapPainter;
    initializePainter(currentBitmapPainter, mCurrentLayerPixmap, blitRect);

    painter.setOpacity(paintedImage->getOpacity() - (1.0-painter.opacity()));
    painter.setWorldMatrixEnabled(false);

    if (isCurrentFrame)
    {

        currentBitmapPainter.setWorldMatrixEnabled(true);
        currentBitmapPainter.drawImage(paintedImage->topLeft(), *paintedImage->image());

//        // paint the buffer which hasn't been applied to a bitmapImage yet
//        currentBitmapPainter.setCompositionMode(mOptions.cmBufferBlendMode);
//        currentBitmapPainter.drawImage(mBuffer->topLeft(), *mBuffer->image());

        const auto tiles = mTiledBuffer->tiles();
        for (const Tile* tile : tiles) {
//            QPixmap pixTest = QPixmap(64,64);
//            pixTest.fill(Qt::green);
            currentBitmapPainter.drawPixmap(tile->pos(), tile->pixmap());
//            currentBitmapPainter.drawPixmap(tile->pos(), pixTest);
        }

        painter.drawPixmap(blitRect, mCurrentLayerPixmap, blitRect);
    }

//    if (colorize)
//    {
//        QBrush colorBrush = QBrush(Qt::transparent); //no color for the current frame

//        if (nFrame < mFrameNumber)
//        {
//            colorBrush = QBrush(Qt::red);
//        }
//        else if (nFrame > mFrameNumber)
//        {
//            colorBrush = QBrush(Qt::blue);
//        }

//        painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
//        painter.drawRect(paintedImage->bounds());
//    }

    // If the current frame on the current layer has a transformation, we apply it.
//    bool shouldPaintTransform = mRenderTransform && nFrame == mFrameNumber && layer == mObject->getLayer(mCurrentLayerIndex);
//    if (shouldPaintTransform)
//    {
//        paintToImage.clear(mSelection);
//    }

//    painter.setWorldMatrixEnabled(true);
//    prescale(&paintToImage);
//    paintToImage.paintImage(painter, mScaledBitmap, mScaledBitmap.rect(), paintToImage.bounds());

//    if (shouldPaintTransform)
//    {
//        paintTransformedSelection(painter);
//    }
//    static int cc = 0;
//    QString path = QString("C:/Temp/pencil2d/canvas-%1-%2-%3.png")
//        .arg(cc++, 3, 10, QChar('0'))
//        .arg(layer->name())
//        .arg(mFrameNumber);
//    Q_ASSERT(mCanvas->save(path));

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

void CanvasPainter::paintVectorFrame(QPainter& painter,
                                     Layer* layer,
                                     int nFrame,
                                     bool colorize,
                                     bool useLastKeyFrame,
                                     bool isCurrentFrame)
{
#ifdef _DEBUG
    LayerVector* vectorLayer = dynamic_cast<LayerVector*>(layer);
    Q_ASSERT(vectorLayer);
#else
    LayerVector* vectorLayer = static_cast<LayerVector*>(layer);
#endif

    CANVASPAINTER_LOG("Paint Onion skin vector, Frame = %d", nFrame);
    VectorImage* vectorImage = nullptr;
    if (useLastKeyFrame)
    {
        vectorImage = vectorLayer->getLastVectorImageAtFrame(nFrame, 0);
    }
    else
    {
        vectorImage = vectorLayer->getVectorImageAtFrame(nFrame);
    }
    if (vectorImage == nullptr)
    {
        return;
    }

    QImage* strokeImage = new QImage(mCanvas->size(), QImage::Format_ARGB32_Premultiplied);

    if (mRenderTransform) {
        vectorImage->setSelectionTransformation(mSelectionTransform);
    }

    vectorImage->outputImage(strokeImage, mViewTransform, mOptions.bOutlines, mOptions.bThinLines, mOptions.bAntiAlias);

    // Go through a Bitmap image to paint the onion skin colour
    BitmapImage rasterizedVectorImage;
    rasterizedVectorImage.setImage(strokeImage);

    if (colorize)
    {
        QBrush colorBrush = QBrush(Qt::transparent); //no color for the current frame

        if (nFrame < mFrameNumber)
        {
            colorBrush = QBrush(Qt::red);
        }
        else if (nFrame > mFrameNumber)
        {
            colorBrush = QBrush(Qt::blue);
        }
        rasterizedVectorImage.drawRect(strokeImage->rect(),
                                 Qt::NoPen, colorBrush,
                                 QPainter::CompositionMode_SourceIn, false);
    }

    // Don't transform the image here as we used the viewTransform in the image output
    painter.setWorldMatrixEnabled(false);

    painter.setOpacity(vectorImage->getOpacity() - (1.0-painter.opacity()));
    if (isCurrentFrame)
    {
        // Paste buffer onto image to see stroke in realtime
        rasterizedVectorImage.paste(mBuffer, mOptions.cmBufferBlendMode);
    }

    // Paint buffer pasted on top of vector image:
    // fixes polyline not being rendered properly
    rasterizedVectorImage.paintImage(painter);
}

void CanvasPainter::paintTransformedSelection(QPainter& painter) const
{
    // Make sure there is something selected
    if (mSelection.width() == 0 || mSelection.height() == 0)
        return;

    Layer* layer = mObject->getLayer(mCurrentLayerIndex);

    if (layer->type() == Layer::BITMAP)
    {
        // Get the transformed image
        BitmapImage* bitmapImage = static_cast<LayerBitmap*>(layer)->getLastBitmapImageAtFrame(mFrameNumber, 0);
        if (bitmapImage == nullptr) { return; };
        BitmapImage transformedImage = bitmapImage->transformed(mSelection, mSelectionTransform, mOptions.bAntiAlias);

        // Paint the transformation output
        painter.setWorldMatrixEnabled(true);
        transformedImage.paintImage(painter);
    }
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

        CANVASPAINTER_LOG("  Render Layer[%d] %s", i, layer->name());
        switch (layer->type())
        {
        case Layer::BITMAP: { paintBitmapFrame(painter, blitRect, layer, mFrameNumber, false, true, i == mCurrentLayerIndex); break; }
        case Layer::VECTOR: { paintVectorFrame(painter, layer, mFrameNumber, false, true, i == mCurrentLayerIndex); break; }
        default: break;
        }
    }
}
