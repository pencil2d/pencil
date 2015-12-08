/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2013-2014 Matt Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation;

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

void CanvasRenderer::paint( Object* object, int layer, int frame, QRect rect )
{
    Q_ASSERT( object );
    mObject = object;

    mLayerIndex = layer;
    mFrameNumber = frame;

    QPainter painter( mCanvas );

    painter.setWorldTransform( mViewTransform );
    painter.setRenderHint( QPainter::SmoothPixmapTransform, mOptions.bBlurryZoom );
    painter.setRenderHint( QPainter::Antialiasing, mOptions.bAntiAlias );

    // Don't set clip rect, paint whole canvas.
    //painter.setClipRect( rect );
    //painter.setClipping( true );

    painter.setWorldMatrixEnabled( true );

    paintBackground( painter );
    paintOnionSkin( painter );
    paintCurrentFrame( painter );

    // post effects
    if ( mOptions.bAxis )
    {
        paintAxis( painter );
    }

    if ( mOptions.bGrid )
    {
        paintGrid( painter );
    }
}

void CanvasRenderer::paintBackground( QPainter& painter )
{
    mCanvas->fill( Qt::transparent );
}

void CanvasRenderer::paintOnionSkin( QPainter& painter )
{
    Layer* layer = mObject->getLayer( mLayerIndex );

    if ( layer->keyFrameCount() == 0 )
    {
        return;
    }

    qreal minOpacity = mOptions.fOnionSkinMinOpacity / 100;
    qreal maxOpacity = mOptions.fOnionSkinMaxOpacity / 100;


    int iStartFrame = std::max( mFrameNumber - mOptions.nPrevOnionSkinCount, 1 );
    int iEndFrame = mFrameNumber + mOptions.nNextOnionSkinCount;

    if ( mOptions.bPrevOnionSkin )
    {
        qreal prevOpacityIncrement = (maxOpacity - minOpacity) / mOptions.nPrevOnionSkinCount;

        int onionPosition = mOptions.nPrevOnionSkinCount - mFrameNumber + iStartFrame;

        qreal opacity = minOpacity + (prevOpacityIncrement * onionPosition);

        // Paint onion skin before current frame.
        for ( int i = iStartFrame; i < mFrameNumber; ++i )
        {
            painter.setOpacity( opacity );

            switch ( layer->type() )
            {
                case Layer::BITMAP: { paintBitmapFrame( painter, layer, i, mOptions.bColorizePrevOnion, false ); break; }
                case Layer::VECTOR: { paintVectorFrame( painter, layer, i, mOptions.bColorizePrevOnion, false ); break; }
                case Layer::CAMERA: break;
                case Layer::SOUND: break;
                default: Q_ASSERT( false ); break;
            }
            opacity = opacity + prevOpacityIncrement;
        }
    }

    if ( mOptions.bNextOnionSkin )
    {
        qreal nextOpacityIncrement = (maxOpacity - minOpacity) / mOptions.nNextOnionSkinCount;
        qreal opacity = maxOpacity;

        // Paint onion skin after current frame.
        for ( int i = mFrameNumber + 1; i <= iEndFrame; ++i )
        {
            painter.setOpacity( opacity );

            switch ( layer->type() )
            {
                case Layer::BITMAP: { paintBitmapFrame( painter, layer, i, mOptions.bColorizeNextOnion, false ); break; }
                case Layer::VECTOR: { paintVectorFrame( painter, layer, i, mOptions.bColorizeNextOnion, false ); break; }
                case Layer::CAMERA: break;
                case Layer::SOUND: break;
                default: Q_ASSERT( false ); break;
            }

            opacity = opacity - nextOpacityIncrement;
        }
    }
}

