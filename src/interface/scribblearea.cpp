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
#include <cmath>
#include <QtGui>
#include <QHashIterator>
#include <QMessageBox>

#include "beziercurve.h"
#include "editor.h"
#include "layerbitmap.h"
#include "layervector.h"
#include "layercamera.h"
#include "bitmapimage.h"
#include "pencilsettings.h"
#include "toolmanager.h"
#include "strokemanager.h"
#include "layermanager.h"
#include "popupcolorpalettewidget.h"

#include "scribblearea.h"

#define round(f) ((int)(f + 0.5))

ScribbleArea::ScribbleArea( QWidget *parent, Editor *editor )
: QWidget( parent )
, instantTool( false )
{
    this->m_pEditor = editor;
    m_strokeManager = new StrokeManager();

    QSettings settings( "Pencil", "Pencil" );

    followContour = 0;

    curveOpacity = ( 100 - settings.value( "curveOpacity" ).toInt() ) / 100.0; // default value is 1.0
    int curveSmoothingLevel = settings.value( "curveSmoothing" ).toInt();
    if ( curveSmoothingLevel == 0 ) { curveSmoothingLevel = 20; settings.setValue( "curveSmoothing", curveSmoothingLevel ); } // default
    curveSmoothing = curveSmoothingLevel / 20.0; // default value is 1.0

    if ( settings.value( SETTING_HIGH_RESOLUTION ).toString() == "true" )
    {
        m_strokeManager->useHighResPosition( true );
    }

    m_antialiasing = true; // default value is true (because it's prettier)
    if ( settings.value( "antialiasing" ).toString() == "false" )
    {
        m_antialiasing = false;
    }
    shadows = false; // default value is false
    if ( settings.value( "shadows" ).toString() == "true" )
    {
        shadows = true;
    }

    // Qt::WA_StaticContents ensure that the widget contents are rooted to the top-left corner and don't change when the widget is resized.
    setAttribute( Qt::WA_StaticContents );
    modified = false;
    m_isSimplified = false;
    m_usePressure = true;
    m_makeInvisible = false;
    somethingSelected = false;

    onionPrev = true;
    m_isMultiLayerOnionSkin = true;
    onionNext = false;
    m_showThinLines = false;
    m_showAllLayers = 1;
    myView = QMatrix(); // identity matrix
    myTempView = QMatrix();
    transMatrix = QMatrix();
    centralView = QMatrix();

    QString background = settings.value( "background" ).toString();

    background = "white";
    setBackgroundBrush( background );
    bufferImg = new BitmapImage;

    QRect newSelection( QPoint( 0, 0 ), QSize( 0, 0 ) );
    mySelection = newSelection;
    myTransformedSelection = newSelection;
    myTempTransformedSelection = newSelection;
    offset.setX( 0 );
    offset.setY( 0 );
    selectionTransformation.reset();

    tol = 7.0;

    readCanvasFromCache = true;
    mouseInUse = false;
    keyboardInUse = false;
    setMouseTracking( true ); // reacts to mouse move events, even if the button is not pressed

    debugRect = QRectF( 0, 0, 0, 0 );

    setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding ) );
    QPixmapCache::setCacheLimit( 30 * 2 * 1024 );
    updateAll = false;

    myFlipX = 1.0; // can be used as "scale"
    myFlipY = 1.0; // idem

    // color wheel popup
    m_popupPaletteWidget = new PopupColorPaletteWidget( this );

    onionBlue = true;
    onionRed = true;
    //onionColor = Qt::blue;
    toggledOnionColor();

    useGridA = false;
    useGridB = false;
}

void ScribbleArea::updateToolCursor()
{
    setCursor( currentTool()->cursor() );
}

void ScribbleArea::setCurveOpacity( int newOpacity )
{
    curveOpacity = newOpacity / 100.0;
    QSettings settings( "Pencil", "Pencil" );
    settings.setValue( "curveOpacity", 100 - newOpacity );
    updateAllVectorLayers();
}

void ScribbleArea::setCurveSmoothing( int newSmoothingLevel )
{
    curveSmoothing = newSmoothingLevel / 20.0;
    QSettings settings( "Pencil", "Pencil" );
    settings.setValue( "curveSmoothing", newSmoothingLevel );
}

void ScribbleArea::setHighResPosition( int x )
{
    QSettings settings( "Pencil", "Pencil" );
    if ( x == 0 )
    {
        m_strokeManager->useHighResPosition( false );
        settings.setValue( SETTING_HIGH_RESOLUTION, "false" );
    }
    else {
        m_strokeManager->useHighResPosition( true );
        settings.setValue( SETTING_HIGH_RESOLUTION, "true" );
    }
}

void ScribbleArea::setAntialiasing( int x )
{
    QSettings settings( "Pencil", "Pencil" );
    if ( x == 0 ) { m_antialiasing = false; settings.setValue( "antialiasing", "false" ); }
    else { m_antialiasing = true; settings.setValue( "antialiasing", "true" ); }
    updateAllVectorLayers();
}

void ScribbleArea::setShadows( int x )
{
    QSettings settings( "Pencil", "Pencil" );
    if ( x == 0 ) { shadows = false; settings.setValue( "shadows", "false" ); }
    else { shadows = true; settings.setValue( "shadows", "true" ); }
    update();
}

void ScribbleArea::setToolCursors( int x )
{
    pencilSettings()->setValue( SETTING_TOOL_CURSOR, ( x != 0 ) );
}

void ScribbleArea::setStyle( int x )
{
    QSettings settings( "Pencil", "Pencil" );
    if ( x == 0 ) { settings.setValue( "style", "default" ); }
    else { settings.setValue( "style", "aqua" ); }
    update();
}

void ScribbleArea::setBackground( int number )
{
    if ( number == 1 ) { setBackgroundBrush( "checkerboard" ); }
    if ( number == 2 ) { setBackgroundBrush( "white" ); }
    if ( number == 3 ) { setBackgroundBrush( "grey" ); }
    if ( number == 4 ) { setBackgroundBrush( "dots" ); }
    if ( number == 5 ) { setBackgroundBrush( "weave" ); }
    updateAllFrames();
}

void ScribbleArea::setBackgroundBrush( QString brushName )
{
    QSettings settings( "Pencil", "Pencil" );
    settings.setValue( "background", brushName );
    backgroundBrush = getBackgroundBrush( brushName );
}

QBrush ScribbleArea::getBackgroundBrush( QString brushName )
{
    QBrush brush = QBrush( Qt::white );
    if ( brushName == "white" )
    {
        brush = QBrush( Qt::white );
    }
    if ( brushName == "grey" )
    {
        brush = QBrush( Qt::lightGray );
    }
    if ( brushName == "checkerboard" )
    {
        QPixmap pattern( 16, 16 );
        pattern.fill( QColor( 255, 255, 255 ) );
        QPainter painter( &pattern );
        painter.fillRect( QRect( 0, 0, 8, 8 ), QColor( 220, 220, 220 ) );
        painter.fillRect( QRect( 8, 8, 8, 8 ), QColor( 220, 220, 220 ) );
        painter.end();
        brush.setTexture( pattern );
    }
    if ( brushName == "dots" )
    {
        QPixmap pattern( ":background/dots.png" );
        brush.setTexture( pattern );
    }
    if ( brushName == "weave" )
    {
        QPixmap pattern( ":background/weave.jpg" );
        brush.setTexture( pattern );
    }
    if ( brushName == "grid" )
    {
        brush.setTextureImage( QImage( ":background/grid.jpg" ) );
    }
    return brush;
}

/************************************************************************************/
// update methods

void ScribbleArea::updateFrame()
{
    updateFrame( m_pEditor->layerManager()->currentFrameIndex() );
}

void ScribbleArea::updateFrame( int frame )
{
    setView();
    int frameNumber = m_pEditor->layerManager()->LastFrameAtFrame( frame );
    QPixmapCache::remove( "frame" + QString::number( frameNumber ) );
    readCanvasFromCache = true;

    update();
}

void ScribbleArea::updateAllFrames()
{
    setView();
    QPixmapCache::clear();
    readCanvasFromCache = true;
    update();
    updateAll = false;
}

void ScribbleArea::updateAllVectorLayersAtCurrentFrame()
{
    updateAllVectorLayersAt( m_pEditor->layerManager()->currentFrameIndex() );
}

void ScribbleArea::updateAllVectorLayersAt( int frameNumber )
{
    for ( int i = 0; i < m_pEditor->object()->getLayerCount(); i++ )
    {
        Layer *layer = m_pEditor->object()->getLayer( i );
        if ( layer->type() == Layer::VECTOR ) { ( ( LayerVector * )layer )->getLastVectorImageAtFrame( frameNumber, 0 )->setModified( true ); }
    }
    updateFrame( m_pEditor->layerManager()->currentFrameIndex() );
}

void ScribbleArea::updateAllVectorLayers()
{
    for ( int i = 0; i < m_pEditor->object()->getLayerCount(); i++ )
    {
        Layer *layer = m_pEditor->object()->getLayer( i );
        if ( layer->type() == Layer::VECTOR ) { ( ( LayerVector * )layer )->setModified( true ); }
    }
    updateAllFrames();
}

void ScribbleArea::setModified( int layerNumber, int frameNumber )
{
    Layer *layer = m_pEditor->object()->getLayer( layerNumber );
    if ( layer->type() == Layer::VECTOR )
    {
        layer->setModified( frameNumber, true );
    }
    if ( layer->type() == Layer::BITMAP )
    {
        ( ( LayerImage * )layer )->setModified( frameNumber, true );
    }

    emit modification( layerNumber );

    updateAllFrames();
}

void ScribbleArea::togglePopupPalette()
{
    m_popupPaletteWidget->popup();
}


/************************************************************************************/
// key event handlers

void ScribbleArea::escape()
{
    deselectAll();
}

void ScribbleArea::keyPressEvent( QKeyEvent *event )
{
    keyPressed( event );
}

void ScribbleArea::keyPressed( QKeyEvent *event )
{
    keyboardInUse = true;
    if ( mouseInUse ) { return; } // prevents shortcuts calls while drawing, todo: same check for remaining shortcuts (in connects).
    if ( currentTool()->keyPressEvent( event ) ) {
        // has been handled by tool
        return;
    }

    // --- fixed control key shortcuts ---
    if ( event->modifiers() == ( Qt::ControlModifier | Qt::ShiftModifier ) )
    {
        qreal width = currentTool()->properties.width;
        qreal feather = currentTool()->properties.feather;
        setTemporaryTool( ERASER );
        m_pEditor->toolManager()->setWidth( width + ( 200 - width ) / 41 ); // minimum size: 0.2 + 4.8 = 5 units. maximum size 200 + 0.
        m_pEditor->toolManager()->setFeather( feather ); //anticipates future implementation of feather (not used yet).
        return;
    }

    // ---- fixed normal keys ----
    switch ( event->key() )
    {
    case Qt::Key_Right:
        if ( somethingSelected )
        {
            myTempTransformedSelection.translate( 1, 0 );
            myTransformedSelection = myTempTransformedSelection;
            calculateSelectionTransformation();
            update();
        }
        else
        {
            m_pEditor->scrubForward();
            event->ignore();
        }
        break;
    case Qt::Key_Left:
        if ( somethingSelected )
        {
            myTempTransformedSelection.translate( -1, 0 );
            myTransformedSelection = myTempTransformedSelection;
            calculateSelectionTransformation();
            update();
        }
        else
        {
            m_pEditor->scrubBackward();
            event->ignore();
        }
        break;
    case Qt::Key_Up:
        if ( somethingSelected )
        {
            myTempTransformedSelection.translate( 0, -1 );
            myTransformedSelection = myTempTransformedSelection;
            calculateSelectionTransformation();
            update();
        }
        else
        {
            m_pEditor->previousLayer();
            event->ignore();
        }
        break;
    case Qt::Key_Down:
        if ( somethingSelected )
        {
            myTempTransformedSelection.translate( 0, 1 );
            myTransformedSelection = myTempTransformedSelection;
            calculateSelectionTransformation();
            update();
        }
        else
        {
            m_pEditor->nextLayer();
            event->ignore();
        }
        break;
    case Qt::Key_Return:
        if ( somethingSelected )
        {
            paintTransformedSelection();
            deselectAll();
        }
        else
        {
            event->ignore();
        }
        break;
    case Qt::Key_Escape:
        if ( somethingSelected )
        {
            escape();
        }
        break;
    case Qt::Key_Backspace:
        if ( somethingSelected )
        {
            deleteSelection();
        }
        break;
    case Qt::Key_F1:
        m_isSimplified = true;
        emit outlinesChanged( m_isSimplified );
        updateAllVectorLayersAtCurrentFrame();
        break;
    case Qt::Key_F2:
        updateAllVectorLayersAtCurrentFrame();
        break;
    case Qt::Key_F3:
        updateAllVectorLayersAtCurrentFrame();
        break;
    case Qt::Key_Space:
        setTemporaryTool( HAND ); // just call "setTemporaryTool()" to activate temporarily any tool
        break;
    default:
        event->ignore();
    }
}

