/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2013-2014 Matt Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation;

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/
#include <cmath>
#include <QScopedPointer>
#include <QtGui>
#include <QMessageBox>

#include "beziercurve.h"
#include "object.h"
#include "editor.h"
#include "layerbitmap.h"
#include "layervector.h"
#include "layercamera.h"
#include "bitmapimage.h"
#include "pencilsettings.h"
#include "toolmanager.h"
#include "strokemanager.h"
#include "layermanager.h"
#include "playbackmanager.h"
#include "popupcolorpalettewidget.h"

#include "scribblearea.h"

#define round(f) ((int)(f + 0.5))


// #define DRAW_AXIS

ScribbleArea::ScribbleArea( QWidget* parent ) : QWidget( parent )
{
    mStrokeManager = new StrokeManager();

    QSettings settings( PENCIL2D, PENCIL2D );

    int curveSmoothingLevel = settings.value( "curveSmoothing" ).toInt();
    if ( curveSmoothingLevel == 0 ) { curveSmoothingLevel = 20; settings.setValue( "curveSmoothing", curveSmoothingLevel ); } // default
    mCurveSmoothingLevel = curveSmoothingLevel / 20.0; // default value is 1.0

    initDisplayEffect( mEffects );

    // Qt::WA_StaticContents ensure that the widget contents are rooted to the top-left corner
    // and don't change when the widget is resized.
    setAttribute( Qt::WA_StaticContents );

    mMakeInvisible = false;
    somethingSelected = false;

    mMultiLayerOnionSkin = true;
    mShowThinLines = false;
    mShowAllLayers = 1;
    
    QString background = settings.value( "background" ).toString();
    setBackgroundBrush( background );

    mBufferImg = new BitmapImage;

    QRect newSelection( QPoint( 0, 0 ), QSize( 0, 0 ) );
    mySelection = newSelection;
    myTransformedSelection = newSelection;
    myTempTransformedSelection = newSelection;
    offset.setX( 0 );
    offset.setY( 0 );
    selectionTransformation.reset();

    tol = 7.0;

    mMouseInUse = false;
    mKeyboardInUse = false;
    setMouseTracking( true ); // reacts to mouse move events, even if the button is not pressed

    debugRect = QRectF( 0, 0, 0, 0 );

    setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding ) );

    QPixmapCache::setCacheLimit( 100 * 1024 );

    mNeedUpdateAll = false;

    myFlipX = 1.0; // can be used as "scale"
    myFlipY = 1.0; // idem

    // color wheel popup
    m_popupPaletteWidget = new PopupColorPaletteWidget( this );

    onionBlue = true;
    onionRed = true;
    toggledOnionColor();
}

ScribbleArea::~ScribbleArea()
{
    QSettings settings( PENCIL2D, PENCIL2D );

    QList< QVariant > savedList;
    for ( uint32_t e : mEffects )
    {
        savedList.append( QVariant( e ) );
    }
    settings.setValue( SETTING_DISPLAY_EFFECT, savedList );
}

void ScribbleArea::updateToolCursor()
{
    setCursor( currentTool()->cursor() );
}

