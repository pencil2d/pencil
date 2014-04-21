#include "timelinecells.h"

#include <QSettings>
#include "editor.h"
#include "timeline.h"
#include "layermanager.h"

TimeLineCells::TimeLineCells( TimeLine* parent, Editor* editor, TIMELINE_CELL_TYPE type )
: QWidget( parent )
{
    this->timeLine = parent;
    this->editor = editor;
    m_eType = type;

    cache = NULL;
    QSettings settings( "Pencil", "Pencil" );

    frameLength = settings.value( "length" ).toInt();
    if ( frameLength == 0 )
    {
        frameLength = 240; settings.setValue( "length", frameLength );
    }

    shortScrub = settings.value( "shortScrub" ).toBool();

    startY = 0;
    endY = 0;
    mouseMoveY = 0;
    startLayerNumber = -1;
    frameOffset = 0;
    layerOffset = 0;

    frameSize = ( settings.value( "frameSize" ).toInt() );
    if ( frameSize == 0 ) { frameSize = 12; settings.setValue( "frameSize", frameSize ); }

    fontSize = ( settings.value( "labelFontSize" ).toInt() );
    if ( fontSize == 0 ) { fontSize = 12; settings.setValue( "labelFontSize", fontSize ); }

    layerHeight = ( settings.value( "layerHeight" ).toInt() );
    if ( layerHeight == 0 ) { layerHeight = 20; settings.setValue( "layerHeight", layerHeight ); }

    setMinimumSize( 500, 4 * layerHeight );
    setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding ) );
    setAttribute( Qt::WA_OpaquePaintEvent, false );
}

int TimeLineCells::getFrameNumber( int x )
{
    int frameNumber = frameOffset + 1 + ( x - m_offsetX ) / frameSize;
    //	qDebug()<< frameNumber;
    return frameNumber;
}

int TimeLineCells::getFrameX( int frameNumber )
{
    int x = m_offsetX + ( frameNumber - frameOffset )*frameSize;
    return x;
}

int TimeLineCells::getLayerNumber( int y )
{
    int layerNumber = layerOffset + ( y - m_offsetY ) / layerHeight;
    layerNumber = editor->object()->getLayerCount() - 1 - layerNumber;
    if ( y < m_offsetY )
    {
        layerNumber = -1;
    }

    if ( layerNumber >= editor->object()->getLayerCount() )
    {
        layerNumber = editor->object()->getLayerCount();
    }
    return layerNumber;
}