void ScribbleArea::keyReleaseEvent( QKeyEvent *event )
{
    keyboardInUse = false;
    if ( mouseInUse ) { return; }
    if ( instantTool ) // temporary tool
    {
        currentTool()->keyReleaseEvent( event );
        setPrevTool();
        return;
    }
    if ( currentTool()->keyReleaseEvent( event ) )
    {
        // has been handled by tool
        return;
    }
}

/************************************************************************************/
// mouse and tablet event handlers

void ScribbleArea::wheelEvent( QWheelEvent *event )
{
    if ( event->modifiers() & Qt::ControlModifier )
    {
        if ( event->delta() > 0 ) //+ve for wheel up
        {
            zoom();
        }
        else
        {
            zoom1();
        }
    }
}

void ScribbleArea::tabletEvent( QTabletEvent *event )
{
    //qDebug() << "Device" << event->device() << "Pointer type" << event->pointerType();
    m_strokeManager->tabletEvent( event );

    //qDebug() << event->hiResGlobalPos();
    currentTool()->adjustPressureSensitiveProperties( pow( ( float )m_strokeManager->getPressure(), 2.0f ),
                                                      event->pointerType() == QTabletEvent::Cursor );

    if ( event->pointerType() == QTabletEvent::Eraser )
    {
        getEditor()->toolManager()->tabletSwitchToEraser();
    }
    else
    {
        getEditor()->toolManager()->tabletRestorePrevTool();
    }
    event->ignore(); // indicates that the tablet event is not accepted yet, so that it is propagated as a mouse event)
}

QPointF ScribbleArea::pixelToPoint( QPointF pixel )
{
    bool invertible = true;
    return myTempView.inverted( &invertible ).map( QPointF( pixel ) );
}

void ScribbleArea::mousePressEvent( QMouseEvent *event )
{
    mouseInUse = true;

    m_strokeManager->mousePressEvent( event );

    if ( !m_strokeManager->isTabletInUse() )   // a mouse is used instead of a tablet
    {
        m_strokeManager->setPressure( 1.0 );
        currentTool()->adjustPressureSensitiveProperties( 1.0, true );

        //----------------code for starting hand tool when middle mouse is pressed
        if ( event->buttons() & Qt::MidButton )
        {
            //qDebug() << "Hand Start " << event->pos();
            prevMode = currentTool()->type();
            getEditor()->toolManager()->setCurrentTool( HAND );
        }
    }

    if ( !( event->button() == Qt::NoButton ) )    // if the user is pressing the left or right button
    {
        lastPixel = m_strokeManager->getLastPressPixel();
        bool invertible = true;
        lastPoint = myTempView.inverted( &invertible ).map( QPointF( lastPixel ) );
    }

    // ----- assisted tool adjusment -- todo: simplify this
    if ( event->button() == Qt::LeftButton ) {
        if ( ( event->modifiers() == Qt::ShiftModifier ) && ( currentTool()->properties.width > -1 ) )
        {
            //adjust width if not locked
            currentTool()->startAdjusting( WIDTH, 1 );
            return;
        }
        if ( ( event->modifiers() == Qt::ControlModifier ) && ( currentTool()->properties.feather > -1 ) )
        {
            //adjust feather if not locked
            currentTool()->startAdjusting( FEATHER, 1 );
            return;
        }
        if ( ( event->modifiers() == ( Qt::ShiftModifier | Qt::AltModifier ) ) && ( currentTool()->properties.width > -1 ) )
        {
            //adjust width if not locked
            currentTool()->startAdjusting( WIDTH, 0 );
            return;
        }
        if ( ( event->modifiers() == ( Qt::ControlModifier | Qt::AltModifier ) ) && ( currentTool()->properties.feather > -1 ) )
        {
            //adjust feather if not locked
            currentTool()->startAdjusting( FEATHER, 0 );
            return;
        }
    }

    // ---- checks layer availability ------
    Layer *layer = m_pEditor->getCurrentLayer();
    if ( layer == NULL ) { return; }

    if ( layer->type() == Layer::VECTOR )
    {
        VectorImage *vectorImage = ( ( LayerVector * )layer )->getLastVectorImageAtFrame( m_pEditor->layerManager()->currentFrameIndex(), 0 );
        if ( vectorImage == NULL ) { return; }
    }

    if ( layer->type() == Layer::BITMAP )
    {
        BitmapImage *bitmapImage = ( ( LayerBitmap * )layer )->getLastBitmapImageAtFrame( m_pEditor->layerManager()->currentFrameIndex(), 0 );
        if ( bitmapImage == NULL ) { return; }
    }

    if ( !layer->visible && currentTool()->type() != HAND && ( event->button() != Qt::RightButton ) )
    {
        QMessageBox::warning( this, tr( "Warning" ),
                              tr( "You are drawing on a hidden layer! Please select another layer (or make the current layer visible)." ),
                              QMessageBox::Ok,
                              QMessageBox::Ok );
        mouseInUse = false;
        return;
    }
    // ---

    bool invertible = true;
    currentPoint = myTempView.inverted( &invertible ).map( QPointF( currentPixel ) );

    // the user is also pressing the mouse
    if ( event->buttons() & Qt::LeftButton || event->buttons() & Qt::RightButton )
    {
        offset = currentPoint - lastPoint;
    }

    if ( event->button() == Qt::RightButton )
    {
        getTool( HAND )->mousePressEvent( event );
        return;
    }

    currentTool()->mousePressEvent( event );
}

bool ScribbleArea::areLayersSane() const
{
    Layer *layer = m_pEditor->getCurrentLayer();
    // ---- checks ------
    if ( layer == NULL ) { return false; }
    if ( layer->type() == Layer::VECTOR )
    {
        VectorImage *vectorImage = ( ( LayerVector * )layer )->getLastVectorImageAtFrame( m_pEditor->layerManager()->currentFrameIndex(), 0 );
        if ( vectorImage == NULL ) { return false; }
    }
    if ( layer->type() == Layer::BITMAP )
    {
        BitmapImage *bitmapImage = ( ( LayerBitmap * )layer )->getLastBitmapImageAtFrame( m_pEditor->layerManager()->currentFrameIndex(), 0 );
        if ( bitmapImage == NULL ) { return false; }
    }
    // ---- end checks ------

    return true;
}

bool ScribbleArea::isLayerPaintable() const
{
    if ( !areLayersSane() )
        return false;

    Layer *layer = m_pEditor->getCurrentLayer();
    return layer->type() == Layer::BITMAP || layer->type() == Layer::VECTOR;
}

void ScribbleArea::mouseMoveEvent( QMouseEvent *event )
{
    if ( !areLayersSane() )
    {
        return;
    }

    m_strokeManager->mouseMoveEvent( event );
    currentPixel = m_strokeManager->getCurrentPixel();
    bool invertible = true;
    currentPoint = myTempView.inverted( &invertible ).map( QPointF( currentPixel ) );

    // the user is also pressing the mouse (= dragging)
    if ( event->buttons() & Qt::LeftButton || event->buttons() & Qt::RightButton )
    {
        offset = currentPoint - lastPoint;
        // --- use SHIFT + drag to resize WIDTH / use CTRL + drag to resize FEATHER ---
        if ( currentTool()->isAdjusting )
        {
            currentTool()->adjustCursor( offset.x(), offset.y() ); //updates cursors given org width or feather and x
            return;
        }
    }

    if ( event->buttons() == Qt::RightButton )
    {
        getTool( HAND )->mouseMoveEvent( event );
        return;
    }

    currentTool()->mouseMoveEvent( event );
}

void ScribbleArea::mouseReleaseEvent( QMouseEvent *event )
{
    mouseInUse = false;

    // ---- checks ------
    if ( currentTool()->isAdjusting )
    {
        currentTool()->stopAdjusting();
        return; // [SHIFT]+drag OR [CTRL]+drag
    }

    if ( !areLayersSane() )
    {
        return;
    }

    m_strokeManager->mouseReleaseEvent( event );

    if ( event->button() == Qt::RightButton )
    {
        getTool( HAND )->mouseReleaseEvent( event );
        return;
    }

    currentTool()->mouseReleaseEvent( event );

    if ( currentTool()->type() == EYEDROPPER )
    {
        setCurrentTool( prevToolType );
    }

    // ---- last check (at the very bottom of mouseRelease) ----
    if ( instantTool && !keyboardInUse ) // temp tool and released all keys ?
    {
        setPrevTool();
    }
}

void ScribbleArea::mouseDoubleClickEvent( QMouseEvent *event )
{
    currentTool()->mouseDoubleClickEvent( event );
}

/************************************************************************************/
// paint methods

void ScribbleArea::paintBitmapBuffer()
{
    Layer *layer = m_pEditor->getCurrentLayer();
    // ---- checks ------
    if ( layer == NULL ) { return; }
    // Clear the temporary pixel path
    BitmapImage *targetImage = ( ( LayerBitmap * )layer )->getLastBitmapImageAtFrame( m_pEditor->layerManager()->currentFrameIndex(), 0 );
    if ( targetImage != NULL )
    {
        QPainter::CompositionMode cm = QPainter::CompositionMode_SourceOver;
        switch ( currentTool()->type() )
        {
        case ERASER:
            cm = QPainter::CompositionMode_DestinationOut;
            break;
        case BRUSH:
            if ( getTool( BRUSH )->properties.preserveAlpha ) { cm = QPainter::CompositionMode_SourceAtop; }
            if ( followContour )
            {
                // writes on the layer below
                if ( m_pEditor->layerManager()->currentLayerIndex() > 0 )
                {
                    Layer *layer2 = m_pEditor->getCurrentLayer( -1 );
                    if ( layer2->type() == Layer::BITMAP )
                    {
                        targetImage = ( ( LayerBitmap * )layer2 )->getLastBitmapImageAtFrame( m_pEditor->layerManager()->currentFrameIndex(), 0 );
                    }
                }
            }
            break;
        case PEN:
            if ( getTool( PEN )->properties.preserveAlpha ) { cm = QPainter::CompositionMode_SourceAtop; }
            break;
        case PENCIL:
            if ( getTool( PENCIL )->properties.preserveAlpha ) { cm = QPainter::CompositionMode_SourceAtop; }
            break;
        default: //nothing
            break;
        }
        targetImage->paste( bufferImg, cm );
    }
    QRect rect = myTempView.mapRect( bufferImg->boundaries );
    // Clear the buffer
    bufferImg->clear();

    //setModified(layer, editor->currentFrame);
    ( ( LayerImage * )layer )->setModified( m_pEditor->layerManager()->currentFrameIndex(), true );
    emit modification();
    QPixmapCache::remove( "frame" + QString::number( m_pEditor->layerManager()->currentFrameIndex() ) );
    readCanvasFromCache = false;
    updateCanvas( m_pEditor->layerManager()->currentFrameIndex(), rect.adjusted( -1, -1, 1, 1 ) );
    update( rect );
}

void ScribbleArea::clearBitmapBuffer()
{
    bufferImg->clear();
}

void ScribbleArea::drawLine( QPointF P1, QPointF P2, QPen pen, QPainter::CompositionMode cm )
{
    bufferImg->drawLine( P1, P2, pen, cm, m_antialiasing );
}

void ScribbleArea::drawPath( QPainterPath path, QPen pen, QBrush brush, QPainter::CompositionMode cm )
{
    bufferImg->drawPath( path, pen, brush, cm, m_antialiasing );
}

void ScribbleArea::refreshBitmap( QRect rect, int rad )
{
    update( myTempView.mapRect( rect.normalized().adjusted( -rad, -rad, +rad, +rad ) ) );
}