void CanvasRenderer::paintBitmapFrame( QPainter& painter, Layer* layer, int nFrame, bool colorize, bool useLastKeyFrame )
{
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
    if (useLastKeyFrame) {
        bitmapImage = bitmapLayer->getLastBitmapImageAtFrame( nFrame, 0 );
    }
    else {
        bitmapImage = bitmapLayer->getBitmapImageAtFrame( nFrame );
    }

    if ( bitmapImage == nullptr )
    {
        return;
    }

    BitmapImage* tempBitmapImage = new BitmapImage;
    tempBitmapImage->paste(bitmapImage);

    if ( colorize ) {
        QBrush colorBrush = QBrush(Qt::transparent); //no color for the current frame

        if (nFrame < mFrameNumber) {
            colorBrush = QBrush(Qt::red);
        }
        else if (nFrame > mFrameNumber) {
            colorBrush = QBrush(Qt::blue);
        }

        tempBitmapImage->drawRect(  bitmapImage->bounds(),
                                    Qt::NoPen,
                                    colorBrush,
                                    QPainter::CompositionMode_SourceIn,
                                    false);
    }

    painter.setWorldMatrixEnabled( true );
    tempBitmapImage->paintImage( painter );

    delete tempBitmapImage;
}

void CanvasRenderer::paintVectorFrame( QPainter& painter, Layer* layer, int nFrame, bool colorize, bool useLastKeyFrame )
{
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
    if (useLastKeyFrame) {
        vectorImage = vectorLayer->getLastVectorImageAtFrame( nFrame, 0 );
    }
    else {
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

    if ( colorize ) {
        QBrush colorBrush = QBrush(Qt::transparent); //no color for the current frame

        if (nFrame < mFrameNumber) {
            colorBrush = QBrush(Qt::red);
        }
        else if (nFrame > mFrameNumber) {
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

void CanvasRenderer::paintCurrentFrame( QPainter& painter )
{
    if (mOptions.nShowAllLayers > 0) {

        if (mOptions.nShowAllLayers == 1) {
            painter.setOpacity( 0.8 );
        }
        else {
            painter.setOpacity( 1.0 );
        }

        for ( int i = 0; i < mObject->getLayerCount(); ++i )
        {
            Layer* layer = mObject->getLayer( i );
            if ( i == mLayerIndex )
            {
                continue; // current layer should be paint at last.
            }

            switch ( layer->type() )
            {
                case Layer::BITMAP: { paintBitmapFrame( painter, layer, mFrameNumber ); break; }
                case Layer::VECTOR: { paintVectorFrame( painter, layer, mFrameNumber ); break; }
                case Layer::CAMERA: break;
                case Layer::SOUND: break;
                default: Q_ASSERT( false ); break;
            }
        }
    }


    painter.setOpacity( 1.0 );

    Layer* currentLayer = mObject->getLayer( mLayerIndex );
    switch ( currentLayer->type() )
    {
        case Layer::BITMAP: { paintBitmapFrame( painter, currentLayer, mFrameNumber ); break; }
        case Layer::VECTOR: { paintVectorFrame( painter, currentLayer, mFrameNumber ); break; }
        case Layer::CAMERA: break;
        case Layer::SOUND: break;
        default: Q_ASSERT( false ); break;
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
    const int gridSize = 30;

    QRectF boundingRect = mCanvas->rect();

    int w = boundingRect.width();
    int h = boundingRect.height();

    boundingRect.adjust( ( -w / 2 ), ( -h / 2 ), 0, 0 );

    int left = round100( boundingRect.left(), gridSize ) - gridSize;
    int right = round100( boundingRect.right(), gridSize ) + gridSize;
    int top = round100( boundingRect.top(), gridSize ) - gridSize;
    int bottom = round100( boundingRect.bottom(), gridSize ) + gridSize;

    QPen pen( Qt::lightGray );
    pen.setWidth(0); // 0 will draw a 1px line always independent from the transformation
    pen.setCosmetic( true );
    painter.setPen( pen );
    painter.setWorldMatrixEnabled( false );
    painter.setBrush( Qt::NoBrush );

    for ( int x = left; x < right; x += gridSize )
    {
        painter.drawLine( x, top, x, bottom );
    }

    for ( int y = top; y < bottom; y += gridSize )
    {
        painter.drawLine( left, y, right, y );
    }
}
