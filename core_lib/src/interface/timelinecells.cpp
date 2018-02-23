/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2012-2018 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/

#include "timelinecells.h"

#include <QSettings>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QInputDialog>

#include "object.h"
#include "editor.h"
#include "timeline.h"
#include "layermanager.h"
#include "playbackmanager.h"
#include "preferencemanager.h"
#include "toolmanager.h"



TimeLineCells::TimeLineCells( TimeLine* parent, Editor* editor, TIMELINE_CELL_TYPE type ) : QWidget( parent )
{
    timeLine = parent;
    mEditor = editor;
    mPrefs = editor->preference();
    mType = type;

    mFrameLength = mPrefs->getInt(SETTING::TIMELINE_SIZE);
    mFontSize = mPrefs->getInt(SETTING::LABEL_FONT_SIZE);
    mFrameSize = mPrefs->getInt(SETTING::FRAME_SIZE);
    mbShortScrub = mPrefs->isOn(SETTING::SHORT_SCRUB);
    mDrawFrameNumber = mPrefs->isOn(SETTING::DRAW_LABEL);

    setMinimumSize( 500, 4 * mLayerHeight );
    setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding ) );
    setAttribute( Qt::WA_OpaquePaintEvent, false );

    connect( mPrefs, &PreferenceManager::optionChanged, this, &TimeLineCells::loadSetting );
}

TimeLineCells::~TimeLineCells()
{
    if ( mCache ) delete mCache;
}

void TimeLineCells::loadSetting(SETTING setting)
{
    switch (setting)
    {
    case SETTING::TIMELINE_SIZE:
        mFrameLength = mPrefs->getInt(SETTING::TIMELINE_SIZE);
        timeLine->updateLength();
        break;
    case SETTING::LABEL_FONT_SIZE:
        mFontSize = mPrefs->getInt(SETTING::LABEL_FONT_SIZE);
        break;
    case SETTING::FRAME_SIZE:
        mFrameSize = mPrefs->getInt(SETTING::FRAME_SIZE);
        timeLine->updateLength();
        break;
    case SETTING::SHORT_SCRUB:
        mbShortScrub = mPrefs->isOn(SETTING::SHORT_SCRUB);
        break;
    case SETTING::DRAW_LABEL:
        mDrawFrameNumber = mPrefs->isOn(SETTING::DRAW_LABEL);
        break;
    default:
        break;
    }
    updateContent();
}

int TimeLineCells::getFrameNumber( int x )
{
    int frameNumber = mFrameOffset + 1 + ( x - mOffsetX ) / mFrameSize;
    return frameNumber;
}

int TimeLineCells::getFrameX( int frameNumber )
{
    int x = mOffsetX + ( frameNumber - mFrameOffset ) * mFrameSize;
    return x;
}

int TimeLineCells::getLayerNumber( int y )
{
    int layerNumber = mLayerOffset + ( y - mOffsetY ) / mLayerHeight;

    // Layers numbers are displayed in descending order
    // The last row is layer 0
    if (layerNumber <= mEditor->object()->getLayerCount())
        layerNumber = mEditor->object()->getLayerCount() - 1 - layerNumber;
    else
        layerNumber = 0;

    if ( y < mOffsetY )
    {
        layerNumber = -1;
    }

    if ( layerNumber >= mEditor->object()->getLayerCount() )
    {
        layerNumber = mEditor->object()->getLayerCount();
    }

    //If the mouse release event if fired with mouse off the frame of the application
    // mEditor->object()->getLayerCount() doesn't return the correct value.
    if (layerNumber <-1)
    {
        layerNumber=-1;
    }


    return layerNumber;
}

int TimeLineCells::getLayerY( int layerNumber )
{
    //return offsetY + (layerNumber-layerOffset)*layerHeight;
    return mOffsetY + ( mEditor->object()->getLayerCount() - 1 - layerNumber - mLayerOffset )*mLayerHeight;
}

void TimeLineCells::updateFrame( int frameNumber )
{
    int x = getFrameX( frameNumber );
    update( x - mFrameSize, 0, mFrameSize + 1, height() );
}