void ScribbleArea::refreshVector( QRect rect, int rad )
{
    update( rect.normalized().adjusted( -rad, -rad, +rad, +rad ) );
}

void ScribbleArea::grid()
{
    QPainter painter( this );
    painter.setWorldMatrixEnabled( true );
    painter.setWorldMatrix( centralView.inverted() * transMatrix * centralView );
    painter.drawPixmap( QPoint( 0, 0 ), canvas );
    painter.drawImage( QPoint( 100, 100 ), QImage( ":background/grid" ) ); //TODO The grid is being drawn but the white background over rides it!
}

void ScribbleArea::paintEvent( QPaintEvent *event )
{
    //qDebug() << "paint event!" << QDateTime::currentDateTime() << event->rect();
    QPainter painter( this );

    // draws the background (if necessary)
    if ( mouseInUse && currentTool()->type() == HAND )
    {
        painter.setWorldMatrix( myTempView );
        painter.setWorldMatrixEnabled( true );
        painter.setPen( Qt::NoPen );
        painter.setBrush( backgroundBrush );
        painter.drawRect( ( myTempView ).inverted().mapRect( QRect( -2, -2, width() + 3, height() + 3 ) ) );  // this is necessary to have the background move with the view
    }

    // process the canvas (or not)
    if ( !mouseInUse && readCanvasFromCache )
    {
        // --- we retrieve the canvas from the cache; we create it if it doesn't exist
        int curIndex = m_pEditor->layerManager()->currentFrameIndex();
        int frameNumber = m_pEditor->layerManager()->LastFrameAtFrame( curIndex );

        QString strCachedFrameKey = "frame" + QString::number( frameNumber );
        if ( !QPixmapCache::find( strCachedFrameKey, canvas ) )
        {
            updateCanvas( m_pEditor->layerManager()->currentFrameIndex(), event->rect() );
            QPixmapCache::insert( strCachedFrameKey, canvas );
        }
    }
    if ( currentTool()->type() == MOVE )
    {
        Layer *layer = m_pEditor->getCurrentLayer();
        if ( !layer ) { return; }
        if ( layer->type() == Layer::VECTOR ) { ( ( LayerVector * )layer )->getLastVectorImageAtFrame( m_pEditor->layerManager()->currentFrameIndex(), 0 )->setModified( true ); }
        updateCanvas( m_pEditor->layerManager()->currentFrameIndex(), event->rect() );
    }
    // paints the canvas
    painter.setWorldMatrixEnabled( true );
    painter.setWorldMatrix( centralView.inverted() * transMatrix * centralView );
    painter.drawPixmap( QPoint( 0, 0 ), canvas );
    //  painter.drawImage(QPoint(100,100),QImage(":background/grid"));//TODO Success a grid is drawn
    Layer *layer = m_pEditor->getCurrentLayer();
    if ( !layer ) { return; }

    if ( !m_pEditor->playing )    // we don't need to display the following when the animation is playing
    {
        painter.setWorldMatrix( myTempView );

        if ( layer->type() == Layer::VECTOR )
        {
            VectorImage *vectorImage = ( ( LayerVector * )layer )->getLastVectorImageAtFrame( m_pEditor->layerManager()->currentFrameIndex(), 0 );

            if ( currentTool()->type() == SMUDGE || currentTool()->type() == HAND )
            {
                //bufferImg->clear();
                painter.save();
                painter.setWorldMatrixEnabled( false );
                painter.setRenderHint( QPainter::Antialiasing, false );
                // ----- paints the edited elements
                QPen pen2( Qt::black, 0.5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin );
                painter.setPen( pen2 );
                QColor colour;
                // ------------ vertices of the edited curves
                colour = QColor( 200, 200, 200 );
                painter.setBrush( colour );
                for ( int k = 0; k < vectorSelection.curve.size(); k++ )
                {
                    int curveNumber = vectorSelection.curve.at( k );
                    //QPainterPath path = vectorImage->curve[curveNumber].getStrokedPath();
                    //bufferImg->drawPath( myTempView.map(path), pen2, colour, QPainter::CompositionMode_SourceOver, false);
                    for ( int vertexNumber = -1; vertexNumber < vectorImage->getCurveSize( curveNumber ); vertexNumber++ )
                    {
                        QPointF vertexPoint = vectorImage->getVertex( curveNumber, vertexNumber );
                        QRectF rectangle = QRectF( ( myView * transMatrix * centralView ).map( vertexPoint ) - QPointF( 3.0, 3.0 ), QSizeF( 7, 7 ) );
                        if ( rect().contains( ( myView * transMatrix * centralView ).map( vertexPoint ).toPoint() ) )
                        {
                            painter.drawRect( rectangle.toRect() );
                            //bufferImg->drawRect( rectangle.toRect(), pen2, colour, QPainter::CompositionMode_SourceOver, false);
                        }
                    }
                }
                // ------------ selected vertices of the edited curves
                colour = QColor( 100, 100, 255 );
                painter.setBrush( colour );
                for ( int k = 0; k < vectorSelection.vertex.size(); k++ )
                {
                    VertexRef vertexRef = vectorSelection.vertex.at( k );
                    QPointF vertexPoint = vectorImage->getVertex( vertexRef );
                    QRectF rectangle0 = QRectF( ( myView * transMatrix * centralView ).map( vertexPoint ) - QPointF( 3.0, 3.0 ), QSizeF( 7, 7 ) );
                    painter.drawRect( rectangle0.toRect() );
                }
                // ----- paints the closest vertices
                colour = QColor( 255, 0, 0 );
                painter.setBrush( colour );
                if ( vectorSelection.curve.size() > 0 )
                {
                    for ( int k = 0; k < closestVertices.size(); k++ )
                    {
                        VertexRef vertexRef = closestVertices.at( k );
                        QPointF vertexPoint = vectorImage->getVertex( vertexRef );
                        //if ( vectorImage->isSelected(vertexRef) ) vertexPoint = selectionTransformation.map( vertexPoint );
                        QRectF rectangle = QRectF( ( myView * transMatrix * centralView ).map( vertexPoint ) - QPointF( 3.0, 3.0 ), QSizeF( 7, 7 ) );
                        painter.drawRect( rectangle.toRect() );
                        //bufferImg->drawRect( rectangle.toRect(), pen2, colour, QPainter::CompositionMode_SourceOver, false);
                    }
                }
                painter.restore();
            }

            if ( currentTool()->type() == MOVE )
            {
                // ----- paints the closest curves
                bufferImg->clear();
                QPen pen2( Qt::black, 0.5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin );
                QColor colour = QColor( 100, 100, 255 );

                for ( int k = 0; k < closestCurves.size(); k++ )
                {
                    qreal scale = myTempView.determinant(); //todo: check whether it's correct (det = area?)
                    //qreal scale = sqrt(myTempView.det()); or qreal scale = sqrt(myTempView.m11()*myTempView.m22());
                    int idx = closestCurves[ k ];
                    if ( vectorImage->curve.size() <= idx )
                    {
                        // safety check
                        continue;
                    }
                    BezierCurve myCurve = vectorImage->curve[ closestCurves[ k ] ];
                    if ( myCurve.isPartlySelected() )
                    {
                        myCurve.transform( selectionTransformation );
                    }
                    QPainterPath path = myCurve.getStrokedPath( 1.2 / scale, false );
                    bufferImg->drawPath( ( myView * transMatrix * centralView ).map( path ), pen2, colour, QPainter::CompositionMode_SourceOver, m_antialiasing );
                }
            }
        }

        // paints the  buffer image
        if ( m_pEditor->getCurrentLayer() != NULL )
        {
            painter.setOpacity( 1.0 );
            if ( m_pEditor->getCurrentLayer()->type() == Layer::BITMAP ) { painter.setWorldMatrixEnabled( true ); }
            if ( m_pEditor->getCurrentLayer()->type() == Layer::VECTOR ) { painter.setWorldMatrixEnabled( false ); }
            bufferImg->paintImage( painter );
        }

        // paints the selection outline
        if ( somethingSelected && ( myTempTransformedSelection.isValid() || m_moveMode == ROTATION ) ) // @revise
        {
            // outline of the transformed selection
            painter.setWorldMatrixEnabled( false );
            painter.setOpacity( 1.0 );
            QPolygon tempRect = ( myView * transMatrix * centralView ).mapToPolygon( myTempTransformedSelection.normalized().toRect() );

            Layer *layer = m_pEditor->getCurrentLayer();
            if ( layer != NULL )
            {
                if ( layer->type() == Layer::BITMAP )
                {
                    painter.setBrush( Qt::NoBrush );
                    painter.setPen( Qt::DashLine );
                }
                if ( layer->type() == Layer::VECTOR )
                {
                    painter.setBrush( QColor( 0, 0, 0, 20 ) );
                    painter.setPen( Qt::gray );
                }
                painter.drawPolygon( tempRect );

                if ( layer->type() != Layer::VECTOR || currentTool()->type() != SELECT )
                {
                    painter.setPen( Qt::SolidLine );
                    painter.setBrush( QBrush( Qt::gray ) );
                    painter.drawRect( tempRect.point( 0 ).x() - 3, tempRect.point( 0 ).y() - 3, 6, 6 );
                    painter.drawRect( tempRect.point( 1 ).x() - 3, tempRect.point( 1 ).y() - 3, 6, 6 );
                    painter.drawRect( tempRect.point( 2 ).x() - 3, tempRect.point( 2 ).y() - 3, 6, 6 );
                    painter.drawRect( tempRect.point( 3 ).x() - 3, tempRect.point( 3 ).y() - 3, 6, 6 );
                }
            }
        }
    }
    // clips to the frame of the camera
    if ( layer->type() == Layer::CAMERA )
    {
        QRect rect = ( ( LayerCamera * )layer )->getViewRect();
        rect.translate( width() / 2, height() / 2 );
        painter.setWorldMatrixEnabled( false );
        painter.setPen( Qt::NoPen );
        painter.setBrush( QColor( 0, 0, 0, 160 ) );
        painter.drawRect( QRect( 0, 0, width(), ( height() - rect.height() ) / 2 ) );
        painter.drawRect( QRect( 0, rect.bottom(), width(), ( height() - rect.height() ) / 2 ) );
        painter.drawRect( QRect( 0, rect.top(), ( width() - rect.width() ) / 2, rect.height() - 1 ) );
        painter.drawRect( QRect( ( width() + rect.width() ) / 2, rect.top(), ( width() - rect.width() ) / 2, rect.height() - 1 ) );
        painter.setPen( Qt::black );
        painter.setBrush( Qt::NoBrush );
        painter.drawRect( rect );
    }
    // outlines the frame of the viewport
    painter.setWorldMatrixEnabled( false );
    painter.setPen( QPen( Qt::gray, 2 ) );
    painter.setBrush( Qt::NoBrush );
    painter.drawRect( QRect( 0, 0, width(), height() ) );
    // shadow
    if ( shadows && !m_pEditor->playing && ( !mouseInUse || currentTool()->type() == HAND ) )
    {
        int radius1 = 12;
        int radius2 = 8;
        QLinearGradient shadow = QLinearGradient( 0, 0, 0, radius1 );
        setGaussianGradient( shadow, Qt::black, 0.15, 0.0 );
        painter.setPen( Qt::NoPen );
        painter.setBrush( shadow );
        painter.drawRect( QRect( 0, 0, width(), radius1 ) );
        shadow.setFinalStop( radius1, 0 );
        painter.setBrush( shadow );
        painter.drawRect( QRect( 0, 0, radius1, height() ) );
        shadow.setStart( 0, height() );
        shadow.setFinalStop( 0, height() - radius2 );
        painter.setBrush( shadow );
        painter.drawRect( QRect( 0, height() - radius2, width(), height() ) );
        shadow.setStart( width(), 0 );
        shadow.setFinalStop( width() - radius2, 0 );
        painter.setBrush( shadow );
        painter.drawRect( QRect( width() - radius2, 0, width(), height() ) );
    }
    event->accept();
}