int TimeLineCells::getLayerY( int layerNumber )
{
    //return offsetY + (layerNumber-layerOffset)*layerHeight;
    return m_offsetY + ( editor->object()->getLayerCount() - 1 - layerNumber - layerOffset )*layerHeight;
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
    if ( cache == NULL ) { cache = new QPixmap( size() ); }
    if ( cache->isNull() ) return;

    QPainter painter( cache );

    Object* object = editor->object();
    
    Q_ASSERT_X( object != nullptr, "", "" );

    Layer* layer = object->getLayer( editor->layerManager()->currentLayerIndex() );
    if ( layer == NULL ) return;

    // grey background of the view
    painter.setPen( Qt::NoPen );
    painter.setBrush( Qt::lightGray );
    painter.drawRect( QRect( 0, 0, width(), height() ) );

    // --- draw layers of the current object
    for ( int i = 0; i < object->getLayerCount(); i++ )
    {
        if ( i != editor->layerManager()->currentLayerIndex() )
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
                                        getLayerHeight(), false, editor->allLayers() );
                    break;
                }
            }
        }
    }
    if ( abs( getMouseMoveY() ) > 5 )
    {
        if ( m_eType == TIMELINE_CELL_TYPE::Tracks )
        {
            layer->paintTrack( painter, this, m_offsetX, getLayerY( editor->layerManager()->currentLayerIndex() ) + getMouseMoveY(), width() - m_offsetX, getLayerHeight(), true, frameSize );
        }
        if ( m_eType == TIMELINE_CELL_TYPE::Layers )
        {
            layer->paintLabel( painter, this, 0, getLayerY( editor->layerManager()->currentLayerIndex() ) + getMouseMoveY(), width() - 1, getLayerHeight(), true, editor->allLayers() );
        }
        painter.setPen( Qt::black );
        painter.drawRect( 0, getLayerY( getLayerNumber( endY ) ) - 1, width(), 2 );
    }
    else
    {
        if ( m_eType == TIMELINE_CELL_TYPE::Tracks )
        {
            layer->paintTrack( painter, this, m_offsetX, getLayerY( editor->layerManager()->currentLayerIndex() ), width() - m_offsetX, getLayerHeight(), true, frameSize );
        }
        if ( m_eType == TIMELINE_CELL_TYPE::Layers )
        {
            layer->paintLabel( painter, this, 0, getLayerY( editor->layerManager()->currentLayerIndex() ), width() - 1, getLayerHeight(), true, editor->allLayers() );
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
        if ( editor->allLayers() == 0 ) { painter.setBrush( Qt::NoBrush ); }
        if ( editor->allLayers() == 1 ) { painter.setBrush( Qt::darkGray ); }
        if ( editor->allLayers() == 2 ) { painter.setBrush( Qt::black ); }
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
        int fps = editor->fps;
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

void TimeLineCells::paintEvent( QPaintEvent* event )
{
    Q_UNUSED( event );
    Object* object = editor->object();
    if ( object == NULL ) return;
    Layer* layer = editor->layerManager()->currentLayer();
    if ( layer == NULL ) return;

    QPainter painter( this );
    if ( ( !editor->playing && !timeLine->scrubbing ) || cache == NULL )
    {
        drawContent();
    }
    if ( cache )
    {
        painter.drawPixmap( QPoint( 0, 0 ), *cache );
    }

    if ( m_eType == TIMELINE_CELL_TYPE::Tracks )
    {
        // --- draw the position of the current frame
        if ( editor->layerManager()->currentFrameIndex() > frameOffset )
        {
            painter.setBrush( QColor( 255, 0, 0, 128 ) );
            painter.setPen( Qt::NoPen );
            painter.setFont( QFont( "helvetica", 10 ) );
            //painter.setCompositionMode(QPainter::CompositionMode_Source); // this causes the message: QPainter::setCompositionMode: PorterDuff modes not supported on device
            QRect scrubRect;
            scrubRect.setTopLeft( QPoint( getFrameX( editor->layerManager()->currentFrameIndex() - 1 ), 0 ) );
            scrubRect.setBottomRight( QPoint( getFrameX( editor->layerManager()->currentFrameIndex() ), height() ) );
            if ( shortScrub )
            {
                scrubRect.setBottomRight( QPoint( getFrameX( editor->layerManager()->currentFrameIndex() ), 19 ) );
            }
            painter.drawRect( scrubRect );
            painter.setPen( QColor( 70, 70, 70, 255 ) );
            int incr = 0;
            if ( editor->layerManager()->currentFrameIndex() < 10 )
            {
                incr = 4;
            }
            else { incr = 0; }
            painter.drawText( QPoint( getFrameX( editor->layerManager()->currentFrameIndex() - 1 ) + incr, 15 ),
                              QString::number( editor->layerManager()->currentFrameIndex() ) );
        }
    }
}

void TimeLineCells::resizeEvent( QResizeEvent* event )
{
    if ( cache ) delete cache;
    cache = new QPixmap( size() );
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

    //qDebug() << "Timeline Type = " << m_eType;

    switch ( m_eType )
    {
    case TIMELINE_CELL_TYPE::Layers:
        if ( layerNumber != -1 && layerNumber < editor->object()->getLayerCount() )
        {
            if ( event->pos().x() < 15 )
            {
                editor->switchVisibilityOfLayer( layerNumber );
            }
            else
            {
                editor->setCurrentLayer( layerNumber );
                update();
            }
        }
        if ( layerNumber == -1 )
        {
            if ( event->pos().x() < 15 )
            {
                editor->toggleShowAllLayers();
            }
        }
        break;
    case TIMELINE_CELL_TYPE::Tracks:
        if ( frameNumber == editor->layerManager()->currentFrameIndex() && ( !shortScrub || ( shortScrub && startY < 20 ) ) )
        {
            timeLine->scrubbing = true;
        }
        else
        {
            if ( ( layerNumber != -1 ) && layerNumber < editor->object()->getLayerCount() )
            {
                editor->object()->getLayer( layerNumber )->mousePress( event, frameNumber );
                editor->setCurrentLayer( layerNumber );
                update();
            }
            else
            {
                if ( frameNumber > 0 )
                {
                    editor->scrubTo( frameNumber );
                    timeLine->scrubbing = true;
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
    int layerNumber = getLayerNumber( event->pos().y() );

    if ( m_eType == TIMELINE_CELL_TYPE::Tracks )
    {
        if ( timeLine->scrubbing )
        {
            editor->scrubTo( frameNumber );
        }
        else
        {
            if ( layerNumber != -1 && layerNumber < editor->object()->getLayerCount() )
            {
                editor->object()->getLayer( layerNumber )->mouseMove( event, frameNumber );
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
    if ( m_eType == TIMELINE_CELL_TYPE::Tracks && layerNumber != -1 && layerNumber < editor->object()->getLayerCount() )
    {
        editor->object()->getLayer( layerNumber )->mouseRelease( event, frameNumber );
    }
    if ( m_eType == TIMELINE_CELL_TYPE::Layers && layerNumber != startLayerNumber && startLayerNumber != -1 && layerNumber != -1 )
    {
        editor->moveLayer( startLayerNumber, layerNumber );
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
        if ( shortScrub ) scrubChange( 0 );
        else scrubChange( 1 );
    }

    // -- layer --
    Layer* layer = editor->object()->getLayer( layerNumber );
    if ( layer )
    {
        if ( m_eType == TIMELINE_CELL_TYPE::Tracks && ( layerNumber != -1 ) && ( frameNumber > 0 ) && layerNumber < editor->object()->getLayerCount() )
        {
            editor->object()->getLayer( layerNumber )->mouseDoubleClick( event, frameNumber );
        }
        else if ( m_eType == TIMELINE_CELL_TYPE::Layers )
        {
            layer->editProperties();
            update();
        }
    }
}

void TimeLineCells::fontSizeChange( int x )
{
    fontSize = x;
    QSettings settings( "Pencil", "Pencil" );
    settings.setValue( "labelFontSize", x );
    updateContent();
}

void TimeLineCells::frameSizeChange( int x )
{
    frameSize = x;
    QSettings settings( "Pencil", "Pencil" );
    settings.setValue( "frameSize", x );
    updateContent();
}

void TimeLineCells::scrubChange( int x )
{
    QSettings settings( "Pencil", "Pencil" );
    if ( x == 0 ) { shortScrub = false; settings.setValue( "shortScrub", "false" ); }
    else { shortScrub = true; settings.setValue( "shortScrub", "true" ); }
    update();
}

void TimeLineCells::labelChange( int x )
{
    QSettings settings( "Pencil", "Pencil" );
    if ( x == 0 )
    {
        drawFrameNumber = false;
        settings.setValue( "drawLabel", "false" );
    }
    else
    {
        drawFrameNumber = true;
        settings.setValue( "drawLabel", "true" );
    }
    updateContent();
}

void TimeLineCells::lengthChange( QString x )
{
    bool ok;
    int dec = x.toInt( &ok, 10 );
    frameLength = dec;
    timeLine->updateLength( frameLength );
    updateContent();
    QSettings settings( "Pencil", "Pencil" );
    settings.setValue( "length", dec );
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