void TimeLineCells::updateContent()
{
    drawContent();
    update();
}

void TimeLineCells::drawContent()
{
    if ( mCache == NULL )
    {
        mCache = new QPixmap( size() );
        if ( mCache->isNull() )
        {
            // fail to create cache
            return;
        }
    }

    QPainter painter( mCache );

    Object* object = mEditor->object();

    Q_ASSERT(object != nullptr);

    Layer* layer = object->getLayer( mEditor->layers()->currentLayerIndex() );
    if ( layer == NULL ) return;

    // grey background of the view
    painter.setPen( Qt::NoPen );
    painter.setBrush( Qt::lightGray );
    painter.drawRect( QRect( 0, 0, width(), height() ) );

    // --- draw layers of the current object
    for ( int i = 0; i < object->getLayerCount(); i++ )
    {
        if ( i != mEditor->layers()->currentLayerIndex() )
        {
            Layer* layeri = object->getLayer( i );
            if ( layeri != NULL )
            {
                switch ( mType )
                {
                case TIMELINE_CELL_TYPE::Tracks:
                    layeri->paintTrack( painter, this, mOffsetX,
                                        getLayerY( i ), width() - mOffsetX,
                                        getLayerHeight(), false, mFrameSize );
                    break;

                case TIMELINE_CELL_TYPE::Layers:
                    layeri->paintLabel( painter, this, 0,
                                        getLayerY( i ), width() - 1,
                                        getLayerHeight(), false, mEditor->allLayers() );
                    break;
                }
            }
        }
    }
    if ( abs( getMouseMoveY() ) > 5 )
    {
        if ( mType == TIMELINE_CELL_TYPE::Tracks )
        {
            layer->paintTrack( painter, this, mOffsetX, getLayerY( mEditor->layers()->currentLayerIndex() ) + getMouseMoveY(), width() - mOffsetX, getLayerHeight(), true, mFrameSize );
        }
        if ( mType == TIMELINE_CELL_TYPE::Layers )
        {
            layer->paintLabel( painter, this, 0, getLayerY( mEditor->layers()->currentLayerIndex() ) + getMouseMoveY(), width() - 1, getLayerHeight(), true, mEditor->allLayers() );
        }
        painter.setPen( Qt::black );
        painter.drawRect( 0, getLayerY( getLayerNumber( mEndY ) ) - 1, width(), 2 );
    }
    else
    {
        if ( mType == TIMELINE_CELL_TYPE::Tracks )
        {
            layer->paintTrack( painter,
                               this,
                               mOffsetX,
                               getLayerY( mEditor->layers()->currentLayerIndex() ),
                               width() - mOffsetX,
                               getLayerHeight(),
                               true,
                               mFrameSize );
        }
        if ( mType == TIMELINE_CELL_TYPE::Layers )
        {
            layer->paintLabel( painter,
                               this, 
                               0, 
                               getLayerY( mEditor->layers()->currentLayerIndex() ),
                               width() - 1,
                               getLayerHeight(),
                               true,
                               mEditor->allLayers() );
        }
    }

    // --- draw top
    painter.setPen( Qt::NoPen );
    painter.setBrush( QColor( 220, 220, 220 ) );
    painter.drawRect( QRect( 0, 0, width() - 1, mOffsetY - 1 ) );
    painter.setPen( Qt::gray );
    painter.drawLine( 0, 0, width() - 1, 0 );
    painter.drawLine( 0, mOffsetY - 2, width() - 1, mOffsetY - 2 );
    painter.setPen( Qt::lightGray );
    painter.drawLine( 0, mOffsetY - 3, width() - 1, mOffsetY - 3 );
    painter.drawLine( 0, 0, 0, mOffsetY - 3 );

    if ( mType == TIMELINE_CELL_TYPE::Layers )
    {
        // --- draw circle
        painter.setPen( Qt::black );
        if ( mEditor->allLayers() == 0 ) { painter.setBrush( Qt::NoBrush ); }
        if ( mEditor->allLayers() == 1 ) { painter.setBrush( Qt::darkGray ); }
        if ( mEditor->allLayers() == 2 ) { painter.setBrush( Qt::black ); }
        painter.setRenderHint( QPainter::Antialiasing, true );
        painter.drawEllipse( 6, 4, 9, 9 );
        painter.setRenderHint( QPainter::Antialiasing, false );
    }
    else if ( mType == TIMELINE_CELL_TYPE::Tracks )
    {
        // --- draw ticks
        painter.setPen( QColor( 70, 70, 70, 255 ) );
        painter.setBrush( Qt::darkGray );
        int incr = 0;
        int fps = mEditor->playback()->fps();
        for ( int i = mFrameOffset; i < mFrameOffset + ( width() - mOffsetX ) / mFrameSize; i++ )
        {
            incr = ( i < 9 ) ? 4 : 0;

            if(i + 1 >= timeLine->getRangeLower() && i < timeLine->getRangeUpper())
            {
                painter.setPen( Qt::NoPen );
                painter.setBrush( Qt::yellow );

                painter.drawRect( getFrameX( i ), 1, mFrameSize + 1, 2 );

                painter.setPen( QColor( 70, 70, 70, 255 ) );
                painter.setBrush( Qt::darkGray );
            }

            if ( i%fps == 0 )
            {
                painter.drawLine( getFrameX( i ), 1, getFrameX( i ), 5 );
            }
            else if ( i%fps == fps / 2 )
            {
                painter.drawLine( getFrameX( i ), 1, getFrameX( i ), 5 );
            }
            else
            {
                painter.drawLine( getFrameX( i ), 1, getFrameX( i ), 3 );
            }
            if ( i == 0 || i%fps == fps - 1 )
            {
                painter.drawText( QPoint( getFrameX( i ) + incr, 15 ), QString::number( i + 1 ) );
            }
        }

        // --- draw left border line
        painter.setPen( Qt::darkGray );
        painter.drawLine( 0, 0, 0, height() );
    }
}

