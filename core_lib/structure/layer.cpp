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

#include <climits>
#include <cassert>
#include <QtDebug>
#include <QInputDialog>
#include <QLineEdit>
#include "keyframe.h"
#include "layer.h"
#include "object.h"
#include "timeline.h"
#include "timelinecells.h"


Layer::Layer( Object* pObject, LAYER_TYPE eType ) : QObject( pObject )
{
    mObject = pObject;
    meType = eType;
    mId = 0;
    mName = QString( tr( "Undefined Layer" ) );
    mVisible = true;

    Q_ASSERT( eType != UNDEFINED );
}

Layer::~Layer()
{
    for ( auto pair : mKeyFrames )
    {
        KeyFrame* pKeyFrame = pair.second;
        delete pKeyFrame;
    }
    mKeyFrames.clear();
}

void Layer::foreachKeyFrame( std::function<void( KeyFrame* )> action )
{
    for ( auto pair : mKeyFrames )
    {
        action( pair.second );
    }
}

bool Layer::keyExists( int position )
{
    return ( mKeyFrames.find( position ) != mKeyFrames.end() );
}

KeyFrame* Layer::getKeyFrameAtPosition( int position )
{
    auto it = mKeyFrames.find( position );
    if ( it == mKeyFrames.end() )
    {
        return NullKeyFrame::get();
    }
    return it->second;
}

KeyFrame* Layer::getLastKeyFrameAtPosition( int position )
{
    if ( position < 1 )
    {
        position = 1;
    }
    auto it = mKeyFrames.lower_bound( position );
    if ( it == mKeyFrames.end() )
    {
        return NullKeyFrame::get();
    }
    return it->second;
}

int Layer::getPreviousKeyFramePosition( int position )
{
    auto it = mKeyFrames.upper_bound( position );
    if ( it == mKeyFrames.end() )
    {
        return firstKeyFramePosition();
    }
    return it->first;
}

int Layer::getNextKeyFramePosition( int position )
{
    auto it = mKeyFrames.lower_bound( position );
    if ( it == mKeyFrames.end() )
    {
        return getMaxKeyFramePosition();
    }

    if ( it != mKeyFrames.begin() )
    {
        --it;
    }
    return it->first;
}

int Layer::firstKeyFramePosition()
{
    Q_ASSERT( mKeyFrames.rbegin()->first == 1 );

    return mKeyFrames.rbegin()->first; // rbegin is the lowest key frame position
}

int Layer::getMaxKeyFramePosition()
{
    return mKeyFrames.begin()->first; // begin is the highest key frame position
}

bool Layer::addKeyFrame( int position, KeyFrame* pKeyFrame )
{
    auto it = mKeyFrames.find( position );
    if ( it != mKeyFrames.end() )
    {
        return false;
    }

    pKeyFrame->setPos( position );
    mKeyFrames.insert( std::make_pair( position, pKeyFrame ) );

    return true;
}

bool Layer::removeKeyFrame( int position )
{
//    if ( position == 1 )
//    {
//        // you can't delete 1st frame.
//        //return true;
//    }

    auto it = mKeyFrames.find( position );
    if ( it != mKeyFrames.end() )
    {
        delete it->second;
        mKeyFrames.erase( it );
    }

    if ( position == 1 )
    {
        // you can't delete 1st frame.
        //return true;
        addNewKeyAt( 1 ); // replacing
    }

    return true;
}

bool Layer::moveKeyFrameForward( int position )
{
    return swapKeyFrames( position, position + 1 );
}

bool Layer::moveKeyFrameBackward( int position )
{
    if ( position != 1 ) {
        return swapKeyFrames( position, position - 1 );
    } else {
        return true;
    }
}

