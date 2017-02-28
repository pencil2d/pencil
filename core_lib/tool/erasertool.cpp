#include <QSettings>
#include <QPixmap>
#include <QPainter>

#include "scribblearea.h"

#include "pencilsettings.h"
#include "strokemanager.h"
#include "layermanager.h"
#include "editor.h"
#include "blitrect.h"
#include "layervector.h"
#include "erasertool.h"


EraserTool::EraserTool( QObject *parent ) 
	: StrokeTool( parent )
{
}

ToolType EraserTool::type()
{
    return ERASER;
}

void EraserTool::loadSettings()
{
    m_enabledProperties[WIDTH] = true;
    m_enabledProperties[FEATHER] = true;
    m_enabledProperties[PRESSURE] = true;
    m_enabledProperties[INTERPOLATION] = true;


    QSettings settings( PENCIL2D, PENCIL2D );

    properties.width = settings.value( "eraserWidth" ).toDouble();
    properties.feather = settings.value( "eraserFeather" ).toDouble();

    properties.pressure = settings.value( "eraserPressure" ).toBool();
    properties.invisibility = DISABLED;
    properties.preserveAlpha = OFF;
    properties.inpolLevel = 0;

    // First run
    if ( properties.width <= 0 )
    {
        setWidth(25);
        setFeather(50);
        setPressure(1);
    }
}

void EraserTool::setWidth(const qreal width)
{
    // Set current property
    properties.width = width;

    // Update settings
    QSettings settings( PENCIL2D, PENCIL2D );
    settings.setValue("eraserWidth", width);
    settings.sync();
}

void EraserTool::setFeather( const qreal feather )
{
    // Set current property
    properties.feather = feather;

    // Update settings
    QSettings settings( PENCIL2D, PENCIL2D );
    settings.setValue("eraserFeather", feather);
    settings.sync();
}

void EraserTool::setPressure( const bool pressure )
{
    // Set current property
    properties.pressure = pressure;

    // Update settings
    QSettings settings( PENCIL2D, PENCIL2D );
    settings.setValue("eraserPressure", pressure);
    settings.sync();
}

void EraserTool::setInpolLevel(const int level)
{
    properties.inpolLevel = level;

    QSettings settings( PENCIL2D, PENCIL2D);
    settings.setValue("lineInpol", level);
    settings.sync();
}


QCursor EraserTool::cursor()
{
    if ( isAdjusting ) // being dynamically resized
    {
        return circleCursors(); // two circles cursor
    }
    if ( mEditor->preference()->isOn( SETTING::DOTTED_CURSOR ) )
    {
        return dottedCursor(); // preview stroke size cursor
    }
    if ( mEditor->preference()->isOn( SETTING::TOOL_CURSOR ) )
    {
        return circleCursors();
    }
    return Qt::CrossCursor;
}

void EraserTool::adjustPressureSensitiveProperties( qreal pressure, bool mouseDevice )
{
    if ( properties.pressure && !mouseDevice )
    {
        mCurrentPressure = pressure;
    }
    else
    {
        mCurrentPressure = 1.0;
    }
}

void EraserTool::mousePressEvent( QMouseEvent *event )
{
    if ( event->button() == Qt::LeftButton )
    {
        mEditor->backup( typeName() );
        mScribbleArea->setAllDirty();
    }

    startStroke();
    mLastBrushPoint = getCurrentPoint();
}

void EraserTool::mouseReleaseEvent( QMouseEvent *event )
{

    if ( event->button() == Qt::LeftButton )
    {
        if ( mScribbleArea->isLayerPaintable() )
        {
            drawStroke();
        }

        removeVectorPaint();
    }

    endStroke();
}

void EraserTool::mouseMoveEvent( QMouseEvent *event )
{

    if ( event->buttons() & Qt::LeftButton )
    {
        if ( mScribbleArea->isLayerPaintable() )
        {
            updateStrokes();
            if (properties.inpolLevel != m_pStrokeManager->getInpolLevel()) {
                m_pStrokeManager->setInpolLevel(properties.inpolLevel);
            }
        }
    }
}

// draw a single paint dab at the given location
void EraserTool::paintAt( QPointF )
{
}

