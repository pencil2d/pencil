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

#include "canvaspainter.h"

#include "object.h"
#include "layerbitmap.h"
#include "layervector.h"
#include "bitmapimage.h"
#include "layercamera.h"
#include "vectorimage.h"
#include "util.h"



CanvasPainter::CanvasPainter(QObject* parent) : QObject(parent)
, mLog("CanvasRenderer")
{
    ENABLE_DEBUG_LOG(mLog, false);
}

CanvasPainter::~CanvasPainter()
{
}

void CanvasPainter::setCanvas(QPixmap* canvas)
{
    Q_ASSERT(canvas);
    mCanvas = canvas;
}

void CanvasPainter::setViewTransform(const QTransform view, const QTransform viewInverse)
{
    mViewTransform = view;
    mViewInverse = viewInverse;
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

void CanvasPainter::paint(const Object* object, int layer, int frame, QRect rect)
{
    Q_ASSERT(object);
    mObject = object;

    mCurrentLayerIndex = layer;
    mFrameNumber = frame;

    //QRectF mappedInvCanvas = mViewInverse.mapRect(QRectF(mCanvas->rect()));
    //QSizeF croppedPainter = QSizeF(mappedInvCanvas.size());
    //QRectF aligned = QRectF(QPointF(mappedInvCanvas.topLeft()), croppedPainter);
    QPainter painter(mCanvas);

    painter.setWorldMatrixEnabled(true);
    painter.setWorldTransform(mViewTransform);

    Q_UNUSED(rect);

    paintBackground();

    //painter.setClipRect(aligned); // this aligned rect is valid only for bitmap images.
    paintCurrentFrame(painter);
    paintCameraBorder(painter);

    // post effects
    if (mOptions.bAxis)
    {
        paintAxis(painter);
    }
}

void CanvasPainter::paintBackground()
{
    mCanvas->fill(Qt::transparent);
}

void CanvasPainter::paintOnionSkin(QPainter& painter)
{
    if (!mOptions.onionWhilePlayback && mOptions.isPlaying) return;
    Layer* layer = mObject->getLayer(mCurrentLayerIndex);
    if (layer->visible() == false || layer->keyFrameCount() < 2) return;

    qreal minOpacity = static_cast<qreal>(mOptions.fOnionSkinMinOpacity / 100);
    qreal maxOpacity = static_cast<qreal>(mOptions.fOnionSkinMaxOpacity / 100);
    int currentPosition = mFrameNumber;
    QList<int> onionPrevPositions;
    QList<int> onionNextPositions;

    if (mOptions.bPrevOnionSkin)
    {
        int relativePosition = currentPosition;
        for (int i = 0; i < mOptions.nPrevOnionSkinCount; ++i) {
               int p = layer->getPreviousFrameNumber(relativePosition, mOptions.bIsOnionAbsolute);
               if (p <= 0) break;
               onionPrevPositions.push_back(p);
               relativePosition = p;
        }
    }

    if (mOptions.bNextOnionSkin)
    {
        int relativePosition = currentPosition;
        for (int i = 0; i < mOptions.nNextOnionSkinCount; ++i) {
               int p = layer->getNextFrameNumber(relativePosition, mOptions.bIsOnionAbsolute);
               if (p == -1 || p > layer->getMaxKeyFramePosition()) break;
               onionNextPositions.push_back(p);
               relativePosition = p;
        }
    }

    if (mOptions.isLoopActive)
    {
        if (mOptions.bPrevOnionSkin)
        {
            QList<int> onionPrevPositionsLoop = onionPrevPositions;
            int prevDiff = mOptions.nPrevOnionSkinCount - onionPrevPositions.count();
            int relativePosition = layer->getMaxKeyFramePosition() + 1;
            for (int i = 0; i < prevDiff; i++) {
                int p = layer->getPreviousFrameNumber(relativePosition, mOptions.bIsOnionAbsolute);
                if (p == -1 || onionPrevPositions.contains(p) || onionNextPositions.contains(p)) break;
                onionPrevPositionsLoop.push_back(p);
                relativePosition = p;
            }
            onionPrevPositionsLoop.removeAll(currentPosition);
            onionPrevPositions = onionPrevPositionsLoop;
        }

        if (mOptions.bNextOnionSkin)
        {
            QList<int> onionNextPositionsLoop = onionNextPositions;
            int nextDiff = mOptions.nNextOnionSkinCount - onionNextPositions.count();
            int relativePosition = 0;
            for (int i = 0; i < nextDiff; i++) {
                int p = layer->getNextFrameNumber(relativePosition, mOptions.bIsOnionAbsolute);
                if (p == -1 || onionPrevPositions.contains(p) || onionNextPositions.contains(p)) break;
                onionNextPositionsLoop.push_back(p);
                relativePosition = p;
            }
            onionNextPositionsLoop.removeAll(currentPosition);
            onionNextPositions = onionNextPositionsLoop;
        }
    }

    qDebug() << "================";
    qDebug() << "previous : " << onionPrevPositions;
    qDebug() << "next :     " << onionNextPositions;

    if (mOptions.bPrevOnionSkin)
    {
        qreal prevOpacityIncrement = (maxOpacity - minOpacity) / mOptions.nPrevOnionSkinCount;
        qreal opacity = maxOpacity;

        for( int i=0; i<onionPrevPositions.count(); ++i )
        {
            painter.setOpacity(opacity);

            switch (layer->type())
            {
            case Layer::BITMAP: { paintBitmapFrame(painter, layer, onionPrevPositions[i], mOptions.bColorizePrevOnion, false, FramePosition::PREVIOUS); break; }
            case Layer::VECTOR: { paintVectorFrame(painter, layer, onionPrevPositions[i], mOptions.bColorizePrevOnion, false); break; }
            default: break;
            }

            opacity = opacity - prevOpacityIncrement;
        }
    }

    if (mOptions.bNextOnionSkin)
    {
        qreal nextOpacityIncrement = (maxOpacity - minOpacity) / mOptions.nNextOnionSkinCount;
        qreal opacity = maxOpacity;

        for( int i=0; i<onionNextPositions.count(); ++i )
        {
            painter.setOpacity(opacity);

            switch (layer->type())
            {
            case Layer::BITMAP: { paintBitmapFrame(painter, layer, onionNextPositions[i], mOptions.bColorizeNextOnion, false, FramePosition::NEXT); break; }
            case Layer::VECTOR: { paintVectorFrame(painter, layer, onionNextPositions[i], mOptions.bColorizeNextOnion, false); break; }
            default: break;
            }

            opacity = opacity - nextOpacityIncrement;
        }
    }
}

void CanvasPainter::paintBitmapFrame(QPainter& painter,
                                     Layer* layer,
                                     int nFrame,
                                     bool colorize,
                                     bool useLastKeyFrame,
                                     int framePosition = FramePosition::CURRENT)
{
#ifdef _DEBUG
    LayerBitmap* bitmapLayer = dynamic_cast<LayerBitmap*>(layer);
    if (bitmapLayer == nullptr)
    {
        Q_ASSERT(bitmapLayer);
        return;
    }
#else
    LayerBitmap* bitmapLayer = static_cast<LayerBitmap*>(layer);
#endif

    //qCDebug(mLog) << "Paint Onion skin bitmap, Frame = " << nFrame;
    BitmapImage* paintedImage = nullptr;
    if (useLastKeyFrame)
    {
        paintedImage = bitmapLayer->getLastBitmapImageAtFrame(nFrame, 0);
    }
    else
    {
        paintedImage = bitmapLayer->getBitmapImageAtFrame(nFrame);
    }

    if (paintedImage == nullptr || paintedImage->bounds().isEmpty())
    {
        return;
    }

    paintedImage->loadFile(); // Critical! force the BitmapImage to load the image
    //qCDebug(mLog) << "Paint Image Size:" << paintedImage->image()->size();

    BitmapImage paintToImage;
    paintToImage.paste(paintedImage);

    if (colorize)
    {
        QBrush colorBrush = QBrush(Qt::transparent); //no color for the current frame

        if (framePosition == FramePosition::PREVIOUS)
        {
            colorBrush = QBrush(Qt::red);
        }
        else if (framePosition == FramePosition::NEXT)
        {
            colorBrush = QBrush(Qt::blue);
        }

        paintToImage.drawRect(paintedImage->bounds(),
                              Qt::NoPen,
                              colorBrush,
                              QPainter::CompositionMode_SourceIn,
                              false);
    }

    // If the current frame on the current layer has a transformation, we apply it.
    if (mRenderTransform && nFrame == mFrameNumber && layer == mObject->getLayer(mCurrentLayerIndex))
    {
        paintToImage.clear(mSelection);
        paintTransformedSelection(painter);
    }

    painter.setWorldMatrixEnabled(true);

    prescale(&paintToImage);
    paintToImage.paintImage(painter, mScaledBitmap, mScaledBitmap.rect(), paintToImage.bounds());
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
                                     bool useLastKeyFrame)
{
#ifdef _DEBUG
    LayerVector* vectorLayer = dynamic_cast<LayerVector*>(layer);
    if (vectorLayer == nullptr)
    {
        Q_ASSERT(vectorLayer);
        return;
    }
#else
    LayerVector* vectorLayer = static_cast<LayerVector*>(layer);
#endif

    qCDebug(mLog) << "Paint Onion skin vector, Frame = " << nFrame;
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

    QImage* pImage = new QImage(mCanvas->size(), QImage::Format_ARGB32_Premultiplied);
    vectorImage->outputImage(pImage, mViewTransform, mOptions.bOutlines, mOptions.bThinLines, mOptions.bAntiAlias);

    //painter.drawImage( QPoint( 0, 0 ), *pImage );
    // Go through a Bitmap image to paint the onion skin colour
    BitmapImage tempBitmapImage;
    tempBitmapImage.setImage(pImage);

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
        tempBitmapImage.drawRect(pImage->rect(),
                                 Qt::NoPen, colorBrush,
                                 QPainter::CompositionMode_SourceIn, false);
    }

    painter.setWorldMatrixEnabled(false); // Don't transform the image here as we used the viewTransform in the image output
    tempBitmapImage.paintImage(painter);
}