void ScribbleArea::updateCanvas( int frame, QRect rect )
{
    //qDebug() << "paint canvas!" << QDateTime::currentDateTime();
    // merge the different layers into the ScribbleArea
    QPainter painter( &canvas );
    if ( myTempView.determinant() == 1.0 )
    {
        painter.setRenderHint( QPainter::SmoothPixmapTransform, false );
    }
    else
    {
        painter.setRenderHint( QPainter::SmoothPixmapTransform, m_antialiasing );
    }
    painter.setClipRect( rect );
    painter.setClipping( true );
    setView();
    painter.setWorldMatrix( myTempView );
    painter.setWorldMatrixEnabled( true );

    // background
    painter.setPen( Qt::NoPen );
    painter.setBrush( backgroundBrush );
    painter.drawRect( myTempView.inverted().mapRect( QRect( -2, -2, width() + 3, height() + 3 ) ) );  // this is necessary to have the background move with the view

    QRectF viewRect = getViewRect();
    QRectF vectorViewRect = viewRect.translated( -viewRect.left(), -viewRect.top() );
    QSize sz = viewRect.size().toSize();

    Object *object = m_pEditor->object();
    qreal opacity;

    // --- onionskins ---
    int iStart = 0;
    int iEnd = object->getLayerCount() - 1;
    if ( !m_isMultiLayerOnionSkin ) { // not used ( if required, just make a connection from UI ) // is used now for Single/multiple onionskin Layers
        iStart = iEnd = m_pEditor->layerManager()->currentLayerIndex();
    }
    for ( int i = iStart; i <= iEnd; i++ )
    {
        opacity = 1.0;
        if ( i != m_pEditor->layerManager()->currentLayerIndex() && ( m_showAllLayers == 1 ) ) { opacity = 0.4; }
        Q_ASSERT_X( m_pEditor != NULL, "ScribbleArea.cpp", "Editor should not be null." );
        Q_ASSERT_X( m_pEditor->getCurrentLayer(), "", "Layer should not be null." );

        //qDebug( "Layer Count = %d, current=%d", object->getLayerCount(), i );

        if ( m_pEditor->getCurrentLayer()->type() == Layer::CAMERA ) { opacity = 1.0; }
        Layer *layer = ( object->getLayer( i ) );
        if ( layer->visible && ( m_showAllLayers > 0 || i == m_pEditor->layerManager()->currentLayerIndex() ) ) // change && to || for all layers
        {
            // paints the bitmap images
            if ( layer->type() == Layer::BITMAP )
            {
                LayerBitmap *layerBitmap = ( LayerBitmap * )layer;
                BitmapImage *bitmapImage = layerBitmap->getLastBitmapImageAtFrame( frame, 0 );
                if ( bitmapImage != NULL )
                {
                    painter.setWorldMatrixEnabled( true );

                    // previous frame (onion skin)
                    if ( onionPrev ) {
                        BitmapImage *previousImage = layerBitmap->getLastBitmapImageAtFrame( frame, -1 );
                        if ( previousImage != NULL )
                        {
                            painter.setOpacity( opacity * m_pEditor->getOnionLayer1Opacity() / 100.0 );
                            previousImage->paintImage( painter );
                        }
                        BitmapImage *previousImage2 = layerBitmap->getLastBitmapImageAtFrame( frame, -2 );
                        if ( previousImage2 != NULL )
                        {
                            painter.setOpacity( opacity * m_pEditor->getOnionLayer2Opacity() / 100.0 );
                            previousImage2->paintImage( painter );
                        }
                        BitmapImage *previousImage3 = layerBitmap->getLastBitmapImageAtFrame( frame, -3 );
                        if ( previousImage3 != NULL )
                        {
                            painter.setOpacity( opacity * m_pEditor->getOnionLayer3Opacity() / 100.0 );
                            previousImage3->paintImage( painter );
                        }
                        if ( onionBlue || onionRed ) {
                            painter.setOpacity( 1.0 );
                            painter.setCompositionMode( QPainter::CompositionMode_Lighten );
                            if ( onionBlue && onionRed && onionNext ) {
                                painter.fillRect( viewRect, Qt::red );
                            }
                            else {
                                painter.fillRect( viewRect, onionColor );
                            }
                            painter.setCompositionMode( QPainter::CompositionMode_SourceOver );
                        }
                    }

                    // next frame (onion skin)
                    if ( onionNext )
                    {
                        BitmapImage *nextImage = layerBitmap->getLastBitmapImageAtFrame( frame, 1 );
                        if ( nextImage != NULL )
                        {
                            painter.setOpacity( opacity * m_pEditor->getOnionLayer1Opacity() / 100.0 );
                            nextImage->paintImage( painter );
                        }
                        BitmapImage *nextImage2 = layerBitmap->getLastBitmapImageAtFrame( frame, 2 );
                        if ( nextImage2 != NULL )
                        {
                            painter.setOpacity( opacity * m_pEditor->getOnionLayer2Opacity() / 100.0 );
                            nextImage2->paintImage( painter );
                        }
                        BitmapImage *nextImage3 = layerBitmap->getLastBitmapImageAtFrame( frame, 3 );
                        if ( nextImage3 != NULL )
                        {
                            painter.setOpacity( opacity * m_pEditor->getOnionLayer3Opacity() / 100.0 );
                            nextImage3->paintImage( painter );
                        }
                        if ( onionBlue || onionRed )
                        {
                            painter.setOpacity( 1.0 );
                            painter.setCompositionMode( QPainter::CompositionMode_Lighten );
                            if ( onionBlue && onionRed && onionPrev ) {
                                painter.fillRect( viewRect, Qt::blue );
                            }
                            else {
                                painter.fillRect( viewRect, onionColor );
                            }
                            painter.setCompositionMode( QPainter::CompositionMode_SourceOver );
                        }
                    }
                }
            }

            // paints the vector images onion skins
            if ( layer->type() == Layer::VECTOR )
            {
                LayerVector *layerVector = ( LayerVector * )layer;
                QImage *pImage = layerVector->getLastImageAtFrame( frame, 0,
                                                                   sz,
                                                                   m_isSimplified, m_showThinLines,
                                                                   curveOpacity,
                                                                   m_antialiasing );

                if ( pImage != NULL )
                {
                    painter.setWorldMatrixEnabled( false );

                    // previous frame (onion skin)
                    if ( onionPrev ) {
                        QImage *previousImage = layerVector->getLastImageAtFrame( frame, -1, sz,
                                                                                  m_isSimplified,
                                                                                  m_showThinLines,
                                                                                  curveOpacity,
                                                                                  m_antialiasing );
                        if ( previousImage != NULL )
                        {
                            painter.setOpacity( opacity * m_pEditor->getOnionLayer1Opacity() / 100.0 );
                            painter.drawImage( QPoint( 0, 0 ), *previousImage );
                        }
                        QImage* previousImage2 = layerVector->getLastImageAtFrame( frame, -2, sz,
                                                                                   m_isSimplified,
                                                                                   m_showThinLines,
                                                                                   curveOpacity,
                                                                                   m_antialiasing );
                        if ( previousImage2 != NULL )
                        {
                            painter.setOpacity( opacity * m_pEditor->getOnionLayer2Opacity() / 100.0 );
                            painter.drawImage( QPoint( 0, 0 ), *previousImage2 );
                        }
                        QImage *previousImage3 = layerVector->getLastImageAtFrame( frame, -3, sz,
                                                                                   m_isSimplified,
                                                                                   m_showThinLines,
                                                                                   curveOpacity,
                                                                                   m_antialiasing );
                        if ( previousImage3 != NULL )
                        {
                            painter.setOpacity( opacity * m_pEditor->getOnionLayer3Opacity() / 100.0 );
                            painter.drawImage( QPoint( 0, 0 ), *previousImage3 );
                        }
                        if ( onionBlue || onionRed ) {
                            painter.setOpacity( 1.0 );
                            painter.setCompositionMode( QPainter::CompositionMode_Lighten );
                            if ( onionBlue && onionRed && onionNext ) {
                                painter.fillRect( vectorViewRect, Qt::red );
                            }
                            else {
                                painter.fillRect( vectorViewRect, onionColor );
                            }
                            painter.setCompositionMode( QPainter::CompositionMode_SourceOver );
                        }
                    }

                    // next frame (onion skin)
                    if ( onionNext ) {
                        QImage* nextImage = layerVector->getLastImageAtFrame( frame, 1, sz, m_isSimplified, m_showThinLines, curveOpacity, m_antialiasing );
                        if ( nextImage != NULL )
                        {
                            painter.setOpacity( opacity * m_pEditor->getOnionLayer1Opacity() / 100.0 );
                            painter.drawImage( QPoint( 0, 0 ), *nextImage );
                        }
                        QImage* nextImage2 = layerVector->getLastImageAtFrame( frame, 2, sz,
                                                                               m_isSimplified, m_showThinLines, curveOpacity, m_antialiasing );
                        if ( nextImage2 != NULL )
                        {
                            painter.setOpacity( opacity * m_pEditor->getOnionLayer2Opacity() / 100.0 );
                            painter.drawImage( QPoint( 0, 0 ), *nextImage2 );
                        }
                        QImage* nextImage3 = layerVector->getLastImageAtFrame( frame, 3, sz, m_isSimplified, m_showThinLines, curveOpacity, m_antialiasing );
                        if ( nextImage3 != NULL )
                        {
                            painter.setOpacity( opacity * m_pEditor->getOnionLayer3Opacity() / 100.0 );
                            painter.drawImage( QPoint( 0, 0 ), *nextImage3 );
                        }
                        if ( onionBlue || onionRed ) {
                            painter.setOpacity( 1.0 );
                            painter.setCompositionMode( QPainter::CompositionMode_Lighten );
                            if ( onionBlue && onionRed && onionPrev ) {
                                painter.fillRect( vectorViewRect, Qt::blue );
                            }
                            else {
                                painter.fillRect( vectorViewRect, onionColor );
                            }
                            painter.setCompositionMode( QPainter::CompositionMode_SourceOver );
                        }
                    }
                }
            }
        }
    } // --- end onion skins

    // --- current frame ---
    for ( int i = 0; i < object->getLayerCount(); i++ )
    {
        opacity = 1.0;
        if ( i != m_pEditor->layerManager()->currentLayerIndex() && ( m_showAllLayers == 1 ) ) { opacity = 0.4; }

        if ( m_pEditor->getCurrentLayer()->type() == Layer::CAMERA ) { opacity = 1.0; }
        Layer *layer = ( object->getLayer( i ) );
        if ( layer->visible && ( m_showAllLayers > 0 || i == m_pEditor->layerManager()->currentLayerIndex() ) )
        {
            // paints the bitmap images
            if ( layer->type() == Layer::BITMAP )
            {
                LayerBitmap *layerBitmap = ( LayerBitmap * )layer;
                BitmapImage *bitmapImage = layerBitmap->getLastBitmapImageAtFrame( frame, 0 );
                if ( bitmapImage != NULL )
                {
                    painter.setWorldMatrixEnabled( true );
                    painter.setOpacity( opacity );
                    if ( i == m_pEditor->layerManager()->currentLayerIndex() && somethingSelected && ( myRotatedAngle != 0 || myTempTransformedSelection != mySelection || myFlipX != 1 || myFlipY != 1 ) )
                    {
                        // hole in the original selection -- might support arbitrary shapes in the future
                        painter.setClipping( true );
                        QRegion clip = QRegion( mySelection.toRect() );
                        QRegion totalImage = QRegion( myTempView.inverted().mapRect( QRect( -2, -2, width() + 3, height() + 3 ) ) );
                        QRegion ImageWithHole = totalImage -= clip;
                        painter.setClipRegion( ImageWithHole, Qt::ReplaceClip );
                        //painter.drawImage(bitmapImage->topLeft(), *(bitmapImage->image) );
                        bitmapImage->paintImage( painter );
                        painter.setClipping( false );
                        // transforms the bitmap selection
                        bool smoothTransform = false;

                        if ( myTempTransformedSelection.width() != mySelection.width() || myTempTransformedSelection.height() != mySelection.height() || myRotatedAngle != 0 ) { smoothTransform = true; }
                        BitmapImage selectionClip = bitmapImage->copy( mySelection.toRect() );
                        selectionClip.transform( myTransformedSelection, smoothTransform );
                        QMatrix rm;
                        //TODO: complete matrix calls ( sounds funny :)
                        rm.scale( myFlipX, myFlipY );
                        rm.rotate( myRotatedAngle );
                        QImage rotImg = selectionClip.m_pImage->transformed( rm );
                        QPoint dxy = QPoint( ( myTempTransformedSelection.width() - rotImg.rect().width() ) / 2,
                                             ( myTempTransformedSelection.height() - rotImg.rect().height() ) / 2 );
                        *selectionClip.m_pImage = rotImg; // TODO: find/create a func. (*object = data is not very orthodox)
                        selectionClip.boundaries.translate( dxy );
                        selectionClip.paintImage( painter );
                        //painter.drawImage(selectionClip.topLeft(), *(selectionClip.image));
                    }
                    else
                    {
                        //painter.drawImage(bitmapImage->topLeft(), *(bitmapImage->image) );
                        bitmapImage->paintImage( painter );
                    }
                    //painter.setPen(Qt::red);
                    //painter.setBrush(Qt::NoBrush);
                    //painter.drawRect(bitmapImage->boundaries);
                }
            }
            // paints the vector images
            if ( layer->type() == Layer::VECTOR )
            {
                LayerVector *layerVector = ( LayerVector * )layer;
                VectorImage *vectorImage = layerVector->getLastVectorImageAtFrame( frame, 0 );
                if ( somethingSelected )
                {
                    // transforms the vector selection
                    //calculateSelectionTransformation();
                    vectorImage->setSelectionTransformation( selectionTransformation );
                    //vectorImage->setTransformedSelection(myTempTransformedSelection);
                }
                QImage* image = layerVector->getLastImageAtFrame( frame, 0, sz,
                                                                  m_isSimplified, m_showThinLines,
                                                                  curveOpacity, m_antialiasing );
                if ( image != NULL )
                {
                    painter.setWorldMatrixEnabled( false );
                    painter.setOpacity( opacity );
                    painter.drawImage( QPoint( 0, 0 ), *image );
                }
            }
        }
    }

    // --- grids ---
    if ( m_pEditor->getCurrentLayer() != NULL )
    {
        if ( m_pEditor->getCurrentLayer()->type() == Layer::BITMAP ||
             m_pEditor->getCurrentLayer()->type() == Layer::VECTOR )
        {
            painter.setWorldMatrixEnabled( true );
            painter.setPen( Qt::magenta );
            painter.setBrush( Qt::NoBrush );
            painter.drawRect( viewRect.left(), viewRect.top(), viewRect.width(), viewRect.height() );
            // What kind of grid do we want?
            QPen pen( Qt::gray );
            qreal factorY = viewRect.height() / 24.0f;
            qreal factorX = viewRect.width() / 24.0f;
            if ( useGridA )
            {
                painter.setOpacity( 0.5 );
                painter.setPen( Qt::magenta );
                painter.drawLine( viewRect.left(), 0, viewRect.right(), 0 );
                for ( int y = 1; y < 12; y++ ){
                    if ( y % 3 == 0 )
                    {
                        painter.setPen( Qt::gray );
                    }
                    else
                    {
                        painter.setPen( Qt::lightGray );
                    }
                    painter.drawLine( viewRect.left(), y*factorY, viewRect.right(), y*factorY );
                    painter.drawLine( viewRect.left(), -y*factorY, viewRect.right(), -y*factorY );
                }
                // vertical lines
                painter.setPen( Qt::magenta );
                painter.drawLine( 0, viewRect.top(), 0, viewRect.bottom() );
                for ( int x = 1; x < 12; x++ ){
                    if ( x % 3 == 0 )
                    {
                        painter.setPen( Qt::gray );
                    }
                    else
                    {
                        painter.setPen( Qt::lightGray );
                    }
                    painter.drawLine( x*factorX, viewRect.top(), x*factorX, viewRect.bottom() );
                    painter.drawLine( -x*factorX, viewRect.top(), -x*factorX, viewRect.bottom() );
                }
                if ( useGridB )
                {
                    //painter.setOpacity(0.5);
                    painter.setPen( Qt::gray );
                    for ( int n = 1; n < 12; n++ ){
                        painter.drawText( n*factorX, n*factorY, QString( "%2" ).arg( n ) );
                        painter.drawText( -n*factorX, n*factorY, QString( "%2" ).arg( n ) );
                        painter.drawText( n*factorX, -n*factorY, QString( "%2" ).arg( n ) );
                        painter.drawText( -n*factorX, -n*factorY, QString( "%2" ).arg( n ) );
                    }
                }
            }
        }
    }
    // --- eo grids
    painter.end();
}