void TimeLineCells::paintOnionSkin( QPainter& painter )
{
    Layer* layer = mEditor->layers()->currentLayer();
    if (layer == nullptr) {
        return;
    }

    int frameNumber = mEditor->currentFrame();

    int prevOnionSkinCount = mEditor->preference()->getInt(SETTING::ONION_PREV_FRAMES_NUM);
    int nextOnionSkinCount = mEditor->preference()->getInt(SETTING::ONION_NEXT_FRAMES_NUM);

    bool isAbsolute = (mEditor->preference()->getString(SETTING::ONION_TYPE) == "absolute");

    if (mEditor->preference()->isOn(SETTING::PREV_ONION) && prevOnionSkinCount > 0) {


        int onionFrameNumber = layer->getPreviousFrameNumber(frameNumber, isAbsolute);
        int onionPosition = 0;

        while (onionPosition < prevOnionSkinCount && onionFrameNumber > 0)
        {
            painter.setBrush( QColor( 128, 128, 128, 128 ) );
            painter.setPen( Qt::NoPen );
            QRect onionRect;
            onionRect.setTopLeft( QPoint( getFrameX( onionFrameNumber - 1 ), 0 ) );
            onionRect.setBottomRight( QPoint( getFrameX( onionFrameNumber ), height() ) );
            onionRect.setBottomRight( QPoint( getFrameX( onionFrameNumber ), 19 ) );
            painter.drawRect( onionRect );

            onionFrameNumber = layer->getPreviousFrameNumber(onionFrameNumber, isAbsolute);
            onionPosition++;
        }
    }

    if (mEditor->preference()->isOn(SETTING::NEXT_ONION) && nextOnionSkinCount > 0) {

        int onionFrameNumber = layer->getNextFrameNumber(frameNumber, isAbsolute);
        int onionPosition = 0;

        while (onionPosition < nextOnionSkinCount && onionFrameNumber > 0)
        {
            painter.setBrush( QColor( 128, 128, 128, 128 ) );
            painter.setPen( Qt::NoPen );
            QRect onionRect;
            onionRect.setTopLeft( QPoint( getFrameX( onionFrameNumber - 1 ), 0 ) );
            onionRect.setBottomRight( QPoint( getFrameX( onionFrameNumber ), height() ) );
            onionRect.setBottomRight( QPoint( getFrameX( onionFrameNumber ), 19 ) );
            painter.drawRect( onionRect );

            onionFrameNumber = layer->getNextFrameNumber(onionFrameNumber, isAbsolute);
            onionPosition++;
        }
    }
}

