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
//#include "popupcolorpalettewidget.h"


#define round(f) ((int)(f + 0.5))


ScribbleArea::ScribbleArea( QWidget* parent ) : QWidget( parent ),
mLog( "ScribbleArea" )
{
    setObjectName( "ScribbleArea" );

    // Qt::WA_StaticContents ensure that the widget contents are rooted to the top-left corner
    // and don't change when the widget is resized.
    setAttribute( Qt::WA_StaticContents );

    mStrokeManager = new StrokeManager();
}

ScribbleArea::~ScribbleArea()
{

}

bool ScribbleArea::init()
{
    mPrefs = mEditor->preference();

    connect( mPrefs, &PreferenceManager::optionChanged, this, &ScribbleArea::settingUpdated );

    int curveSmoothingLevel = mPrefs->getInt(SETTING::CURVE_SMOOTHING);
    mCurveSmoothingLevel = curveSmoothingLevel / 20.0; // default value is 1.0

    mQuickSizing = mPrefs->isOn( SETTING::QUICK_SIZING );
    mMakeInvisible = false;
    somethingSelected = false;

    mIsSimplified = mPrefs->isOn( SETTING::OUTLINES );
    mMultiLayerOnionSkin = mPrefs->isOn( SETTING::MULTILAYER_ONION );

    mShowAllLayers = 1;

    mBufferImg = new BitmapImage;
//    mBitmapSelection = new BitmapImage;

    QRect newSelection( QPoint( 0, 0 ), QSize( 0, 0 ) );
    mySelection = newSelection;
    myTransformedSelection = newSelection;
    myTempTransformedSelection = newSelection;
    mOffset.setX( 0 );
    mOffset.setY( 0 );
    selectionTransformation.reset();

    tol = 7.0;

    setMouseTracking( true ); // reacts to mouse move events, even if the button is not pressed

    mDebugRect = QRectF( 0, 0, 0, 0 );

    setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding ) );

    QPixmapCache::setCacheLimit( 100 * 1024 );

    mNeedUpdateAll = false;

    // color wheel popup
    //m_popupPaletteWidget = new PopupColorPaletteWidget( this );

    return true;
}

void ScribbleArea::settingUpdated(SETTING setting)
{
    switch ( setting )
    {
    case SETTING::CURVE_SMOOTHING:
        setCurveSmoothing(mPrefs->getInt(SETTING::CURVE_SMOOTHING));
        break;
    case SETTING::TOOL_CURSOR:
        updateToolCursor();
        break;
    case SETTING::ONION_PREV_FRAMES_NUM:
    case SETTING::ONION_NEXT_FRAMES_NUM:
    case SETTING::ONION_MIN_OPACITY:
    case SETTING::ONION_MAX_OPACITY:
    case SETTING::ANTIALIAS:
    case SETTING::GRID:
    case SETTING::GRID_SIZE:
    case SETTING::PREV_ONION:
    case SETTING::NEXT_ONION:
    case SETTING::ONION_BLUE:
    case SETTING::ONION_RED:
    case SETTING::INVISIBLE_LINES:
    case SETTING::OUTLINES:
        updateAllFrames();
        break;
    case SETTING::QUICK_SIZING:
        mQuickSizing = mPrefs->isOn( SETTING::QUICK_SIZING );
        break;
    case SETTING::MULTILAYER_ONION:
        mMultiLayerOnionSkin = mPrefs->isOn( SETTING::MULTILAYER_ONION );
        updateAllFrames();
    default:
        break;
    }

}

void ScribbleArea::updateToolCursor()
{
    this->setFocus();
    setCursor( currentTool()->cursor() );
    updateAllFrames();
}

void ScribbleArea::setCurveSmoothing( int newSmoothingLevel )
{
    mCurveSmoothingLevel = newSmoothingLevel / 20.0;
    updateAllFrames();
}

void ScribbleArea::setEffect( SETTING e, bool isOn )
{
    mPrefs->set(e, isOn);
    updateAllFrames();
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
            auto vecLayer = static_cast< LayerVector* >( layer );
            vecLayer->getLastVectorImageAtFrame( frameNumber, 0 )->modification();
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
    //m_popupPaletteWidget->popup();
}


