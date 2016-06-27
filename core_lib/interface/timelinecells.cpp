#include "timelinecells.h"

#include <QSettings>
#include <QResizeEvent>
#include <QMouseEvent>
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
    m_eType = type;

    m_pCache = NULL;

    frameLength = mPrefs->getInt(SETTING::TIMELINE_SIZE);
    fontSize = mPrefs->getInt(SETTING::LABEL_FONT_SIZE);
    frameSize = mPrefs->getInt(SETTING::FRAME_SIZE);
    shortScrub = mPrefs->isOn(SETTING::SHORT_SCRUB);
    drawFrameNumber = mPrefs->isOn(SETTING::DRAW_LABEL);

    startY = 0;
    endY = 0;
    mouseMoveY = 0;
    startLayerNumber = -1;
    frameOffset = 0;
    layerOffset = 0;
    layerHeight = 20;


    setMinimumSize( 500, 4 * layerHeight );
    setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding ) );
    setAttribute( Qt::WA_OpaquePaintEvent, false );

    connect( mPrefs, &PreferenceManager::optionChanged, this, &TimeLineCells::loadSetting );
}

void TimeLineCells::loadSetting(SETTING setting)
{
    switch (setting)
    {
    case SETTING::TIMELINE_SIZE:
        frameLength = mPrefs->getInt(SETTING::TIMELINE_SIZE);
        timeLine->updateLength( frameLength );
        break;
    case SETTING::LABEL_FONT_SIZE:
        fontSize = mPrefs->getInt(SETTING::LABEL_FONT_SIZE);
        break;
    case SETTING::FRAME_SIZE:
        frameSize = mPrefs->getInt(SETTING::FRAME_SIZE);
        break;
    case SETTING::SHORT_SCRUB:
        shortScrub = mPrefs->isOn(SETTING::SHORT_SCRUB);
        break;
    case SETTING::DRAW_LABEL:
        drawFrameNumber = mPrefs->isOn(SETTING::DRAW_LABEL);
        break;
    default:
        break;
    }
    updateContent();
}

int TimeLineCells::getFrameNumber( int x )
{
    int frameNumber = frameOffset + 1 + ( x - m_offsetX ) / frameSize;
    return frameNumber;
}

int TimeLineCells::getFrameX( int frameNumber )
{
    int x = m_offsetX + ( frameNumber - frameOffset ) * frameSize;
    return x;
}

int TimeLineCells::getLayerNumber( int y )
{
    int layerNumber = layerOffset + ( y - m_offsetY ) / layerHeight;

    // Layers numbers are displayed in descending order
    // The last row is layer 0
    if (layerNumber <= mEditor->object()->getLayerCount())
        layerNumber = mEditor->object()->getLayerCount() - 1 - layerNumber;
    else
        layerNumber = 0;

    if ( y < m_offsetY )
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
    return m_offsetY + ( mEditor->object()->getLayerCount() - 1 - layerNumber - layerOffset )*layerHeight;
}

void TimeLineCells::updateFrame( int frameNumber )
{
    int x = getFrameX( frameNumber );
    update( x - frameSize, 0, frameSize + 1, height() );
}

void TimeLineCells::updateContent()
{
    drawContent();
    update();
}