void TimeLineCells::paintEvent( QPaintEvent* event )
{
    Q_UNUSED( event );

    Object* object = mEditor->object();
    Layer* layer = mEditor->layers()->currentLayer();

    Q_ASSUME( object != nullptr && layer != nullptr );

    QPainter painter( this );

    bool isPlaying = mEditor->playback()->isPlaying();
    if ( ( !isPlaying && !timeLine->scrubbing ) || mCache == NULL )
    {
        drawContent();
    }
    if ( mCache )
    {
        painter.drawPixmap( QPoint( 0, 0 ), *mCache );
    }

    if ( mType == TIMELINE_CELL_TYPE::Tracks )
    {
        if (!isPlaying) {
            paintOnionSkin(painter);
        }

        // --- draw the position of the current frame
        if ( mEditor->currentFrame() > mFrameOffset )
        {
            painter.setBrush( QColor( 255, 0, 0, 128 ) );
            painter.setPen( Qt::NoPen );
            //painter.setCompositionMode(QPainter::CompositionMode_Source); // this causes the message: QPainter::setCompositionMode: PorterDuff modes not supported on device
            QRect scrubRect;
            scrubRect.setTopLeft( QPoint( getFrameX( mEditor->currentFrame() - 1 ), 0 ) );
            scrubRect.setBottomRight( QPoint( getFrameX( mEditor->currentFrame() ), height() ) );
            if ( mbShortScrub )
            {
                scrubRect.setBottomRight( QPoint( getFrameX( mEditor->currentFrame() ), 19 ) );
            }
            painter.drawRect( scrubRect );
            painter.setPen( QColor( 70, 70, 70, 255 ) );
            int incr = 0;
            if ( mEditor->currentFrame() < 10 )
            {
                incr = 4;
            }
            else { incr = 0; }
            painter.drawText( QPoint( getFrameX( mEditor->currentFrame() - 1 ) + incr, 15 ),
                              QString::number( mEditor->currentFrame() ) );
        }
    }
}

void TimeLineCells::resizeEvent( QResizeEvent* event )
{
    clearCache();
    updateContent();
    event->accept();
    emit lengthChanged( getFrameLength() );
}

