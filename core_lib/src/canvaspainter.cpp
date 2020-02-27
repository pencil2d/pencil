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

void CanvasPainter::paintCached()
{
    QPixmap tempPixmap(mCanvas->size());
    tempPixmap.fill(Qt::transparent);
    mCanvas->fill(Qt::transparent);
    QPainter tempPainter;
    QPainter painter;
    initializePainter(tempPainter, tempPixmap);
    initializePainter(painter, *mCanvas);

    if (!mPreLayersCache)
    {
        renderPreLayers(painter);
        mPreLayersCache.reset(new QPixmap(*mCanvas));
    }
    else
    {
        painter.setWorldMatrixEnabled(false);
        painter.drawPixmap(0, 0, *(mPreLayersCache.get()));
        painter.setWorldMatrixEnabled(true);
    }

    renderCurLayer(painter);

    if (!mPostLayersCache)
    {
        renderPostLayers(tempPainter);
        mPostLayersCache.reset(new QPixmap(tempPixmap));
        painter.setWorldMatrixEnabled(false);
        painter.drawPixmap(0, 0, tempPixmap);
        painter.setWorldMatrixEnabled(true);
    }
    else
    {
        painter.setWorldMatrixEnabled(false);
        painter.drawPixmap(0, 0, *(mPostLayersCache.get()));
        painter.setWorldMatrixEnabled(true);
    }
}

void CanvasPainter::resetLayerCache()
{
    mPreLayersCache.reset();
    mPostLayersCache.reset();
}

void CanvasPainter::initializePainter(QPainter& painter, QPixmap& pixmap)
{
    painter.begin(&pixmap);
    painter.setWorldMatrixEnabled(true);
    painter.setWorldTransform(mViewTransform);
}

void CanvasPainter::renderPreLayers(QPixmap *pixmap)
{
    QPainter painter;
    initializePainter(painter, *pixmap);
    renderPreLayers(painter);
}

void CanvasPainter::renderPreLayers(QPainter& painter)
{
    if (mOptions.nShowAllLayers > 0)
    {
        paintCurrentFrame(painter, 0, mCurrentLayerIndex-1);
    }

    paintOnionSkin(painter);
    painter.setOpacity(1.0);
}

void CanvasPainter::renderCurLayer(QPixmap *pixmap)
{
    QPainter painter;
    initializePainter(painter, *pixmap);
    renderCurLayer(painter);
}

void CanvasPainter::renderCurLayer(QPainter& painter)
{
    paintCurrentFrame(painter, mCurrentLayerIndex, mCurrentLayerIndex);
}

void CanvasPainter::renderPostLayers(QPixmap *pixmap)
{
    QPainter painter;
    initializePainter(painter, *pixmap);
    renderPostLayers(painter);
}

void CanvasPainter::renderPostLayers(QPainter& painter)
{
    if (mOptions.nShowAllLayers > 0)
    {
        paintCurrentFrame(painter, mCurrentLayerIndex+1, mObject->getLayerCount()-1);
    }

    paintCameraBorder(painter);

    // post effects
    if (mOptions.bAxis)
    {
        paintAxis(painter);
    }
}

void CanvasPainter::setPaintSettings(const Object* object, int currentLayer, int frame, QRect rect, BitmapImage *buffer)
{
    Q_UNUSED(rect)
    Q_ASSERT(object);
    mObject = object;

    mCurrentLayerIndex = currentLayer;
    mFrameNumber = frame;
    mBuffer = buffer;
}

