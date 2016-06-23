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
#include "keyframefactory.h"
#include "layer.h"
#include "object.h"
#include "timeline.h"
#include "timelinecells.h"

// Used to sort the selected frames list
//
bool sortAsc(int left, int right)
{
    return left < right;
}

Layer::Layer( Object* pObject, LAYER_TYPE eType ) : QObject( pObject )
{
    mObject = pObject;
    meType = eType;
    mName = QString( tr( "Undefined Layer" ) );
    
    mId = pObject->getUniqueLayerID();

    //addNewEmptyKeyAt( 1 );

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

KeyFrame* Layer::getKeyFrameAt( int position )
{
    auto it = mKeyFrames.find( position );
    if ( it == mKeyFrames.end() )
    {
        //return NullKeyFrame::get();
        return nullptr;
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

int Layer::getPreviousFrameNumber( int position, bool isAbsolute )
{
    int prevNumber;

    if (isAbsolute) {
        prevNumber = getPreviousKeyFramePosition(position);
    }
    else {
        prevNumber = position - 1;
    }


    if (prevNumber == position)
    {
        return -1; // There is no previous keyframe
    }
    else
    {
        return prevNumber;
    }
}

int Layer::getNextFrameNumber( int position, bool isAbsolute )
{
    int nextNumber;

    if (isAbsolute) {
        nextNumber = getNextKeyFramePosition(position);
    }
    else {
        nextNumber = position + 1;
    }


    if (nextNumber == position) {
        return -1; // There is no next keyframe
    }
    else {
        return nextNumber;
    }
}

int Layer::firstKeyFramePosition()
{
    if ( !mKeyFrames.empty() )
    {
        return mKeyFrames.rbegin()->first; // rbegin is the lowest key frame position
    }
    return 0;
}

int Layer::getMaxKeyFramePosition()
{
    if ( !mKeyFrames.empty() )
    {
        return mKeyFrames.begin()->first; // begin is the highest key frame position
    }
    return 0;
}

bool Layer::addNewEmptyKeyAt( int position )
{
    if ( position <= 0 )
    {
        return false;
    }
    KeyFrame* key = KeyFrameFactory::create( meType, mObject );
    if ( key == nullptr )
    {
        return false;
    }
    return addKeyFrame( position, key );
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
    auto it = mKeyFrames.find( position );
    if ( it != mKeyFrames.end() )
    {
        delete it->second;
        mKeyFrames.erase( it );
    }

    return true;
}

bool Layer::moveKeyFrameForward( int position )
{
    return swapKeyFrames( position, position + 1 );
}

bool Layer::moveKeyFrameBackward( int position )
{
    if ( position != 1 )
    {
        return swapKeyFrames( position, position - 1 );
    }
    else
    {
        return true;
    }
}

bool Layer::swapKeyFrames( int position1, int position2 ) //Current behaviour, need to refresh the swapped cels
{
    bool keyPosition1 = false, keyPosition2 = false;
    KeyFrame* pFirstFrame  = nullptr;
    KeyFrame* pSecondFrame = nullptr;

    if ( keyExists( position1 ) )
    {
        auto firstFrame = mKeyFrames.find( position1 );
        pFirstFrame = firstFrame->second;

        mKeyFrames.erase( position1 );

        //pFirstFrame = getKeyFrameAt( position1 );
        //removeKeyFrame( position1 );

        keyPosition1 = true;
    }

	if ( keyExists( position2 ) )
    {
        auto secondFrame = mKeyFrames.find( position2 );
        pSecondFrame = secondFrame->second;

        mKeyFrames.erase( position2 );

        //pSecondFrame = getKeyFrameAt( position2 );
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
        addNewEmptyKeyAt( position1 );
    }

    if ( keyPosition1 )
    {
        //addKeyFrame( position2, pFirstFrame );
        pFirstFrame->setPos( position2 );
        mKeyFrames.insert( std::make_pair( position2, pFirstFrame ) );
    } 
	else if ( position2 == 1 )
	{
		addNewEmptyKeyAt( position2 );
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
        if ( type() == BITMAP ) col = QColor( 151, 176, 244 );
        if ( type() == VECTOR ) col = QColor( 150, 242, 150 );
        if ( type() == SOUND ) col = QColor( 237, 147, 147 );
        if ( type() == CAMERA ) col = QColor( 239, 232, 148 );

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

    //qDebug() << "LayerType:" << ( int )( meType );

    for ( auto pair : mKeyFrames )
    {
        int framePos = pair.first;
        
        int recLeft = cells->getFrameX( framePos ) - frameSize + 2;
        int recTop = y + 1;
        int recWidth = frameSize - 2;
        int recHeight = height - 4;

        KeyFrame* key = pair.second;
        if ( key->length() > 1 )  
        {
            // This is especially for sound clip.
            // Sound clip is the only type of KeyFrame that has variant frame length.
            recWidth = frameSize * key->length() - 2;
        }

        if ( pair.second->isSelected() )
        {
            painter.setBrush( QColor( 60, 60, 60 ) );
        }
        else if ( selected )
        {
            painter.setBrush( QColor( 60, 60, 60, 120 ) );
        }

        painter.drawRect( recLeft, recTop, recWidth, recHeight );
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
    linearGrad.setColorAt( 0, QColor( 255, 255, 255, 128 ) );
    linearGrad.setColorAt( 0.50, QColor( 255, 255, 255, 64 ) );
    linearGrad.setColorAt( 1, QColor( 255, 255, 255, 0 ) );
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
        setUpdated();
    }
}

void Layer::setUpdated()
{
    mObject->setLayerUpdated(mId);
}

void Layer::setModified( int position, bool )
{
    auto it = mKeyFrames.find( position );
    if ( it != mKeyFrames.end() )
    {
        //KeyFrame* pKeyFrame = it->second;
        //pKeyFrame->
    }
}

bool Layer::isFrameSelected(int position)
{
    return mSelectedFrames_byLast.contains(position);
}

void Layer::setFrameSelected(int position, bool isSelected)
{
    KeyFrame *keyFrame = getKeyFrameAt(position);
    if (keyFrame != nullptr) {
        if (isSelected && !mSelectedFrames_byLast.contains(position)) {

            // Add the selected frame to the lists
            //
            mSelectedFrames_byLast.insert(0, position);
            mSelectedFrames_byPosition.append(position);

            // We need to keep the list of selected frames sorted
            // in order to easily handle their movement
            //
            qSort(mSelectedFrames_byPosition.begin(), mSelectedFrames_byPosition.end(), sortAsc);

        }
        else if (!isSelected){

            // Remove the selected frame from the lists
            //
            int iLast = mSelectedFrames_byLast.indexOf(position);
            mSelectedFrames_byLast.removeAt(iLast);

            int iPos = mSelectedFrames_byPosition.indexOf(position);
            mSelectedFrames_byPosition.removeAt(iPos);
        }
        keyFrame->setSelected(isSelected);
    }
}

void Layer::toggleFrameSelected(int position, bool allowMultiple)
{
    bool wasSelected = isFrameSelected(position);

    if (!allowMultiple) {
        deselectAll();
    }

    setFrameSelected(position, !wasSelected);
}

void Layer::extendSelectionTo(int position)
{
    if (mSelectedFrames_byLast.count() > 0 ) {
        int lastSelected = mSelectedFrames_byLast[0];
        int startPos;
        int endPos;

        if (lastSelected < position) {
            startPos = lastSelected;
            endPos = position;
        }
        else {
            startPos = position;
            endPos = lastSelected;
        }

        int i = startPos;
        while (i <= endPos) {
            setFrameSelected(i, true);
            i++;
        }
    }
}

void Layer::selectAllFramesAfter( int position )
{
    int startPosition = position;
    int endPosition = getMaxKeyFramePosition();

    if (!keyExists(startPosition)) {
        startPosition = getNextKeyFramePosition(startPosition);
    }

    if (startPosition > 0 && startPosition <= endPosition ) {
        deselectAll();
        setFrameSelected(startPosition, true);
        extendSelectionTo(endPosition);
    }
}

void Layer::deselectAll()
{
    mSelectedFrames_byLast.clear();
    mSelectedFrames_byPosition.clear();

    for ( auto pair : mKeyFrames )
    {
        pair.second->setSelected(false);
    }
}

bool Layer::moveSelectedFrames(int offset)
{

    if (offset != 0 && mSelectedFrames_byPosition.count() > 0) {

        // If we are moving to the right we start moving selected frames from the highest (right) to the lowest (left)
        int indexInSelection = mSelectedFrames_byPosition.count() - 1;
        int step = -1;

        if (offset < 0) {

            // If we are moving to the left we start moving selected frames from the lowest (left) to the highest (right)
            indexInSelection = 0;
            step = 1;

            // Check if we are not moving out of the timeline
            if (mSelectedFrames_byPosition[0] + offset < 1) {
                return false;
            }
        }


        while ( indexInSelection > -1 && indexInSelection < mSelectedFrames_byPosition.count() ) {

            int fromPos = mSelectedFrames_byPosition[indexInSelection];
            int toPos = fromPos + offset;

            // Get the frame to move
            KeyFrame *selectedFrame = getKeyFrameAt(fromPos);

            if (selectedFrame != nullptr) {

                mKeyFrames.erase(fromPos);

                // Slide back every frame between fromPos to toPos
                // to avoid having 2 frames in the same position
                //
                bool isBetween = true;
                int targetPosition = fromPos;

                while (isBetween) {

                    int framePosition = targetPosition - step;

                    KeyFrame *frame = getKeyFrameAt(framePosition);

                    if (frame != nullptr) {
                        mKeyFrames.erase(framePosition);

                        frame->setPos(targetPosition);
                        mKeyFrames.insert( std::make_pair( targetPosition, frame ) );
                    }

                    targetPosition = targetPosition - step;
                    if (fromPos < toPos && (targetPosition < fromPos || targetPosition >= toPos)) {
                        isBetween = false;
                    }
                    if (fromPos > toPos && (targetPosition > fromPos || targetPosition <= toPos)) {
                        isBetween = false;
                    }
                }

                // If the first frame is moving, we need to create a new first frame
                if (fromPos == 1) {
                    addNewEmptyKeyAt(1);
                }

                // Update the position of the selected frame
                selectedFrame->setPos(toPos);
                mKeyFrames.insert( std::make_pair( toPos, selectedFrame ) );
            }

            indexInSelection = indexInSelection + step;
        }


        // Update selection lists
        //
        for (int i=0; i<mSelectedFrames_byPosition.count(); i++) {
            mSelectedFrames_byPosition[i] = mSelectedFrames_byPosition[i] + offset;
        }
        for (int i=0; i<mSelectedFrames_byLast.count(); i++) {
            mSelectedFrames_byLast[i] = mSelectedFrames_byLast[i] + offset;
        }

        return true;
    }
    else {
        return false;
    }

}

bool isLayerPaintable( Layer* layer )
{
    Q_ASSERT( layer != nullptr );

    switch ( layer->type() )
    {
        case Layer::BITMAP:
        case Layer::VECTOR:
            return true;
        default:
            break;
    }
    return false;
}
