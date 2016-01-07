
#include "brushtool.h"

#include <cmath>
#include <QSettings>
#include <QPixmap>
#include <QPainter>

#include "layervector.h"
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

    QSettings settings( PENCIL2D, PENCIL2D );

    properties.width = settings.value( "brushWidth" ).toDouble();
    properties.feather = settings.value( "brushFeather", 15.0 ).toDouble();

    properties.pressure = settings.value( "brushPressure", true ).toBool();
    properties.invisibility = DISABLED;
    properties.preserveAlpha = OFF;

    // First run
    //
    if ( properties.width <= 0 )
    {
        setWidth(15);
    }

    if ( std::isnan( properties.feather ) )
    {
        setFeather( 15 );
    }
}

void BrushTool::setWidth(const qreal width)
{
    // Set current property
    properties.width = width;

    // Update settings
    QSettings settings( PENCIL2D, PENCIL2D );
    settings.setValue("brushWidth", width);
    settings.sync();
}

void BrushTool::setFeather( const qreal feather )
{
    // Set current property
    properties.feather = feather;

    // Update settings
    QSettings settings( PENCIL2D, PENCIL2D );
    settings.setValue("brushFeather", feather);
    settings.sync();
}

void BrushTool::setPressure( const bool pressure )
{
    // Set current property
    properties.pressure = pressure;

    // Update settings
    QSettings settings( PENCIL2D, PENCIL2D );
    settings.setValue("brushPressure", pressure);
    settings.sync();
}

void BrushTool::enteringThisTool(){
    has_started_drawing = false;
}

void BrushTool::leavingThisTool(){
    mouseReleaseEvent( NULL );
}

QCursor BrushTool::cursor()
{
    if ( isAdjusting ) // being dynamically resized
    {
        return circleCursors(); // two circles cursor
    }
    if ( mEditor->preference()->isOn( SETTING::DOTTED_CURSOR ) )
    {
        return dottedCursor(); // preview stroke size cursor
    }
    if ( mEditor->preference()->isOn( SETTING::TOOL_CURSOR ) ) // doesn't need else
    {
        return QCursor( QPixmap( ":icons/brush.png" ), 0, 13 );
    }
    return Qt::CrossCursor;
}

void BrushTool::adjustPressureSensitiveProperties( qreal pressure, bool mouseDevice )
{
//    Layer* layer = mEditor->layers()->currentLayer();

//    // In Bitmap mode, the brush tool pressure only handles opacity while the Pen tool
//    // only handles size. Pencil tool handles both.

//    QColor currentColor = mEditor->color()->frontColor();
//    currentPressuredColor = currentColor;

//    if ( layer->type() == Layer::BITMAP && properties.pressure && !mouseDevice )
//    {
//        currentPressuredColor.setAlphaF( currentColor.alphaF() * pressure * pressure );
//    }

//    mCurrentWidth = properties.width;

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
    if (event == NULL || event->button() == Qt::LeftButton )
    {
        mEditor->backup( typeName() );
        mScribbleArea->setAllDirty();
    }

    lastBrushPoint = getCurrentPoint();
    startStroke();
    has_started_drawing = true;
}



void BrushTool::mouseReleaseEvent( QMouseEvent *event )
{
    /*
     * Because we never use the event for anything
     * besides checking what button was pressed, we
     * can safely make a special case for when the
     * event is NULL. If in the future other
     * properties are used like position data, we
     * should maybe use another parameter for this
     * special case.
     */
    Layer* layer = mEditor->layers()->currentLayer();

    if ( event == NULL || event->button() == Qt::LeftButton )
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
        else if ( layer->type() == Layer::VECTOR && mStrokePoints.size() > -1 )
        {
            // Clear the temporary pixel path
            mScribbleArea->clearBitmapBuffer();
            qreal tol = mScribbleArea->getCurveSmoothing() / mEditor->view()->scaling();
            BezierCurve curve( mStrokePoints, mStrokePressures, tol );
            curve.setWidth( properties.width );
            curve.setFeather( properties.feather );
            curve.setInvisibility( false );
            curve.setVariableWidth( properties.pressure );
            curve.setColourNumber( mEditor->color()->frontColorNumber() );

            auto pLayerVector = static_cast< LayerVector* >( layer );
            VectorImage* vectorImage = pLayerVector->getLastVectorImageAtFrame( mEditor->currentFrame(), 0 );
            vectorImage->insertCurve( 0, curve, mEditor->view()->scaling(), false );

            mScribbleArea->setModified( mEditor->layers()->currentLayerIndex(), mEditor->currentFrame() );
            mScribbleArea->setAllDirty();
        }
    }

    endStroke();
}

void BrushTool::mouseMoveEvent( QMouseEvent *event )
{
    if (!has_started_drawing)
    {
      mousePressEvent( NULL );
    }
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

        qreal opacity = mCurrentPressure / 2;
        mCurrentWidth = properties.width;
        qreal brushWidth = mCurrentWidth;

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
        qreal brushWidth = 2.0 * properties.width * mCurrentPressure;

        int rad = qRound( ( brushWidth / 2 + 2 ) * mEditor->view()->scaling() );

        QPen pen( mEditor->color()->frontColor(),
                  brushWidth * mEditor->view()->scaling(),
                  Qt::SolidLine,
                  Qt::RoundCap,
                  Qt::RoundJoin );

        if ( p.size() == 4 )
        {
            QPainterPath path( p[ 0 ] );
            path.cubicTo( p[ 1 ],
                          p[ 2 ],
                          p[ 3 ] );
            mScribbleArea->drawPath( path, pen, Qt::NoBrush, QPainter::CompositionMode_Source );
            mScribbleArea->refreshVector( path.boundingRect().toRect(), rad );
        }
    }
}
