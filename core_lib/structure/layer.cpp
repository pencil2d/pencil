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
    m_pObject = pObject;
    m_eType = eType;
    id = 0;
    name = QString( tr( "Undefined Layer" ) );
    visible = true;

    Q_ASSERT( eType != UNDEFINED );
}

Layer::~Layer()
{
    for ( auto pair : m_KeyFrames )
    {
        KeyFrame* pKeyFrame = pair.second;
        delete pKeyFrame;
    }
    m_KeyFrames.clear();
}

void Layer::foreachKeyFrame( std::function<void( KeyFrame* )> action )
{
    for ( auto pair : m_KeyFrames )
    {
        action( pair.second );
    }
}


bool Layer::hasKeyFrameAtPosition( int position )
{
    return ( m_KeyFrames.find( position ) != m_KeyFrames.end() );
}

KeyFrame* Layer::getKeyFrameAtPosition( int position )
{
    auto it = m_KeyFrames.find( position );
    if ( it == m_KeyFrames.end() )
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
    auto it = m_KeyFrames.lower_bound( position );
    if ( it == m_KeyFrames.end() )
    {
        return NullKeyFrame::get();
    }
    return it->second;
}

int Layer::getPreviousKeyFramePosition( int position )
{
    auto it = m_KeyFrames.upper_bound( position );
    if ( it == m_KeyFrames.end() )
    {
        return getFirstKeyFramePosition();
    }
    return it->first;
}

int Layer::getNextKeyFramePosition( int position )
{
    auto it = m_KeyFrames.lower_bound( position );
    if ( it == m_KeyFrames.end() )
    {
        return getMaxKeyFramePosition();
    }

    if ( it != m_KeyFrames.begin() )
    {
        --it;
    }
    return it->first;
}

int Layer::getFirstKeyFramePosition()
{
    Q_ASSERT( m_KeyFrames.rbegin()->first == 1 );

    return m_KeyFrames.rbegin()->first; // rbegin is the lowest key frame position
}

int Layer::getMaxKeyFramePosition()
{
    return m_KeyFrames.begin()->first; // begin is the highest key frame position
}

bool Layer::addKeyFrame( int position, KeyFrame* pKeyFrame )
{
    auto it = m_KeyFrames.find( position );
    if ( it != m_KeyFrames.end() )
    {
        // key already exist.
        return false;
    }

    pKeyFrame->setPos( position );
    m_KeyFrames.insert( std::make_pair( position, pKeyFrame ) );

    return true;
}

bool Layer::removeKeyFrame( int position )
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

QDomElement Layer::createDomElement( QDomDocument& doc )
{
    QDomElement layerTag = doc.createElement( "layer" );
    layerTag.setAttribute( "name", name );
    layerTag.setAttribute( "visibility", visible );
    layerTag.setAttribute( "type", m_eType );

    qDebug() << "    Layer name=" << name << " visi=" << visible << " type=" << m_eType;
    return layerTag;
}

void Layer::loadDomElement( QDomElement element )
{
    name = element.attribute( "name" );
    visible = ( element.attribute( "visibility" ) == "1" );
    m_eType = static_cast< LAYER_TYPE >( element.attribute( "type" ).toInt() );
}

void Layer::paintTrack( QPainter& painter, TimeLineCells* cells, int x, int y, int width, int height, bool selected, int frameSize )
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

    for ( auto pair : m_KeyFrames )
    {
        int framePos = pair.first;
        KeyFrame* pKeyFrame = pair.second;

        if ( selected )
        {
            painter.setBrush( QColor( 60, 60, 60 ) );
            painter.drawRect( cells->getFrameX( framePos ) - frameSize + 2, y + 1, frameSize - 2, height - 4 );
        }
        else
        {
            //QColor color = ( selected )?
            /*
            if ( selected )
                painter.setBrush( QColor( 125, 125, 125 ) );
            else
                painter.setBrush( QColor( 125, 125, 125, 125 ) );
            */
            /*
            if ( framesModified.at( i ) )
            {
                painter.setBrush( QColor( 255, 125, 125, 125 ) );
            }
            */
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

    if ( visible )
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
    painter.drawText( QPoint( 45, y + ( 2 * height ) / 3 ), name );
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
                                          name, &ok );
    if ( ok && !text.isEmpty() )
    {
        name = text;
    }
}

void Layer::setModified( int position, bool isModified )
{
    auto it = m_KeyFrames.find( position );
    if ( it != m_KeyFrames.end() )
    {
        KeyFrame* pKeyFrame = it->second;
        //pKeyFrame->
    }
}
