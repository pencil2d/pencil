/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2013-2014 Matt Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation;

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/
#include "scribblearea.h"

#include <cmath>
#include <QScopedPointer>
#include <QMessageBox>
#include <QPixmapCache>

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
#include "preview.h"


#define round(f) ((int)(f + 0.5))


ScribbleArea::ScribbleArea( QWidget* parent ) : QWidget( parent ),
mLog( "ScribbleArea" )
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
    else if ( brushName == "grey" )
    {
        brush = QBrush( Qt::lightGray );
    }
    else if ( brushName == "checkerboard" )
    {
        QPixmap pattern( 16, 16 );
        pattern.fill( QColor( 255, 255, 255 ) );
        QPainter painter( &pattern );
        painter.fillRect( QRect( 0, 0, 8, 8 ), QColor( 220, 220, 220 ) );
        painter.fillRect( QRect( 8, 8, 8, 8 ), QColor( 220, 220, 220 ) );
        painter.end();
        brush.setTexture( pattern );
    }
    else if ( brushName == "dots" )
    {
        QPixmap pattern( ":background/dots.png" );
        brush.setTexture( pattern );
    }
    else if ( brushName == "weave" )
    {
        QPixmap pattern( ":background/weave.jpg" );
        brush.setTexture( pattern );
    }
    else if ( brushName == "grid" )
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
    int frameNumber = mEditor->layers()->LastFrameAtFrame( frame );
    QPixmapCache::remove( "frame" + QString::number( frameNumber ) );

    update();
}