void CanvasPainter::paintTransformedSelection(QPainter& painter)
{
    // Make sure there is something selected
    if (mSelection.width() == 0 || mSelection.height() == 0)
        return;

    Layer* layer = mObject->getLayer(mCurrentLayerIndex);

    if (layer->type() == Layer::BITMAP)
    {
        // Get the transformed image
        BitmapImage* bitmapImage = dynamic_cast<LayerBitmap*>(layer)->getLastBitmapImageAtFrame(mFrameNumber, 0);
        BitmapImage transformedImage = bitmapImage->transformed(mSelection, mSelectionTransform, mOptions.bAntiAlias);

        // Paint the transformation output
        painter.setWorldMatrixEnabled(true);
        transformedImage.paintImage(painter);
    }
}

void CanvasPainter::paintCurrentFrame(QPainter& painter)
{
    //bool isCamera = mObject->getLayer(mCurrentLayerIndex)->type() == Layer::CAMERA;
    painter.setOpacity(1.0);

    for (int i = 0; i < mObject->getLayerCount(); ++i)
    {
        Layer* layer = mObject->getLayer(i);

        if (layer->visible() == false)
            continue;

        if (i == mCurrentLayerIndex) {
            paintOnionSkin(painter);
            painter.setOpacity(1.0);
        }

        if (i == mCurrentLayerIndex || mOptions.nShowAllLayers > 0)
        {
            switch (layer->type())
            {
            case Layer::BITMAP: { paintBitmapFrame(painter, layer, mFrameNumber, false, true); break; }
            case Layer::VECTOR: { paintVectorFrame(painter, layer, mFrameNumber, false, true); break; }
            default: break;
            }
        }
    }
}

