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


EraserTool::EraserTool( QObject *parent ) :
StrokeTool( parent )
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


    QSettings settings( "Pencil", "Pencil" );

    properties.width = settings.value( "eraserWidth" ).toDouble();
    properties.feather = settings.value( "eraserFeather" ).toDouble();

    properties.pressure = settings.value( "eraserPressure" ).toBool();
    properties.invisibility = DISABLED;
    properties.preserveAlpha = OFF;

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
    QSettings settings( "Pencil", "Pencil" );
    settings.setValue("eraserWidth", width);
    settings.sync();
}

void EraserTool::setFeather( const qreal feather )
{
    // Set current property
    properties.feather = feather;

    // Update settings
    QSettings settings( "Pencil", "Pencil" );
    settings.setValue("eraserFeather", feather);
    settings.sync();
}

void EraserTool::setPressure( const bool pressure )
{
    // Set current property
    properties.pressure = pressure;

    // Update settings
    QSettings settings( "Pencil", "Pencil" );
    settings.setValue("eraserPressure", pressure);
    settings.sync();
}

QCursor EraserTool::cursor()
{
    if ( isAdjusting ) // being dynamically resized
    {
        return circleCursors(); // two circles cursor
    }
    if ( pencilSettings()->value( SETTING_TOOL_CURSOR ).toBool() )
    {
        return circleCursors();
    }
    return Qt::CrossCursor;
}

void EraserTool::adjustPressureSensitiveProperties( qreal pressure, bool mouseDevice )
{
    mCurrentWidth = properties.width;
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
    lastBrushPoint = getCurrentPoint();
}

void EraserTool::mouseReleaseEvent( QMouseEvent *event )
{
    Layer* layer = mEditor->layers()->currentLayer();

    if ( event->button() == Qt::LeftButton )
    {
        if ( mScribbleArea->isLayerPaintable() )
        {
            drawStroke();
        }

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

    endStroke();
}

void EraserTool::mouseMoveEvent( QMouseEvent *event )
{
    Layer* layer = mEditor->layers()->currentLayer();

    if ( event->buttons() & Qt::LeftButton )
    {
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

        qreal opacity = 1.0;
        mCurrentWidth = properties.width;
        qreal brushWidth = (mCurrentWidth + (mCurrentPressure * mCurrentWidth)) * 0.5;
        qreal brushStep = (0.5 * brushWidth) - ((properties.feather/100.0) * brushWidth * 0.5);
        brushStep = qMax( 1.0, brushStep );

        BlitRect rect;

        QPointF a = lastBrushPoint;
        QPointF b = getCurrentPoint();

        qreal distance = 4 * QLineF( b, a ).length();
        int steps = qRound( distance ) / brushStep;

        for ( int i = 0; i < steps; i++ )
        {
            QPointF point = lastBrushPoint + ( i + 1 ) * ( brushStep )* ( b - lastBrushPoint ) / distance;
            rect.extend( point.toPoint() );
            mScribbleArea->drawBrush( point,
                                      brushWidth,
                                      properties.feather,
                                      QColor(255, 255, 255, 255),
                                      opacity );

            if ( i == ( steps - 1 ) )
            {
                lastBrushPoint = point;
            }
        }

        int rad = qRound( brushWidth ) / 2 + 2;
        mScribbleArea->refreshBitmap( rect, rad );
    }
    else if ( layer->type() == Layer::VECTOR )
    {
        QPen pen( Qt::white, mCurrentWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin );
        int rad = qRound( ( mCurrentWidth / 2 + 2 ) * mEditor->view()->scaling() );

        if ( p.size() == 4 ) {
            QSizeF size( 2, 2 );
            QPainterPath path( p[ 0 ] );
            path.cubicTo( p[ 1 ],
                          p[ 2 ],
                          p[ 3 ] );
            mScribbleArea->drawPath( path, pen, Qt::NoBrush, QPainter::CompositionMode_Source );
            mScribbleArea->refreshVector( path.boundingRect().toRect(), rad );
        }
    }
}