void ScribbleArea::setGaussianGradient( QGradient &gradient, QColor colour, qreal opacity, qreal offset )
{
    int r = colour.red();
    int g = colour.green();
    int b = colour.blue();
    qreal a = colour.alphaF();
    gradient.setColorAt( 0.0, QColor( r, g, b, qRound( a * 255 * opacity ) ) );
    gradient.setColorAt( offset + 0.0 * ( 1.0 - offset ), QColor( r, g, b, qRound( a * 255 * opacity ) ) );
    gradient.setColorAt( offset + 0.1 * ( 1.0 - offset ), QColor( r, g, b, qRound( a * 245 * opacity ) ) );
    gradient.setColorAt( offset + 0.2 * ( 1.0 - offset ), QColor( r, g, b, qRound( a * 217 * opacity ) ) );
    gradient.setColorAt( offset + 0.3 * ( 1.0 - offset ), QColor( r, g, b, qRound( a * 178 * opacity ) ) );
    gradient.setColorAt( offset + 0.4 * ( 1.0 - offset ), QColor( r, g, b, qRound( a * 134 * opacity ) ) );
    gradient.setColorAt( offset + 0.5 * ( 1.0 - offset ), QColor( r, g, b, qRound( a * 94 * opacity ) ) );
    gradient.setColorAt( offset + 0.6 * ( 1.0 - offset ), QColor( r, g, b, qRound( a * 60 * opacity ) ) );
    gradient.setColorAt( offset + 0.7 * ( 1.0 - offset ), QColor( r, g, b, qRound( a * 36 * opacity ) ) );
    gradient.setColorAt( offset + 0.8 * ( 1.0 - offset ), QColor( r, g, b, qRound( a * 20 * opacity ) ) );
    gradient.setColorAt( offset + 0.9 * ( 1.0 - offset ), QColor( r, g, b, qRound( a * 10 * opacity ) ) );
    gradient.setColorAt( offset + 1.0 * ( 1.0 - offset ), QColor( r, g, b, 0 ) );
}


void ScribbleArea::drawBrush( QPointF thePoint, qreal brushWidth, qreal offset, QColor fillColour, qreal opacity )
{
    QRadialGradient radialGrad( thePoint, 0.5 * brushWidth );
    setGaussianGradient( radialGrad, fillColour, opacity, offset );

    //radialGrad.setCenter( thePoint );
    //radialGrad.setFocalPoint( thePoint );

    QRectF rectangle( thePoint.x() - 0.5 * brushWidth, thePoint.y() - 0.5 * brushWidth, brushWidth, brushWidth );

    BitmapImage *tempBitmapImage = new BitmapImage;

    // FIXME: check this.
    /*
    if ( followContour )
    {
    tempBitmapImage = new BitmapImage( rectangle.toRect(), QColor( 0, 0, 0, 0 ) );
    //tempBitmapImage->drawRect( rectangle, Qt::NoPen, QColor(0,0,0,0), QPainter::CompositionMode_Source, antialiasing);
    Layer *layer = m_pEditor->getCurrentLayer();
    if ( layer == NULL ) { return; }
    int index = ( ( LayerImage * )layer )->getLastIndexAtFrame( m_pEditor->layerManager()->currentFrameIndex() );
    if ( index == -1 ) { return; }
    BitmapImage *bitmapImage = ( ( LayerBitmap * )layer )->getLastBitmapImageAtFrame( m_pEditor->layerManager()->currentFrameIndex(), 0 );
    if ( bitmapImage == NULL ) { qDebug() << "NULL image pointer!" << m_pEditor->layerManager()->currentLayerIndex() << m_pEditor->layerManager()->currentFrameIndex();  return; }
    BitmapImage::floodFill( bitmapImage, tempBitmapImage, thePoint.toPoint(), qRgba( 255, 255, 255, 0 ), fillColour.rgb(), 20 * 20, false );
    tempBitmapImage->drawRect( rectangle.toRect(), Qt::NoPen, radialGrad, QPainter::CompositionMode_SourceIn, m_antialiasing );
    }
    else
    */
    {
        tempBitmapImage = new BitmapImage;
        tempBitmapImage->drawRect( rectangle, Qt::NoPen, radialGrad, QPainter::CompositionMode_Source, m_antialiasing );
    }

    bufferImg->paste( tempBitmapImage );
    delete tempBitmapImage;
}


void ScribbleArea::drawTexturedBrush( BitmapImage *bmiSource_, QPointF srcPoint_, QPointF thePoint_, qreal brushWidth_, qreal offset_, qreal opacity_ )
{
    // drawTexturedBrush() is not used anymore although code can be used for textured brushes.
    // TODO: Add argument "pixelStep_", send texture through "bmiSource_"; texture brush should work.
    QRadialGradient radialGrad( thePoint_, 0.5 * brushWidth_ );
    setGaussianGradient( radialGrad, QColor( 255, 255, 255, 255 ), opacity_, offset_ );

    QRectF srcRect( srcPoint_.x() - 0.5 * brushWidth_, srcPoint_.y() - 0.5 * brushWidth_, brushWidth_, brushWidth_ );
    QRectF trgRect( thePoint_.x() - 0.5 * brushWidth_, thePoint_.y() - 0.5 * brushWidth_, brushWidth_, brushWidth_ );

    BitmapImage bmiSrcClip = bmiSource_->copy( srcRect.toRect() );
    BitmapImage *bmiTmpClip = new BitmapImage;
    bmiTmpClip->drawRect( trgRect, Qt::NoPen, radialGrad, QPainter::CompositionMode_Source, m_antialiasing );
    bmiSrcClip.boundaries.moveTo( trgRect.topLeft().toPoint() );
    bmiTmpClip->paste( &bmiSrcClip, QPainter::CompositionMode_SourceAtop );
    bufferImg->paste( bmiTmpClip );
    delete bmiTmpClip;
}

void ScribbleArea::blurBrush( BitmapImage *bmiSource_, QPointF srcPoint_, QPointF thePoint_, qreal brushWidth_, qreal offset_, qreal opacity_ )
{
    QRadialGradient radialGrad( thePoint_, 0.5 * brushWidth_ );
    setGaussianGradient( radialGrad, QColor( 255, 255, 255, 127 ), opacity_, offset_ );

    QRectF srcRect( srcPoint_.x() - 0.5 * brushWidth_, srcPoint_.y() - 0.5 * brushWidth_, brushWidth_, brushWidth_ );
    QRectF trgRect( thePoint_.x() - 0.5 * brushWidth_, thePoint_.y() - 0.5 * brushWidth_, brushWidth_, brushWidth_ );

    BitmapImage bmiSrcClip = bmiSource_->copy( srcRect.toRect() );
    BitmapImage bmiTmpClip = bmiSrcClip; // todo: find a shorter way

    bmiTmpClip.drawRect( srcRect, Qt::NoPen, radialGrad, QPainter::CompositionMode_Source, m_antialiasing );
    bmiSrcClip.boundaries.moveTo( trgRect.topLeft().toPoint() );
    bmiTmpClip.paste( &bmiSrcClip, QPainter::CompositionMode_SourceAtop );
    bufferImg->paste( &bmiTmpClip );
}

void ScribbleArea::liquifyBrush( BitmapImage *bmiSource_, QPointF srcPoint_, QPointF thePoint_, qreal brushWidth_, qreal offset_, qreal opacity_ )
{
    QPointF delta = ( thePoint_ - srcPoint_ ); // increment vector
    QRectF trgRect( thePoint_.x() - 0.5 * brushWidth_, thePoint_.y() - 0.5 * brushWidth_, brushWidth_, brushWidth_ );

    QRadialGradient radialGrad( thePoint_, 0.5 * brushWidth_ );
    setGaussianGradient( radialGrad, QColor( 255, 255, 255, 255 ), opacity_, offset_ );

    // Create gradient brush
    BitmapImage* bmiTmpClip = new BitmapImage;
    bmiTmpClip->drawRect( trgRect, Qt::NoPen, radialGrad, QPainter::CompositionMode_Source, m_antialiasing );

    // Slide texture/pixels of the source image
    qreal factor, factorGrad;
    int xb, yb, xa, ya;

    for ( yb = bmiTmpClip->boundaries.top(); yb < bmiTmpClip->boundaries.bottom(); yb++ )
    {
        for ( xb = bmiTmpClip->boundaries.left(); xb < bmiTmpClip->boundaries.right(); xb++ )
        {
            QColor color;
            color.setRgba( bmiTmpClip->pixel( xb, yb ) );
            factorGrad = color.alphaF(); // any from r g b a is ok

            xa = xb - factorGrad*delta.x();
            ya = yb - factorGrad*delta.y();

            color.setRgba( bmiSource_->pixel( xa, ya ) );
            factor = color.alphaF();

            if ( factor > 0.0 )
            {
                color.setRed( color.red() / factor );
                color.setGreen( color.green() / factor );
                color.setBlue( color.blue() / factor );
                color.setAlpha( 255 ); // Premultiplied color

                color.setRed( color.red()*factorGrad );
                color.setGreen( color.green()*factorGrad );
                color.setBlue( color.blue()*factorGrad );
                color.setAlpha( 255 * factorGrad ); // Premultiplied color

                bmiTmpClip->setPixel( xb, yb, color.rgba() );
            }
            else {
                bmiTmpClip->setPixel( xb, yb, qRgba( 255, 255, 255, 255 ) );
            }
        }
    }
    bufferImg->paste( bmiTmpClip );
    delete bmiTmpClip;
}