void CanvasPainter::paintAxis(QPainter& painter)
{
    painter.setPen(Qt::green);
    painter.drawLine(QLineF(0, -500, 0, 500));

    painter.setPen(Qt::red);
    painter.drawLine(QLineF(-500, 0, 500, 0));
}

int round100(double f, int gridSize)
{
    return static_cast<int>(f) / gridSize * gridSize;
}

void CanvasPainter::paintGrid(QPainter& painter)
{
    int gridSizeW = mOptions.nGridSizeW;
    int gridSizeH = mOptions.nGridSizeH;

    QRectF rect = painter.viewport();
    QRectF boundingRect = mViewTransform.inverted().mapRect(rect);

    int left = round100(boundingRect.left(), gridSizeW) - gridSizeW;
    int right = round100(boundingRect.right(), gridSizeW) + gridSizeW;
    int top = round100(boundingRect.top(), gridSizeH) - gridSizeH;
    int bottom = round100(boundingRect.bottom(), gridSizeH) + gridSizeH;

    QPen pen(Qt::lightGray);
    pen.setCosmetic(true);
    painter.setPen(pen);
    painter.setWorldMatrixEnabled(true);
    painter.setBrush(Qt::NoBrush);
    QPainter::RenderHints previous_renderhints = painter.renderHints();
    painter.setRenderHint(QPainter::Antialiasing, false);
    // draw vertical gridlines
    for (int x = left; x < right; x += gridSizeW)
    {
        painter.drawLine(x, top, x, bottom);
    }

    // draw horizontal gridlines
    for (int y = top; y < bottom; y += gridSizeH)
    {
        painter.drawLine(left, y, right, y);
    }
    painter.setRenderHints(previous_renderhints);
}