void TimeLineCells::drawContent()
{
    if ( m_pCache == NULL )
    {
        m_pCache = new QPixmap( size() );
        if ( m_pCache->isNull() )
        {
            // fail to create cache
            return;
        }
    }

    QPainter painter( m_pCache );

    Object* object = mEditor->object();

    Q_ASSERT_X( object != nullptr, "", "" );

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
                switch ( m_eType )
                {
                case TIMELINE_CELL_TYPE::Tracks:
                    layeri->paintTrack( painter, this, m_offsetX,
                                        getLayerY( i ), width() - m_offsetX,
                                        getLayerHeight(), false, frameSize );
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
        if ( m_eType == TIMELINE_CELL_TYPE::Tracks )
        {
            layer->paintTrack( painter, this, m_offsetX, getLayerY( mEditor->layers()->currentLayerIndex() ) + getMouseMoveY(), width() - m_offsetX, getLayerHeight(), true, frameSize );
        }
        if ( m_eType == TIMELINE_CELL_TYPE::Layers )
        {
            layer->paintLabel( painter, this, 0, getLayerY( mEditor->layers()->currentLayerIndex() ) + getMouseMoveY(), width() - 1, getLayerHeight(), true, mEditor->allLayers() );
        }
        painter.setPen( Qt::black );
        painter.drawRect( 0, getLayerY( getLayerNumber( endY ) ) - 1, width(), 2 );
    }
    else
    {
        if ( m_eType == TIMELINE_CELL_TYPE::Tracks )
        {
            layer->paintTrack( painter,
                               this,
                               m_offsetX,
                               getLayerY( mEditor->layers()->currentLayerIndex() ),
                               width() - m_offsetX,
                               getLayerHeight(),
                               true,
                               frameSize );
        }
        if ( m_eType == TIMELINE_CELL_TYPE::Layers )
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
    painter.drawRect( QRect( 0, 0, width() - 1, m_offsetY - 1 ) );
    painter.setPen( Qt::gray );
    painter.drawLine( 0, 0, width() - 1, 0 );
    painter.drawLine( 0, m_offsetY - 2, width() - 1, m_offsetY - 2 );
    painter.setPen( Qt::lightGray );
    painter.drawLine( 0, m_offsetY - 3, width() - 1, m_offsetY - 3 );
    painter.drawLine( 0, 0, 0, m_offsetY - 3 );

    if ( m_eType == TIMELINE_CELL_TYPE::Layers )
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
    else if ( m_eType == TIMELINE_CELL_TYPE::Tracks )
    {
        // --- draw ticks
        painter.setPen( QColor( 70, 70, 70, 255 ) );
        painter.setBrush( Qt::darkGray );
        painter.setFont( QFont( "helvetica", 10 ) );
        int incr = 0;
        int fps = mEditor->playback()->fps();
        for ( int i = frameOffset; i < frameOffset + ( width() - m_offsetX ) / frameSize; i++ )
        {
            incr = ( i < 9 ) ? 4 : 0;

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
    if ( ( !isPlaying && !timeLine->scrubbing ) || m_pCache == NULL )
    {
        drawContent();
    }
    if ( m_pCache )
    {
        painter.drawPixmap( QPoint( 0, 0 ), *m_pCache );
    }

    if ( m_eType == TIMELINE_CELL_TYPE::Tracks )
    {
        if (!isPlaying) {
            paintOnionSkin(painter);
        }

        // --- draw the position of the current frame
        if ( mEditor->currentFrame() > frameOffset )
        {
            painter.setBrush( QColor( 255, 0, 0, 128 ) );
            painter.setPen( Qt::NoPen );
            painter.setFont( QFont( "helvetica", 10 ) );
            //painter.setCompositionMode(QPainter::CompositionMode_Source); // this causes the message: QPainter::setCompositionMode: PorterDuff modes not supported on device
            QRect scrubRect;
            scrubRect.setTopLeft( QPoint( getFrameX( mEditor->currentFrame() - 1 ), 0 ) );
            scrubRect.setBottomRight( QPoint( getFrameX( mEditor->currentFrame() ), height() ) );
            if ( shortScrub )
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
    if ( m_pCache ) delete m_pCache;
    m_pCache = new QPixmap( size() );
    updateContent();
    event->accept();
}

void TimeLineCells::mousePressEvent( QMouseEvent* event )
{
    int frameNumber = getFrameNumber( event->pos().x() );
    int layerNumber = getLayerNumber( event->pos().y() );

    startY = event->pos().y();
    startLayerNumber = layerNumber;
    endY = event->pos().y();

    startFrameNumber = frameNumber;
    lastFrameNumber = startFrameNumber;

    canMoveFrame    = false;
    movingFrames    = false;

    canBoxSelect    = false;
    boxSelecting    = false;

    clickSelecting  = false;

    mEditor->tools()->currentTool()->switchingLayers();
    switch ( m_eType )
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
        if ( frameNumber == mEditor->currentFrame() && ( !shortScrub || ( shortScrub && startY < 20 ) ) )
        {
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

                    mEditor->setCurrentLayer( layerNumber );
                }

                Layer *currentLayer = mEditor->object()->getLayer(layerNumber);


                // Check if we are using the alt key
                //
                if (event->modifiers() == Qt::AltModifier) {

                    // If it is the case, we select everything that is after the selected frame
                    //
                    clickSelecting = true;
                    canMoveFrame = true;

                    currentLayer->selectAllFramesAfter(frameNumber);

                }
                // Check if we are clicking on a non selected frame
                //
                else if (!currentLayer->isFrameSelected(frameNumber)) {

                    // If it is the case, we select it
                    //
                    canBoxSelect = true;
                    clickSelecting = true;

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
                    canMoveFrame = true;
                }

                currentLayer->mousePress( event, frameNumber );
                update();
            }
            else
            {
                if ( frameNumber > 0 )
                {
                    mEditor->scrubTo( frameNumber );

                    timeLine->scrubbing = true;
                    qDebug( "Scrub to %d frame", frameNumber );
                }
            }
        }
        break;
    }
}

void TimeLineCells::mouseMoveEvent( QMouseEvent* event )
{
    if ( m_eType == TIMELINE_CELL_TYPE::Layers )
    {
        endY = event->pos().y();
        emit mouseMovedY( endY - startY );
    }
    int frameNumber = getFrameNumber( event->pos().x() );
    //int layerNumber = getLayerNumber( event->pos().y() );

    if ( m_eType == TIMELINE_CELL_TYPE::Tracks )
    {
        if ( timeLine->scrubbing )
        {
            mEditor->scrubTo( frameNumber );
        }
        else
        {
            if ( startLayerNumber != -1 && startLayerNumber < mEditor->object()->getLayerCount() )
            {
                Layer *currentLayer = mEditor->object()->getLayer(startLayerNumber);

                // Did we move to another frame ?
                //
                if ( frameNumber != lastFrameNumber ) {

                    // Check if the frame we clicked was selected
                    //
                    if ( canMoveFrame ) {

                        // If it is the case, we move the selected frames in the layer
                        //
                        movingFrames        = true;

                        int offset = frameNumber - lastFrameNumber;
                        currentLayer->moveSelectedFrames(offset);

                        mEditor->updateCurrentFrame();

                    }
                    else if ( canBoxSelect ){

                        // Otherwise, we do a box select
                        //
                        boxSelecting        = true;

                        currentLayer->deselectAll();
                        currentLayer->setFrameSelected(startFrameNumber, true);
                        currentLayer->extendSelectionTo(frameNumber);
                    }

                    lastFrameNumber = frameNumber;
                }

                currentLayer->mouseMove( event, frameNumber );
            }
        }
    }
    timeLine->update();
}

void TimeLineCells::mouseReleaseEvent( QMouseEvent* event )
{
    qDebug( "TimeLineCell: mouse release event." );

    endY = startY;
    emit mouseMovedY( 0 );
    timeLine->scrubbing = false;
    int frameNumber = getFrameNumber( event->pos().x() );
    if ( frameNumber < 1 ) frameNumber = -1;
    int layerNumber = getLayerNumber( event->pos().y() );
    if ( m_eType == TIMELINE_CELL_TYPE::Tracks && layerNumber != -1 && layerNumber < mEditor->object()->getLayerCount() )
    {
        Layer *currentLayer = mEditor->object()->getLayer(layerNumber);

        if (!timeLine->scrubbing && !movingFrames && !clickSelecting && !boxSelecting ) {

            // Deselecting if we didn't move, scrub nor select anything
            //
            if ( event->modifiers() == Qt::ControlModifier ) {
                // Add/remove from already selected
                currentLayer->toggleFrameSelected(frameNumber, true);
            }
            else {
                currentLayer->toggleFrameSelected(frameNumber, false);
            }
        }

        currentLayer->mouseRelease( event, frameNumber );
        update();
    }
    if ( m_eType == TIMELINE_CELL_TYPE::Layers && layerNumber != startLayerNumber && startLayerNumber != -1 && layerNumber != -1 )
    {
        mEditor->moveLayer( startLayerNumber, layerNumber );
    }
    update();
}

void TimeLineCells::mouseDoubleClickEvent( QMouseEvent* event )
{
    int frameNumber = getFrameNumber( event->pos().x() );
    int layerNumber = getLayerNumber( event->pos().y() );

    // -- short scrub --
    if ( event->pos().y() < 20 )
    {
        mPrefs->set(SETTING::SHORT_SCRUB, !shortScrub);
    }

    // -- layer --
    Layer* layer = mEditor->object()->getLayer( layerNumber );
    if ( layer )
    {
        if ( m_eType == TIMELINE_CELL_TYPE::Tracks && ( layerNumber != -1 ) && ( frameNumber > 0 ) && layerNumber < mEditor->object()->getLayerCount() )
        {
            mEditor->object()->getLayer( layerNumber )->mouseDoubleClick( event, frameNumber );
        }
        else if ( m_eType == TIMELINE_CELL_TYPE::Layers )
        {
            layer->editProperties();
            update();
        }
    }
}

void TimeLineCells::hScrollChange( int x )
{
    frameOffset = x;
    update();
}

void TimeLineCells::vScrollChange( int x )
{
    layerOffset = x;
    update();
}