void ScribbleArea::drawPolyline( QList<QPointF> points, QPointF endPoint )
{
    if ( !areLayersSane() )
    {
        return;
    }

    if ( points.size() > 0 )
    {
        QPen pen2( m_pEditor->colorManager()->frontColor(),
                   getTool( PEN )->properties.width,
                   Qt::SolidLine,
                   Qt::RoundCap,
                   Qt::RoundJoin );
        QPainterPath tempPath = BezierCurve( points ).getSimplePath();
        tempPath.lineTo( endPoint );
        QRect updateRect = myTempView.mapRect( tempPath.boundingRect().toRect() ).adjusted( -10, -10, 10, 10 );
        if ( m_pEditor->getCurrentLayer()->type() == Layer::VECTOR )
        {
            tempPath = myTempView.map( tempPath );
            if ( m_makeInvisible )
            {
                pen2.setWidth( 0 );
                pen2.setStyle( Qt::DotLine );
            }
            else
            {
                pen2.setWidth( getTool( PEN )->properties.width * myTempView.m11() );
            }
        }
        bufferImg->clear();
        bufferImg->drawPath( tempPath, pen2, Qt::NoBrush, QPainter::CompositionMode_SourceOver, m_antialiasing );

        update( updateRect );
    }
}

void ScribbleArea::endPolyline( QList<QPointF> points )
{
    if ( !areLayersSane() )
    {
        return;
    }

    Layer *layer = m_pEditor->getCurrentLayer();

    if ( layer->type() == Layer::VECTOR )
    {
        BezierCurve curve = BezierCurve( points );
        if ( m_makeInvisible )
        {
            curve.setWidth( 0 );
        }
        else
        {
            curve.setWidth( getTool( PEN )->properties.width );
        }
        curve.setColourNumber( m_pEditor->colorManager()->frontColorNumber() );
        curve.setVariableWidth( false );
        curve.setInvisibility( m_makeInvisible );
        //curve.setSelected(true);
        ( ( LayerVector * )layer )->getLastVectorImageAtFrame( m_pEditor->layerManager()->currentFrameIndex(), 0 )->addCurve( curve, qAbs( myTempView.m11() ) );
    }
    if ( layer->type() == Layer::BITMAP )
    {
        drawPolyline( points, points.last() );
        BitmapImage *bitmapImage = ( ( LayerBitmap * )layer )->getLastBitmapImageAtFrame( m_pEditor->layerManager()->currentFrameIndex(), 0 );
        bitmapImage->paste( bufferImg );
    }
    bufferImg->clear();
    setModified( m_pEditor->layerManager()->currentLayerIndex(), m_pEditor->layerManager()->currentFrameIndex() );
}

void ScribbleArea::resizeEvent( QResizeEvent *event )
{
    //resize( size() );
    QWidget::resizeEvent( event );
    canvas = QPixmap( size() );
    recentre();
    updateAllFrames();
}

void ScribbleArea::zoom()
{
    centralView.scale( 1.2, 1.2 );
    setView();
    updateAllFrames();
}

void ScribbleArea::zoom1()
{
    centralView.scale( 0.8, 0.8 );
    setView();
    updateAllFrames();
}

void ScribbleArea::rotatecw()
{
    centralView.rotate( 20 );
    setView();
    updateAllFrames();
}

void ScribbleArea::rotateacw()
{
    centralView.rotate( -20 );
    setView();
    updateAllFrames();
}

void ScribbleArea::recentre()
{
    centralView = QMatrix( 1, 0, 0, 1, 0.5 * width(), 0.5 * height() );
    setView();
    QPixmapCache::clear();
    updateAllFrames();
}

/************************************************************************************/
// view handling

void ScribbleArea::setMyView( QMatrix view )
{
    myView = view;
}

QMatrix ScribbleArea::getMyView()
{
    return myView;
}

void ScribbleArea::setView()
{
    setView( getView() );
}

void ScribbleArea::setView( QMatrix view )
{
    for ( int i = 0; i < m_pEditor->object()->getLayerCount(); i++ )
    {
        Layer *layer = m_pEditor->object()->getLayer( i );
        if ( layer->type() == Layer::VECTOR )
        {
            ( ( LayerVector * )layer )->setView( view * centralView );
        }
    }
    myTempView = view * centralView;
}

void ScribbleArea::resetView()
{
    m_pEditor->resetMirror();
    myView.reset();
    myTempView = myView * centralView;
    recentre();
}

QMatrix ScribbleArea::getView()
{
    Layer *layer = m_pEditor->getCurrentLayer();
    if ( layer == NULL )
    {
        return QMatrix(); // TODO: error
    }
    if ( layer->type() == Layer::CAMERA )
    {
        return ( ( LayerCamera * )layer )->getViewAtFrame( m_pEditor->layerManager()->currentFrameIndex() );
        qDebug() << "viewCamera" << ( ( LayerCamera * )layer )->getViewAtFrame( m_pEditor->layerManager()->currentFrameIndex() );
    }
    else
    {
        return myView;
    }
}

QRectF ScribbleArea::getViewRect()
{
    QRectF rect = QRectF( -width() / 2, -height() / 2, width(), height() );
    Layer *layer = m_pEditor->getCurrentLayer();
    if ( layer == NULL ) { return rect; }
    if ( layer->type() == Layer::CAMERA )
    {
        return ( ( LayerCamera * )layer )->getViewRect();
    }
    else
    {
        return rect;
    }
}

QPointF ScribbleArea::getCentralPoint()
{
    return myTempView.inverted().map( QPoint( width() / 2, height() / 2 ) );
}

void ScribbleArea::setTransformationMatrix( QMatrix matrix )
{
    transMatrix = matrix;
    update();
    setAllDirty();
}

void ScribbleArea::applyTransformationMatrix()
{
    Layer *layer = m_pEditor->getCurrentLayer();
    if ( layer == NULL ) { return; }

    clearBitmapBuffer();
    if ( layer->type() == Layer::CAMERA )
    {
        LayerCamera *layerCamera = ( LayerCamera * )layer;
        QMatrix view = layerCamera->getViewAtFrame( m_pEditor->layerManager()->currentFrameIndex() );
        layerCamera->loadImageAtFrame( m_pEditor->layerManager()->currentFrameIndex(), view * transMatrix );
        //Camera* camera = ((LayerCamera*)layer)->getLastCameraAtFrame(editor->currentFrame, 0);
        //camera->view = camera->view * transMatrix;
    }
    else
    {
        myView = myView * transMatrix;
    }
    transMatrix.reset();
    updateAllVectorLayers();
    setAllDirty();
}

/************************************************************************************/
// selection handling

void ScribbleArea::calculateSelectionRect()
{
    selectionTransformation.reset();
    Layer *layer = m_pEditor->getCurrentLayer();
    if ( layer == NULL ) { return; }
    if ( layer->type() == Layer::VECTOR )
    {
        VectorImage *vectorImage = ( ( LayerVector * )layer )->getLastVectorImageAtFrame( m_pEditor->layerManager()->currentFrameIndex(), 0 );
        vectorImage->calculateSelectionRect();
        setSelection( vectorImage->getSelectionRect(), true );
    }
}

void ScribbleArea::calculateSelectionTransformation() // Vector layer transform
{
    qreal c1x, c1y, c2x, c2y, scaleX, scaleY;
    c1x = 0.5 * ( myTempTransformedSelection.left() + myTempTransformedSelection.right() );
    c1y = 0.5 * ( myTempTransformedSelection.top() + myTempTransformedSelection.bottom() );
    c2x = 0.5 * ( mySelection.left() + mySelection.right() );
    c2y = 0.5 * ( mySelection.top() + mySelection.bottom() );
    if ( mySelection.width() == 0 ) { scaleX = 1.0; }
    else { scaleX = myTempTransformedSelection.width() / mySelection.width(); }
    if ( mySelection.height() == 0 ) { scaleY = 1.0; }
    else { scaleY = myTempTransformedSelection.height() / mySelection.height(); }
    selectionTransformation.reset();
    selectionTransformation.translate( c1x, c1y );
    selectionTransformation.scale( scaleX, scaleY );
    selectionTransformation.translate( -c2x, -c2y );
    //modification();
}

void ScribbleArea::paintTransformedSelection()
{
    Layer *layer = m_pEditor->getCurrentLayer();
    if ( layer == NULL )
    {
        return;
    }

    if ( somethingSelected )    // there is something selected
    {
        if ( layer->type() == Layer::BITMAP && ( myRotatedAngle != 0.0 || myTransformedSelection != mySelection || myFlipX != 1 || myFlipY != 1 ) )
        {
            //backup();
            BitmapImage *bitmapImage = ( ( LayerBitmap * )layer )->getLastBitmapImageAtFrame( m_pEditor->layerManager()->currentFrameIndex(), 0 );
            if ( bitmapImage == NULL )
            {
                qDebug() << "NULL image pointer!"
                    << m_pEditor->layerManager()->currentLayerIndex()
                    << m_pEditor->layerManager()->currentFrameIndex();
                return;
            }

            bool smoothTransform = false;
            if ( myTransformedSelection.width() != mySelection.width() || myTransformedSelection.height() != mySelection.height() || m_moveMode == ROTATION ) { smoothTransform = true; }
            QMatrix rm;
            rm.scale( myFlipX, myFlipY );
            rm.rotate( myRotatedAngle );
            BitmapImage selectionClip = bitmapImage->copy( mySelection.toRect() );
            selectionClip.transform( myTransformedSelection, smoothTransform );
            QImage rotImg = selectionClip.m_pImage->transformed( rm, Qt::SmoothTransformation );
            QPoint dxy = QPoint( ( myTempTransformedSelection.width() - rotImg.rect().width() ) / 2,
                                 ( myTempTransformedSelection.height() - rotImg.rect().height() ) / 2 );
            *selectionClip.m_pImage = rotImg; // TODO: find/create a func. (*object = data is not very orthodox)
            selectionClip.boundaries.translate( dxy );
            bitmapImage->clear( mySelection.toRect() );
            bitmapImage->paste( &selectionClip );
        }
        if ( layer->type() == Layer::VECTOR )
        {
            // vector transformation
            LayerVector *layerVector = ( LayerVector * )layer;
            VectorImage *vectorImage = layerVector->getLastVectorImageAtFrame( m_pEditor->layerManager()->currentFrameIndex(), 0 );
            vectorImage->applySelectionTransformation();
            selectionTransformation.reset();
        }
        setModified( m_pEditor->layerManager()->currentLayerIndex(), m_pEditor->layerManager()->currentFrameIndex() );
    }
}

void ScribbleArea::setSelection( QRectF rect, bool trueOrFalse )
{
    mySelection = rect;
    myTransformedSelection = rect;
    myTempTransformedSelection = rect;
    somethingSelected = trueOrFalse;
    displaySelectionProperties();
}

