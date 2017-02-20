#include <QSettings>
#include <QPixmap>
#include <QMouseEvent>

#include "layermanager.h"
#include "layervector.h"
#include "layerbitmap.h"
#include "colormanager.h"
#include "strokemanager.h"
#include "layermanager.h"
#include "editor.h"
#include "scribblearea.h"
#include "pencilsettings.h"
#include "blitrect.h"

#include "penciltool.h"

PencilTool::PencilTool( QObject* parent ) : StrokeTool( parent )
{
}


void PencilTool::loadSettings()
{
    m_enabledProperties[WIDTH] = true;
    m_enabledProperties[PRESSURE] = true;
    m_enabledProperties[VECTORMERGE] = true;

    QSettings settings( PENCIL2D, PENCIL2D );
    properties.width = settings.value( "pencilWidth" ).toDouble();
    properties.feather = 1;
    properties.pressure = settings.value( "pencilPressure" ).toBool();
    properties.invisibility = 1;
    properties.preserveAlpha = 0;

    if ( properties.width <= 0 )
    {
        // setting the default value to 4
        // seems to give great results with pressure on
        setWidth( 4 );
        setPressure( 1 );
    }

}

void PencilTool::setWidth(const qreal width)
{
    // Set current property
    properties.width = width;

    // Update settings
    QSettings settings( PENCIL2D, PENCIL2D );
    settings.setValue("pencilWidth", width);
    settings.sync();
}

void PencilTool::setFeather( const qreal feather )
{
    properties.feather = feather;
}

void PencilTool::setInvisibility( const bool )
{
    // force value
    properties.invisibility = 1;
}

void PencilTool::setPressure( const bool pressure )
{
    // Set current property
    properties.pressure = pressure;

    // Update settings
    QSettings settings( PENCIL2D, PENCIL2D );
    settings.setValue("pencilPressure", pressure);
    settings.sync();
}

void PencilTool::setPreserveAlpha( const bool preserveAlpha )
{
    // force value
    properties.preserveAlpha = 0;
}


QCursor PencilTool::cursor()
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
        return QCursor( QPixmap( ":icons/pencil2.png" ), 0, 16 );
    }
    return Qt::CrossCursor;
}

void PencilTool::mousePressEvent( QMouseEvent *event )
{
    mLastBrushPoint = getCurrentPoint();

    if ( event->button() == Qt::LeftButton )
    {
        mEditor->backup( typeName() );

        mScribbleArea->setAllDirty();
        startStroke(); //start and appends first stroke

        //Layer *layer = m_pEditor->getCurrentLayer();

        if ( mEditor->layers()->currentLayer()->type() == Layer::BITMAP ) // in case of bitmap, first pixel(mouseDown) is drawn
        {
            drawStroke();
        }
        else if ( mEditor->layers()->currentLayer()->type() == Layer::VECTOR )
        {
            if ( !mEditor->preference()->isOn(SETTING::INVISIBLE_LINES) )
            {
                mScribbleArea->toggleThinLines();
            }
        }
    }

    mMouseDownPoint = getCurrentPoint();
    mLastBrushPoint = getCurrentPoint();
}

void PencilTool::mouseMoveEvent( QMouseEvent *event )
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

void PencilTool::mouseReleaseEvent( QMouseEvent *event )
{
    Layer* layer = mEditor->layers()->currentLayer();

    if ( event->button() == Qt::LeftButton )
    {
        if ( layer->type() == Layer::BITMAP || layer->type() == Layer::VECTOR )
        {
            if (getCurrentPoint()==mMouseDownPoint)
            {
                paintAt(mMouseDownPoint);
            }
            else
            {
                drawStroke();
            }
        }

        if ( layer->type() == Layer::BITMAP )
        {
            mScribbleArea->paintBitmapBuffer();
            mScribbleArea->setAllDirty();
            mScribbleArea->clearBitmapBuffer();
        }
        else if ( layer->type() == Layer::VECTOR &&  mStrokePoints.size() > -1 )
        {
            // Clear the temporary pixel path
            mScribbleArea->clearBitmapBuffer();
            qreal tol = mScribbleArea->getCurveSmoothing() / mEditor->view()->scaling();
            qDebug() << "pressures " << mStrokePressures;
            BezierCurve curve( mStrokePoints, mStrokePressures, tol );

            curve.setWidth( 0 );
            curve.setFeather( 0 );
            curve.setInvisibility( true );
            curve.setVariableWidth( false );
            curve.setColourNumber( mEditor->color()->frontColorNumber() );
            VectorImage* vectorImage = ( ( LayerVector * )layer )->getLastVectorImageAtFrame( mEditor->currentFrame(), 0 );

            vectorImage->addCurve( curve, qAbs( mEditor->view()->scaling() ), properties.vectorMergeEnabled );
            mScribbleArea->setModified( mEditor->layers()->currentLayerIndex(), mEditor->currentFrame() );
            mScribbleArea->setAllDirty();
        }
    }

    endStroke();
}

void PencilTool::adjustPressureSensitiveProperties( qreal pressure, bool mouseDevice )
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

// draw a single paint dab at the given location
void PencilTool::paintAt( QPointF point )
{
    qDebug() << "Made a single dab at " << point;
    Layer* layer = mEditor->layers()->currentLayer();
    if ( layer->type() == Layer::BITMAP )
    {
        qreal opacity = 1.0f;
        if (properties.pressure == true)
        {
            opacity = mCurrentPressure / 2;
        }
        mCurrentWidth = properties.width;
        qreal brushWidth = mCurrentWidth;

        BlitRect rect;

        rect.extend( point.toPoint() );
        mScribbleArea->drawPencil(point,
                                  brushWidth,
                                  mEditor->color()->frontColor(),
                                  opacity);

        int rad = qRound( brushWidth ) / 2 + 2;
        mScribbleArea->refreshBitmap( rect, rad );
    }
}


void PencilTool::drawStroke()
{
    StrokeTool::drawStroke();
    QList<QPointF> p = m_pStrokeManager->interpolateStroke();

    Layer* layer = mEditor->layers()->currentLayer();

    if ( layer->type() == Layer::BITMAP )
    {
        qreal opacity = 1.0f;
        if (properties.pressure == true)
        {
            opacity = mCurrentPressure / 2;
        }

        mCurrentWidth = properties.width * mCurrentPressure;
        qreal brushWidth = mCurrentWidth;

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
            mScribbleArea->drawPencil(point,
                                      brushWidth,
                                      mEditor->color()->frontColor(),
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
        QPen pen( mEditor->color()->frontColor(),
                  1,
                  Qt::DotLine,
                  Qt::RoundCap,
                  Qt::RoundJoin );

        int rad = qRound( ( properties.width / 2 + 2 ) * mEditor->view()->scaling() );

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