bool Layer::swapKeyFrames( int position1, int position2 ) //Current behaviour, need to refresh the swapped cels
{

    bool keyPosition1 = false, keyPosition2 = false;
    KeyFrame* pFirstFrame;
    KeyFrame* pSecondFrame;


    if ( keyExists( position1 ) )
    {
        auto firstFrame = mKeyFrames.find( position1 );
        pFirstFrame = firstFrame->second;

        mKeyFrames.erase( position1 );

        //pFirstFrame = getKeyFrameAtPosition( position1 );
        //removeKeyFrame( position1 );

        keyPosition1 = true;
    }

	if ( keyExists( position2 ) )
    {
        auto secondFrame = mKeyFrames.find( position2 );
        pSecondFrame = secondFrame->second;

        mKeyFrames.erase( position2 );

        //pSecondFrame = getKeyFrameAtPosition( position2 );
        //removeKeyFrame( position2 );

        keyPosition2 = true;
    }

    if ( keyPosition2 )
    {
        //addKeyFrame( position1, pSecondFrame );
        pSecondFrame->setPos( position1 );
        mKeyFrames.insert( std::make_pair( position1, pSecondFrame ) );
    } 
	else if ( position1 == 1 ) 
	{
        addNewKeyAt( position1 );
    }

    if ( keyPosition1 )
    {
        //addKeyFrame( position2, pFirstFrame );
        pFirstFrame->setPos( position2 );
        mKeyFrames.insert( std::make_pair( position2, pFirstFrame ) );
    } 
	else if ( position2 == 1 )
	{
		addNewKeyAt( position2 );
    }

    return true;

}

bool Layer::loadKey( KeyFrame* pKey )
{
    auto it = mKeyFrames.find( pKey->pos() );
    if ( it != mKeyFrames.end() )
    {
        delete it->second;
        mKeyFrames.erase( it );
    }
    mKeyFrames.insert( std::make_pair( pKey->pos(), pKey ) );
    return true;
}

bool Layer::save( QString strDataFolder )
{
	for ( auto pair : mKeyFrames )
	{
		KeyFrame* pKeyFrame = pair.second;
		saveKeyFrame( pKeyFrame, strDataFolder );
	}
    return false;
}

