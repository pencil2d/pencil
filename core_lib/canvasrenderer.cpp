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
#include <QPainter>


CanvasRenderer::CanvasRenderer( QObject* parent )
    : QObject( parent )
    , mLog( "CanvasRenderer" )
{
    mLog.setEnabled( QtDebugMsg, false );
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

void CanvasRenderer::paint( Object* object, int layer, int frame )
{
    Q_ASSERT( object );
    mObject = object;

    mLayerIndex = layer;
    mFrameNumber = frame;

    QPainter painter( mCanvas );
    painter.setTransform( mViewTransform );
    painter.setRenderHint( QPainter::SmoothPixmapTransform, false );
    painter.setWorldMatrixEnabled( true );

    paintBackground( painter );
    paintOnionSkin( painter );
    paintCurrentFrame( painter );
}

void CanvasRenderer::paintBackground( QPainter& painter )
{
    painter.setPen( Qt::NoPen );
    painter.setBrush( QBrush( Qt::white ) );

    mCanvas->fill( Qt::white );
}

void CanvasRenderer::paintOnionSkin( QPainter& painter )
{
    painter.setOpacity( 0.3 );

    Layer* layer = mObject->getLayer( mLayerIndex );

    if ( layer->keyFrameCount() == 0 )
    {
        return;
    }

    int iStartFrame = std::max( mFrameNumber - mOptions.nPrevOnionSkinCount, 1 );
    int iEndFrame = mFrameNumber + mOptions.nNextOnionSkinCount;

    if ( mOptions.bPrevOnionSkin )
    {
        // Paint onion skin before current frame.
        for ( int i = iStartFrame; i < mFrameNumber; ++i )
        {
            switch ( layer->type() )
            {
                case Layer::BITMAP: { paintBitmapFrame( painter, layer, i ); break; }
                case Layer::VECTOR: { paintVectorFrame( painter, layer, i ); break; }
                case Layer::CAMERA: break;
                case Layer::SOUND: break;
                default: Q_ASSERT( false ); break;
            }
        }
    }

    if ( mOptions.bNextOnionSkin )
    {
        // Paint onion skin after current frame.
        for ( int i = mFrameNumber; i <= iEndFrame; ++i )
        {
            switch ( layer->type() )
            {
                case Layer::BITMAP: { paintBitmapFrame( painter, layer, i ); break; }
                case Layer::VECTOR: { paintVectorFrame( painter, layer, i ); break; }
                case Layer::CAMERA: break;
                case Layer::SOUND: break;
                default: Q_ASSERT( false ); break;
            }
        }
    }
}

void CanvasRenderer::paintBitmapFrame( QPainter& painter, Layer* layer, int nFrame )
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
    BitmapImage* bitmapImage = bitmapLayer->getBitmapImageAtFrame( nFrame );
    if ( bitmapImage == nullptr )
    {
        return;
    }

    bitmapImage->paintImage( painter );
}


void CanvasRenderer::paintVectorFrame( QPainter& painter, Layer* layer, int nFrame )
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
    VectorImage* vectorImage = vectorLayer->getVectorImageAtFrame( nFrame );
    if ( vectorImage == nullptr )
    {
        return;
    }

    vectorImage->paintImage( painter, true, true, true );
}

void CanvasRenderer::paintCurrentFrame( QPainter& painter )
{
    painter.setOpacity( 0.8 );

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