void TimeLineCells::mousePressEvent( QMouseEvent* event )
{
    if ( primaryButton != Qt::NoButton ) return;
    int frameNumber = getFrameNumber( event->pos().x() );
    int layerNumber = getLayerNumber( event->pos().y() );

    mStartY = event->pos().y();
    mStartLayerNumber = layerNumber;
    mEndY = event->pos().y();

    mStartFrameNumber = frameNumber;
    mLastFrameNumber = mStartFrameNumber;

    mCanMoveFrame    = false;
    mMovingFrames    = false;

    mCanBoxSelect    = false;
    mBoxSelecting    = false;

    mClickSelecting  = false;

    primaryButton = event->button();

    mEditor->tools()->currentTool()->switchingLayers();
    switch ( mType )
    {
    case TIMELINE_CELL_TYPE::Layers:
        if ( layerNumber != -1 && layerNumber < mEditor->object()->getLayerCount() )
        {
            if ( event->pos().x() < 15 )
            {
                mEditor->switchVisibilityOfLayer( layerNumber );
            }
            else
            {
                mEditor->layers()->setCurrentLayer( layerNumber );
            }
        }
        if ( layerNumber == -1 )
        {
            if ( event->pos().x() < 15 )
            {
                mEditor->toggleShowAllLayers();
            }
        }
        break;
    case TIMELINE_CELL_TYPE::Tracks:
        if ( event->button() == Qt::MidButton )
        {
            mLastFrameNumber = getFrameNumber( event->pos().x() );
        }
        else
        {
            if ( frameNumber == mEditor->currentFrame() && ( !mbShortScrub || ( mbShortScrub && mStartY < 20 ) ) )
            {
                if (mEditor->playback()->isPlaying())
                {
                    mEditor->playback()->stop();
                }
                timeLine->scrubbing = true;
            }
            else
            {
                if ( ( layerNumber != -1 ) && layerNumber < mEditor->object()->getLayerCount() )
                {
                    int previousLayerNumber = mEditor->layers()->currentLayerIndex();

                    if (previousLayerNumber != layerNumber) {
                        Layer *previousLayer = mEditor->object()->getLayer(previousLayerNumber);
                        previousLayer->deselectAll();

                        mEditor->layers()->setCurrentLayer( layerNumber );
                    }

                    Layer *currentLayer = mEditor->object()->getLayer(layerNumber);


                    // Check if we are using the alt key
                    //
                    if (event->modifiers() == Qt::AltModifier) {

                        // If it is the case, we select everything that is after the selected frame
                        //
                        mClickSelecting = true;
                        mCanMoveFrame = true;

                        currentLayer->selectAllFramesAfter(frameNumber);

                    }
                    // Check if we are clicking on a non selected frame
                    //
                    else if (!currentLayer->isFrameSelected(frameNumber)) {

                        // If it is the case, we select it
                        //
                        mCanBoxSelect = true;
                        mClickSelecting = true;

                        if ( event->modifiers() == Qt::ControlModifier ) {
                            // Add/remove from already selected
                            currentLayer->toggleFrameSelected(frameNumber, true);
                        }
                        else if ( event->modifiers() == Qt::ShiftModifier ) {
                            // Select a range from the last selected
                            currentLayer->extendSelectionTo(frameNumber);
                        }
                        else {
                            currentLayer->toggleFrameSelected(frameNumber, false);
                        }
                    }
                    else {

                        // We clicked on a selected frame, we can move it
                        //
                        mCanMoveFrame = true;
                    }

                    currentLayer->mousePress( event, frameNumber );
                    timeLine->updateContent();
                }
                else
                {
                    if ( frameNumber > 0 )
                    {
                        if (mEditor->playback()->isPlaying())
                        {
                            mEditor->playback()->stop();
                        }

                        mEditor->scrubTo( frameNumber );

                        timeLine->scrubbing = true;
                        qDebug( "Scrub to %d frame", frameNumber );
                    }
                }
            }
        }
        break;
    }
}

void TimeLineCells::mouseMoveEvent( QMouseEvent* event )
{
    if ( mType == TIMELINE_CELL_TYPE::Layers )
    {
        mEndY = event->pos().y();
        emit mouseMovedY( mEndY - mStartY );
    }
    int frameNumber = getFrameNumber( event->pos().x() );

    if ( mType == TIMELINE_CELL_TYPE::Tracks )
    {
        if ( primaryButton == Qt::MidButton )
        {
            // qMin( max_frame_offset, qMax ( min_frame_offset, draw_frame_offset ) )
            mFrameOffset = qMin( qMax( 0, mFrameLength - width() / getFrameSize() ), qMax( 0, mFrameOffset + mLastFrameNumber - frameNumber ) );
            update();
            emit offsetChanged( mFrameOffset );
        }
        else
        {
            if ( timeLine->scrubbing )
            {
                mEditor->scrubTo( frameNumber );
            }
            else
            {
                if ( mStartLayerNumber != -1 && mStartLayerNumber < mEditor->object()->getLayerCount() )
                {
                    Layer *currentLayer = mEditor->object()->getLayer(mStartLayerNumber);

                    // Did we move to another frame ?
                    if ( frameNumber != mLastFrameNumber )
                    {
                        // Check if the frame we clicked was selected
                        if ( mCanMoveFrame ) {

                            // If it is the case, we move the selected frames in the layer
                            mMovingFrames = true;

                            int offset = frameNumber - mLastFrameNumber;
                            currentLayer->moveSelectedFrames(offset);
                            mEditor->updateCurrentFrame();
                        }
                        else if ( mCanBoxSelect )
                        {
                            // Otherwise, we do a box select
                            mBoxSelecting = true;

                            currentLayer->deselectAll();
                            currentLayer->setFrameSelected(mStartFrameNumber, true);
                            currentLayer->extendSelectionTo(frameNumber);
                        }
                        mLastFrameNumber = frameNumber;
                    }
                    currentLayer->mouseMove( event, frameNumber );
                }
            }
        }
    }
    timeLine->update();
}

