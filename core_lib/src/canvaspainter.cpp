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

QPainter* CanvasPainter::initializePainter(QPixmap *pixmap)
{
    QPainter *painter = new QPainter(pixmap);

    painter->setWorldMatrixEnabled(true);
    painter->setWorldTransform(mViewTransform);

    //QRectF mappedInvCanvas = mViewInverse.mapRect(QRectF(mCanvas->rect()));
    //QSizeF croppedPainter = QSizeF(mappedInvCanvas.size());
    //QRectF aligned = QRectF(QPointF(mappedInvCanvas.topLeft()), croppedPainter);

    //painter.setClipRect(aligned); // this aligned rect is valid only for bitmap images.

    return painter;
}

void CanvasPainter::renderPreLayers(QPixmap *pixmap)
{
    QScopedPointer<QPainter> painter(initializePainter(pixmap));
    renderPreLayers(painter.get());
}

void CanvasPainter::renderPreLayers(QPainter *painter)
{
    if (mOptions.nShowAllLayers > 0)
    {
        paintCurrentFrame(*painter, 0, mCurrentLayerIndex-1);
    }

    paintOnionSkin(*painter);
    painter->setOpacity(1.0);
}

void CanvasPainter::renderCurLayer(QPixmap *pixmap)
{
    QScopedPointer<QPainter> painter(initializePainter(pixmap));
    renderCurLayer(painter.get());
}

void CanvasPainter::renderCurLayer(QPainter *painter)
{
    paintCurrentFrame(*painter, mCurrentLayerIndex, mCurrentLayerIndex);
}

void CanvasPainter::renderPostLayers(QPixmap *pixmap)
{
    QScopedPointer<QPainter> painter(initializePainter(pixmap));
    renderPostLayers(painter.get());
}

void CanvasPainter::renderPostLayers(QPainter *painter)
{
    if (mOptions.nShowAllLayers > 0)
    {
        paintCurrentFrame(*painter, mCurrentLayerIndex+1, mObject->getLayerCount()-1);
    }

    paintCameraBorder(*painter);

    // post effects
    if (mOptions.bAxis)
    {
        paintAxis(*painter);
    }
}

void CanvasPainter::setPaintSettings(const Object* object, int currentLayer, int frame, QRect rect, BitmapImage *buffer)
{
    Q_UNUSED(rect);
    Q_ASSERT(object);
    mObject = object;

    mCurrentLayerIndex = currentLayer;
    mFrameNumber = frame;
    mBuffer = buffer;
}

void CanvasPainter::paint()
{
    QScopedPointer<QPainter> painter(initializePainter(mCanvas));

    renderPreLayers(painter.get());
    renderCurLayer(painter.get());
    renderPostLayers(painter.get());
}

void CanvasPainter::paintBackground()
{
    mCanvas->fill(Qt::transparent);
}

void CanvasPainter::paintOnionSkin(QPainter& painter)
{
    if (!mOptions.onionWhilePlayback && mOptions.isPlaying) { return; }

    Layer* layer = mObject->getLayer(mCurrentLayerIndex);

    if (layer->visible() == false)
        return;

    if (layer->keyFrameCount() == 0)
        return;

    qreal minOpacity = static_cast<qreal>(mOptions.fOnionSkinMinOpacity / 100);
    qreal maxOpacity = static_cast<qreal>(mOptions.fOnionSkinMaxOpacity / 100);

    if (mOptions.bPrevOnionSkin && mFrameNumber > 1)
    {
        // Paint onion skin before current frame.
        qreal prevOpacityIncrement = (maxOpacity - minOpacity) / mOptions.nPrevOnionSkinCount;
        qreal opacity = maxOpacity;

        int onionFrameNumber = mFrameNumber;
        if (mOptions.bIsOnionAbsolute)
        {
            onionFrameNumber = layer->getPreviousFrameNumber(onionFrameNumber+1, true);
        }
        onionFrameNumber = layer->getPreviousFrameNumber(onionFrameNumber, mOptions.bIsOnionAbsolute);

        int onionPosition = 0;

        while (onionPosition < mOptions.nPrevOnionSkinCount && onionFrameNumber > 0)
        {
            painter.setOpacity(opacity);

            switch (layer->type())
            {
            case Layer::BITMAP: { paintBitmapFrame(painter, layer, onionFrameNumber, mOptions.bColorizePrevOnion, false, false); break; }
            case Layer::VECTOR: { paintVectorFrame(painter, layer, onionFrameNumber, mOptions.bColorizePrevOnion, false, false); break; }
            default: break;
            }
            opacity = opacity - prevOpacityIncrement;

            onionFrameNumber = layer->getPreviousFrameNumber(onionFrameNumber, mOptions.bIsOnionAbsolute);
            onionPosition++;
        }
    }

    if (mOptions.bNextOnionSkin)
    {
        // Paint onion skin after current frame.
        qreal nextOpacityIncrement = (maxOpacity - minOpacity) / mOptions.nNextOnionSkinCount;
        qreal opacity = maxOpacity;

        int onionFrameNumber = layer->getNextFrameNumber(mFrameNumber, mOptions.bIsOnionAbsolute);
        int onionPosition = 0;

        while (onionPosition < mOptions.nNextOnionSkinCount && onionFrameNumber > 0)
        {
            painter.setOpacity(opacity);

            switch (layer->type())
            {
            case Layer::BITMAP: { paintBitmapFrame(painter, layer, onionFrameNumber, mOptions.bColorizeNextOnion, false, false); break; }
            case Layer::VECTOR: { paintVectorFrame(painter, layer, onionFrameNumber, mOptions.bColorizeNextOnion, false, false); break; }
            default: break;
            }
            opacity = opacity - nextOpacityIncrement;

            onionFrameNumber = layer->getNextFrameNumber(onionFrameNumber, mOptions.bIsOnionAbsolute);
            onionPosition++;
        }
    }
}