/************************************************************************/
/* key event handlers                                                   */
/************************************************************************/

void ScribbleArea::escape()
{
    deselectAll();
}

void ScribbleArea::keyPressEvent( QKeyEvent *event )
{
    // Don't handle this event on auto repeat
    //
    if (event->isAutoRepeat()) {
        return;
    }

    mKeyboardInUse = true;

    if ( mMouseInUse ){ return; } // prevents shortcuts calls while drawing

    if ( instantTool ){ return; } // prevents shortcuts calls while using instant tool


    if ( currentTool()->keyPressEvent( event ) )
    {
        // has been handled by tool
        return;
    }

    // --- fixed control key shortcuts ---
    if ( event->modifiers() == ( Qt::ControlModifier | Qt::ShiftModifier ) )
    {
        setTemporaryTool( ERASER );
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
                paintTransformedSelection();
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
                paintTransformedSelection();
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
                paintTransformedSelection();
            }
            else
            {
                mEditor->layers()->gotoPreviouslayer();
                event->ignore();
            }
            break;
        case Qt::Key_Down:
            if ( somethingSelected )
            {
                myTempTransformedSelection.translate( 0, 1 );
                myTransformedSelection = myTempTransformedSelection;
                calculateSelectionTransformation();
                paintTransformedSelection();
            }
            else
            {
                mEditor->layers()->gotoNextLayer();
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
        case Qt::Key_Space:
            setTemporaryTool( HAND ); // just call "setTemporaryTool()" to activate temporarily any tool
            break;
        default:
            event->ignore();
    }
}