void EraserTool::drawStroke()
{
    StrokeTool::drawStroke();
    QList<QPointF> p = m_pStrokeManager->interpolateStroke();

    Layer* layer = mEditor->layers()->currentLayer();

    if ( layer->type() == Layer::BITMAP )
    {
        for ( int i = 0; i < p.size(); i++ )
        {
            p[ i ] = mEditor->view()->mapScreenToCanvas( p[ i ] );
        }

        qreal opacity = m_pStrokeManager->getPressure();
        mCurrentWidth = properties.width;

        qreal brushWidth = (mCurrentWidth + ( m_pStrokeManager->getPressure() * mCurrentWidth)) * 0.5;
        qreal brushStep = (0.5 * brushWidth) - ((properties.feather/100.0) * brushWidth * 0.5);
        brushStep = qMax( 1.0, brushStep );

        BlitRect rect;

        QPointF a = mLastBrushPoint;
        QPointF b = getCurrentPoint();

        qreal distance = 4 * QLineF( b, a ).length();
        int steps = qRound( distance ) / brushStep;

        for ( int i = 0; i < steps; i++ )
        {
            QPointF point = mLastBrushPoint + ( i + 1 ) * ( brushStep )* ( b - mLastBrushPoint ) / distance;
            rect.extend( point.toPoint() );
            mScribbleArea->drawBrush( point,
                                      brushWidth,
                                      properties.feather,
                                      QColor(255, 255, 255, 255),
                                      opacity );

            if ( i == ( steps - 1 ) )
            {
                mLastBrushPoint = point;
            }
        }

        int rad = qRound( brushWidth ) / 2 + 2;
        mScribbleArea->refreshBitmap( rect, rad );
    }
    else if ( layer->type() == Layer::VECTOR )
    {
        qreal brushWidth = 0;
        if (properties.pressure ) {
            brushWidth = properties.width * m_pStrokeManager->getPressure();
        }
        else {
            brushWidth = properties.width;
        }

        QPen pen( Qt::white, brushWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin );
        int rad = qRound( ( brushWidth / 2 + 2 ) * mEditor->view()->scaling() );

        if ( p.size() == 4 ) {
            QSizeF size( 2, 2 );
            QPainterPath path( p[ 0 ] );
            path.cubicTo( p[ 1 ],
                          p[ 2 ],
                          p[ 3 ] );
            qDebug() << path;
            mScribbleArea->drawPath( path, pen, Qt::NoBrush, QPainter::CompositionMode_Source );
            mScribbleArea->refreshVector( path.boundingRect().toRect(), rad );
        }
    }
}

void EraserTool::removeVectorPaint()
{
    Layer* layer = mEditor->layers()->currentLayer();
    if ( layer->type() == Layer::BITMAP )
    {
        mScribbleArea->paintBitmapBuffer();
        mScribbleArea->setAllDirty();
        mScribbleArea->clearBitmapBuffer();
    }
    else if ( layer->type() == Layer::VECTOR )
    {
        VectorImage *vectorImage = ( ( LayerVector * )layer )->getLastVectorImageAtFrame( mEditor->currentFrame(), 0 );
        // Clear the area containing the last point
        //vectorImage->removeArea(lastPoint);
        // Clear the temporary pixel path
        mScribbleArea->clearBitmapBuffer();
        vectorImage->deleteSelectedPoints();
        //update();
        mScribbleArea->setModified( mEditor->layers()->currentLayerIndex(), mEditor->currentFrame() );
        mScribbleArea->setAllDirty();
    }
}

void EraserTool::updateStrokes()
{
    Layer* layer = mEditor->layers()->currentLayer();
    if ( layer->type() == Layer::BITMAP || layer->type() == Layer::VECTOR )
    {
        drawStroke();
    }

    if ( layer->type() == Layer::VECTOR )
    {
        qreal radius = ( properties.width / 2 ) / mEditor->view()->scaling();
        QList<VertexRef> nearbyVertices = ( ( LayerVector * )layer )->getLastVectorImageAtFrame( mEditor->currentFrame(), 0 )
            ->getVerticesCloseTo( getCurrentPoint(), radius );
        for ( int i = 0; i < nearbyVertices.size(); i++ )
        {
            ( ( LayerVector * )layer )->getLastVectorImageAtFrame( mEditor->currentFrame(), 0 )->setSelected( nearbyVertices.at( i ), true );
        }
        //update();
        mScribbleArea->setAllDirty();
    }
}