void CanvasPainter::renderGrid(QPainter& painter)
{
    if (mOptions.bGrid)
    {
        painter.setWorldTransform(mViewTransform);
        paintGrid(painter);
    }
}

void CanvasPainter::paintCameraBorder(QPainter &painter)
{
    LayerCamera* cameraLayer = nullptr;
    bool isCameraMode = false;

    // Find the first visiable camera layers
    for (int i = 0; i < mObject->getLayerCount(); ++i)
    {
        Layer* layer = mObject->getLayer(i);
        if (layer->type() == Layer::CAMERA && layer->visible())
        {
            cameraLayer = static_cast<LayerCamera*>(layer);
            isCameraMode = (i == mCurrentLayerIndex);
            break;
        }
    }

    if (cameraLayer == nullptr) { return; }

    QRectF viewRect = painter.viewport();
    QRect boundingRect;
    mCameraRect = cameraLayer->getViewRect();

    QRegion rg2(mCameraRect);
    if (isCameraMode)
    {
        painter.setWorldMatrixEnabled(false);
        QTransform center = QTransform::fromTranslate(viewRect.width() / 2.0, viewRect.height() / 2.0);
        boundingRect = viewRect.toAlignedRect();
        mCameraRect = center.mapRect(mCameraRect);
        rg2 = center.map(rg2);
    }
    else
    {
        painter.setWorldMatrixEnabled(true);
        QTransform viewInverse = mViewTransform.inverted();
        boundingRect = viewInverse.mapRect(viewRect).toAlignedRect();

        QTransform camTransform = cameraLayer->getViewAtFrame(mFrameNumber);
        mCameraRect = camTransform.inverted().mapRect(mCameraRect);
        rg2 = camTransform.inverted().map(rg2);
    }

    painter.setOpacity(1.0);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(0, 0, 0, 80));

    QRegion rg1(boundingRect);
    QRegion rg3 = rg1.subtracted(rg2);

    painter.setClipRegion(rg3);
    painter.drawRect(boundingRect);

    /*
    painter.setClipping(false);

    QPen pen( Qt::black,
                2,
                Qt::SolidLine,
                Qt::FlatCap,
                Qt::MiterJoin );
    painter.setPen( pen );
    painter.setBrush( Qt::NoBrush );
    painter.drawRect( mCameraRect.adjusted( -1, -1, 1, 1) );
    */
}

QRect CanvasPainter::getCameraRect()
{
    return mCameraRect;
}
