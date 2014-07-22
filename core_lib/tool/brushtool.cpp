
#include "brushtool.h"
#include <QSettings>
#include <QPixmap>
#include <QPainter>
#include "layer.h"
#include "editor.h"
#include "pencilsettings.h"
#include "colormanager.h"
#include "strokemanager.h"
#include "layermanager.h"
#include "scribblearea.h"
#include "blitrect.h"


BrushTool::BrushTool( QObject *parent ) :
StrokeTool( parent )
{
}

ToolType BrushTool::type()
{
    return BRUSH;
}

void BrushTool::loadSettings()
{
    QSettings settings( "Pencil", "Pencil" );

    properties.width = settings.value( "brushWidth" ).toDouble();
    properties.feather = settings.value( "brushFeather" ).toDouble();

    properties.pressure = ON;
    properties.invisibility = DISABLED;
    properties.preserveAlpha = OFF;

    if ( properties.width <= 0 )
    {
        properties.width = 15;
        settings.setValue( "brushWidth", properties.width );
    }
    if ( properties.feather <= 0 )
    {
        properties.feather = 200;
        settings.setValue( "brushFeather", properties.feather );
    }
}

QCursor BrushTool::cursor()
{
    if ( isAdjusting ) // being dynamically resized
    {
        return circleCursors(); // two circles cursor
    }
    if ( pencilSettings()->value( SETTING_TOOL_CURSOR ).toBool() ) // doesn't need else
    {
        return QCursor( QPixmap( ":icons/brush.png" ), 0, 13 );
    }
    return Qt::CrossCursor;
}

void BrushTool::adjustPressureSensitiveProperties( qreal pressure, bool mouseDevice )
{
    mCurrentWidth = properties.width;
    if ( mScribbleArea->usePressure() && !mouseDevice )
    {
        mCurrentPressure = pressure;
    }
    else
    {
        mCurrentPressure = 1.0;
    }
}

void BrushTool::mousePressEvent( QMouseEvent *event )
{
    if ( event->button() == Qt::LeftButton )
    {
        mEditor->backup( typeName() );
        mScribbleArea->setAllDirty();
    }

    startStroke();
    lastBrushPoint = getCurrentPoint();
}

void BrushTool::mouseReleaseEvent( QMouseEvent *event )
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
        }
        else if ( layer->type() == Layer::VECTOR )
        {
        }
    }

    endStroke();
}

void BrushTool::mouseMoveEvent( QMouseEvent *event )
{
    Layer* layer = mEditor->layers()->currentLayer();

    if ( layer->type() == Layer::BITMAP || layer->type() == Layer::VECTOR )
    {
        if ( event->buttons() & Qt::LeftButton )
        {
            drawStroke();
        }
    }
}

// draw a single paint dab at the given location
void BrushTool::paintAt( QPointF )
{
}

void BrushTool::drawStroke()
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
        qreal brushWidth = mCurrentWidth + 0.5 * properties.feather;
        qreal offset = qMax( 0.0, mCurrentWidth - 0.5 * properties.feather ) / brushWidth;
        opacity = mCurrentPressure;
        brushWidth = brushWidth * mCurrentPressure;

        qreal brushStep = 0.5 * mCurrentWidth + 0.5 * properties.feather;
        brushStep = brushStep * mCurrentPressure;

        //        if (usePressure) { brushStep = brushStep * tabletPressure; }
        brushStep = qMax( 1.0, brushStep );

        mCurrentWidth = properties.width;
        BlitRect rect;

        QRadialGradient radialGrad( QPointF( 0, 0 ), 0.5 * brushWidth );
        mScribbleArea->setGaussianGradient( radialGrad,
                                            mEditor->color()->frontColor(),
                                            opacity,
                                            offset );

        QPointF a = lastBrushPoint;
        QPointF b = getCurrentPoint();

        //        foreach (QSegment segment, calculateStroke(brushWidth))
        //        {
        //            QPointF a = lastBrushPoint;
        //            QPointF b = m_pScribbleArea->pixelToPoint(segment.second);

        qreal distance = 4 * QLineF( b, a ).length();
        int steps = qRound( distance ) / brushStep;

        for ( int i = 0; i < steps; i++ )
        {
            QPointF point = lastBrushPoint + ( i + 1 ) * ( brushStep )* ( b - lastBrushPoint ) / distance;
            rect.extend( point.toPoint() );
            mScribbleArea->drawBrush( point,
                                      brushWidth,
                                      offset,
                                      mEditor->color()->frontColor(),
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
        QPen pen( Qt::gray, 1, Qt::DashLine, Qt::RoundCap, Qt::RoundJoin );
        int rad = qRound( ( mCurrentWidth / 2 + 2 ) * qAbs( mEditor->view()->scaling() ) );

        //        foreach (QSegment segment, calculateStroke(currentWidth))
        //        {
        //            QPointF a = segment.first;
        //            QPointF b = segment.second;
        //            m_pScribbleArea->drawLine(a, b, pen, QPainter::CompositionMode_SourceOver);
        //            m_pScribbleArea->refreshVector(QRect(a.toPoint(), b.toPoint()), rad);
        //        }
        if ( p.size() == 4 )
        {
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