void CanvasPainter::paintBitmapFrame(QPainter& painter,
                                     Layer* layer,
                                     int nFrame,
                                     bool colorize,
                                     bool useLastKeyFrame,
                                     bool isCurrentFrame)
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

    if ((paintedImage == nullptr || paintedImage->bounds().isEmpty())
        && !(isCurrentFrame && mBuffer != nullptr && !mBuffer->bounds().isEmpty()))
    {
        return;
    }

    if (paintedImage == nullptr)
    {
        paintedImage = new BitmapImage();
    }

    paintedImage->loadFile(); // Critical! force the BitmapImage to load the image
    //qCDebug(mLog) << "Paint Image Size:" << paintedImage->image()->size();

    BitmapImage paintToImage;
    paintToImage.paste(paintedImage);

    if (isCurrentFrame)
    {
        paintToImage.paste(mBuffer, mOptions.cmBufferBlendMode);
    }

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
        QRectF mappedOrigImage = mViewTransform.mapRect(QRectF(origImage.rect()));
        mScaledBitmap = mScaledBitmap.scaled(mappedOrigImage.size().toSize(),
                                             Qt::KeepAspectRatio, Qt::SmoothTransformation);
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

    if (isCurrentFrame)
    {
        tempBitmapImage.paste(mBuffer, mOptions.cmBufferBlendMode);
    }

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

void CanvasPainter::paintBuffer(BitmapImage *targetImage, BitmapImage *buffer, Layer::LAYER_TYPE layerType)
{
    /*QPainter::CompositionMode prevCompositionMode = targetImage.compositionMode();
    //qDebug() << (prevCompositionMode == QPainter::CompositionMode_SourceOver);
    targetImage.setCompositionMode(mOptions.cmBufferBlendMode);

    buffer->paintImage(targetImage);
    targetImage.setCompositionMode(prevCompositionMode);*/
}

/** Paints layers within the specified range for the current frame.
 *
 *  @param painter The painter to paint to
 *  @param startLayer The first layer to paint (inclusive)
 *  @param endLayer The last layer to paint (inclusive)
 */
void CanvasPainter::paintCurrentFrame(QPainter& painter, int startLayer, int endLayer)
{
    //bool isCamera = mObject->getLayer(mCurrentLayerIndex)->type() == Layer::CAMERA;
    painter.setOpacity(1.0);

    for (int i = startLayer; i <= endLayer; ++i)
    {
        Layer* layer = mObject->getLayer(i);

        if (layer->visible() == false)
            continue;

        switch (layer->type())
        {
        case Layer::BITMAP: { paintBitmapFrame(painter, layer, mFrameNumber, false, true, i == mCurrentLayerIndex); break; }
        case Layer::VECTOR: { paintVectorFrame(painter, layer, mFrameNumber, false, true, i == mCurrentLayerIndex); break; }
        default: break;
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

    if (isCameraMode)
    {
        painter.setWorldMatrixEnabled(false);
        QTransform center = QTransform::fromTranslate(viewRect.width() / 2.0, viewRect.height() / 2.0);
        boundingRect = viewRect.toAlignedRect();
        mCameraRect = center.mapRect(mCameraRect);
    }
    else
    {
        painter.setWorldMatrixEnabled(true);
        QTransform viewInverse = mViewTransform.inverted();
        boundingRect = viewInverse.mapRect(viewRect).toAlignedRect();

        QTransform camTransform = cameraLayer->getViewAtFrame(mFrameNumber);
        mCameraRect = camTransform.inverted().mapRect(mCameraRect);

    }

    painter.setOpacity(1.0);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(0, 0, 0, 80));

    QRegion rg1(boundingRect);
    QRegion rg2(mCameraRect);
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