void ScribbleArea::setCurveSmoothing( int newSmoothingLevel )
{
    mCurveSmoothingLevel = newSmoothingLevel / 20.0;
    QSettings settings( "Pencil", "Pencil" );
    settings.setValue( "curveSmoothing", newSmoothingLevel );
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

void ScribbleArea::updateCurrentFrame()
{
    updateFrame( mEditor->currentFrame() );
}

void ScribbleArea::updateFrame( int frame )
{
    // TODO: implement the cache
    setView( getView() );
    int frameNumber = mEditor->layers()->LastFrameAtFrame( frame );
    QPixmapCache::remove( "frame" + QString::number( frameNumber ) );

    update();
}

void ScribbleArea::updateAllFrames()
{
    setView( getView() );
    QPixmapCache::clear();
    update();
    mNeedUpdateAll = false;
}

void ScribbleArea::updateAllVectorLayersAtCurrentFrame()
{
    updateAllVectorLayersAt( mEditor->currentFrame() );
}

void ScribbleArea::updateAllVectorLayersAt( int frameNumber )
{
    for ( int i = 0; i < mEditor->object()->getLayerCount(); i++ )
    {
        Layer *layer = mEditor->object()->getLayer( i );
        if ( layer->type() == Layer::VECTOR ) { ( ( LayerVector * )layer )->getLastVectorImageAtFrame( frameNumber, 0 )->setModified( true ); }
    }
    updateFrame( mEditor->currentFrame() );
}

void ScribbleArea::updateAllVectorLayers()
{
    for ( int i = 0; i < mEditor->object()->getLayerCount(); i++ )
    {
        Layer *layer = mEditor->object()->getLayer( i );
        if ( layer->type() == Layer::VECTOR )
        {
            //( ( LayerVector * )layer )->setModified( true );
        }
    }
    updateAllFrames();
}

void ScribbleArea::setModified( int layerNumber, int frameNumber )
{
    Layer *layer = mEditor->object()->getLayer( layerNumber );
    if ( layer->type() == Layer::VECTOR )
    {
        layer->setModified( frameNumber, true );
    }
    else if ( layer->type() == Layer::BITMAP )
    {
        layer->setModified( frameNumber, true );
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
    mKeyboardInUse = true;
    if ( mMouseInUse ) { return; } // prevents shortcuts calls while drawing, todo: same check for remaining shortcuts (in connects).
    if ( currentTool()->keyPressEvent( event ) )
    {
        // has been handled by tool
        return;
    }

    // --- fixed control key shortcuts ---
    if ( event->modifiers() == ( Qt::ControlModifier | Qt::ShiftModifier ) )
    {
        qreal width = currentTool()->properties.width;
        qreal feather = currentTool()->properties.feather;
        setTemporaryTool( ERASER );
        mEditor->tools()->setWidth( width + ( 200 - width ) / 41 ); // minimum size: 0.2 + 4.8 = 5 units. maximum size 200 + 0.
        mEditor->tools()->setFeather( feather ); //anticipates future implementation of feather (not used yet).
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
            mEditor->scrubForward();
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
            mEditor->scrubBackward();
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
            mEditor->previousLayer();
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
            mEditor->nextLayer();
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
        mIsSimplified = true;
        emit outlinesChanged( mIsSimplified );
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
    mKeyboardInUse = false;
    if ( mMouseInUse ) { return; }
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
		auto zoom = [=](int delta)
		{
			if ( delta > 0 )
			{
				zoomIn();
			}
			else
			{
				zoomOut();
			}
		};

		QPoint numPixels = event->pixelDelta();
		QPoint numDegrees = event->angleDelta() / 8;
		if ( !numPixels.isNull() )
		{
			zoom(numPixels.y());
		}
		else if ( !numDegrees.isNull() )
		{
			QPoint numSteps = numDegrees / 15;
			zoom(numSteps.y());
		}

		event->accept();

		/*

		*/
    }
}

void ScribbleArea::tabletEvent( QTabletEvent *event )
{
    //qDebug() << "Device" << event->device() << "Pointer type" << event->pointerType();
    mStrokeManager->tabletEvent( event );

    currentTool()->adjustPressureSensitiveProperties( pow( ( float )mStrokeManager->getPressure(), 2.0f ),
                                                      event->pointerType() == QTabletEvent::Cursor );

    if ( event->pointerType() == QTabletEvent::Eraser )
    {
        editor()->tools()->tabletSwitchToEraser();
    }
    else
    {
        editor()->tools()->tabletRestorePrevTool();
    }
    event->ignore(); // indicates that the tablet event is not accepted yet, so that it is propagated as a mouse event)
}

QPointF ScribbleArea::pixelToPoint( QPointF pixel )
{
    bool invertible = true;
    return myTempView.inverted( &invertible ).map( QPointF( pixel ) );
}


bool ScribbleArea::isLayerPaintable() const
{
    if ( !areLayersSane() )
        return false;

    Layer* layer = mEditor->layers()->currentLayer();
    return layer->type() == Layer::BITMAP || layer->type() == Layer::VECTOR;
}

bool ScribbleArea::areLayersSane() const
{
    Layer* layer = mEditor->layers()->currentLayer();
    // ---- checks ------
    if ( layer == NULL ) { return false; }
    if ( layer->type() == Layer::VECTOR )
    {
        VectorImage *vectorImage = ( ( LayerVector * )layer )->getLastVectorImageAtFrame( mEditor->currentFrame(), 0 );
        if ( vectorImage == NULL ) { return false; }
    }
    if ( layer->type() == Layer::BITMAP )
    {
        BitmapImage *bitmapImage = ( ( LayerBitmap * )layer )->getLastBitmapImageAtFrame( mEditor->currentFrame(), 0 );
        if ( bitmapImage == NULL ) { return false; }
    }
    // ---- end checks ------

    return true;
}

void ScribbleArea::mousePressEvent( QMouseEvent *event )
{
    mMouseInUse = true;

    mStrokeManager->mousePressEvent( event );

    if ( !mStrokeManager->isTabletInUse() )   // a mouse is used instead of a tablet
    {
        mStrokeManager->setPressure( 1.0 );
        currentTool()->adjustPressureSensitiveProperties( 1.0, true );

        //----------------code for starting hand tool when middle mouse is pressed
        if ( event->buttons() & Qt::MidButton )
        {
            //qDebug() << "Hand Start " << event->pos();
            mPrevTemporalToolType = currentTool()->type();
            editor()->tools()->setCurrentTool( HAND );
        }
    }

    if ( !( event->button() == Qt::NoButton ) )    // if the user is pressing the left or right button
    {
        lastPixel = mStrokeManager->getLastPressPixel();
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
    Layer* layer = mEditor->layers()->currentLayer();
    Q_ASSUME( layer != nullptr );

    if ( layer->type() == Layer::VECTOR )
    {
        auto pLayerVector = static_cast< LayerVector* >( layer );
        VectorImage* vectorImage = pLayerVector->getLastVectorImageAtFrame( mEditor->currentFrame(), 0 );
        Q_CHECK_PTR( vectorImage );
    }
    else if ( layer->type() == Layer::BITMAP )
    {
        auto pLayerBitmap = static_cast< LayerBitmap* >( layer );
        BitmapImage* bitmapImage = pLayerBitmap->getLastBitmapImageAtFrame( mEditor->currentFrame(), 0 );
        Q_CHECK_PTR( bitmapImage );
    }

    if ( !layer->visible && currentTool()->type() != HAND && ( event->button() != Qt::RightButton ) )
    {
        QMessageBox::warning( this, tr( "Warning" ),
                              tr( "You are drawing on a hidden layer! Please select another layer (or make the current layer visible)." ),
                              QMessageBox::Ok,
                              QMessageBox::Ok );
        mMouseInUse = false;
        return;
    }
    // ---

    bool invertible = true;
    currentPoint = myTempView.inverted( &invertible ).map( QPointF( currentPixel ) );
    //qDebug() << "CurPoint: " << currentPoint;


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

void ScribbleArea::mouseMoveEvent( QMouseEvent *event )
{
    if ( !areLayersSane() )
    {
        return;
    }

    mStrokeManager->mouseMoveEvent( event );
    currentPixel = mStrokeManager->getCurrentPixel();
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
    mMouseInUse = false;

    // ---- checks ------
    if ( currentTool()->isAdjusting )
    {
        currentTool()->stopAdjusting();
        return; // [SHIFT]+drag OR [CTRL]+drag
    }

    mStrokeManager->mouseReleaseEvent( event );

    if ( event->button() == Qt::RightButton )
    {
        getTool( HAND )->mouseReleaseEvent( event );
        return;
    }

    currentTool()->mouseReleaseEvent( event );

    if ( currentTool()->type() == EYEDROPPER )
    {
        setCurrentTool( mPrevToolType );
    }

    // ---- last check (at the very bottom of mouseRelease) ----
    if ( instantTool && !mKeyboardInUse ) // temp tool and released all keys ?
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
    Layer* layer = mEditor->layers()->currentLayer();
    // ---- checks ------
    if ( layer == NULL ) { return; }
    // Clear the temporary pixel path
    BitmapImage *targetImage = ( ( LayerBitmap * )layer )->getLastBitmapImageAtFrame( mEditor->currentFrame(), 0 );
    if ( targetImage != NULL )
    {
        QPainter::CompositionMode cm = QPainter::CompositionMode_SourceOver;
        switch ( currentTool()->type() )
        {
        case ERASER:
            cm = QPainter::CompositionMode_DestinationOut;
            break;
        case BRUSH:
        case PEN:
        case PENCIL:
            if ( getTool( currentTool()->type() )->properties.preserveAlpha )
            {
                cm = QPainter::CompositionMode_SourceAtop;
            }
            break;
        default: //nothing
            break;
        }
        targetImage->paste( mBufferImg, cm );
    }
    QRect rect = myTempView.mapRect( mBufferImg->boundaries );
    // Clear the buffer
    mBufferImg->clear();

    //setModified(layer, editor->currentFrame);
    layer->setModified( mEditor->currentFrame(), true );
    emit modification();
    QPixmapCache::remove( "frame" + QString::number( mEditor->currentFrame() ) );
    drawCanvas( mEditor->currentFrame(), rect.adjusted( -1, -1, 1, 1 ) );
    update( rect );
}

void ScribbleArea::clearBitmapBuffer()
{
    mBufferImg->clear();
}

void ScribbleArea::drawLine( QPointF P1, QPointF P2, QPen pen, QPainter::CompositionMode cm )
{
    mBufferImg->drawLine( P1, P2, pen, cm, isEffectOn( EFFECT_ANTIALIAS ) );
}

void ScribbleArea::drawPath( QPainterPath path, QPen pen, QBrush brush, QPainter::CompositionMode cm )
{
    mBufferImg->drawPath( path, pen, brush, cm, isEffectOn( EFFECT_ANTIALIAS ) );
}

void ScribbleArea::refreshBitmap( QRect rect, int rad )
{
    update( myTempView.mapRect( rect.normalized().adjusted( -rad, -rad, +rad, +rad ) ) );
}

void ScribbleArea::refreshVector( QRect rect, int rad )
{
    update( rect.normalized().adjusted( -rad, -rad, +rad, +rad ) );
}

void ScribbleArea::paintEvent( QPaintEvent *event )
{
    //qDebug() << "paint event!" << QDateTime::currentDateTime() << event->rect();
    QPainter painter( this );

    // draws the background (if necessary)
    if ( mMouseInUse && currentTool()->type() == HAND )
    {
        painter.setTransform( myTempView );
        painter.setViewTransformEnabled( true );
        painter.setPen( Qt::NoPen );
        painter.setBrush( backgroundBrush );
        painter.drawRect( ( myTempView ).inverted().mapRect( QRect( -2, -2, width() + 3, height() + 3 ) ) );  // this is necessary to have the background move with the view
    }

    // process the canvas (or not)
    if ( !mMouseInUse )
    {
        // --- we retrieve the canvas from the cache; we create it if it doesn't exist
        int curIndex = mEditor->currentFrame();
        int frameNumber = mEditor->layers()->LastFrameAtFrame( curIndex );

        QString strCachedFrameKey = "frame" + QString::number( frameNumber );
        if ( !QPixmapCache::find( strCachedFrameKey, mCanvas ) )
        {
            drawCanvas( mEditor->currentFrame(), event->rect() );
            QPixmapCache::insert( strCachedFrameKey, mCanvas );
        }
    }
    if ( currentTool()->type() == MOVE )
    {
        Layer* layer = mEditor->layers()->currentLayer();
        if ( !layer ) { return; }
        if ( layer->type() == Layer::VECTOR ) { ( ( LayerVector * )layer )->getLastVectorImageAtFrame( mEditor->currentFrame(), 0 )->setModified( true ); }
        drawCanvas( mEditor->currentFrame(), event->rect() );
    }
    
    // paints the canvas
    painter.setViewTransformEnabled( true );
    painter.setTransform( transMatrix );
    painter.drawPixmap( QPoint( 0, 0 ), mCanvas );

    //  painter.drawImage(QPoint(100,100),QImage(":background/grid"));//TODO Success a grid is drawn
    Layer *layer = mEditor->layers()->currentLayer();


    if ( !editor()->playback()->isPlaying() )    // we don't need to display the following when the animation is playing
    {
        painter.setTransform( myTempView );

        if ( layer->type() == Layer::VECTOR )
        {
            VectorImage *vectorImage = ( ( LayerVector * )layer )->getLastVectorImageAtFrame( mEditor->currentFrame(), 0 );

            switch ( currentTool()->type() )
            {
            case SMUDGE:
            case HAND:
            {
                painter.save();
                painter.setViewTransformEnabled( false );
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

            case MOVE:
            {
                // ----- paints the closest curves
                mBufferImg->clear();
                QPen pen2( Qt::black, 0.5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin );
                QColor colour = QColor( 100, 100, 255 );

                for ( int k = 0; k < closestCurves.size(); k++ )
                {
                    qreal scale = myTempView.determinant(); //todo: check whether it's correct (det = area?)
                    //qreal scale = sqrt(myTempView.det()); or qreal scale = sqrt(myTempView.m11()*myTempView.m22());
                    int idx = closestCurves[ k ];
                    if ( vectorImage->m_curves.size() <= idx )
                    {
                        // safety check
                        continue;
                    }
                    BezierCurve myCurve = vectorImage->m_curves[ closestCurves[ k ] ];
                    if ( myCurve.isPartlySelected() )
                    {
                        myCurve.transform( selectionTransformation );
                    }
                    QPainterPath path = myCurve.getStrokedPath( 1.2 / scale, false );
                    mBufferImg->drawPath( ( myView * transMatrix * centralView ).map( path ), pen2, colour,
                                          QPainter::CompositionMode_SourceOver,
                                          isEffectOn( EFFECT_ANTIALIAS ) );
                }
            }
            }
        }

        // paints the  buffer image
        if ( mEditor->layers()->currentLayer() != NULL )
        {
            painter.setOpacity( 1.0 );
            if ( mEditor->layers()->currentLayer()->type() == Layer::BITMAP ) { painter.setViewTransformEnabled( true ); }
            if ( mEditor->layers()->currentLayer()->type() == Layer::VECTOR ) { painter.setViewTransformEnabled( false ); }
            mBufferImg->paintImage( painter );
        }

        // paints the selection outline
        if ( somethingSelected && ( myTempTransformedSelection.isValid() || mMoveMode == ROTATION ) ) // @revise
        {
            // outline of the transformed selection
            painter.setViewTransformEnabled( false );
            painter.setOpacity( 1.0 );
            QPolygon tempRect = ( myView * transMatrix * centralView ).mapToPolygon( myTempTransformedSelection.normalized().toRect() );

            Layer* layer = mEditor->layers()->currentLayer();
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
        painter.setViewTransformEnabled( false );
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
    painter.setViewTransformEnabled( false );
    painter.setPen( QPen( Qt::gray, 2 ) );
    painter.setBrush( Qt::NoBrush );
    painter.drawRect( QRect( 0, 0, width(), height() ) );

    // shadow
    bool isPlaying = editor()->playback()->isPlaying();
    if ( isEffectOn( EFFECT_SHADOW ) && !isPlaying && ( !mMouseInUse || currentTool()->type() == HAND ) )
    {
        drawShadow( painter );
    }


    event->accept();
}

void ScribbleArea::drawCanvas( int frame, QRect rect )
{
    // Merge the different layers into the ScribbleArea.
    //qDebug() << "paint canvas!" << QDateTime::currentDateTime();
    
    QPainter painter( &mCanvas );
    painter.setRenderHint( QPainter::SmoothPixmapTransform, isEffectOn( EFFECT_ANTIALIAS ) );

    painter.setClipRect( rect );
    painter.setClipping( true );
    setView( getView() );
    painter.setTransform( myTempView );
    painter.setViewTransformEnabled( true );

    // background
    painter.setPen( Qt::NoPen );
    painter.setBrush( backgroundBrush );
    painter.drawRect( myTempView.inverted().mapRect( QRect( -2, -2, width() + 3, height() + 3 ) ) );  // this is necessary to have the background move with the view

    QRectF viewRect = getViewRect();
    QRectF vectorViewRect = viewRect.translated( -viewRect.left(), -viewRect.top() );
    QSize sz = viewRect.size().toSize();

    Object *object = mEditor->object();
    qreal opacity;

    // --- onionskins ---
    int iStart = 0;
    int iEnd = object->getLayerCount() - 1;
    if ( !mMultiLayerOnionSkin )
    { // not used ( if required, just make a connection from UI ) // is used now for Single/multiple onionskin Layers
        iStart = iEnd = mEditor->layers()->currentLayerIndex();
    }

    for ( int i = iStart; i <= iEnd; i++ )
    {
        opacity = 1.0;
        if ( i != mEditor->layers()->currentLayerIndex() && ( mShowAllLayers == 1 ) )
        {
            opacity = 0.4;
        }

        Q_ASSERT_X( mEditor != NULL, "ScribbleArea.cpp", "Editor should not be null." );
        Q_ASSERT_X( mEditor->layers()->currentLayer(), "", "Layer should not be null." );

        //qDebug( "Layer Count = %d, current=%d", object->getLayerCount(), i );

        if ( mEditor->layers()->currentLayer()->type() == Layer::CAMERA ) { opacity = 1.0; }
        Layer *layer = ( object->getLayer( i ) );
        if ( layer->visible && ( mShowAllLayers > 0 || i == mEditor->layers()->currentLayerIndex() ) ) // change && to || for all layers
        {
            // paints the bitmap images
            if ( layer->type() == Layer::BITMAP )
            {
                LayerBitmap *layerBitmap = ( LayerBitmap * )layer;
                BitmapImage *bitmapImage = layerBitmap->getLastBitmapImageAtFrame( frame, 0 );
                if ( bitmapImage != NULL )
                {
                    painter.setViewTransformEnabled( true );

                    // previous frame (onion skin)
                    if ( isEffectOn( EFFECT_PREV_ONION ) )
                    {
                        BitmapImage *previousImage = layerBitmap->getLastBitmapImageAtFrame( frame, -1 );
                        if ( previousImage != NULL )
                        {
                            painter.setOpacity( opacity * mEditor->getOnionLayer1Opacity() / 100.0 );
                            previousImage->paintImage( painter );
                        }
                        BitmapImage *previousImage2 = layerBitmap->getLastBitmapImageAtFrame( frame, -2 );
                        if ( previousImage2 != NULL )
                        {
                            painter.setOpacity( opacity * mEditor->getOnionLayer2Opacity() / 100.0 );
                            previousImage2->paintImage( painter );
                        }
                        BitmapImage *previousImage3 = layerBitmap->getLastBitmapImageAtFrame( frame, -3 );
                        if ( previousImage3 != NULL )
                        {
                            painter.setOpacity( opacity * mEditor->getOnionLayer3Opacity() / 100.0 );
                            previousImage3->paintImage( painter );
                        }
                        if ( onionBlue || onionRed )
                        {
                            painter.setOpacity( 1.0 );
                            painter.setCompositionMode( QPainter::CompositionMode_Lighten );
                            if ( onionBlue && onionRed && isEffectOn( EFFECT_NEXT_ONION ) )
                            {
                                painter.fillRect( viewRect, Qt::red );
                            }
                            else
                            {
                                painter.fillRect( viewRect, onionColor );
                            }
                            painter.setCompositionMode( QPainter::CompositionMode_SourceOver );
                        }
                    }

                    // next frame (onion skin)
                    if ( isEffectOn( EFFECT_NEXT_ONION ) )
                    {
                        BitmapImage *nextImage = layerBitmap->getLastBitmapImageAtFrame( frame, 1 );
                        if ( nextImage != NULL )
                        {
                            painter.setOpacity( opacity * mEditor->getOnionLayer1Opacity() / 100.0 );
                            nextImage->paintImage( painter );
                        }
                        BitmapImage *nextImage2 = layerBitmap->getLastBitmapImageAtFrame( frame, 2 );
                        if ( nextImage2 != NULL )
                        {
                            painter.setOpacity( opacity * mEditor->getOnionLayer2Opacity() / 100.0 );
                            nextImage2->paintImage( painter );
                        }
                        BitmapImage *nextImage3 = layerBitmap->getLastBitmapImageAtFrame( frame, 3 );
                        if ( nextImage3 != NULL )
                        {
                            painter.setOpacity( opacity * mEditor->getOnionLayer3Opacity() / 100.0 );
                            nextImage3->paintImage( painter );
                        }
                        if ( onionBlue || onionRed )
                        {
                            painter.setOpacity( 1.0 );
                            painter.setCompositionMode( QPainter::CompositionMode_Lighten );
                            if ( onionBlue && onionRed && isEffectOn( EFFECT_PREV_ONION ) ) {
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
                QScopedPointer< QImage > pImage( new QImage( size(), QImage::Format_ARGB32_Premultiplied ) );
                auto layerVector = static_cast< LayerVector* >( layer );

				painter.setViewTransformEnabled( false );

                // previous frame (onion skin)
                if ( isEffectOn( EFFECT_PREV_ONION ) )
                {
                    VectorImage* pVectorImage = layerVector->getLastVectorImageAtFrame( frame, -3 );
                    pVectorImage->outputImage( pImage.data(), myTempView, mIsSimplified, mShowThinLines, isEffectOn( EFFECT_ANTIALIAS ) );
                    painter.setOpacity( opacity * mEditor->getOnionLayer3Opacity() / 100.0 );
                    painter.drawImage( QPoint( 0, 0 ), *pImage );

                    pVectorImage = layerVector->getLastVectorImageAtFrame( frame, -2 );
                    pVectorImage->outputImage( pImage.data(), myTempView, mIsSimplified, mShowThinLines, isEffectOn( EFFECT_ANTIALIAS ) );
                    painter.setOpacity( opacity * mEditor->getOnionLayer2Opacity() / 100.0 );
                    painter.drawImage( QPoint( 0, 0 ), *pImage );

                    pVectorImage = layerVector->getLastVectorImageAtFrame( frame, -1 );
                    pVectorImage->outputImage( pImage.data(), myTempView, mIsSimplified, mShowThinLines, isEffectOn( EFFECT_ANTIALIAS ) );
                    painter.setOpacity( opacity * mEditor->getOnionLayer1Opacity() / 100.0 );
                    painter.drawImage( QPoint( 0, 0 ), *pImage );

                    if ( onionBlue || onionRed )
                    {
                        painter.setOpacity( 1.0 );
                        painter.setCompositionMode( QPainter::CompositionMode_Lighten );
                        if ( onionBlue && onionRed && isEffectOn( EFFECT_NEXT_ONION ) )
                        {
                            painter.fillRect( vectorViewRect, Qt::red );
                        }
                        else
                        {
                            painter.fillRect( vectorViewRect, onionColor );
                        }
                        painter.setCompositionMode( QPainter::CompositionMode_SourceOver );
                    }
                }

                // next frame (onion skin)
                if ( isEffectOn( EFFECT_NEXT_ONION ) )
                {
                    VectorImage* pVectorImage = layerVector->getLastVectorImageAtFrame( frame, 3 );
                    pVectorImage->outputImage( pImage.data(), myTempView, mIsSimplified, mShowThinLines, isEffectOn( EFFECT_ANTIALIAS ) );
                    painter.setOpacity( opacity * mEditor->getOnionLayer3Opacity() / 100.0 );
                    painter.drawImage( QPoint( 0, 0 ), *pImage );

                    pVectorImage = layerVector->getLastVectorImageAtFrame( frame, 2 );
                    pVectorImage->outputImage( pImage.data(), myTempView, mIsSimplified, mShowThinLines, isEffectOn( EFFECT_ANTIALIAS ) );
                    painter.setOpacity( opacity * mEditor->getOnionLayer2Opacity() / 100.0 );
                    painter.drawImage( QPoint( 0, 0 ), *pImage );

                    pVectorImage = layerVector->getLastVectorImageAtFrame( frame, 1 );
                    pVectorImage->outputImage( pImage.data(), myTempView, mIsSimplified, mShowThinLines, isEffectOn( EFFECT_ANTIALIAS ) );
                    painter.setOpacity( opacity * mEditor->getOnionLayer1Opacity() / 100.0 );
                    painter.drawImage( QPoint( 0, 0 ), *pImage );

                    if ( onionBlue || onionRed )
                    {
                        painter.setOpacity( 1.0 );
                        painter.setCompositionMode( QPainter::CompositionMode_Lighten );
                        if ( onionBlue && onionRed && isEffectOn( EFFECT_PREV_ONION ) )
                        {
                            painter.fillRect( vectorViewRect, Qt::blue );
                        }
                        else
                        {
                            painter.fillRect( vectorViewRect, onionColor );
                        }
                        painter.setCompositionMode( QPainter::CompositionMode_SourceOver );
                    }
                }
				painter.setViewTransformEnabled( true );
            }
        }
    } // --- end onion skins

    // --- current frame ---
    for ( int i = 0; i < object->getLayerCount(); i++ )
    {
        opacity = 1.0;
        if ( i != mEditor->layers()->currentLayerIndex() && ( mShowAllLayers == 1 ) ) { opacity = 0.4; }

        if ( mEditor->layers()->currentLayer()->type() == Layer::CAMERA ) { opacity = 1.0; }
        Layer *layer = ( object->getLayer( i ) );
        if ( layer->visible && ( mShowAllLayers > 0 || i == mEditor->layers()->currentLayerIndex() ) )
        {
            // paints the bitmap images
            if ( layer->type() == Layer::BITMAP )
            {
                LayerBitmap *layerBitmap = ( LayerBitmap * )layer;
                BitmapImage *bitmapImage = layerBitmap->getLastBitmapImageAtFrame( frame, 0 );
                if ( bitmapImage != NULL )
                {
                    painter.setViewTransformEnabled( true );
                    painter.setOpacity( opacity );
                    if ( i == mEditor->layers()->currentLayerIndex() && somethingSelected && ( myRotatedAngle != 0 || myTempTransformedSelection != mySelection || myFlipX != 1 || myFlipY != 1 ) )
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
                        QTransform rm;
                        //TODO: complete matrix calls ( sounds funny :)
                        rm.scale( myFlipX, myFlipY );
                        rm.rotate( myRotatedAngle );
                        QImage rotImg = selectionClip.mImage->transformed( rm );
                        QPoint dxy = QPoint( ( myTempTransformedSelection.width() - rotImg.rect().width() ) / 2,
                                             ( myTempTransformedSelection.height() - rotImg.rect().height() ) / 2 );
                        *selectionClip.mImage = rotImg; // TODO: find/create a func. (*object = data is not very orthodox)
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
                QScopedPointer< QImage > pImage( new QImage( size(), QImage::Format_ARGB32_Premultiplied ) );
                vectorImage->outputImage( pImage.data(), myTempView, mIsSimplified, mShowThinLines, isEffectOn( EFFECT_ANTIALIAS ) );

                painter.setViewTransformEnabled( false );
                painter.setOpacity( opacity );
                painter.drawImage( QPoint( 0, 0 ), *pImage );
            }
        }
    }

    // --- grids ---
	if ( isEffectOn( EFFECT_GRID_A ) )
	{
		drawGrid( painter );
	}
	// --- eo grids
	if ( isEffectOn( EFFECT_AXIS ) )
	{
		drawAxis( painter );
    }

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
        tempBitmapImage->drawRect( rectangle, Qt::NoPen, radialGrad,
                                   QPainter::CompositionMode_Source, isEffectOn( EFFECT_ANTIALIAS ) );
    }

    mBufferImg->paste( tempBitmapImage );
    delete tempBitmapImage;
}

void ScribbleArea::blurBrush( BitmapImage *bmiSource_, QPointF srcPoint_, QPointF thePoint_, qreal brushWidth_, qreal offset_, qreal opacity_ )
{
    QRadialGradient radialGrad( thePoint_, 0.5 * brushWidth_ );
    setGaussianGradient( radialGrad, QColor( 255, 255, 255, 127 ), opacity_, offset_ );

    QRectF srcRect( srcPoint_.x() - 0.5 * brushWidth_, srcPoint_.y() - 0.5 * brushWidth_, brushWidth_, brushWidth_ );
    QRectF trgRect( thePoint_.x() - 0.5 * brushWidth_, thePoint_.y() - 0.5 * brushWidth_, brushWidth_, brushWidth_ );

    BitmapImage bmiSrcClip = bmiSource_->copy( srcRect.toRect() );
    BitmapImage bmiTmpClip = bmiSrcClip; // todo: find a shorter way

    bmiTmpClip.drawRect( srcRect, Qt::NoPen, radialGrad, QPainter::CompositionMode_Source, isEffectOn( EFFECT_ANTIALIAS ) );
    bmiSrcClip.boundaries.moveTo( trgRect.topLeft().toPoint() );
    bmiTmpClip.paste( &bmiSrcClip, QPainter::CompositionMode_SourceAtop );
    mBufferImg->paste( &bmiTmpClip );
}

void ScribbleArea::liquifyBrush( BitmapImage *bmiSource_, QPointF srcPoint_, QPointF thePoint_, qreal brushWidth_, qreal offset_, qreal opacity_ )
{
    QPointF delta = ( thePoint_ - srcPoint_ ); // increment vector
    QRectF trgRect( thePoint_.x() - 0.5 * brushWidth_, thePoint_.y() - 0.5 * brushWidth_, brushWidth_, brushWidth_ );

    QRadialGradient radialGrad( thePoint_, 0.5 * brushWidth_ );
    setGaussianGradient( radialGrad, QColor( 255, 255, 255, 255 ), opacity_, offset_ );

    // Create gradient brush
    BitmapImage* bmiTmpClip = new BitmapImage;
    bmiTmpClip->drawRect( trgRect, Qt::NoPen, radialGrad, QPainter::CompositionMode_Source, isEffectOn( EFFECT_ANTIALIAS ) );

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
    mBufferImg->paste( bmiTmpClip );
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
        QPen pen2( mEditor->color()->frontColor(),
                   getTool( PEN )->properties.width,
                   Qt::SolidLine,
                   Qt::RoundCap,
                   Qt::RoundJoin );
        QPainterPath tempPath = BezierCurve( points ).getSimplePath();
        tempPath.lineTo( endPoint );
        QRect updateRect = myTempView.mapRect( tempPath.boundingRect().toRect() ).adjusted( -10, -10, 10, 10 );
        if ( mEditor->layers()->currentLayer()->type() == Layer::VECTOR )
        {
            tempPath = myTempView.map( tempPath );
            if ( mMakeInvisible )
            {
                pen2.setWidth( 0 );
                pen2.setStyle( Qt::DotLine );
            }
            else
            {
                pen2.setWidth( getTool( PEN )->properties.width * myTempView.m11() );
            }
        }
        mBufferImg->clear();
        mBufferImg->drawPath( tempPath, pen2, Qt::NoBrush, QPainter::CompositionMode_SourceOver, isEffectOn( EFFECT_ANTIALIAS ) );

        update( updateRect );
    }
}

void ScribbleArea::endPolyline( QList<QPointF> points )
{
    if ( !areLayersSane() )
    {
        return;
    }

    Layer* layer = mEditor->layers()->currentLayer();

    if ( layer->type() == Layer::VECTOR )
    {
        BezierCurve curve = BezierCurve( points );
        if ( mMakeInvisible )
        {
            curve.setWidth( 0 );
        }
        else
        {
            curve.setWidth( getTool( PEN )->properties.width );
        }
        curve.setColourNumber( mEditor->color()->frontColorNumber() );
        curve.setVariableWidth( false );
        curve.setInvisibility( mMakeInvisible );
        //curve.setSelected(true);
        ( ( LayerVector * )layer )->getLastVectorImageAtFrame( mEditor->currentFrame(), 0 )->addCurve( curve, qAbs( myTempView.m11() ) );
    }
    if ( layer->type() == Layer::BITMAP )
    {
        drawPolyline( points, points.last() );
        BitmapImage *bitmapImage = ( ( LayerBitmap * )layer )->getLastBitmapImageAtFrame( mEditor->currentFrame(), 0 );
        bitmapImage->paste( mBufferImg );
    }
    mBufferImg->clear();
    setModified( mEditor->layers()->currentLayerIndex(), mEditor->currentFrame() );
}

void ScribbleArea::resizeEvent( QResizeEvent *event )
{
    QWidget::resizeEvent( event );
    mCanvas = QPixmap( size() );
    recentre();
    updateAllFrames();
}

void ScribbleArea::zoomIn()
{
    centralView.scale( 1.2, 1.2 );
    setView( getView() );
    updateAllFrames();
}

void ScribbleArea::zoomOut()
{
    centralView.scale( 0.8, 0.8 );
    setView( getView() );
    updateAllFrames();
}

void ScribbleArea::rotatecw()
{
    centralView.rotate( 20 );
    setView( getView() );
    updateAllFrames();
}

void ScribbleArea::rotateacw()
{
    centralView.rotate( -20 );
    setView( getView() );
    updateAllFrames();
}

void ScribbleArea::recentre()
{
    centralView = QTransform( 1, 0,
                           0, 1,
                           0.5 * width(), 0.5 * height() );
    setView( getView() );
    QPixmapCache::clear();
    updateAllFrames();
}

/************************************************************************************/
// view handling

void ScribbleArea::setMyView( QTransform view )
{
    myView = view;
}

QTransform ScribbleArea::getMyView()
{
    return myView;
}

void ScribbleArea::setView( const QTransform& view )
{
    myTempView = view * centralView;
}

void ScribbleArea::resetView()
{
    mEditor->resetMirror();
    myView.reset();
    myTempView = myView * centralView;
    recentre();
}

QTransform ScribbleArea::getView()
{
    Layer* layer = mEditor->layers()->currentLayer();
    if ( layer == NULL )
    {
        Q_ASSERT( false );
        return QTransform(); // TODO: error
    }

    if ( layer->type() == Layer::CAMERA )
    {
        return ( ( LayerCamera * )layer )->getViewAtFrame( mEditor->currentFrame() );
        qDebug() << "viewCamera" << ( ( LayerCamera * )layer )->getViewAtFrame( mEditor->currentFrame() );
    }
    else
    {
        return myView;
    }
}

QRectF ScribbleArea::getViewRect()
{
    QRectF rect = QRectF( -width() / 2, -height() / 2, width(), height() );
    Layer* layer = mEditor->layers()->currentLayer();
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

void ScribbleArea::setTransformationMatrix( QTransform matrix )
{
    transMatrix = matrix;
    update();
    setAllDirty();
}

void ScribbleArea::applyTransformationMatrix()
{
    Layer* layer = mEditor->layers()->currentLayer();
    if ( layer == NULL ) { return; }

    clearBitmapBuffer();
    if ( layer->type() == Layer::CAMERA )
    {
        LayerCamera *layerCamera = ( LayerCamera * )layer;
        QTransform view = layerCamera->getViewAtFrame( mEditor->currentFrame() );
        layerCamera->loadImageAtFrame( mEditor->currentFrame(), view * transMatrix );
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
    Layer* layer = mEditor->layers()->currentLayer();
    if ( layer == NULL ) { return; }
    if ( layer->type() == Layer::VECTOR )
    {
        VectorImage *vectorImage = ( ( LayerVector * )layer )->getLastVectorImageAtFrame( mEditor->currentFrame(), 0 );
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
    Layer* layer = mEditor->layers()->currentLayer();
    if ( layer == NULL )
    {
        return;
    }

    if ( somethingSelected )    // there is something selected
    {
        if ( layer->type() == Layer::BITMAP && ( myRotatedAngle != 0.0 || myTransformedSelection != mySelection || myFlipX != 1 || myFlipY != 1 ) )
        {
            //backup();
            BitmapImage *bitmapImage = ( ( LayerBitmap * )layer )->getLastBitmapImageAtFrame( mEditor->currentFrame(), 0 );
            if ( bitmapImage == NULL )
            {
                qDebug() << "NULL image pointer!"
                    << mEditor->layers()->currentLayerIndex()
                    << mEditor->currentFrame();
                return;
            }

            bool smoothTransform = false;
            if ( myTransformedSelection.width() != mySelection.width() || myTransformedSelection.height() != mySelection.height() || mMoveMode == ROTATION ) { smoothTransform = true; }
            QTransform rm;
            rm.scale( myFlipX, myFlipY );
            rm.rotate( myRotatedAngle );
            BitmapImage selectionClip = bitmapImage->copy( mySelection.toRect() );
            selectionClip.transform( myTransformedSelection, smoothTransform );
            QImage rotImg = selectionClip.mImage->transformed( rm, Qt::SmoothTransformation );
            QPoint dxy = QPoint( ( myTempTransformedSelection.width() - rotImg.rect().width() ) / 2,
                                 ( myTempTransformedSelection.height() - rotImg.rect().height() ) / 2 );
            *selectionClip.mImage = rotImg; // TODO: find/create a func. (*object = data is not very orthodox)
            selectionClip.boundaries.translate( dxy );
            bitmapImage->clear( mySelection.toRect() );
            bitmapImage->paste( &selectionClip );
        }
        if ( layer->type() == Layer::VECTOR )
        {
            // vector transformation
            LayerVector *layerVector = ( LayerVector * )layer;
            VectorImage *vectorImage = layerVector->getLastVectorImageAtFrame( mEditor->currentFrame(), 0 );
            vectorImage->applySelectionTransformation();
            selectionTransformation.reset();
        }
        setModified( mEditor->layers()->currentLayerIndex(), mEditor->currentFrame() );
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
    Layer* layer = mEditor->layers()->currentLayer();
    if ( layer == NULL ) { return; }
    if ( layer->type() == Layer::VECTOR )
    {
        LayerVector *layerVector = ( LayerVector * )layer;
        VectorImage *vectorImage = layerVector->getLastVectorImageAtFrame( mEditor->currentFrame(), 0 );
        //vectorImage->applySelectionTransformation();
        if ( currentTool()->type() == MOVE )
        {
            int selectedCurve = vectorImage->getFirstSelectedCurve();
            if ( selectedCurve != -1 )
            {
                mEditor->tools()->setWidth( vectorImage->m_curves[ selectedCurve ].getWidth() );
                mEditor->tools()->setFeather( vectorImage->m_curves[ selectedCurve ].getFeather() );
                mEditor->tools()->setInvisibility( vectorImage->m_curves[ selectedCurve ].isInvisible() );
                mEditor->tools()->setPressure( vectorImage->m_curves[ selectedCurve ].getVariableWidth() );
                mEditor->color()->setColorNumber( vectorImage->m_curves[ selectedCurve ].getColourNumber() );
            }

            int selectedArea = vectorImage->getFirstSelectedArea();
            if ( selectedArea != -1 )
            {
                mEditor->color()->setColorNumber( vectorImage->area[ selectedArea ].colourNumber );
            }
        }
    }
}

void ScribbleArea::selectAll()
{
    offset.setX( 0 );
    offset.setY( 0 );
    Layer* layer = mEditor->layers()->currentLayer();
    if ( layer == NULL ) { return; }
    if ( layer->type() == Layer::BITMAP )
    {
        setSelection( myTempView.inverted().mapRect( QRect( -2, -2, width() + 3, height() + 3 ) ), true ); // TO BE IMPROVED
    }
    if ( layer->type() == Layer::VECTOR )
    {
        VectorImage *vectorImage = ( ( LayerVector * )layer )->getLastVectorImageAtFrame( mEditor->currentFrame(), 0 );
        vectorImage->selectAll();
        setSelection( vectorImage->getSelectionRect(), true );
    }
    updateCurrentFrame();
}

void ScribbleArea::deselectAll()
{
    offset.setX( 0 );
    offset.setY( 0 );
    selectionTransformation.reset();
    mySelection.setRect( 10, 10, 20, 20 );
    myTransformedSelection.setRect( 10, 10, 20, 20 );
    myTempTransformedSelection.setRect( 10, 10, 20, 20 );

    Layer* layer = mEditor->layers()->currentLayer();
    if ( layer == NULL ) { return; }
    if ( layer->type() == Layer::VECTOR )
    {
        ( ( LayerVector * )layer )->getLastVectorImageAtFrame( mEditor->currentFrame(), 0 )->deselectAll();
    }
    somethingSelected = false;
    mBufferImg->clear();
    vectorSelection.clear();

    // clear all the data tools may have accumulated
    editor()->tools()->cleanupAllToolsData();

    updateCurrentFrame();
}

void ScribbleArea::toggleOnionNext( bool checked )
{
    setEffect( EFFECT_NEXT_ONION, checked );
    updateAllFrames();
    emit onionNextChanged( checked );
}

void ScribbleArea::toggleOnionPrev( bool checked )
{
    setEffect( EFFECT_PREV_ONION, checked );
    updateAllFrames();
    emit onionPrevChanged( checked );
}

void ScribbleArea::toggleMultiLayerOnionSkin( bool checked )
{
    mMultiLayerOnionSkin = checked;
    updateAllFrames();
    emit multiLayerOnionSkinChanged( mMultiLayerOnionSkin );
}


void ScribbleArea::toggledOnionColor()
{
    if ( onionBlue )
    {
        if ( onionRed )
        {
            onionColor = QColor( 232, 48, 255, 255 ); // subtle violet ( blue + red )
        }
        else
        {
            onionColor = Qt::blue;
        }
    }
    else if ( onionRed )
    {
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
    setEffect( EFFECT_GRID_A, checked );
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
    vectorImage->outputImage( targetImage, myTempView, true, mShowThinLines, true ); // the target image is the vector image with simplified curves (all width=1)
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
                        BezierArea newArea = BezierArea( closedPath, mEditor->color()->frontColorNumber() );
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
    mBufferImg->clear();
    deselectAll();
}

/************************************************************************************/
// tool handling

BaseTool *ScribbleArea::currentTool()
{
    return editor()->tools()->currentTool();
}

BaseTool* ScribbleArea::getTool( ToolType eToolType )
{
    return editor()->tools()->getTool( eToolType );
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

    mPrevToolType = currentTool()->type();

    // --- change cursor ---
    setCursor( currentTool()->cursor() );
    qDebug() << "fn: setCurrentTool " << "call: setCursor()" << "current tool" << currentTool()->typeName();
}

void ScribbleArea::setTemporaryTool( ToolType eToolMode )
{
    instantTool = true; // used to return to previous tool when finished (keyRelease).
    mPrevTemporalToolType = currentTool()->type();
    editor()->tools()->setCurrentTool( eToolMode );
}

void ScribbleArea::deleteSelection()
{
    if ( somethingSelected )      // there is something selected
    {
        Layer* layer = mEditor->layers()->currentLayer();
        if ( layer == NULL ) { return; }
        mEditor->backup( tr( "DeleteSel" ) );
        closestCurves.clear();
        if ( layer->type() == Layer::VECTOR ) { ( ( LayerVector * )layer )->getLastVectorImageAtFrame( mEditor->currentFrame(), 0 )->deleteSelection(); }
        if ( layer->type() == Layer::BITMAP ) { ( ( LayerBitmap * )layer )->getLastBitmapImageAtFrame( mEditor->currentFrame(), 0 )->clear( mySelection ); }
        updateAllFrames();
    }
}

void ScribbleArea::clearImage()
{
    Layer* layer = mEditor->layers()->currentLayer();
    if ( layer == NULL ) { return; }
    if ( layer->type() == Layer::VECTOR )
    {
        mEditor->backup( tr( "ClearImg" ) ); // undo: only before change (just before)
        ( ( LayerVector * )layer )->getLastVectorImageAtFrame( mEditor->currentFrame(), 0 )->clear();
        closestCurves.clear();
        closestVertices.clear();
    }
    else if ( layer->type() == Layer::BITMAP )
    {
        mEditor->backup( tr( "ClearImg" ) );
        ( ( LayerBitmap * )layer )->getLastBitmapImageAtFrame( mEditor->currentFrame(), 0 )->clear();
    }
    else
    {
        return; // skip updates when nothing changes
    }
    //todo: confirm 1 and 2 are not necessary and remove comments
    //emit modification(); //1
    //update(); //2
    setModified( mEditor->layers()->currentLayerIndex(), mEditor->currentFrame() );
}

void ScribbleArea::toggleThinLines()
{
    mShowThinLines = !mShowThinLines;
    emit thinLinesChanged( mShowThinLines );
    setView( myView );
    updateAllFrames();
}

void ScribbleArea::toggleOutlines()
{
    mIsSimplified = !mIsSimplified;
    emit outlinesChanged( mIsSimplified );
    setView( myView );
    updateAllFrames();
}

void ScribbleArea::toggleMirror()
{
    myView = myView * QTransform( -1, 0, 0, 1, 0, 0 );
    setView( myView );
    updateAllFrames();
}

void ScribbleArea::toggleMirrorV()
{
    myView = myView * QTransform( 1, 0, 0, -1, 0, 0 );
    setView( myView );
    updateAllFrames();
}

void ScribbleArea::toggleShowAllLayers()
{
    mShowAllLayers++;
    if ( mShowAllLayers == 3 )
    {
        mShowAllLayers = 0;
    }
    setView( myView );
    updateAllFrames();
}

void ScribbleArea::setPrevTool()
{
    editor()->tools()->setCurrentTool( mPrevTemporalToolType );
    instantTool = false;
}

void ScribbleArea::initDisplayEffect( std::vector< uint32_t >& effects )
{
    effects.resize( EFFECT_COUNT );
    std::fill( effects.begin(), effects.end(), 0 );

    QSettings settings( PENCIL2D, PENCIL2D );

    if ( settings.contains( SETTING_DISPLAY_EFFECT ) )
    {
        QList< QVariant > list = settings.value( SETTING_DISPLAY_EFFECT ).toList();
		for ( int i = 0; i < list.size(); ++i )
        {
            mEffects[ i ] = static_cast< DisplayEffect >( list[ i ].toUInt() );
        }
    }
    else
    {
        // use default value
        effects[ EFFECT_ANTIALIAS ] = 1;
        effects[ EFFECT_SHADOW ] = 0;
		effects[ EFFECT_PREV_ONION ] = 1;
		effects[ EFFECT_NEXT_ONION ] = 0;
		effects[ EFFECT_GRID_A ] = 0;
    }

    effects[ EFFECT_AXIS ] = 0;

#ifdef DRAW_AXIS
    effects[ EFFECT_AXIS ] = 1;
#endif

}

/* Render Canvas */
void ScribbleArea::drawShadow( QPainter& painter )
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

void ScribbleArea::drawAxis( QPainter& painter )
{
	painter.setPen( Qt::green );
	painter.drawLine( QLineF( 0, 0, 0, 500 ) );

	painter.setPen( Qt::red );
	painter.drawLine( QLineF( 0, 0, 500, 0 ) );
}

void ScribbleArea::drawGrid( QPainter& painter )
{
	if ( mEditor->layers()->currentLayer() == nullptr )
	{
		return;
	}

	int gridSize = 30;

	auto round100 = [ = ]( double f ) -> int
	{
		return static_cast< int >( f ) / gridSize * gridSize;
	};

	QRectF boundingRect = painter.clipBoundingRect( );
	int left = round100( boundingRect.left( ) ) - gridSize;
	int right = round100( boundingRect.right( ) ) + gridSize;
	int top = round100( boundingRect.top( ) ) - gridSize;
	int bottom = round100( boundingRect.bottom( ) ) + gridSize;

	QPen pen( Qt::lightGray );
	pen.setCosmetic( true );
	painter.setPen( pen );
	painter.setViewTransformEnabled( true );
	painter.setBrush( Qt::NoBrush );

	for ( int x = left; x < right; x += gridSize )
	{
		painter.drawLine( x, top, x, bottom );
	}

	for ( int y = top; y < bottom; y += gridSize )
	{
		painter.drawLine( left, y, right, y );
	}
}