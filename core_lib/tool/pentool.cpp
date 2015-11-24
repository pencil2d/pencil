#include <QPixmap>

#include "layervector.h"
#include "colormanager.h"
#include "strokemanager.h"
#include "layermanager.h"
#include "pencilsettings.h"
#include "editor.h"
#include "scribblearea.h"

#include "pentool.h"


PenTool::PenTool( QObject *parent ) : StrokeTool( parent )
{
}

void PenTool::loadSettings()
{
    m_enabledProperties[WIDTH] = true;
    m_enabledProperties[PRESSURE] = true;

    QSettings settings( PENCIL2D, PENCIL2D );

    properties.width = settings.value( "penWidth" ).toDouble();
    properties.feather = 0;
    properties.pressure = settings.value( "penPressure" ).toBool();
    properties.invisibility = OFF;
    properties.preserveAlpha = OFF;

    // First run
    if ( properties.width <= 0 )
    {
        setWidth(1.5);
        setPressure(1);
    }

    mCurrentWidth = properties.width;
}

void PenTool::setWidth(const qreal width)
{
    // Set current property
    properties.width = width;

    // Update settings
    QSettings settings( "Pencil", "Pencil" );
    settings.setValue("penWidth", width);
    settings.sync();
}

void PenTool::setFeather( const qreal feather )
{
    // Set current property
    properties.feather = feather;

    // Update settings
    QSettings settings( "Pencil", "Pencil" );
    settings.setValue("penFeather", feather);
    settings.sync();
}

void PenTool::setPressure( const bool pressure )
{
    // Set current property
    properties.pressure = pressure;

    // Update settings
    QSettings settings( "Pencil", "Pencil" );
    settings.setValue("penPressure", pressure);
    settings.sync();
}

QCursor PenTool::cursor()
{
    if ( isAdjusting ) // being dynamically resized
    {
        return QCursor( circleCursors() ); // two circles cursor
    }
    if ( pencilSettings()->value( SETTING_TOOL_CURSOR ).toBool() )
    {
        return QCursor( QPixmap( ":icons/pen.png" ), 7, 0 );
    }
    return Qt::CrossCursor;
}

void PenTool::adjustPressureSensitiveProperties( qreal pressure, bool mouseDevice )
{
    if ( properties.pressure && !mouseDevice )
    {
        mCurrentWidth = 2.0 * properties.width * pressure;
    }
    else
    {
        mCurrentWidth = properties.width;
    }
    // we choose the "normal" width to correspond to a pressure 0.5
}

void PenTool::mousePressEvent( QMouseEvent *event )
{
    if ( event->button() == Qt::LeftButton )
    {
        mEditor->backup( typeName() );
        mScribbleArea->setAllDirty();
    }

    startStroke();
}

void PenTool::mouseReleaseEvent( QMouseEvent *event )
{
    Layer* layer = mEditor->layers()->currentLayer();

    if ( event->button() == Qt::LeftButton )
    {
        if ( isLayerPaintable( layer ) )
        {
            drawStroke();
        }

        if ( layer->type() == Layer::BITMAP )
        {
            mScribbleArea->paintBitmapBuffer();
            mScribbleArea->setAllDirty();
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
            vectorImage->addCurve( curve, mEditor->view()->scaling() );

            mScribbleArea->setModified( mEditor->layers()->currentLayerIndex(), mEditor->currentFrame() );
            mScribbleArea->setAllDirty();
        }
    }

    endStroke();
}

void PenTool::mouseMoveEvent( QMouseEvent *event )
{
    Layer* layer = mEditor->layers()->currentLayer();
    if ( layer->type() == Layer::BITMAP || layer->type() == Layer::VECTOR )
    {
        if ( event->buttons() & Qt::LeftButton )
        {
            drawStroke();
			//qDebug() << "DrawStroke" << event->pos() ;
        }
    }
}

void PenTool::drawStroke()
{
    StrokeTool::drawStroke();
    QList<QPointF> p = m_pStrokeManager->interpolateStroke();

    Layer* layer = mEditor->layers()->currentLayer();

    if ( layer->type() == Layer::BITMAP )
    {
        QPen pen = QPen( mEditor->color()->frontColor(), mCurrentWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin );
        int rad = qRound( mCurrentWidth / 2 ) + 3;

        for ( int i = 0; i < p.size(); i++ )
        {
            p[ i ] = mEditor->view()->mapScreenToCanvas( p[ i ] );
        }

        if ( p.size() == 4 )
        {
            QPainterPath path( p[ 0 ] );
            //path.lineTo( p[ 1 ] );
            //path.lineTo( p[ 2 ] );
            path.lineTo( p[ 3 ] );
            qDebug() << p[ 0 ] << p[ 1 ] << p[ 2 ] << p[ 3 ];
            //path.cubicTo( p[1], p[2], p[3] );
            mScribbleArea->drawPath( path, pen, Qt::NoBrush, QPainter::CompositionMode_Source );
            mScribbleArea->refreshBitmap( path.boundingRect().toRect(), rad );
        }
    }
    else if ( layer->type() == Layer::VECTOR )
    {
        int rad = qRound( ( mCurrentWidth / 2 + 2 ) * mEditor->view()->scaling() );

        QPen pen( mEditor->color()->frontColor(),
                  mCurrentWidth * mEditor->view()->scaling(),
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