void ScribbleArea::keyReleaseEvent( QKeyEvent *event )
{
    // Don't handle this event on auto repeat
    //
    if (event->isAutoRepeat()) {
        return;
    }

    mKeyboardInUse = false;

    if ( mMouseInUse  ) { return; }

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
void ScribbleArea::wheelEvent( QWheelEvent* event )
{
    QPoint pixels = event->pixelDelta();
    QPoint angle = event->angleDelta();
    if ( !pixels.isNull() )
    {
        float delta = pixels.y();
        if(delta < 0)
        {
            mEditor->view()->scaleDown();
        }
        else
        {
            mEditor->view()->scaleUp();
        }
    }
    else if ( !angle.isNull() )
    {
        float delta = angle.y();
        if(delta < 0)
        {
            mEditor->view()->scaleDown();
        }
        else
        {
            mEditor->view()->scaleUp();
        }
    }

    event->accept();
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

    mUsePressure = currentTool()->properties.pressure;

    if ( !( mStrokeManager->isTabletInUse() && currentTool()->properties.pressure ) )
    {
        mStrokeManager->setPressure( 1.0 );
        currentTool()->adjustPressureSensitiveProperties( 1.0, true );
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
        mLastPixel = mStrokeManager->getLastPressPixel();
        mLastPoint = mEditor->view()->mapScreenToCanvas( mLastPixel );
    }

    // ----- assisted tool adjusment -- todo: simplify this
    if ( event->button() == Qt::LeftButton && mQuickSizing)
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

    if ( !layer->mVisible && currentTool()->type() != HAND && ( event->button() != Qt::RightButton ) )
    {
        QMessageBox::warning( this, tr( "Warning" ),
                              tr( "You are drawing on a hidden layer! Please select another layer (or make the current layer visible)." ),
                              QMessageBox::Ok,
                              QMessageBox::Ok );
        mMouseInUse = false;
        return;
    }
    // ---
    mCurrentPixel = mStrokeManager->getCurrentPixel();
    mCurrentPoint = mEditor->view()->mapScreenToCanvas( mCurrentPixel );
    //qDebug() << "CurPoint: " << mCurrentPoint;


    // the user is also pressing the mouse
    if ( event->buttons() & Qt::LeftButton || event->buttons() & Qt::RightButton )
    {
        mOffset = mCurrentPoint - mLastPoint;
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
    mCurrentPixel = mStrokeManager->getCurrentPixel();
    mCurrentPoint = mEditor->view()->mapScreenToCanvas( mCurrentPixel );

    // the user is also pressing the mouse (= dragging)
    if ( event->buttons() & Qt::LeftButton || event->buttons() & Qt::RightButton )
    {
        mOffset = mCurrentPoint - mLastPoint;
        // --- use SHIFT + drag to resize WIDTH / use CTRL + drag to resize FEATHER ---
        if ( currentTool()->isAdjusting )
        {
            ToolPropertyType tool_type;
            tool_type = event->modifiers() & Qt::ControlModifier ? FEATHER : WIDTH;
            currentTool()->adjustCursor( mOffset.x(), tool_type ); //updates cursors given org width or feather and x
            return;
        }
    }

    if ( event->buttons() == Qt::RightButton )
    {
        getTool( HAND )->mouseMoveEvent( event );
        return;
    }

    currentTool()->mouseMoveEvent( event );

#ifdef DEBUG_FPS
    // debug fps count.
    clock_t curTime = clock();
    mDebugTimeQue.push_back( curTime );

    while ( mDebugTimeQue.size() > 100 )
    {
        mDebugTimeQue.pop_front();
    }

    if ( mDebugTimeQue.size() > 2 )
    {
        clock_t interval = mDebugTimeQue.back() - mDebugTimeQue.front();
        double fps = mDebugTimeQue.size() / ( ( double )interval ) * CLOCKS_PER_SEC;
        //qDebug() << fps;
    }
#endif
}

void ScribbleArea::mouseReleaseEvent( QMouseEvent *event )
{
    mMouseInUse = false;

    // ---- checks ------
    if ( currentTool()->isAdjusting )
    {
        currentTool()->stopAdjusting();
        mEditor->tools()->setWidth( currentTool()->properties.width );
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
    mCanvas.fill(Qt::transparent);

    this->setStyleSheet("background-color:yellow;");

    mEditor->view()->setCanvasSize( size() );
    updateAllFrames();
}

/************************************************************************************/
// paint methods

void ScribbleArea::paintBitmapBuffer( )
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

void ScribbleArea::paintBitmapBufferRect( QRect rect )
{
    Layer* layer = mEditor->layers()->currentLayer();

    // ---- checks ------
    Q_ASSERT( layer );
    if ( layer == NULL ) { return; } // TODO: remove in future.

    BitmapImage *targetImage = ( ( LayerBitmap * )layer )->getLastBitmapImageAtFrame( mEditor->currentFrame(), 0 );
    // Clear the temporary pixel path
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
    mBufferImg->drawLine( P1, P2, pen, cm, mPrefs->isOn( SETTING::ANTIALIAS ) );
}

void ScribbleArea::drawPath( QPainterPath path, QPen pen, QBrush brush, QPainter::CompositionMode cm )
{
    mBufferImg->drawPath( path, pen, brush, cm, mPrefs->isOn( SETTING::ANTIALIAS ) );
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

    if ( !mMouseInUse || currentTool()->type() == MOVE || currentTool()->type() == HAND )
    {
        // --- we retrieve the canvas from the cache; we create it if it doesn't exist
        int curIndex = mEditor->currentFrame();
        int frameNumber = mEditor->layers()->LastFrameAtFrame( curIndex );

        QString strCachedFrameKey = "frame" + QString::number( frameNumber );
        if ( !QPixmapCache::find( strCachedFrameKey, mCanvas ) )
        {
            drawCanvas( mEditor->currentFrame(), event->rect() );
            QPixmapCache::insert( strCachedFrameKey, mCanvas );
            //qDebug() << "Repaint canvas!";
        }
    }

    if ( currentTool()->type() == MOVE )
    {
        Layer* layer = mEditor->layers()->currentLayer();
        Q_CHECK_PTR( layer );
        if ( layer->type() == Layer::VECTOR )
        {
            auto vecLayer = static_cast< LayerVector* >( layer );
            vecLayer->getLastVectorImageAtFrame( mEditor->currentFrame(), 0 )->setModified( true );
        }
    }

    QPainter painter( this );

    // paints the canvas
    painter.setWorldMatrixEnabled( false );
    //painter.setTransform( transMatrix ); // FIXME: drag canvas by hand

    painter.drawPixmap( QPoint( 0, 0 ), mCanvas );


    Layer* layer = mEditor->layers()->currentLayer();

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
                        for ( int k = 0; k < mClosestVertices.size(); k++ )
                        {
                            VertexRef vertexRef = mClosestVertices.at( k );
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
                    QColor colour = QColor( 100, 100, 255, 50 );
                    QPen pen2( colour, 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin );


                    for ( int k = 0; k < mClosestCurves.size(); k++ )
                    {
                        float scale = mEditor->view()->scaling(); // FIXME: check whether it's correct (det = area?)

                        int idx = mClosestCurves[ k ];
                        if ( vectorImage->m_curves.size() <= idx )
                        {
                            // safety check
                            continue;
                        }
                        BezierCurve myCurve = vectorImage->m_curves[ mClosestCurves[ k ] ];
                        if ( myCurve.isPartlySelected() )
                        {
                            myCurve.transform( selectionTransformation );
                        }
                        QPainterPath path = myCurve.getStrokedPath( 1.2 / scale, false );
                        mBufferImg->drawPath( mEditor->view()->mapCanvasToScreen( path ),
                                              pen2,
                                              colour,
                                              QPainter::CompositionMode_SourceOver,
                                              mPrefs->isOn( SETTING::ANTIALIAS ) );
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

            //qCDebug( mLog ) << "BufferRect" << mBufferImg->bounds();

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

    // outlines the frame of the viewport
#ifdef _DEBUG
    painter.setWorldMatrixEnabled( false );
    painter.setPen( QPen( Qt::gray, 2 ) );
    painter.setBrush( Qt::NoBrush );
    painter.drawRect( QRect( 0, 0, width(), height() ) );
#endif

    event->accept();
}

void ScribbleArea::drawCanvas( int frame, QRect rect )
{
    Object* object = mEditor->object();

    RenderOptions options;
    options.bPrevOnionSkin = mPrefs->isOn( SETTING::PREV_ONION );
    options.bNextOnionSkin = mPrefs->isOn( SETTING::NEXT_ONION );
    options.bColorizePrevOnion = mPrefs->isOn(SETTING::ONION_RED);
    options.bColorizeNextOnion = mPrefs->isOn(SETTING::ONION_BLUE);
    options.nPrevOnionSkinCount = mPrefs->getInt(SETTING::ONION_PREV_FRAMES_NUM);
    options.nNextOnionSkinCount = mPrefs->getInt(SETTING::ONION_NEXT_FRAMES_NUM);
    options.fOnionSkinMaxOpacity = mPrefs->getInt(SETTING::ONION_MAX_OPACITY);
    options.fOnionSkinMinOpacity = mPrefs->getInt(SETTING::ONION_MIN_OPACITY);
    options.bAntiAlias = mPrefs->isOn( SETTING::ANTIALIAS );
    options.bGrid = mPrefs->isOn( SETTING::GRID );
    options.nGridSize = mPrefs->getInt( SETTING::GRID_SIZE );
    options.bAxis = mPrefs->isOn( SETTING::AXIS );
    options.bThinLines = mPrefs->isOn( SETTING::INVISIBLE_LINES );
    options.bOutlines = mPrefs->isOn( SETTING::OUTLINES );
    options.nShowAllLayers = mShowAllLayers;
    options.bIsOnionAbsolute = (mPrefs->getString( SETTING::ONION_TYPE ) == "absolute");

    mCanvasRenderer.setOptions( options );

    //qDebug() << "Antialias=" << options.bAntiAlias;

    mCanvasRenderer.setCanvas( &mCanvas );
    mCanvasRenderer.setViewTransform( mEditor->view()->getView() );
    mCanvasRenderer.paint( object, mEditor->layers()->currentLayerIndex(), frame, rect );

    return;
}

void ScribbleArea::setGaussianGradient( QGradient &gradient, QColor colour, qreal opacity, qreal mOffset )
{
    if (mOffset < 0) {
        mOffset = 0;
    }
    if (mOffset > 100) {
        mOffset = 100;
    }
    int r = colour.red();
    int g = colour.green();
    int b = colour.blue();
    qreal a = colour.alphaF();

    int mainColorAlpha = qRound( a * 255 * opacity );

    // the more feather (offset), the more softness (opacity)
    //
    int alphaAdded = qRound((mainColorAlpha * mOffset) / 100);

    gradient.setColorAt( 0.0, QColor( r, g, b, mainColorAlpha - alphaAdded ) );
    gradient.setColorAt( 1.0, QColor( r, g, b, 0 ) );
    gradient.setColorAt( 1.0 - (mOffset/100.0), QColor( r, g, b, mainColorAlpha - alphaAdded ) );
}

void ScribbleArea::drawPen( QPointF thePoint, qreal brushWidth, QColor fillColour, qreal opacity )
{
    qreal offset = 64;

    QRadialGradient radialGrad( thePoint, 0.5 * brushWidth );
    setGaussianGradient( radialGrad, fillColour, opacity/2, offset );

    QRectF rectangle( thePoint.x() - 0.5 * brushWidth, thePoint.y() - 0.5 * brushWidth, brushWidth, brushWidth );

    mBufferImg->drawEllipse( rectangle, Qt::NoPen, radialGrad,
                             QPainter::CompositionMode_SourceOver, mPrefs->isOn( SETTING::ANTIALIAS ) );
}

void ScribbleArea::drawPencil( QPointF thePoint, qreal brushWidth, QColor fillColour, qreal opacity )
{
    drawBrush(thePoint, brushWidth, 50, fillColour, opacity / 5);
}

void ScribbleArea::drawBrush( QPointF thePoint, qreal brushWidth, qreal mOffset, QColor fillColour, qreal opacity, bool usingFeather )
{
    QRectF rectangle( thePoint.x() - 0.5 * brushWidth, thePoint.y() - 0.5 * brushWidth, brushWidth, brushWidth );

    BitmapImage* tempBitmapImage = new BitmapImage;
    if (usingFeather==true)
    {
        QRadialGradient radialGrad( thePoint, 0.5 * brushWidth );
        setGaussianGradient( radialGrad, fillColour, opacity, mOffset );

        tempBitmapImage->drawEllipse( rectangle, Qt::NoPen, radialGrad,
                                   QPainter::CompositionMode_Source, mPrefs->isOn( SETTING::ANTIALIAS ) );
    }
    else
    {
        tempBitmapImage->drawEllipse( rectangle, Qt::NoPen, QBrush(fillColour, Qt::SolidPattern),
                                   QPainter::CompositionMode_Source, mPrefs->isOn( SETTING::ANTIALIAS ) );
    }
    mBufferImg->paste( tempBitmapImage );
    delete tempBitmapImage;
}

void ScribbleArea::blurBrush( BitmapImage *bmiSource_, QPointF srcPoint_, QPointF thePoint_, qreal brushWidth_, qreal mOffset_, qreal opacity_ )
{
    QRadialGradient radialGrad( thePoint_, 0.5 * brushWidth_ );
    setGaussianGradient( radialGrad, QColor( 255, 255, 255, 127 ), opacity_, mOffset_ );

    QRectF srcRect( srcPoint_.x() - 0.5 * brushWidth_, srcPoint_.y() - 0.5 * brushWidth_, brushWidth_, brushWidth_ );
    QRectF trgRect( thePoint_.x() - 0.5 * brushWidth_, thePoint_.y() - 0.5 * brushWidth_, brushWidth_, brushWidth_ );

    BitmapImage bmiSrcClip = bmiSource_->copy( srcRect.toRect() );
    BitmapImage bmiTmpClip = bmiSrcClip; // todo: find a shorter way

    bmiTmpClip.drawRect( srcRect, Qt::NoPen, radialGrad, QPainter::CompositionMode_Source, mPrefs->isOn( SETTING::ANTIALIAS ) );
    bmiSrcClip.bounds().moveTo( trgRect.topLeft().toPoint() );
    bmiTmpClip.paste( &bmiSrcClip, QPainter::CompositionMode_SourceAtop );
    mBufferImg->paste( &bmiTmpClip );
}

void ScribbleArea::liquifyBrush( BitmapImage *bmiSource_, QPointF srcPoint_, QPointF thePoint_, qreal brushWidth_, qreal mOffset_, qreal opacity_ )
{
    QPointF delta = ( thePoint_ - srcPoint_ ); // increment vector
    QRectF trgRect( thePoint_.x() - 0.5 * brushWidth_, thePoint_.y() - 0.5 * brushWidth_, brushWidth_, brushWidth_ );

    QRadialGradient radialGrad( thePoint_, 0.5 * brushWidth_ );
    setGaussianGradient( radialGrad, QColor( 255, 255, 255, 255 ), opacity_, mOffset_ );

    // Create gradient brush
    BitmapImage* bmiTmpClip = new BitmapImage;
    bmiTmpClip->drawRect( trgRect, Qt::NoPen, radialGrad, QPainter::CompositionMode_Source, mPrefs->isOn( SETTING::ANTIALIAS ) );

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
                   getTool( POLYLINE )->properties.width,
                   Qt::SolidLine,
                   Qt::RoundCap,
                   Qt::RoundJoin );
        QPainterPath tempPath;
        if ( currentTool()->properties.bezier_state )
        {
            tempPath = BezierCurve( points ).getSimplePath();
        }
        else
        {
            tempPath = BezierCurve( points ).getStraightPath();
        }
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
                pen2.setWidth( getTool( POLYLINE )->properties.width * mEditor->view()->scaling() );
            }
        }
        mBufferImg->clear();
        mBufferImg->drawPath( tempPath, pen2, Qt::NoBrush, QPainter::CompositionMode_SourceOver, mPrefs->isOn( SETTING::ANTIALIAS ) );

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
            curve.setWidth( getTool( POLYLINE )->properties.width );
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
        //qDebug() << "viewCamera" << ( ( LayerCamera * )layer )->getViewAtFrame( mEditor->currentFrame() );
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

    if ( mySelection.width() == 0 )
    {
        scaleX = 1.0;
    }
    else
    {
        scaleX = myTempTransformedSelection.width() / mySelection.width();
    }

    if ( mySelection.height() == 0 )
    {
        scaleY = 1.0;
    }
    else
    {
        scaleY = myTempTransformedSelection.height() / mySelection.height();
    }

    selectionTransformation.reset();
    selectionTransformation.translate( c1x, c1y );
    selectionTransformation.rotate(myRotatedAngle);
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
        if ( layer->type() == Layer::BITMAP )
        {
            mCanvasRenderer.setTransformedSelection(mySelection.toRect(), selectionTransformation);
        }
        else if ( layer->type() == Layer::VECTOR )
        {
            // vector transformation
            LayerVector *layerVector = ( LayerVector * )layer;
            VectorImage *vectorImage = layerVector->getLastVectorImageAtFrame( mEditor->currentFrame(), 0 );
            vectorImage->setSelectionTransformation( selectionTransformation );

        }
        setModified( mEditor->layers()->currentLayerIndex(), mEditor->currentFrame() );
    }
    update();
}

void ScribbleArea::applyTransformedSelection()
{
    mCanvasRenderer.ignoreTransformedSelection();

    Layer* layer = mEditor->layers()->currentLayer();
    if ( layer == NULL )
    {
        return;
    }

    if ( somethingSelected )    // there is something selected
    {
        if ( layer->type() == Layer::BITMAP )
        {
            BitmapImage* bitmapImage = dynamic_cast< LayerBitmap* >( layer )->getLastBitmapImageAtFrame( mEditor->currentFrame(), 0 );

            BitmapImage* transformedImage = new BitmapImage(bitmapImage->transformed(mySelection.toRect(), selectionTransformation, mPrefs->isOn(SETTING::ANTIALIAS)));

            bitmapImage->clear(mySelection);
            bitmapImage->paste(transformedImage, QPainter::CompositionMode_SourceOver);

            delete transformedImage;
        }
        else if ( layer->type() == Layer::VECTOR )
        {

            VectorImage *vectorImage = ((LayerVector *)layer)->getLastVectorImageAtFrame(mEditor->currentFrame(), 0);
            vectorImage->applySelectionTransformation();

        }

        setModified( mEditor->layers()->currentLayerIndex(), mEditor->currentFrame() );
        deselectAll();
    }

    updateCurrentFrame();
}

void ScribbleArea::cancelTransformedSelection()
{
    mCanvasRenderer.ignoreTransformedSelection();

    if (somethingSelected) {

        Layer* layer = mEditor->layers()->currentLayer();
        if ( layer == NULL )
        {
            return;
        }

        if ( layer->type() == Layer::VECTOR ) {

            VectorImage *vectorImage = ((LayerVector *)layer)->getLastVectorImageAtFrame(mEditor->currentFrame(), 0);
            vectorImage->setSelectionTransformation( QTransform() );
        }

        setSelection( mySelection, true );

        selectionTransformation.reset();

        mOffset.setX( 0 );
        mOffset.setY( 0 );

        updateCurrentFrame();
    }
}

void ScribbleArea::setSelection( QRectF rect, bool trueOrFalse )
{

    mySelection = rect;
    myTransformedSelection = rect;
    myTempTransformedSelection = rect;
    somethingSelected = trueOrFalse;


    // Temporary disabled this as it breaks selection rotate key (ctrl) event.
    //
    // displaySelectionProperties();
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
    mOffset.setX( 0 );
    mOffset.setY( 0 );
    Layer* layer = mEditor->layers()->currentLayer();

    Q_ASSERT( layer );
    if ( layer == NULL ) { return; }

    if ( layer->type() == Layer::BITMAP )
    {
        // Only selects the entire screen erea
        //setSelection( mEditor->view()->mapScreenToCanvas( QRectF( -2, -2, width() + 3, height() + 3 ) ), true ); // TO BE IMPROVED

        // Selects the drawn area (bigger or smaller than the screen). It may be more accurate to select all this way
        // as the drawing area is not limited
        //
        BitmapImage *bitmapImage = ( ( LayerBitmap * )layer )->getLastBitmapImageAtFrame( mEditor->currentFrame(), 0 );
        setSelection(bitmapImage->bounds(), true);


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
    mOffset.setX( 0 );
    mOffset.setY( 0 );
    myRotatedAngle = 0;
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
    isTransforming = false;

    mBufferImg->clear();

    //mBitmapSelection.clear();
    vectorSelection.clear();

    // clear all the data tools may have accumulated
    editor()->tools()->cleanupAllToolsData();

    updateCurrentFrame();
}

void ScribbleArea::toggleThinLines()
{
    bool previousValue = mPrefs->isOn(SETTING::INVISIBLE_LINES);
    setEffect( SETTING::INVISIBLE_LINES, !previousValue );
}

void ScribbleArea::toggleOutlines()
{
    mIsSimplified = !mIsSimplified;
    setEffect( SETTING::OUTLINES, mIsSimplified );
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
    // Only switch to remporary tool if not already in this state
    // and temporary tool is not already the current tool.
    //
    if (!instantTool && currentTool()->type() != eToolMode) {

        instantTool = true; // used to return to previous tool when finished (keyRelease).
        mPrevTemporalToolType = currentTool()->type();
        editor()->tools()->setCurrentTool( eToolMode );
    }
}

void ScribbleArea::deleteSelection()
{
    if ( somethingSelected )      // there is something selected
    {
        Layer* layer = mEditor->layers()->currentLayer();
        if ( layer == NULL ) { return; }
        mEditor->backup( tr( "DeleteSel" ) );
        mClosestCurves.clear();
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
        mClosestCurves.clear();
        mClosestVertices.clear();
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

    setModified( mEditor->layers()->currentLayerIndex(), mEditor->currentFrame() );
}

void ScribbleArea::setPrevTool()
{
    editor()->tools()->setCurrentTool( mPrevTemporalToolType );
    instantTool = false;
}

/* Render Canvas */

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
