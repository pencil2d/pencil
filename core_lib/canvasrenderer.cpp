/*

Pencil - Traditional Animation Software
Copyright (C) 2012-2017 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/

#include "canvasrenderer.h"

#include "object.h"
#include "layerbitmap.h"
#include "layervector.h"
#include "bitmapimage.h"
#include "layercamera.h"
#include "vectorimage.h"
#include "util.h"



CanvasRenderer::CanvasRenderer( QObject* parent ) : QObject( parent )
    , mLog( "CanvasRenderer" )
{
    ENABLE_DEBUG_LOG( mLog, false );
}

CanvasRenderer::~CanvasRenderer()
{
}

void CanvasRenderer::setCanvas( QPixmap* canvas )
{
    Q_ASSERT( canvas );
    mCanvas = canvas;
}

void CanvasRenderer::setViewTransform( QTransform viewTransform )
{
    mViewTransform = viewTransform;
}

void CanvasRenderer::setTransformedSelection(QRect selection, QTransform transform)
{
    // Make sure that the selection is not empty
    //
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

void CanvasRenderer::ignoreTransformedSelection()
{
    mRenderTransform = false;
}

void CanvasRenderer::paint( Object* object, int layer, int frame, QRect rect )
{
    Q_ASSERT( object );
    mObject = object;

    mCurrentLayerIndex = layer;
    mFrameNumber = frame;

    QPainter painter( mCanvas );

    painter.setWorldMatrixEnabled(true);
    painter.setWorldTransform( mViewTransform );
    painter.setRenderHint( QPainter::Antialiasing, true );

    // Don't set clip rect, paint whole canvas. `rect` therefore unused.
    Q_UNUSED(rect);
    //painter.setClipRect( rect );
    //painter.setClipping( true );

    paintBackground();
    paintOnionSkin( painter );

    paintCurrentFrame( painter );
    paintCameraBorder( painter );

    // post effects
    if ( mOptions.bAxis )
    {
        paintAxis( painter );
    }
}

void CanvasRenderer::paintBackground()
{
    mCanvas->fill( Qt::transparent );
}

void CanvasRenderer::paintOnionSkin( QPainter& painter )
{
    Layer* layer = mObject->getLayer( mCurrentLayerIndex );

    if ( layer->keyFrameCount() == 0 )
    {
        return;
    }

    qreal minOpacity = mOptions.fOnionSkinMinOpacity / 100;
    qreal maxOpacity = mOptions.fOnionSkinMaxOpacity / 100;

    if ( mOptions.bPrevOnionSkin && mFrameNumber > 1 )
    {
        // Paint onion skin before current frame.
        qreal prevOpacityIncrement = (maxOpacity - minOpacity) / mOptions.nPrevOnionSkinCount;
        qreal opacity = maxOpacity;

        int onionFrameNumber = layer->getPreviousFrameNumber(mFrameNumber, mOptions.bIsOnionAbsolute);
        int onionPosition = 0;

        while (onionPosition < mOptions.nPrevOnionSkinCount && onionFrameNumber > 0)
        {
            painter.setOpacity( opacity );

            switch ( layer->type() )
            {
                case Layer::BITMAP: { paintBitmapFrame( painter, mCurrentLayerIndex, onionFrameNumber, mOptions.bColorizePrevOnion, false ); break; }
                case Layer::VECTOR: { paintVectorFrame( painter, mCurrentLayerIndex, onionFrameNumber, mOptions.bColorizePrevOnion, false ); break; }
                case Layer::CAMERA: break;
                case Layer::SOUND: break;
                default: Q_ASSERT( false ); break;
            }
            opacity = opacity - prevOpacityIncrement;

            onionFrameNumber = layer->getPreviousFrameNumber(onionFrameNumber, mOptions.bIsOnionAbsolute);
            onionPosition++;
        }
    }

    if ( mOptions.bNextOnionSkin )
    {
        // Paint onion skin after current frame.
        qreal nextOpacityIncrement = (maxOpacity - minOpacity) / mOptions.nNextOnionSkinCount;
        qreal opacity = maxOpacity;

        int onionFrameNumber = layer->getNextFrameNumber(mFrameNumber, mOptions.bIsOnionAbsolute);
        int onionPosition = 0;

        while (onionPosition < mOptions.nNextOnionSkinCount && onionFrameNumber > 0)
        {
            painter.setOpacity( opacity );

            switch ( layer->type() )
            {
                case Layer::BITMAP: { paintBitmapFrame( painter, mCurrentLayerIndex, onionFrameNumber, mOptions.bColorizeNextOnion, false ); break; }
                case Layer::VECTOR: { paintVectorFrame( painter, mCurrentLayerIndex, onionFrameNumber, mOptions.bColorizeNextOnion, false ); break; }
                case Layer::CAMERA: break;
                case Layer::SOUND: break;
                default: Q_ASSERT( false ); break;
            }
            opacity = opacity - nextOpacityIncrement;

            onionFrameNumber = layer->getNextFrameNumber(onionFrameNumber, mOptions.bIsOnionAbsolute);
            onionPosition++;
        }
    }
}

void CanvasRenderer::paintBitmapFrame( QPainter& painter,
                                      int layerId,
                                      int nFrame,
                                      bool colorize,
                                      bool useLastKeyFrame )
{
    Layer* layer = mObject->getLayer( layerId );

    if ( !layer->visible() )
    {
        return;
    }

    LayerBitmap* bitmapLayer = dynamic_cast< LayerBitmap* >( layer );
    if ( bitmapLayer == nullptr )
    {
        Q_ASSERT( bitmapLayer );
        return;
    }

    qCDebug( mLog ) << "Paint Onion skin bitmap, Frame = " << nFrame;
    BitmapImage* bitmapImage;
    if (useLastKeyFrame)
    {
        bitmapImage = bitmapLayer->getLastBitmapImageAtFrame( nFrame, 0 );
    }
    else
    {
        bitmapImage = bitmapLayer->getBitmapImageAtFrame( nFrame );
    }

    if ( bitmapImage == nullptr )
    {
        return;
    }

    BitmapImage tempBitmapImage;
    tempBitmapImage.paste(bitmapImage);

    if ( colorize )
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

        tempBitmapImage.drawRect(bitmapImage->bounds(),
                                 Qt::NoPen,
                                 colorBrush,
                                 QPainter::CompositionMode_SourceIn,
                                 false);
    }

    // If the current frame on the current layer has a transformation, we apply it.
    if (mRenderTransform && nFrame == mFrameNumber && layerId == mCurrentLayerIndex )
    {
        tempBitmapImage.clear(mSelection);
        paintTransformedSelection(painter);
    }

    painter.setWorldMatrixEnabled( true );

    tempBitmapImage.paintImage( painter );
}

void CanvasRenderer::paintVectorFrame( QPainter& painter,
                                       int layerId,
                                       int nFrame,
                                       bool colorize,
                                       bool useLastKeyFrame )
{
    Layer* layer = mObject->getLayer( layerId );

    if ( !layer->visible() )
    {
        return;
    }

    LayerVector* vectorLayer = dynamic_cast< LayerVector* >( layer );
    if ( vectorLayer == nullptr )
    {
        Q_ASSERT( vectorLayer );
        return;
    }

    qCDebug( mLog ) << "Paint Onion skin vector, Frame = " << nFrame;
    VectorImage* vectorImage;
    if (useLastKeyFrame)
    {
        vectorImage = vectorLayer->getLastVectorImageAtFrame( nFrame, 0 );
    }
    else
    {
        vectorImage = vectorLayer->getVectorImageAtFrame( nFrame );
    }
    if ( vectorImage == nullptr )
    {
        return;
    }

    QImage* pImage = new QImage( mCanvas->size(), QImage::Format_ARGB32_Premultiplied );
    vectorImage->outputImage( pImage, mViewTransform, mOptions.bOutlines, mOptions.bThinLines, mOptions.bAntiAlias );


    //painter.drawImage( QPoint( 0, 0 ), *pImage );

    // Go through a Bitmap image to paint the onion skin colour
    //
    BitmapImage* tempBitmapImage = new BitmapImage();
    tempBitmapImage->setImage(pImage);

    if ( colorize )
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

        tempBitmapImage->drawRect(  pImage->rect(),
                                    Qt::NoPen,
                                    colorBrush,
                                    QPainter::CompositionMode_SourceIn,
                                    false);
    }

    painter.setWorldMatrixEnabled( false ); //Don't tranform the image here as we used the viewTransform in the image output
    tempBitmapImage->paintImage( painter );

    delete tempBitmapImage;
}

void CanvasRenderer::paintTransformedSelection( QPainter& painter )
{
    // Make sure there is something selected
    //
    if (mSelection.width() == 0 || mSelection.height() == 0) {
        return;
    }

    Layer* layer = mObject->getLayer( mCurrentLayerIndex );

    if (layer->type() == Layer::BITMAP) {

        // Get the transformed image
        //
        BitmapImage* bitmapImage = dynamic_cast< LayerBitmap* >( layer )->getLastBitmapImageAtFrame( mFrameNumber, 0 );

        BitmapImage transformedImage = bitmapImage->transformed(mSelection, mSelectionTransform, mOptions.bAntiAlias);


        // Paint the transformation output
        //
        painter.setWorldMatrixEnabled( true );
        transformedImage.paintImage(painter);
    }
}

void CanvasRenderer::paintCurrentFrame( QPainter& painter )
{
    bool isCamera = mObject->getLayer(mCurrentLayerIndex)->type() == Layer::CAMERA;
    for ( int i = 0; i < mObject->getLayerCount(); ++i )
    {
        Layer* layer = mObject->getLayer( i );
        if ( i == mCurrentLayerIndex || mOptions.nShowAllLayers != 1 )
        {
            painter.setOpacity( 1.0 );
        }
        else if ( !isCamera )
        {
            painter.setOpacity( 0.8 );
        }

        if ( i == mCurrentLayerIndex || mOptions.nShowAllLayers > 0 )
        {
            switch ( layer->type() )
            {
                case Layer::BITMAP: { paintBitmapFrame( painter, i, mFrameNumber ); break; }
                case Layer::VECTOR: { paintVectorFrame( painter, i, mFrameNumber ); break; }
                case Layer::CAMERA: break;
                case Layer::SOUND: break;
                default: Q_ASSERT( false ); break;
            }
        }
    }
}

void CanvasRenderer::paintAxis( QPainter& painter )
{
    painter.setPen( Qt::green );
    painter.drawLine( QLineF( 0, -500, 0, 500 ) );

    painter.setPen( Qt::red );
    painter.drawLine( QLineF( -500, 0, 500, 0 ) );
}

int round100( double f, int gridSize )
{
    return static_cast< int >( f ) / gridSize * gridSize;
}

void CanvasRenderer::paintGrid( QPainter& painter )
{
    int gridSize = mOptions.nGridSize;

    QRectF rect = painter.viewport();
    QRectF boundingRect = mViewTransform.inverted().mapRect( rect );

    int left = round100( boundingRect.left(), gridSize ) - gridSize;
    int right = round100( boundingRect.right(), gridSize ) + gridSize;
    int top = round100( boundingRect.top(), gridSize ) - gridSize;
    int bottom = round100( boundingRect.bottom(), gridSize ) + gridSize;
    
    QPen pen( Qt::lightGray );
    pen.setCosmetic( true );
    painter.setPen( pen );
    painter.setWorldMatrixEnabled( true );
    painter.setBrush( Qt::NoBrush );
    QPainter::RenderHints previous_renderhints = painter.renderHints();
    painter.setRenderHint( QPainter::Antialiasing, false );
    for ( int x = left; x < right; x += gridSize )
    {
        painter.drawLine( x, top, x, bottom );
    }

    for ( int y = top; y < bottom; y += gridSize )
    {
        painter.drawLine( left, y, right, y );
    }
    painter.setRenderHints(previous_renderhints);
}

void CanvasRenderer::renderGrid(QPainter& painter)
{
    if ( mOptions.bGrid )
    {
        painter.setWorldTransform( mViewTransform );
        paintGrid( painter );
    }
}

void CanvasRenderer::paintCameraBorder(QPainter &painter)
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
        boundingRect = viewRect.toRect();
        mCameraRect = center.mapRect(mCameraRect);
    }
    else
    {
        painter.setWorldMatrixEnabled(true);
        QTransform viewInverse = mViewTransform.inverted();
        boundingRect = viewInverse.mapRect(viewRect).toRect();

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

QRect CanvasRenderer::getCameraRect()
{
    return mCameraRect;
}
