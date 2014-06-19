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

#include "penciltool.h"

PencilTool::PencilTool( QObject *parent ) :
StrokeTool( parent )
{
}

void PencilTool::loadSettings()
{
    QSettings settings( "Pencil", "Pencil" );

    properties.width = settings.value( "pencilWidth" ).toDouble();
    properties.feather = settings.value( "pencilFeather" ).toDouble();
    properties.pressure = 1;
    properties.invisibility = 1;
    properties.preserveAlpha = 0;

    if ( properties.width <= 0 )
    {
        properties.width = 1;
        settings.setValue( "pencilWidth", properties.width );
    }
    if ( properties.feather > -1 ) // replace with: <=0 to allow feather
    {
        properties.feather = -1;
        settings.setValue( "pencilFeather", properties.feather );
    }
}

QCursor PencilTool::cursor()
{
    if ( isAdjusting ) // being dynamically resized
    {
        return circleCursors(); // two circles cursor
    }

    if ( pencilSettings()->value( SETTING_TOOL_CURSOR ).toBool() )
    {
        return QCursor( QPixmap( ":icons/pencil2.png" ), 0, 16 );
    }
    return Qt::CrossCursor;
}

void PencilTool::mousePressEvent( QMouseEvent *event )
{
    if ( event->button() == Qt::LeftButton )
    {
        mEditor->backup( typeName() );

        if ( !mScribbleArea->showThinLines() )
        {
            mScribbleArea->toggleThinLines();
        }
        mScribbleArea->setAllDirty();
        startStroke(); //start and appends first stroke

        //Layer *layer = m_pEditor->getCurrentLayer();

        if ( mEditor->layers()->currentLayer()->type() == Layer::BITMAP ) // in case of bitmap, first pixel(mouseDown) is drawn
        {
            drawStroke();
        }
    }
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
            drawStroke();
        }

        if ( layer->type() == Layer::BITMAP )
        {
            mScribbleArea->paintBitmapBuffer();
            mScribbleArea->setAllDirty();
        }
        else if ( layer->type() == Layer::VECTOR &&  mStrokePoints.size() > -1 )
        {
            // Clear the temporary pixel path
            mScribbleArea->clearBitmapBuffer();
            qreal tol = mScribbleArea->getCurveSmoothing() / qAbs( mScribbleArea->getViewScaleX() );
            qDebug() << "pressures " << mStrokePressures;
            BezierCurve curve( mStrokePoints, mStrokePressures, tol );

            curve.setWidth( 0 );
            curve.setFeather( 0 );
            curve.setInvisibility( true );
            curve.setVariableWidth( false );
            curve.setColourNumber( mEditor->color()->frontColorNumber() );
            VectorImage* vectorImage = ( ( LayerVector * )layer )->getLastVectorImageAtFrame( mEditor->currentFrame(), 0 );

            vectorImage->addCurve( curve, qAbs( mScribbleArea->getViewScaleX() ) );
            mScribbleArea->setModified( mEditor->layers()->currentLayerIndex(), mEditor->currentFrame() );
            mScribbleArea->setAllDirty();
        }
    }

    endStroke();
}

void PencilTool::adjustPressureSensitiveProperties( qreal pressure, bool mouseDevice )
{
    QColor currentColor = mEditor->color()->frontColor();
    currentPressuredColor = currentColor;
    if ( mScribbleArea->usePressure() && !mouseDevice )
    {
        currentPressuredColor.setAlphaF( currentColor.alphaF() * pressure );
    }
    else
    {
        currentPressuredColor.setAlphaF( currentColor.alphaF() );
    }

    mCurrentWidth = properties.width;
}

void PencilTool::drawStroke()
{
    StrokeTool::drawStroke();
    QList<QPointF> p = m_pStrokeManager->interpolateStroke();

    Layer* layer = mEditor->layers()->currentLayer();
    int rad;

    if ( layer->type() == Layer::BITMAP )
    {
        QPen pen( QBrush( currentPressuredColor ), properties.width, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin );
        QBrush brush( currentPressuredColor, Qt::SolidPattern );
        rad = qRound( properties.width / 2 ) + 3;

        for ( int i = 0; i < p.size(); i++ ) {
            p[ i ] = mScribbleArea->pixelToPoint( p[ i ] );
        }

        if ( p.size() == 4 ) {
            // qDebug() << p;
            QPainterPath path( p[ 0 ] );
            path.cubicTo( p[ 1 ],
                          p[ 2 ],
                          p[ 3 ] );
            //m_pScribbleArea->drawPath(path, pen, brush, QPainter::CompositionMode_SoftLight );
            mScribbleArea->drawPath( path, pen, brush, QPainter::CompositionMode_SourceOver );

            if ( false ) // debug
            {
                QSizeF size( 2, 2 );
                QRectF rect( p[ 0 ], size );

                QPen penBlue( Qt::blue );

                mScribbleArea->mBufferImg->drawRect( rect, Qt::NoPen, QBrush( Qt::red ), QPainter::CompositionMode_Source, false );
                mScribbleArea->mBufferImg->drawRect( QRectF( p[ 3 ], size ), Qt::NoPen, QBrush( Qt::red ), QPainter::CompositionMode_Source, false );
                mScribbleArea->mBufferImg->drawRect( QRectF( p[ 1 ], size ), Qt::NoPen, QBrush( Qt::green ), QPainter::CompositionMode_Source, false );
                mScribbleArea->mBufferImg->drawRect( QRectF( p[ 2 ], size ), Qt::NoPen, QBrush( Qt::green ), QPainter::CompositionMode_Source, false );
                mScribbleArea->mBufferImg->drawLine( p[ 0 ], p[ 1 ], penBlue, QPainter::CompositionMode_Source, true );
                mScribbleArea->mBufferImg->drawLine( p[ 2 ], p[ 3 ], penBlue, QPainter::CompositionMode_Source, true );
                mScribbleArea->refreshBitmap( QRectF( p[ 0 ], p[ 3 ] ).toRect(), 20 );
                mScribbleArea->refreshBitmap( rect.toRect(), rad );
            }

            mScribbleArea->refreshBitmap( path.boundingRect().toRect(), rad );
        }
    }
    else if ( layer->type() == Layer::VECTOR )
    {
        QPen pen( mEditor->color()->frontColor(),
                  1,
                  Qt::DotLine,
                  Qt::RoundCap,
                  Qt::RoundJoin );

        rad = qRound( ( properties.width / 2 + 2 ) * qAbs( mScribbleArea->getTempViewScaleX() ) );

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