void TimeLineCells::mouseReleaseEvent( QMouseEvent* event )
{
    qDebug( "TimeLineCell: mouse release event." );
    if ( event->button() != primaryButton ) return;

    primaryButton = Qt::NoButton;
    mEndY = mStartY;
    emit mouseMovedY( 0 );
    timeLine->scrubbing = false;
    int frameNumber = getFrameNumber( event->pos().x() );
    if ( frameNumber < 1 ) frameNumber = -1;
    int layerNumber = getLayerNumber( event->pos().y() );
    if ( mType == TIMELINE_CELL_TYPE::Tracks && primaryButton != Qt::MidButton && layerNumber != -1 && layerNumber < mEditor->object()->getLayerCount() )
    {
        Layer *currentLayer = mEditor->object()->getLayer(layerNumber);

        if (!timeLine->scrubbing && !mMovingFrames && !mClickSelecting && !mBoxSelecting )
        {
            // De-selecting if we didn't move, scrub nor select anything
            bool multipleSelection = (event->modifiers() == Qt::ControlModifier);
            
            // Add/remove from already selected
            currentLayer->toggleFrameSelected(frameNumber, multipleSelection);
        }

        currentLayer->mouseRelease( event, frameNumber );
    }
    if ( mType == TIMELINE_CELL_TYPE::Layers && layerNumber != mStartLayerNumber && mStartLayerNumber != -1 && layerNumber != -1 )
    {
        mEditor->moveLayer( mStartLayerNumber, layerNumber );
    }
    timeLine->updateContent();
}

void TimeLineCells::mouseDoubleClickEvent( QMouseEvent* event )
{
    int frameNumber = getFrameNumber( event->pos().x() );
    int layerNumber = getLayerNumber( event->pos().y() );

    // -- short scrub --
    if ( event->pos().y() < 20 )
    {
        mPrefs->set(SETTING::SHORT_SCRUB, !mbShortScrub);
    }

    // -- layer --
    Layer* layer = mEditor->object()->getLayer( layerNumber );
    if ( layer )
    {
        if ( mType == TIMELINE_CELL_TYPE::Tracks && ( layerNumber != -1 ) && ( frameNumber > 0 ) && layerNumber < mEditor->object()->getLayerCount() )
        {
            mEditor->object()->getLayer( layerNumber )->mouseDoubleClick( event, frameNumber );
        }
        else if ( mType == TIMELINE_CELL_TYPE::Layers )
        {
            if (layer->type() == Layer::CAMERA)
            {
                layer->editProperties();
            }
            else
            {
                QRegExp regex("([\\xFFEF-\\xFFFF])+");

                bool ok;
                QString text = QInputDialog::getText(NULL, tr("Layer Properties"),
                                                     tr("Layer name:"), QLineEdit::Normal,
                                                     layer->name(), &ok);
                if (ok && !text.isEmpty())
                {
                    text.replace(regex, "");
                    mEditor->layers()->renameLayer(layer, text);
                }
            }
        }
    }
}

void TimeLineCells::hScrollChange( int x )
{
    mFrameOffset = x;
    update();
}

void TimeLineCells::vScrollChange( int x )
{
    mLayerOffset = x;
    update();
}

void TimeLineCells::setMouseMoveY( int x )
{
    mMouseMoveY = x;
    if ( x == 0 )
    {
        update();
    }
}