void ScribbleArea::updateAllFrames()
{
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
        if ( layer->type() == Layer::VECTOR )
        {
            ( ( LayerVector * )layer )->getLastVectorImageAtFrame( frameNumber, 0 )->setModified( true );
        }
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

    if ( mMouseInUse ){ return; } // prevents shortcuts calls while drawing, todo: same check for remaining shortcuts (in connects).

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
				mEditor->zoomIn();
			}
			else
			{
				mEditor->zoomOut();
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
    else {
        editor()->tools()->tabletRestorePrevTool();
    }
    event->ignore(); // indicates that the tablet event is not accepted yet, so that it is propagated as a mouse event)
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

void ScribbleArea::mousePressEvent( QMouseEvent* event )
{
    mMouseInUse = true;

    mStrokeManager->mousePressEvent( event );
    if ( mStrokeManager->isTabletInUse() || currentTool()->properties.pressure == false )
    {
        mUsePressure = false;
        mStrokeManager->setPressure( 1.0 );
        currentTool()->adjustPressureSensitiveProperties( 1.0, true );
    }
    else
    {
        mUsePressure = true;
    }

	//----------------code for starting hand tool when middle mouse is pressed
	if ( event->buttons() & Qt::MidButton )
	{
		//qDebug() << "Hand Start " << event->pos();
		mPrevTemporalToolType = currentTool()->type();
		editor()->tools()->setCurrentTool( HAND );
	}
	else if ( event->button() == Qt::LeftButton )    // if the user is pressing the left or right button
    {
        lastPixel = mStrokeManager->getLastPressPixel();
        lastPoint = mEditor->view()->mapScreenToCanvas( lastPixel );
    }

    // ----- assisted tool adjusment -- todo: simplify this
    if ( event->button() == Qt::LeftButton )
	{
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

    currentPoint = mEditor->view()->mapScreenToCanvas( currentPixel );
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

	Q_EMIT refreshPreview();
	
    mStrokeManager->mouseMoveEvent( event );
    currentPixel = mStrokeManager->getCurrentPixel();
    currentPoint = mEditor->view()->mapScreenToCanvas( currentPixel );

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

void ScribbleArea::resizeEvent( QResizeEvent *event )
{
    QWidget::resizeEvent( event );
    mCanvas = QPixmap( size() );
    mEditor->view()->setCanvasSize( size() );
    updateAllFrames();
}

/************************************************************************************/
// paint methods

void ScribbleArea::paintBitmapBuffer()
{
    Layer* layer = mEditor->layers()->currentLayer();
    
    // ---- checks ------
    Q_ASSERT( layer );
    if ( layer == NULL ) { return; } // TODO: remove in future.
	

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

    qCDebug( mLog ) << "Paste Rect" << mBufferImg->bounds();

    QRect rect = mEditor->view()->getView().mapRect( mBufferImg->bounds() );

    // Clear the buffer
    mBufferImg->clear();

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

void ScribbleArea::refreshBitmap( const QRectF& rect, int rad )
{
	// TODO: temp disable
    //QRectF updatedRect = mEditor->view()->mapCanvasToScreen( rect.normalized().adjusted( -rad, -rad, +rad, +rad ) );
    //update( updatedRect.toRect() );
	
	update();
}

void ScribbleArea::refreshVector( const QRectF& rect, int rad )
{
    // Does not work
//    QRectF updatedRect = mEditor->view()->mapCanvasToScreen( rect.normalized().adjusted( -rad, -rad, +rad, +rad ) );
//    update( updatedRect.toRect() );

    update();
}

void ScribbleArea::paintEvent( QPaintEvent* event )
{
    //qCDebug( mLog ) << "Paint event!" << QDateTime::currentDateTime() << event->rect();

    QPainter painter( this );

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
		Q_CHECK_PTR( layer );
        if ( layer->type() == Layer::VECTOR )
        {
            auto vecLayer = static_cast<LayerVector*>( layer );
            vecLayer->getLastVectorImageAtFrame( mEditor->currentFrame(), 0 )->setModified( true );
        }
    }


    // paints the canvas
    painter.setWorldMatrixEnabled( false );
    //painter.setTransform( transMatrix ); // FIXME: drag canvas by hand
    painter.drawPixmap( QPoint( 0, 0 ), mCanvas );


    Layer *layer = mEditor->layers()->currentLayer();

    if ( !editor()->playback()->isPlaying() )    // we don't need to display the following when the animation is playing
    {
        if ( layer->type() == Layer::VECTOR )
        {
            VectorImage *vectorImage = ( ( LayerVector * )layer )->getLastVectorImageAtFrame( mEditor->currentFrame(), 0 );

            switch ( currentTool()->type() )
            {
            case SMUDGE:
            case HAND:
            {
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
                    
                    for ( int vertexNumber = -1; vertexNumber < vectorImage->getCurveSize( curveNumber ); vertexNumber++ )
                    {
                        QPointF vertexPoint = vectorImage->getVertex( curveNumber, vertexNumber );
                        QRectF rectangle( mEditor->view()->mapCanvasToScreen( vertexPoint ) - QPointF( 3.0, 3.0 ), QSizeF( 7, 7 ) );
                        if ( rect().contains( mEditor->view()->mapCanvasToScreen( vertexPoint ).toPoint() ) )
                        {
                            painter.drawRect( rectangle );
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
                    QRectF rectangle0 = QRectF( mEditor->view()->mapCanvasToScreen( vertexPoint ) - QPointF( 3.0, 3.0 ), QSizeF( 7, 7 ) );
                    painter.drawRect( rectangle0 );
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
                        
                        QRectF rectangle = QRectF( mEditor->view()->mapCanvasToScreen( vertexPoint ) - QPointF( 3.0, 3.0 ), QSizeF( 7, 7 ) );
                        painter.drawRect( rectangle );
                   
                    }
                }
                painter.restore();
				break;
            }

            case MOVE:
            {
                // ----- paints the closest curves
                mBufferImg->clear();
                QPen pen2( Qt::black, 0.5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin );
                QColor colour = QColor( 100, 100, 255 );

                for ( int k = 0; k < closestCurves.size(); k++ )
                {
                    float scale = mEditor->view()->scaling(); // FIXME: check whether it's correct (det = area?)
                    
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
                    mBufferImg->drawPath( mEditor->view()->mapCanvasToScreen( path ), 
                                          pen2, 
                                          colour,
                                          QPainter::CompositionMode_SourceOver,
                                          isEffectOn( EFFECT_ANTIALIAS ) );
                }
				break;
            }
            } // end siwtch
        }

        // paints the  buffer image
        if ( mEditor->layers()->currentLayer() != NULL )
        {
            painter.setOpacity( 1.0 );
            if ( mEditor->layers()->currentLayer()->type() == Layer::BITMAP ) 
			{ 
				painter.setWorldMatrixEnabled( true );
				painter.setTransform( mEditor->view()->getView() );
			}
            else if ( mEditor->layers()->currentLayer()->type() == Layer::VECTOR )
			{ 
				painter.setWorldMatrixEnabled( false );
			}
			
			qCDebug( mLog ) << "BufferRect" << mBufferImg->bounds();

            mBufferImg->paintImage( painter );
        }

        // paints the selection outline
        if ( somethingSelected && ( myTempTransformedSelection.isValid() || mMoveMode == ROTATION ) ) // @revise
        {
            // outline of the transformed selection
            painter.setWorldMatrixEnabled( false );
            painter.setOpacity( 1.0 );
            QPolygon tempRect = mEditor->view()->getView().mapToPolygon( myTempTransformedSelection.normalized().toRect() );

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
#ifdef _DEBUG
    painter.setWorldMatrixEnabled( false );
    painter.setPen( QPen( Qt::gray, 2 ) );
    painter.setBrush( Qt::NoBrush );
    painter.drawRect( QRect( 0, 0, width(), height() ) );
#endif

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
    
    QPainter painter( &mCanvas );
    painter.setRenderHint( QPainter::SmoothPixmapTransform, isEffectOn( EFFECT_ANTIALIAS ) );

    painter.setClipRect( rect );
    painter.setClipping( true );
    
    painter.setTransform( mEditor->view()->getView() );
    painter.setWorldMatrixEnabled( true );

    // background
    painter.setPen( Qt::NoPen );
    painter.setBrush( backgroundBrush );
    painter.drawRect( mEditor->view()->mapScreenToCanvas( QRect( -2, -2, width() + 3, height() + 3 ) ) );  // this is necessary to have the background move with the view

    QRectF viewRect = getViewRect();
    QRectF vectorViewRect = viewRect.translated( -viewRect.left(), -viewRect.top() );

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
                    painter.setWorldMatrixEnabled( true );

                    // previous frame (onion skin)
                    if ( isEffectOn( EFFECT_PREV_ONION ) &&  frame > 1 && frame < layerBitmap->getMaxKeyFramePosition()+1)
                    {
                        int prevFramesNum = mEditor->getOnionPrevFramesNum();
                        float onionOpacity = mEditor->getOnionMaxOpacity();
                        
                        for ( int j = 0; j < prevFramesNum; j++ )
                        {
                            if ((frame - j) > 1) {
                                BitmapImage *previousImage = layerBitmap->getLastBitmapImageAtFrame( frame, -(j + 1) );
                                if ( previousImage != NULL)
                                {
                                    painter.setOpacity( opacity * onionOpacity / 100.0 );
                                    previousImage->paintImage( painter );
                                    if ( prevFramesNum != 1 ) onionOpacity -= (mEditor->getOnionMaxOpacity() - mEditor->getOnionMinOpacity()) / (prevFramesNum - 1);
                                }
                            }
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
                    if ( isEffectOn( EFFECT_NEXT_ONION ) && frame < layerBitmap->getMaxKeyFramePosition())
                    {
                        int nextFramesNum = mEditor->getOnionNextFramesNum();
                        float onionOpacity = mEditor->getOnionMaxOpacity();
                        
                        for ( int j = 0; j < nextFramesNum; j++ )
                        {
                            if ((frame + j) < layerBitmap->getMaxKeyFramePosition()) {
                                BitmapImage *nextImage = layerBitmap->getLastBitmapImageAtFrame( frame, j + 1 );
                                if ( nextImage != NULL )
                                {
                                    painter.setOpacity( opacity * onionOpacity / 100.0 );
                                    nextImage->paintImage( painter );
                                    if ( nextFramesNum != 1 ) onionOpacity -= (mEditor->getOnionMaxOpacity() - mEditor->getOnionMinOpacity()) / (nextFramesNum - 1);
                                }
                            }
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

                painter.setWorldMatrixEnabled( false );

                // previous frame (onion skin)
                if ( isEffectOn( EFFECT_PREV_ONION ) &&  frame > 1  && frame < layerVector->getMaxKeyFramePosition()+1)
                {
                    QTransform viewTransform = mEditor->view()->getView();
                    int prevFramesNum = mEditor->getOnionPrevFramesNum();
                    float onionOpacity = mEditor->getOnionMinOpacity();
                    
                    for ( int j = 0; j < prevFramesNum; j++ )
                    {
                        if ((frame - (prevFramesNum - j)) > 0) {
                            VectorImage* pVectorImage = layerVector->getLastVectorImageAtFrame( frame, -(prevFramesNum - j));
                            pVectorImage->outputImage( pImage.data(), viewTransform, mIsSimplified, mShowThinLines, isEffectOn( EFFECT_ANTIALIAS ) );
                            painter.setOpacity( opacity * onionOpacity / 100.0 );
                            painter.drawImage( QPoint( 0, 0 ), *pImage );
                            if (prevFramesNum != 1) onionOpacity += (mEditor->getOnionMaxOpacity() - mEditor->getOnionMinOpacity()) / (prevFramesNum - 1);
                        }
                    }
                    
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
                if ( isEffectOn( EFFECT_NEXT_ONION ) && frame < layerVector->getMaxKeyFramePosition() )
                {
                    QTransform viewTransform = mEditor->view()->getView();
                    int nextFramesNum = mEditor->getOnionNextFramesNum();
                    float onionOpacity = mEditor->getOnionMinOpacity();
                    
                    for ( int j = 0; j < nextFramesNum; j++ )
                    {
                        VectorImage* pVectorImage = layerVector->getLastVectorImageAtFrame( frame, nextFramesNum - j);
                        pVectorImage->outputImage( pImage.data(), viewTransform, mIsSimplified, mShowThinLines, isEffectOn( EFFECT_ANTIALIAS ) );
                        painter.setOpacity( opacity * onionOpacity / 100.0 );
                        painter.drawImage( QPoint( 0, 0 ), *pImage );
                        if (nextFramesNum != 1) onionOpacity += (mEditor->getOnionMaxOpacity() - mEditor->getOnionMinOpacity()) / (nextFramesNum - 1);
                    }

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
                painter.setWorldMatrixEnabled( true );
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
                    painter.setWorldMatrixEnabled( true );
                    painter.setOpacity( opacity );
                    if ( i == mEditor->layers()->currentLayerIndex() 
                         && somethingSelected 
                         && ( myRotatedAngle != 0 || myTempTransformedSelection != mySelection || myFlipX != 1 || myFlipY != 1 ) )
                    {
                        // hole in the original selection -- might support arbitrary shapes in the future
                        
                        painter.setClipping( true );

                        QRegion clip = QRegion( mySelection.toRect() );
                        QRegion totalImage = QRegion( mEditor->view()->mapScreenToCanvas( QRectF( -2, -2, width() + 3, height() + 3 ) ).toRect() );
                        QRegion ImageWithHole = totalImage -= clip;
                        painter.setClipRegion( ImageWithHole, Qt::ReplaceClip );
                        //painter.drawImage(bitmapImage->topLeft(), *(bitmapImage->image) );
                        bitmapImage->paintImage( painter );
                        painter.setClipping( false );
                        // transforms the bitmap selection
                        bool smoothTransform = false;

                        if ( myTempTransformedSelection.width() != mySelection.width() 
                             || myTempTransformedSelection.height() != mySelection.height() 
                             || myRotatedAngle != 0 )
                        { 
                            smoothTransform = true;
                        }
                        BitmapImage selectionClip = bitmapImage->copy( mySelection.toRect() );
                        selectionClip.transform( myTransformedSelection, smoothTransform );
                        QTransform rm;

                        //TODO: complete matrix calls ( sounds funny :)
                        rm.scale( myFlipX, myFlipY );
                        rm.rotate( myRotatedAngle );
                        QImage* rotImg = new QImage( selectionClip.image()->transformed( rm ) );
                        QPoint dxy = QPoint( ( myTempTransformedSelection.width() - rotImg->rect().width() ) / 2,
                                             ( myTempTransformedSelection.height() - rotImg->rect().height() ) / 2 );
                        selectionClip.setImage( rotImg ); // TODO: find/create a func. (*object = data is not very orthodox)
                        selectionClip.bounds().translate( dxy );
                        selectionClip.paintImage( painter );
                    }
                    else
                    {
                        bitmapImage->paintImage( painter );
                    }
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
                QTransform view = mEditor->view()->getView();
                QScopedPointer< QImage > pImage( new QImage( size(), QImage::Format_ARGB32_Premultiplied ) );
                vectorImage->outputImage( pImage.data(), view, mIsSimplified, mShowThinLines, isEffectOn( EFFECT_ANTIALIAS ) );

                painter.setWorldMatrixEnabled( false );
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
        //drawAxis( painter );
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

    BitmapImage* tempBitmapImage = new BitmapImage;
    tempBitmapImage = new BitmapImage;
    tempBitmapImage->drawRect( rectangle, Qt::NoPen, radialGrad,
                               QPainter::CompositionMode_Source, isEffectOn( EFFECT_ANTIALIAS ) );

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
    bmiSrcClip.bounds().moveTo( trgRect.topLeft().toPoint() );
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

    for ( yb = bmiTmpClip->bounds().top(); yb < bmiTmpClip->bounds().bottom(); yb++ )
    {
        for ( xb = bmiTmpClip->bounds().left(); xb < bmiTmpClip->bounds().right(); xb++ )
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

        QRectF updateRect = mEditor->view()->mapCanvasToScreen( tempPath.boundingRect().toRect() ).adjusted( -10, -10, 10, 10 );
        if ( mEditor->layers()->currentLayer()->type() == Layer::VECTOR )
        {
            tempPath = mEditor->view()->mapCanvasToScreen( tempPath );
            if ( mMakeInvisible )
            {
                pen2.setWidth( 0 );
                pen2.setStyle( Qt::DotLine );
            }
            else
            {
                pen2.setWidth( getTool( PEN )->properties.width * mEditor->view()->scaling() );
            }
        }
        mBufferImg->clear();
        mBufferImg->drawPath( tempPath, pen2, Qt::NoBrush, QPainter::CompositionMode_SourceOver, isEffectOn( EFFECT_ANTIALIAS ) );

        update( updateRect.toRect() );
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
        ( ( LayerVector * )layer )->getLastVectorImageAtFrame( mEditor->currentFrame(), 0 )->addCurve( curve, mEditor->view()->scaling() );
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

/************************************************************************************/
// view handling

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
        return mEditor->view()->getView();
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
    return mEditor->view()->mapScreenToCanvas( QPointF( width() / 2, height() / 2 ) );
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
            QImage* rotImg = new QImage( selectionClip.image()->transformed( rm, Qt::SmoothTransformation ) );
            QPoint dxy = QPoint( ( myTempTransformedSelection.width() - rotImg->rect().width() ) / 2,
                                 ( myTempTransformedSelection.height() - rotImg->rect().height() ) / 2 );
            selectionClip.setImage( rotImg ); // TODO: find/create a func. (*object = data is not very orthodox)
            selectionClip.bounds().translate( dxy );
            bitmapImage->clear( mySelection.toRect() );
            bitmapImage->paste( &selectionClip );
        }
        else if ( layer->type() == Layer::VECTOR )
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

    Q_ASSERT( layer );
    if ( layer == NULL ) { return; }

    if ( layer->type() == Layer::BITMAP )
    {
        setSelection( mEditor->view()->mapScreenToCanvas( QRectF( -2, -2, width() + 3, height() + 3 ) ), true ); // TO BE IMPROVED
    }
    else if ( layer->type() == Layer::VECTOR )
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

/************************************************************************************/
// tool handling

BaseTool* ScribbleArea::currentTool()
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
    //TODO: confirm 1 and 2 are not necessary and remove comments
    //emit modification(); //1
    //update(); //2
    setModified( mEditor->layers()->currentLayerIndex(), mEditor->currentFrame() );
}

void ScribbleArea::toggleThinLines()
{
    mShowThinLines = !mShowThinLines;
    emit thinLinesChanged( mShowThinLines );
    
    updateAllFrames();
}

void ScribbleArea::toggleOutlines()
{
    mIsSimplified = !mIsSimplified;
    emit outlinesChanged( mIsSimplified );
    
    updateAllFrames();
}

void ScribbleArea::toggleShowAllLayers()
{
    mShowAllLayers++;
    if ( mShowAllLayers == 3 )
    {
        mShowAllLayers = 0;
    }
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

//#define DRAW_AXIS
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
	painter.drawLine( QLineF( 0, -500, 0, 500 ) );

	painter.setPen( Qt::red );
	painter.drawLine( QLineF( -500, 0, 500, 0 ) );
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
    painter.setWorldMatrixEnabled( true );
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

void ScribbleArea::paletteColorChanged(QColor color)
{
    updateAllVectorLayersAtCurrentFrame();
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
    deselectAll();
}