void ScribbleArea::displaySelectionProperties()
{
    Layer *layer = m_pEditor->getCurrentLayer();
    if ( layer == NULL ) { return; }
    if ( layer->type() == Layer::VECTOR )
    {
        LayerVector *layerVector = ( LayerVector * )layer;
        VectorImage *vectorImage = layerVector->getLastVectorImageAtFrame( m_pEditor->layerManager()->currentFrameIndex(), 0 );
        //vectorImage->applySelectionTransformation();
        if ( currentTool()->type() == MOVE )
        {
            int selectedCurve = vectorImage->getFirstSelectedCurve();
            if ( selectedCurve != -1 )
            {
                m_pEditor->toolManager()->setWidth( vectorImage->curve[ selectedCurve ].getWidth() );
                m_pEditor->toolManager()->setFeather( vectorImage->curve[ selectedCurve ].getFeather() );
                m_pEditor->toolManager()->setInvisibility( vectorImage->curve[ selectedCurve ].isInvisible() );
                m_pEditor->toolManager()->setPressure( vectorImage->curve[ selectedCurve ].getVariableWidth() );
                m_pEditor->colorManager()->pickColorNumber( vectorImage->curve[ selectedCurve ].getColourNumber() );
            }

            int selectedArea = vectorImage->getFirstSelectedArea();
            if ( selectedArea != -1 )
            {
                m_pEditor->colorManager()->pickColorNumber( vectorImage->area[ selectedArea ].colourNumber );
            }
        }
    }
}

void ScribbleArea::selectAll()
{
    offset.setX( 0 );
    offset.setY( 0 );
    Layer *layer = m_pEditor->getCurrentLayer();
    if ( layer == NULL ) { return; }
    if ( layer->type() == Layer::BITMAP )
    {
        setSelection( myTempView.inverted().mapRect( QRect( -2, -2, width() + 3, height() + 3 ) ), true ); // TO BE IMPROVED
    }
    if ( layer->type() == Layer::VECTOR )
    {
        VectorImage *vectorImage = ( ( LayerVector * )layer )->getLastVectorImageAtFrame( m_pEditor->layerManager()->currentFrameIndex(), 0 );
        vectorImage->selectAll();
        setSelection( vectorImage->getSelectionRect(), true );
    }
    updateFrame();
}

void ScribbleArea::deselectAll()
{
    offset.setX( 0 );
    offset.setY( 0 );
    selectionTransformation.reset();
    mySelection.setRect( 10, 10, 20, 20 );
    myTransformedSelection.setRect( 10, 10, 20, 20 );
    myTempTransformedSelection.setRect( 10, 10, 20, 20 );

    Layer *layer = m_pEditor->getCurrentLayer();
    if ( layer == NULL ) { return; }
    if ( layer->type() == Layer::VECTOR )
    {
        ( ( LayerVector * )layer )->getLastVectorImageAtFrame( m_pEditor->layerManager()->currentFrameIndex(), 0 )->deselectAll();
    }
    somethingSelected = false;
    bufferImg->clear();
    vectorSelection.clear();

    // clear all the data tools may have accumulated
    getEditor()->toolManager()->cleanupAllToolsData();

    updateFrame();
}

void ScribbleArea::toggleOnionNext( bool checked )
{
    onionNext = checked;
    updateAllFrames();
    emit onionNextChanged( onionNext );
}

void ScribbleArea::toggleOnionPrev( bool checked )
{
    onionPrev = checked;
    updateAllFrames();
    emit onionPrevChanged( onionPrev );
}

void ScribbleArea::toggleMultiLayerOnionSkin( bool checked )
{
    m_isMultiLayerOnionSkin = checked;
    updateAllFrames();
    emit multiLayerOnionSkinChanged( m_isMultiLayerOnionSkin );
}


void ScribbleArea::toggledOnionColor()
{
    if ( onionBlue ) {
        if ( onionRed ) {
            onionColor = QColor( 232, 48, 255, 255 ); // subtle violet ( blue + red )
        }
        else {
            onionColor = Qt::blue;
        }
    }
    else if ( onionRed ) {
        onionColor = Qt::red;
    }
}

void ScribbleArea::toggleOnionBlue( bool checked )
{
    onionBlue = checked;
    toggledOnionColor();
    updateAllFrames();
}

void ScribbleArea::toggleOnionRed( bool checked )
{
    onionRed = checked;
    toggledOnionColor();
    updateAllFrames();
}

void ScribbleArea::toggleGridA( bool checked )
{
    useGridA = checked;
    updateAllFrames();
}

void ScribbleArea::toggleGridB( bool checked )
{
    useGridB = checked;
    updateAllFrames();
}

void ScribbleArea::floodFill( VectorImage *vectorImage, QPoint point, QRgb targetColour, QRgb replacementColour, int tolerance )
{
    bool invertible;

    QPointF initialPoint = myTempView.inverted( &invertible ).map( QPointF( point ) );

    // Step 1: peforms a standard (pixel-based) flood fill, and finds the vertices on the contour of the filled area
    qreal tol = 8.0 / qAbs( myTempView.m11() ); // tolerance for finding vertices along the contour of the flood-filled area
    qreal tol2 = 1.5 / qAbs( myTempView.m11() ); // tolerance for connecting contour vertices from different curves // should be small as close points of different curves are supposed to coincide
    QList<QPoint> queue; // queue all the pixels of the filled area (as they are found)
    QList<QPoint> contourPixels; // list of the pixels near the contour of the filled area
    int j, k;
    bool condition;
    //vectorImage->update(true, showThinLines); // update the vector image with simplified curves (all width=1)
    QImage *targetImage = new QImage( size(), QImage::Format_ARGB32_Premultiplied );
    vectorImage->outputImage( targetImage, size(), myTempView, true, m_showThinLines, 1.0, true ); // the target image is the vector image with simplified curves (all width=1)
    //QImage* replaceImage = &bufferImg;
    QImage *replaceImage = new QImage( size(), QImage::Format_ARGB32_Premultiplied );
    QList<VertexRef> points = vectorImage->getAllVertices(); // refs of all the points
    QList<VertexRef> boxPoints; // refs of points inside the bounding box
    QList<VertexRef> contourPoints; // refs of points near the contour pixels
    QList<VertexRef> vertices;
    if ( BitmapImage::rgbDistance( targetImage->pixel( point.x(), point.y() ), targetColour ) > tolerance ) { return; }
    queue.append( point );
    int boxLeft = point.x();
    int boxRight = point.x();
    int boxTop = point.y();
    int boxBottom = point.y();
    // ----- flood fill and remember the contour pixels -> contourPixels
    // ----- from the standard flood fill algorithm
    // ----- http://en.wikipedia.org/wiki/Flood_fill
    j = -1;
    k = 1;
    for ( int i = 0; i < queue.size(); i++ )
    {
        point = queue.at( i );
        if ( replaceImage->pixel( point.x(), point.y() ) != replacementColour  && BitmapImage::rgbDistance( targetImage->pixel( point.x(), point.y() ), targetColour ) < tolerance )
        {
            //image.setPixel( point.x(), point.y(), replacementColour);
            j = -1;
            condition = ( point.x() + j > 0 );
            while ( replaceImage->pixel( point.x() + j, point.y() ) != replacementColour  && BitmapImage::rgbDistance( targetImage->pixel( point.x() + j, point.y() ), targetColour ) < tolerance && condition )
            {
                j = j - 1;
                condition = ( point.x() + j > 0 );
            }
            if ( !condition ) { floodFillError( 1 ); return; }
            if ( BitmapImage::rgbDistance( targetImage->pixel( point.x() + j, point.y() ), targetColour ) >= tolerance )      // bumps into the contour
            {
                contourPixels.append( point + QPoint( j, 0 ) );
            }

            k = 1;
            condition = ( point.x() + k < targetImage->width() - 1 );
            while ( replaceImage->pixel( point.x() + k, point.y() ) != replacementColour  && BitmapImage::rgbDistance( targetImage->pixel( point.x() + k, point.y() ), targetColour ) < tolerance && condition )
            {
                k = k + 1;
                condition = ( point.x() + k < targetImage->width() - 1 );
            }
            if ( !condition ) { floodFillError( 1 ); return; }
            if ( BitmapImage::rgbDistance( targetImage->pixel( point.x() + k, point.y() ), targetColour ) >= tolerance )      // bumps into the contour
            {
                contourPixels.append( point + QPoint( k, 0 ) );
            }
            if ( point.x() + k > boxRight ) { boxRight = point.x() + k; }
            if ( point.x() + j < boxLeft ) { boxLeft = point.x() + j; }
            for ( int x = j + 1; x < k; x++ )
            {
                replaceImage->setPixel( point.x() + x, point.y(), replacementColour );
                if ( point.y() - 1 > 0 && queue.size() < targetImage->height() * targetImage->width() )
                {
                    if ( replaceImage->pixel( point.x() + x, point.y() - 1 ) != replacementColour )
                    {
                        if ( BitmapImage::rgbDistance( targetImage->pixel( point.x() + x, point.y() - 1 ), targetColour ) < tolerance )
                        {
                            queue.append( point + QPoint( x, -1 ) );
                            if ( point.y() - 1 < boxBottom ) { boxBottom = point.y() - 1; }
                        }
                        else   // bumps into the contour
                        {
                            contourPixels.append( point + QPoint( x, -1 ) );
                        }
                    }
                }
                else { floodFillError( 1 ); return; }
                if ( point.y() + 1 < targetImage->height() && queue.size() < targetImage->height() * targetImage->width() )
                {
                    if ( replaceImage->pixel( point.x() + x, point.y() + 1 ) != replacementColour )
                    {
                        if ( BitmapImage::rgbDistance( targetImage->pixel( point.x() + x, point.y() + 1 ), targetColour ) < tolerance )
                        {
                            queue.append( point + QPoint( x, 1 ) );
                            if ( point.y() + 1 > boxTop ) { boxTop = point.y() + 1; }
                        }
                        else   // bumps into the contour
                        {
                            contourPixels.append( point + QPoint( x, 1 ) );
                        }
                    }
                }
                else { floodFillError( 1 ); return; }
            }
        }
    }
    // --- finds the bounding box of the filled area, and all the points contained inside (+ 1*tol)  -> boxPoints
    QPointF mBoxTopRight = myTempView.inverted( &invertible ).map( QPointF( qMax( boxLeft, boxRight ) + 1 * tol, qMax( boxTop, boxBottom ) + 1 * tol ) );
    QPointF mBoxBottomLeft = myTempView.inverted( &invertible ).map( QPointF( qMin( boxLeft, boxRight ) - 1 * tol, qMin( boxTop, boxBottom ) - 1 * tol ) );
    QRectF boundingBox = QRectF( mBoxBottomLeft.x() - 1, mBoxBottomLeft.y() - 1, qAbs( mBoxBottomLeft.x() - mBoxTopRight.x() ) + 2, qAbs( mBoxBottomLeft.y() - mBoxTopRight.y() ) + 2 );
    debugRect = QRectF( 0, 0, 0, 0 );
    debugRect = boundingBox;
    for ( int l = 0; l < points.size(); l++ )
    {
        QPointF mPoint = vectorImage->getVertex( points.at( l ) );
        if ( boundingBox.contains( mPoint ) )
        {
            // -----
            //vectorImage->setSelected(points.at(l), true);
            boxPoints.append( points.at( l ) );
        }
    }
    // ---- finds the points near the contourPixels -> contourPoints
    for ( int i = 0; i < contourPixels.size(); i++ )
    {
        QPointF mPoint = myTempView.inverted( &invertible ).map( QPointF( contourPixels.at( i ) ) );
        vertices = vectorImage->getAndRemoveVerticesCloseTo( mPoint, tol, &boxPoints );
        //contourPoints << vertices;
        for ( int m = 0; m < vertices.size(); m++ ) // for each ?
        {
            contourPoints.append( vertices.at( m ) );
        }
    }
    // ---- points of sharp peaks may be missing in contourPoints ---> we correct for that
    for ( int i = 0; i < contourPoints.size(); i++ )
    {
        VertexRef theNextVertex = contourPoints[ i ].nextVertex();
        if ( !contourPoints.contains( theNextVertex ) )     // if the next vertex is not in the list of contour points
        {
            if ( contourPoints.contains( theNextVertex.nextVertex() ) )       // but the next-next vertex is...
            {
                contourPoints.append( theNextVertex );
                //qDebug() << "----- found SHARP point (type 1a) ------";
            }
            QList<VertexRef> closePoints = vectorImage->getVerticesCloseTo( theNextVertex, tol2 );
            for ( int j = 0; j < closePoints.size(); j++ )
            {
                if ( closePoints[ j ] != theNextVertex )     // ...or a point connected to the next vertex is
                {
                    if ( contourPoints.contains( closePoints[ j ].nextVertex() ) || contourPoints.contains( closePoints[ j ].prevVertex() ) )
                    {
                        contourPoints.append( theNextVertex );
                        contourPoints.append( closePoints[ j ] );
                        //qDebug() << "----- found SHARP point (type 2a) ------";
                    }
                }
            }
        }
        VertexRef thePreviousVertex = contourPoints[ i ].prevVertex();
        if ( !contourPoints.contains( thePreviousVertex ) )     // if the previous vertex is not in the list of contour points
        {
            if ( contourPoints.contains( thePreviousVertex.prevVertex() ) )       // but the prev-prev vertex is...
            {
                contourPoints.append( thePreviousVertex );
                //qDebug() << "----- found SHARP point (type 1b) ------";
            }
            QList<VertexRef> closePoints = vectorImage->getVerticesCloseTo( thePreviousVertex, tol2 );
            for ( int j = 0; j < closePoints.size(); j++ )
            {
                if ( closePoints[ j ] != thePreviousVertex )     // ...or a point connected to the previous vertex is
                {
                    if ( contourPoints.contains( closePoints[ j ].nextVertex() ) || contourPoints.contains( closePoints[ j ].prevVertex() ) )
                    {
                        contourPoints.append( thePreviousVertex );
                        contourPoints.append( closePoints[ j ] );
                        //qDebug() << "----- found SHARP point (type 2b) ------";
                    }
                }
            }
        }
    }

    // 1 --- stop here (for debugging purpose)
    /*qDebug() << "CONTOUR POINTS:";
    for(int i=0; i < contourPoints.size(); i++) {
    qDebug() << "(" << contourPoints.at(i).curveNumber << "," << contourPoints.at(i).vertexNumber << ")";
    }*/
    // -----
    vectorImage->setSelected( contourPoints, true );
    update();

    // 2 --- or continue

    // Step 2: finds closed paths among the selected vertices: we start from a vertex and build a tree of connected vertices
    //while (contourPoints.size() > 0) {
    QList<VertexRef> tree;
    QList<int> fatherNode; // given the index in tree (of a vertex), return the index (in tree) of its father vertex; this will define the tree structure
    QList<int> leaves; // list of indices in tree which correspond to end of branches (leaves)

    // Step 2.1: build tree
    int rootIndex = -1;
    bool rootIndexFound = false;
    while ( !rootIndexFound && rootIndex < contourPoints.size() - 1 )
    {
        rootIndex++;
        if ( vectorImage->getVerticesCloseTo( vectorImage->getVertex( contourPoints.at( rootIndex ) ), tol2, &contourPoints ).size() > 1 )
        {
            // this point is connected!
            rootIndexFound = true;
        }
    }
    if ( !rootIndexFound ) { floodFillError( 3 ); return; }
    tree << contourPoints.at( rootIndex );
    fatherNode.append( -1 );
    //leaves << 0;
    contourPoints.removeAt( rootIndex );
    VertexRef vertex0 = tree.at( 0 );
    //qDebug() << "ROOT = " <<  vertex0.curveNumber << "," << vertex0.vertexNumber;
    j = 0;
    bool success = false;
    int counter = 0;
    while ( !success && j > -1 && counter < 1000 )
    {
        counter++;
        //qDebug() << "------";
        VertexRef vertex = tree.at( j );
        //qDebug() << j << "/" << tree.size() << "   " << vertex.curveNumber << "," << vertex.vertexNumber << "->" << fatherNode.at(j);
        int index1 = contourPoints.indexOf( vertex.nextVertex() );
        if ( index1 != -1 )
        {
            //qDebug() << "next vertex";
            tree.append( vertex.nextVertex() );
            fatherNode.append( j );
            contourPoints.removeAt( index1 );
            j = tree.size() - 1;
        }
        else
        {
            int index2 = contourPoints.indexOf( vertex.prevVertex() );
            if ( index2 != -1 )
            {
                // qDebug() << "previous vertex";
                tree.append( vertex.prevVertex() );
                fatherNode.append( j );
                contourPoints.removeAt( index2 );
                j = tree.size() - 1;
            }
            else
            {
                QList<VertexRef> pointsNearby = vectorImage->getVerticesCloseTo( vectorImage->getVertex( vertex ), tol2, &contourPoints );
                if ( pointsNearby.size() > 0 )
                {
                    //qDebug() << "close vertex";
                    tree << pointsNearby.at( 0 );
                    fatherNode.append( j );
                    contourPoints.removeAt( contourPoints.indexOf( pointsNearby.at( 0 ) ) );
                    j = tree.size() - 1;
                }
                else
                {
                    qreal dist = vectorImage->getDistance( vertex, vertex0 );
                    //qDebug() << "is it a leave ? " << j << "dist = " << dist << "-" << tol2;
                    if ( ( ( vertex.curveNumber == vertex0.curveNumber ) && ( qAbs( vertex.vertexNumber - vertex0.vertexNumber ) == 1 ) ) || ( dist < tol2 ) )
                    {
                        // we found a leaf close to the root of the tree - does the closed path contain the initial point?
                        QList<VertexRef> closedPath;
                        int pathIndex = j;
                        if ( dist > 0 ) { closedPath.prepend( vertex0 ); }
                        closedPath.prepend( tree.at( pathIndex ) );
                        while ( ( pathIndex = fatherNode.at( pathIndex ) ) != -1 )
                        {
                            closedPath.prepend( tree.at( pathIndex ) );
                        }
                        BezierArea newArea = BezierArea( closedPath, m_pEditor->colorManager()->frontColorNumber() );
                        vectorImage->updateArea( newArea );
                        if ( newArea.path.contains( initialPoint ) )
                        {
                            vectorImage->addArea( newArea );
                            //qDebug() << "Yes!";
                            success = true;
                        }
                        else
                        {
                            //qDebug() << "No! almost";
                            j = fatherNode.at( j );
                        }
                    }
                    else
                    {
                        //qDebug() << "No!";
                        leaves << j;
                        j = fatherNode.at( j );
                    }
                }
            }
        }

        //
    }

    if ( !success ) { floodFillError( 2 ); return; }
    //qDebug() << "failure!" << contourPoints.size();
    replaceImage->fill( qRgba( 0, 0, 0, 0 ) );
    deselectAll();

    // -- debug --- (display tree)
    /*for(int indexm=0; indexm < tree.size(); indexm++) {
    qDebug() << indexm  << ") " << tree.at(indexm).curveNumber << "," << tree.at(indexm).vertexNumber << " -> " << fatherNode.at(indexm);
    //if (leaves.contains(indexm)) vectorImage->setSelected( tree.at(indexm), true);
    vectorImage->setSelected( tree.at(indexm), true);
    update();
    //sleep( 1 );
    QMessageBox::warning(this, tr("My Application"), tr("all the tree points"), QMessageBox::Ok, QMessageBox::Ok);
    }*/
    delete targetImage;
    update();
}