void Layer::paintTrack( QPainter& painter, TimeLineCells* cells, int x, int y, int width, int height, bool selected, int frameSize )
{
    painter.setFont( QFont( "helvetica", height / 2 ) );
    if ( mVisible )
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

        paintFrames( painter, cells, x, y, width, height, selected, frameSize );

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

void Layer::paintFrames( QPainter& painter, TimeLineCells* cells, int x, int y, int width, int height, bool selected, int frameSize )
{
    painter.setPen( QPen( QBrush( QColor( 40, 40, 40 ) ), 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin ) );

    //qDebug() << "LayerType:" << static_cast<int>( m_eType );

    for ( auto pair : mKeyFrames )
    {
        int framePos = pair.first;
        if ( selected )
        {
            painter.setBrush( QColor( 60, 60, 60 ) );
            painter.drawRect( cells->getFrameX( framePos ) - frameSize + 2, y + 1, frameSize - 2, height - 4 );
        }
        else
        {
            painter.drawRect( cells->getFrameX( framePos ) - frameSize + 2, y + 1, frameSize - 2, height - 4 );

        }
    }
}

void Layer::paintLabel( QPainter& painter, TimeLineCells* cells, int x, int y, int width, int height, bool selected, int allLayers )
{
    Q_UNUSED( cells );
    painter.setBrush( Qt::lightGray );
    painter.setPen( QPen( QBrush( QColor( 100, 100, 100 ) ), 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin ) );
    painter.drawRect( x, y - 1, width, height ); // empty rectangle  by default

    if ( mVisible )
    {
        if ( allLayers == 0 )  painter.setBrush( Qt::NoBrush );
        if ( allLayers == 1 )   painter.setBrush( Qt::darkGray );
        if ( ( allLayers == 2 ) || selected )  painter.setBrush( Qt::black );
    }
    else
    {
        painter.setBrush( Qt::NoBrush );
    }
    painter.setPen( Qt::black );
    painter.setRenderHint( QPainter::Antialiasing, true );
    painter.drawEllipse( x + 6, y + 4, 9, 9 );
    painter.setRenderHint( QPainter::Antialiasing, false );

    if ( selected )
    {
        paintSelection( painter, x, y, width, height );
    }

    if ( type() == BITMAP ) painter.drawPixmap( QPoint( 20, y + 2 ), QPixmap( ":/icons/layer-bitmap.png" ) );
    if ( type() == VECTOR ) painter.drawPixmap( QPoint( 20, y + 2 ), QPixmap( ":/icons/layer-vector.png" ) );
    if ( type() == SOUND ) painter.drawPixmap( QPoint( 21, y + 2 ), QPixmap( ":/icons/layer-sound.png" ) );
    if ( type() == CAMERA ) painter.drawPixmap( QPoint( 21, y + 2 ), QPixmap( ":/icons/layer-camera.png" ) );

    painter.setFont( QFont( "helvetica", height / 2 ) );
    painter.setPen( Qt::black );
    painter.drawText( QPoint( 45, y + ( 2 * height ) / 3 ), mName );
}

void Layer::paintSelection( QPainter& painter, int x, int y, int width, int height )
{
    QLinearGradient linearGrad( QPointF( 0, y ), QPointF( 0, y + height ) );
    QSettings settings( "Pencil", "Pencil" );
    QString style = settings.value( "style" ).toString();
    if ( style == "aqua" )
    {
        linearGrad.setColorAt( 0, QColor( 225, 225, 255, 100 ) );
        linearGrad.setColorAt( 0.10, QColor( 225, 225, 255, 80 ) );
        linearGrad.setColorAt( 0.20, QColor( 225, 225, 255, 64 ) );
        linearGrad.setColorAt( 0.35, QColor( 225, 225, 255, 20 ) );
        linearGrad.setColorAt( 0.351, QColor( 0, 0, 0, 32 ) );
        linearGrad.setColorAt( 0.66, QColor( 245, 255, 235, 32 ) );
        linearGrad.setColorAt( 1, QColor( 245, 255, 235, 128 ) );
    }
    else
    {
        linearGrad.setColorAt( 0, QColor( 255, 255, 255, 128 ) );
        linearGrad.setColorAt( 0.49, QColor( 255, 255, 255, 0 ) );
        linearGrad.setColorAt( 0.50, QColor( 0, 0, 0, 0 ) );
        linearGrad.setColorAt( 1, QColor( 0, 0, 0, 48 ) );
    }
    painter.setBrush( linearGrad );
    painter.setPen( Qt::NoPen );
    painter.drawRect( x, y, width, height - 1 );
}

void Layer::mousePress( QMouseEvent* event, int frameNumber )
{
    Q_UNUSED( event );
    Q_UNUSED( frameNumber );
}

void Layer::mouseDoubleClick( QMouseEvent* event, int frameNumber )
{
    Q_UNUSED( event );
    Q_UNUSED( frameNumber );
}

void Layer::mouseMove( QMouseEvent* event, int frameNumber )
{
    Q_UNUSED( event );
    Q_UNUSED( frameNumber );
}

void Layer::mouseRelease( QMouseEvent* event, int frameNumber )
{
    Q_UNUSED( event );
    Q_UNUSED( frameNumber );
}

void Layer::editProperties()
{
    bool ok;
    QString text = QInputDialog::getText( NULL, tr( "Layer Properties" ),
                                          tr( "Layer name:" ), QLineEdit::Normal,
                                          mName, &ok );
    if ( ok && !text.isEmpty() )
    {
        mName = text;
    }
}

void Layer::setModified( int position, bool isModified )
{
    auto it = mKeyFrames.find( position );
    if ( it != mKeyFrames.end() )
    {
        KeyFrame* pKeyFrame = it->second;
        //pKeyFrame->
    }
}
