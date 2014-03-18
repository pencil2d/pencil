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
#include "KeyFrame.h"
#include "timeline.h"
#include "timelinecells.h"
#include "layerimage.h"

LayerImage::LayerImage( Object* object, LAYER_TYPE eType ) : Layer( object, eType )
{
    frameClicked = -1;
    frameOffset = 0;
}

LayerImage::~LayerImage()
{
    for ( auto pair : m_KeyFrames )
    {
        KeyFrame* pKeyFrame = pair.second;
        delete pKeyFrame;
    }
    m_KeyFrames.clear();
}

bool LayerImage::hasKeyFrameAtPosition( int position )
{
    return ( m_KeyFrames.find( position ) != m_KeyFrames.end() );
}

KeyFrame* LayerImage::getKeyFrameAtPosition( int position )
{
    auto it = m_KeyFrames.find( position );
    if ( it == m_KeyFrames.end() )
    {
        return NullKeyFrame::get();
    }
    return it->second;
}

KeyFrame* LayerImage::getLastKeyFrameAtPosition( int position )
{
    auto it = m_KeyFrames.lower_bound( position );
    if ( it == m_KeyFrames.end() )
    {
        return NullKeyFrame::get();
    }
    return it->second;
}

int LayerImage::getPreviousKeyFramePosition( int position )
{
    auto it = m_KeyFrames.upper_bound( position );
    if ( it == m_KeyFrames.end() )
    {
        return getFirstKeyFramePosition();
    }
    return it->first;
}

int LayerImage::getNextKeyFramePosition( int position )
{
    auto it = m_KeyFrames.lower_bound( position );
    if ( it == m_KeyFrames.end() )
    {
        return getMaxFramePosition();
    }

    if ( it != m_KeyFrames.begin() )
    {
        --it;
    }
    return it->first;
}

int LayerImage::getFirstFramePosition()
{
    Q_ASSERT( m_KeyFrames.rbegin()->first == 1 );

    return m_KeyFrames.rbegin()->first; // rbegin is the lowest key frame position
}

int LayerImage::getMaxFramePosition()
{
    return m_KeyFrames.begin()->first; // begin is the highest key frame position
}

int LayerImage::getIndexAtFrame( int frameNumber )
{
    int index = -1;
    /*
    for (int i = 0; i < framesPosition.size(); i++)
    {
    if (framesPosition.at(i) == frameNumber)
    {
    index = i;
    }
    }
    */
    return index;
}

int LayerImage::getLastIndexAtFrame( int frameNumber )
{
    int position = -1;
    int index = -1;
    /*
    for(int i=0; i < framesPosition.size(); i++)
    {
    if (framesPosition.at(i) > position && framesPosition.at(i) <= frameNumber)
    {
    position = framesPosition.at(i);
    index = i;
    }
    }
    */
    return index;
}

void LayerImage::paintTrack( QPainter& painter, TimeLineCells* cells, int x, int y, int width, int height, bool selected, int frameSize )
{
    painter.setFont( QFont( "helvetica", height / 2 ) );
    if ( visible )
    {
        QColor col;
        if ( type() == BITMAP ) col = QColor( 130, 130, 245 );
        if ( type() == VECTOR ) col = QColor( 100, 205, 150 );
        if ( type() == SOUND ) col = QColor( 245, 130, 130 );
        if ( type() == CAMERA ) col = QColor( 100, 128, 140 );
        if ( !selected ) col = QColor( ( 1 * col.red() + 2 * 200 ) / 3, ( 1 * col.green() + 2 * 200 ) / 3, ( 1 * col.blue() + 2 * 200 ) / 3 );

        painter.setBrush( col );
        painter.setPen( QPen( QBrush( QColor( 100, 100, 100 ) ), 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin ) );
        painter.drawRect( x, y - 1, width, height );

        paintImages( painter, cells, x, y, width, height, selected, frameSize );

        // changes the apparence if selected
        if ( selected )
        {
            paintSelection( painter, x, y, width, height );
        }
    }
    else
    {
        painter.setBrush( Qt::gray );
        painter.setPen( QPen( QBrush( QColor( 100, 100, 100 ) ), 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin ) );
        painter.drawRect( x, y - 1, width, height ); // empty rectangle  by default
    }
}

void LayerImage::paintImages( QPainter& painter, TimeLineCells* cells, int x, int y, int width, int height, bool selected, int frameSize )
{
    Q_UNUSED( x );
    Q_UNUSED( width );
    painter.setPen( QPen( QBrush( QColor( 40, 40, 40 ) ), 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin ) );
    if ( visible )
    {
        /*
        for(int i=0; i < framesPosition.size(); i++)
        {
        if (framesSelected.at(i))
        {
        painter.setBrush(QColor(60,60,60));
        //painter.drawRect(x+(framesPosition.at(i)+frameOffset-1)*frameSize+2, y+1, frameSize-2, height-4);
        painter.drawRect( cells->getFrameX(framesPosition.at(i)+frameOffset)-frameSize+2, y+1, frameSize-2, height-4);
        }
        else
        {
        if (selected)
        painter.setBrush(QColor(125,125,125));
        else
        painter.setBrush(QColor(125,125,125,125));
        if (framesModified.at(i)) painter.setBrush(QColor(255,125,125,125));
        painter.drawRect( cells->getFrameX(framesPosition.at(i))-frameSize+2, y+1, frameSize-2, height-4 );
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

bool LayerImage::addNewKeyFrameAt( int frameNumber )
{
    /*
    int index = getIndexAtFrame(frameNumber);
    if (index == -1)
    {
    framesPosition.append(frameNumber);
    framesSelected.append(false);
    framesFilename.append("");
    framesModified.append(false);
    bubbleSort();

    return true;
    }
    else
    {
    return false;
    }
    */
    return true;
}

void LayerImage::setModified( int frameNumber, bool trueOrFalse )
{
    /*
    int index = getLastIndexAtFrame(frameNumber);
    if (index != -1)
    {
    framesModified[index] = trueOrFalse;
    object()->modification();
    }
    */
}

void LayerImage::deselectAllFrames()
{
    /*
    for(int i=0; i < framesPosition.size(); i++)
    {
    framesSelected[i] = false;
    }
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

bool LayerImage::addKeyFrame( int position, KeyFrame* pKeyFrame )
{
    removeKeyFrame( position );

    pKeyFrame->setPos( position );
    m_KeyFrames.insert( std::make_pair( position, pKeyFrame ) );

    return true;
}

bool LayerImage::removeKeyFrame( int position )
{
    if ( position == 1 )
    {
        // you can't delete 1st frame.
        return true;
    }

    auto it = m_KeyFrames.find( position );
    if ( it != m_KeyFrames.end() )
    {
        delete it->second;
        m_KeyFrames.erase( it );
    }

    return true;
}

void LayerImage::foreachKeyFrame( std::function<void( KeyFrame* )> func )
{
    for ( auto pair : m_KeyFrames )
    {
        func( pair.second );
    }
}
