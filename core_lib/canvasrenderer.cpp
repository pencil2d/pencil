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
#include <QPainter>


CanvasRenderer::CanvasRenderer( QObject* parent ) : QObject( parent )
{
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
    /*
    for ( int i = 0; i < mObject->getLayerCount(); ++i )
    {

    }
    */

    Layer* layer = mObject->getLayer( mLayerIndex );

    switch ( layer->type() )
    {
        case Layer::BITMAP:
        {
            LayerBitmap* bitmapLayer = static_cast< LayerBitmap* >( layer );
            paintOnionSkinBitmap( bitmapLayer );
            break;
        }
        case Layer::VECTOR:
        {
            break;
        }
        default:
        {
            break;
        }
    }

}

void CanvasRenderer::paintOnionSkinBitmap( LayerBitmap* layer )
{

}


void CanvasRenderer::paintCurrentFrame( QPainter& painter )
{

}