void CanvasPainter::paint()
{
    QPainter painter;
    initializePainter(painter, *mCanvas);

    renderPreLayers(painter);
    renderCurLayer(painter);
    renderPostLayers(painter);
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

void CanvasPainter::paintOverlayCenter(QPainter &painter)
{
    QRect rect = getCameraRect();

    Layer* layer = mObject->getLayer(mCurrentLayerIndex);
    bool isCameraMode = (layer->type() == Layer::CAMERA);

    painter.save();
    painter.setCompositionMode(QPainter::RasterOp_NotSourceAndNotDestination);

    QPen pen(Qt::DashLine);
    qreal space = 10;
    QVector<qreal> dashes;
    dashes << 10 << space << 10 << space << 10 << space;
    pen.setDashPattern(dashes);
    pen.setCosmetic(true);
    painter.setPen(pen);
    painter.setWorldMatrixEnabled(!isCameraMode);
    painter.setBrush(Qt::NoBrush);
    QPainter::RenderHints previous_renderhints = painter.renderHints();
    painter.setRenderHint(QPainter::Antialiasing, false);

    painter.drawLine(rect.x() + rect.width()/4, rect.y() + rect.height()/2, rect.x() + rect.width()*3/4, rect.y() + rect.height()/2);
    painter.drawLine(rect.x() + rect.width()/2, rect.y() + rect.height()/4, rect.x() + rect.width()/2, rect.y() + rect.height()*3/4);

    painter.setRenderHints(previous_renderhints);
    painter.restore();
}

void CanvasPainter::paintOverlayThirds(QPainter &painter)
{
    QRect rect = getCameraRect();
    painter.save();
    painter.setCompositionMode(QPainter::RasterOp_NotSourceAndNotDestination);

    Layer* layer = mObject->getLayer(mCurrentLayerIndex);
    bool isCameraMode = (layer->type() == Layer::CAMERA);

    QPen pen(Qt::DashLine);
    qreal space = 10;
    QVector<qreal> dashes;
    dashes << 10 << space << 10 << space << 10 << space;
    pen.setDashPattern(dashes);
    pen.setCosmetic(true);
    painter.setPen(pen);
    painter.setWorldMatrixEnabled(!isCameraMode);
    painter.setBrush(Qt::NoBrush);
    QPainter::RenderHints previous_renderhints = painter.renderHints();
    painter.setRenderHint(QPainter::Antialiasing, false);

    painter.drawLine(rect.x(), rect.y() + (rect.height()/3), rect.right(), rect.y() + (rect.height()/3));
    painter.drawLine(rect.x(), rect.y() + (rect.height() * 2/3), rect.x() + rect.width(), rect.y() + (rect.height() * 2/3));
    painter.drawLine(rect.x() + rect.width()/3, rect.y(), rect.x() + rect.width()/3, rect.y() + rect.height());
    painter.drawLine(rect.x() + rect.width() *2/3, rect.y(), rect.x() + rect.width() *2/3, rect.y() + rect.height());

    painter.setRenderHints(previous_renderhints);
    painter.restore();
}

void CanvasPainter::paintOverlayGolden(QPainter &painter)
{
    QRect rect = getCameraRect();
    painter.save();
    painter.setCompositionMode(QPainter::RasterOp_NotSourceAndNotDestination);

    Layer* layer = mObject->getLayer(mCurrentLayerIndex);
    bool isCameraMode = (layer->type() == Layer::CAMERA);

    QPen pen(Qt::DashLine);
    qreal space = 10;
    QVector<qreal> dashes;
    dashes << 10 << space << 10 << space << 10 << space;
    pen.setDashPattern(dashes);
    pen.setCosmetic(true);
    painter.setPen(pen);
    painter.setWorldMatrixEnabled(!isCameraMode);
    painter.setBrush(Qt::NoBrush);
    QPainter::RenderHints previous_renderhints = painter.renderHints();
    painter.setRenderHint(QPainter::Antialiasing, false);

    painter.drawLine(rect.x(), static_cast<int>(rect.y() + (rect.height() * 0.38)), rect.right(), static_cast<int>(rect.y() + (rect.height() * 0.38)));
    painter.drawLine(rect.x(), static_cast<int>(rect.y() + (rect.height() * 0.62)), rect.x() + rect.width(), static_cast<int>(rect.y() + (rect.height() * 0.62)));
    painter.drawLine(static_cast<int>(rect.x() + rect.width() * 0.38), rect.y(), static_cast<int>(rect.x() + rect.width() * 0.38), rect.bottom());
    painter.drawLine(static_cast<int>(rect.x() + rect.width() * 0.62), rect.y(), static_cast<int>(rect.x() + rect.width() * 0.62), rect.bottom());

    painter.setRenderHints(previous_renderhints);
    painter.restore();
}

void CanvasPainter::paintOverlaySafeAreas(QPainter &painter)
{
    QRect rect = getCameraRect();

    Layer* layer = mObject->getLayer(mCurrentLayerIndex);
    bool isCameraMode = (layer->type() == Layer::CAMERA);

    painter.save();
    painter.setCompositionMode(QPainter::RasterOp_NotSourceAndNotDestination);
    QPen pen(Qt::DashLine);
    qreal space = 10;
    QVector<qreal> dashes;
    dashes << 10 << space << 10 << space << 10 << space;
    pen.setDashPattern(dashes);
    pen.setCosmetic(true);
    painter.setPen(pen);
    painter.setWorldMatrixEnabled(!isCameraMode);
    painter.setBrush(Qt::NoBrush);
    QPainter::RenderHints previous_renderhints = painter.renderHints();
    painter.setRenderHint(QPainter::TextAntialiasing, true);

    if (mOptions.bActionSafe)
    {
        int action = mOptions.nActionSafe;
        QRect safeAction = QRect(rect.x() + rect.width()*action/200, rect.y() + rect.height()*action/200, rect.width()*(100-action)/100, rect.height()*(100-action)/100);
        painter.drawRect(safeAction);

        if (mOptions.bShowSafeAreaHelperText) {
            painter.drawText(safeAction.x(), safeAction.y()-1, tr("Safe Action area %1 %").arg(action));
        }
    }
    if (mOptions.bTitleSafe)
    {
        int title = mOptions.nTitleSafe;
        QRect safeTitle = QRect(rect.x() + rect.width()*title/200, rect.y() + rect.height()*title/200, rect.width()*(100-title)/100, rect.height()*(100-title)/100);
        painter.drawRect(safeTitle);

        if (mOptions.bShowSafeAreaHelperText) {
            painter.drawText(safeTitle.x(), safeTitle.y()-1, tr("Safe Title area %1 %").arg(title));
        }
    }

    painter.setRenderHints(previous_renderhints);
    painter.restore();
}

void CanvasPainter::renderGrid(QPainter& painter)
{
    if (mOptions.bGrid)
    {
        painter.setWorldTransform(mViewTransform);
        paintGrid(painter);
    }
}

void CanvasPainter::renderOverlays(QPainter &painter)
{
    if (mOptions.bCenter)
    {
        painter.setWorldTransform(mViewTransform);
        paintOverlayCenter(painter);
    }
    if (mOptions.bThirds)
    {
        painter.setWorldTransform(mViewTransform);
        paintOverlayThirds(painter);
    }
    if (mOptions.bGoldenRatio)
    {
        painter.setWorldTransform(mViewTransform);
        paintOverlayGolden(painter);
    }
    if (mOptions.bSafeArea)
    {
        painter.setWorldTransform(mViewTransform);
        paintOverlaySafeAreas(painter);
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