void ScribbleArea::floodFillError( int errorType )
{
    QString message, error;
    if ( errorType == 1 ) { message = "There is a gap in your drawing (or maybe you have zoomed too much)."; }
    if ( errorType == 2 || errorType == 3 ) message = "Sorry! This doesn't always work."
        "Please try again (zoom a bit, click at another location... )<br>"
        "if it doesn't work, zoom a bit and check that your paths are connected by pressing F1.).";

    if ( errorType == 1 ) { error = "Out of bound."; }
    if ( errorType == 2 ) { error = "Could not find a closed path."; }
    if ( errorType == 3 ) { error = "Could not find the root index."; }
    QMessageBox::warning( this, tr( "Flood fill error" ), message + "<br><br>Error: " + error, QMessageBox::Ok, QMessageBox::Ok );
    bufferImg->clear();
    deselectAll();
}

/************************************************************************************/
// tool handling

BaseTool *ScribbleArea::currentTool()
{
    return getEditor()->toolManager()->currentTool();
}

BaseTool* ScribbleArea::getTool( ToolType eToolType )
{
    return getEditor()->toolManager()->getTool( eToolType );
}

// TODO: check this method
void ScribbleArea::setCurrentTool( ToolType eToolMode )
{
    if ( currentTool() != NULL && eToolMode != currentTool()->type() )
    {
        qDebug() << "Set Current Tool" << BaseTool::TypeName( eToolMode );
        if ( BaseTool::TypeName( eToolMode ) == "" )
        {
            // tool does not exist
            //Q_ASSERT_X( false, "", "" );
            return;
        }

        // XXX tool->setActive()
        if ( currentTool()->type() == MOVE )
        {
            paintTransformedSelection();
            deselectAll();
        }
        else if ( currentTool()->type() == POLYLINE )
        {
            escape();
        }
    }

    prevToolType = currentTool()->type();

    // --- change cursor ---
    setCursor( currentTool()->cursor() );
    qDebug() << "fn: setCurrentTool " << "call: setCursor()" << "current tool" << currentTool()->typeName();
}

void ScribbleArea::setTemporaryTool( ToolType eToolMode )
{
    instantTool = true; // used to return to previous tool when finished (keyRelease).
    prevMode = currentTool()->type();
    getEditor()->toolManager()->setCurrentTool( eToolMode );
}

void ScribbleArea::deleteSelection()
{
    if ( somethingSelected )      // there is something selected
    {
        Layer *layer = m_pEditor->getCurrentLayer();
        if ( layer == NULL ) { return; }
        m_pEditor->backup( tr( "DeleteSel" ) );
        closestCurves.clear();
        if ( layer->type() == Layer::VECTOR ) { ( ( LayerVector * )layer )->getLastVectorImageAtFrame( m_pEditor->layerManager()->currentFrameIndex(), 0 )->deleteSelection(); }
        if ( layer->type() == Layer::BITMAP ) { ( ( LayerBitmap * )layer )->getLastBitmapImageAtFrame( m_pEditor->layerManager()->currentFrameIndex(), 0 )->clear( mySelection ); }
        updateAllFrames();
    }
}

void ScribbleArea::clearImage()
{
    Layer *layer = m_pEditor->getCurrentLayer();
    if ( layer == NULL ) { return; }
    if ( layer->type() == Layer::VECTOR )
    {
        m_pEditor->backup( tr( "ClearImg" ) ); // undo: only before change (just before)
        ( ( LayerVector * )layer )->getLastVectorImageAtFrame( m_pEditor->layerManager()->currentFrameIndex(), 0 )->clear();
        closestCurves.clear();
        closestVertices.clear();
    }
    else if ( layer->type() == Layer::BITMAP )
    {
        m_pEditor->backup( tr( "ClearImg" ) );
        ( ( LayerBitmap * )layer )->getLastBitmapImageAtFrame( m_pEditor->layerManager()->currentFrameIndex(), 0 )->clear();
    }
    else
    {
        return; // skip updates when nothing changes
    }
    //todo: confirm 1 and 2 are not necessary and remove comments
    //emit modification(); //1
    //update(); //2
    setModified( m_pEditor->layerManager()->currentLayerIndex(), m_pEditor->layerManager()->currentFrameIndex() );
}

void ScribbleArea::toggleThinLines()
{
    m_showThinLines = !m_showThinLines;
    emit thinLinesChanged( m_showThinLines );
    setView( myView );
    updateAllFrames();
}

void ScribbleArea::toggleOutlines()
{
    m_isSimplified = !m_isSimplified;
    emit outlinesChanged( m_isSimplified );
    setView( myView );
    updateAllFrames();
}

void ScribbleArea::toggleMirror()
{
    myView = myView * QMatrix( -1, 0, 0, 1, 0, 0 );
    myTempView = myView * centralView;
    setView( myView );
    updateAllFrames();
}

void ScribbleArea::toggleMirrorV()
{
    myView = myView * QMatrix( 1, 0, 0, -1, 0, 0 );
    myTempView = myView * centralView;
    setView( myView );
    updateAllFrames();
}

void ScribbleArea::toggleShowAllLayers()
{
    m_showAllLayers++;
    if ( m_showAllLayers == 3 )
    {
        m_showAllLayers = 0;
    }
    //m_showAllLayers = ( m_showAllLayers + 1 ) % 3; // 0 1 2 repeated todo: subst. prev. lines
    //emit showAllLayersChanged(showAllLayers);
    setView( myView );
    updateAllFrames();
}

void ScribbleArea::setPrevTool()
{
    getEditor()->toolManager()->setCurrentTool( prevMode );
    instantTool = false;
}
