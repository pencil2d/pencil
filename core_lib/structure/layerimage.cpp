/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation;

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/
#include <QtDebug>
#include <QMouseEvent>
#include <QImage>
#include <QPainter>
#include "object.h"
#include "timeline.h"
#include "timelinecells.h"
#include "layerimage.h"

LayerImage::LayerImage( Object* object, LAYER_TYPE eType ) : Layer( object, eType )
{
}

void LayerImage::paintImages( QPainter& painter, TimeLineCells* cells, int x, int y, int width, int height, bool selected, int frameSize )
{
    Q_UNUSED( x );
    Q_UNUSED( width );
    painter.setPen( QPen( QBrush( QColor( 40, 40, 40 ) ), 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin ) );

    if ( visible )
    {
        /*
        for ( int i = 0; i < framesPosition.size(); i++ )
        {
            if ( framesSelected.at( i ) )
            {
                painter.setBrush( QColor( 60, 60, 60 ) );
                //painter.drawRect(x+(framesPosition.at(i)+frameOffset-1)*frameSize+2, y+1, frameSize-2, height-4);
                painter.drawRect( cells->getFrameX( framesPosition.at( i ) + frameOffset ) - frameSize + 2, y + 1, frameSize - 2, height - 4 );
            }
            else
            {
                if ( selected )
                    painter.setBrush( QColor( 125, 125, 125 ) );
                else
                    painter.setBrush( QColor( 125, 125, 125, 125 ) );
                if ( framesModified.at( i ) ) painter.setBrush( QColor( 255, 125, 125, 125 ) );
                painter.drawRect( cells->getFrameX( framesPosition.at( i ) ) - frameSize + 2, y + 1, frameSize - 2, height - 4 );
            }
        }
        */
    }
}

void LayerImage::mousePress( QMouseEvent* event, int frameNumber )
{
    /*
    frameClicked = frameNumber;
    int index = getIndexAtFrame(frameNumber);
    if (index == -1)
    {
    deselectAllFrames();
    }
    else
    {
    if ( (event->modifiers() != Qt::ShiftModifier) && (!framesSelected.at(index)) && (event->buttons() != Qt::RightButton) )
    {
    deselectAllFrames();
    }
    framesSelected[index] = true;
    }
    if (event->modifiers() == Qt::AltModifier)
    {
    for(int i=index; i < framesPosition.size(); i++)
    {
    framesSelected[i] = true;
    }
    }
    */
}

void LayerImage::mouseDoubleClick( QMouseEvent*, int frameNumber )
{
    /*
    int index = getIndexAtFrame(frameNumber);
    if (index != -1)
    {
    for(int i=index; i < framesPosition.size(); i++)
    {
    framesSelected[i] = true;
    }
    }
    */
}


void LayerImage::mouseMove( QMouseEvent* event, int frameNumber )
{
    Q_UNUSED( event );
    /*
    frameOffset = frameNumber - frameClicked;
    bool ok = true;
    for ( int i = 0; i < framesPosition.size(); i++ )
    {
    if (framesSelected.at(i))
    {
    if (framesPosition.at(i) + frameOffset < 1) ok = false;
    for ( int j = 0; j < framesPosition.size(); j++ )
    {
    if (!framesSelected.at(j))
    {
    if (framesPosition.at(i) + frameOffset == framesPosition.at(j))
    {
    ok = false;
    }
    }
    }
    }
    }
    if (ok == false) frameOffset = 0;
    */
}

void LayerImage::mouseRelease( QMouseEvent*, int frameNumber )
{
    /*
    Q_UNUSED(frameNumber);

    qDebug( "LayerImage: mouse release." );
    for ( int i = 0; i < framesPosition.size(); i++ )
    {
    if ( framesSelected.at( i ) && frameOffset != 0 )
    {
    int originalFrame = framesPosition[i];
    framesPosition[i] = originalFrame + frameOffset;
    object()->modification();
    }
    }
    bubbleSort();
    frameOffset = 0;
    */
}

bool LayerImage::saveImages( QString path, int layerNumber )
{
    /*
    qDebug() << "Saving images of layer n. " << layerNumber;
    QDir dir(path);

    // always saves all frames, no optimization
    for(int i=0; i < framesPosition.size(); i++)
    {
    //qDebug() << "Trying to save " << framesFilename.at(i) << " of layer n. " << layerNumber;
    saveImage( i, path );
    }
    qDebug() << "Layer " << layerNumber << "done";
    */
    return true;
}

