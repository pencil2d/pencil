
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
    m_enabledProperties[WIDTH] = true;
    m_enabledProperties[FEATHER] = true;
    m_enabledProperties[PRESSURE] = true;



    QSettings settings( "Pencil", "Pencil" );

    properties.width = settings.value( "brushWidth" ).toDouble();
    properties.feather = settings.value( "brushFeather" ).toDouble();

    properties.pressure = settings.value( "brushPressure" ).toBool();
    properties.invisibility = DISABLED;
    properties.preserveAlpha = OFF;

    // First run
    //
    if ( properties.width <= 0 )
    {
        setWidth(15);
        setFeather(15);
        setPressure(1);
    }
}

void BrushTool::setWidth(const qreal width)
{
    // Set current property
    properties.width = width;

    // Update settings
    QSettings settings( "Pencil", "Pencil" );
    settings.setValue("brushWidth", width);
    settings.sync();
}

void BrushTool::setFeather( const qreal feather )
{
    // Set current property
    properties.feather = feather;

    // Update settings
    QSettings settings( "Pencil", "Pencil" );
    settings.setValue("brushFeather", feather);
    settings.sync();
}

void BrushTool::setPressure( const bool pressure )
{
    // Set current property
    properties.pressure = pressure;

    // Update settings
    QSettings settings( "Pencil", "Pencil" );
    settings.setValue("brushPressure", pressure);
    settings.sync();
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
    if ( properties.pressure && !mouseDevice )
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
        qreal brushWidth = mCurrentWidth;
        qreal brushStep = (0.5 * mCurrentWidth);
        brushStep = qMax( 1.0, brushStep );

        mCurrentWidth = properties.width;
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
