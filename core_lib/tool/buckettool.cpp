#include <QPixmap>
#include <QPainter>
#include <QMouseEvent>

#include "layer.h"
#include "layervector.h"
#include "layerbitmap.h"
#include "layermanager.h"
#include "colormanager.h"
#include "strokemanager.h"

#include "pencilsettings.h"
#include "editor.h"
#include "scribblearea.h"

#include "buckettool.h"

BucketTool::BucketTool( QObject *parent ) :
StrokeTool( parent )
{
}


ToolType BucketTool::type()
{
    return BUCKET;
}

void BucketTool::loadSettings()
{
    properties.width = 4;
    properties.feather = 10;
}

QCursor BucketTool::cursor()
{
    if ( pencilSettings()->value( SETTING_TOOL_CURSOR ).toBool() )
    {
        QPixmap pixmap( ":icons/bucketTool.png" );
        QPainter painter( &pixmap );
        painter.setPen( Qt::blue );   // FIXME: need to get current color
        painter.drawLine( QPoint( 5, 16 ), QPoint( 5, 18 ) );
        painter.end();

        return QCursor( pixmap, 4, 20 );
    }
    else
    {
        return Qt::CrossCursor;
    }
}

void BucketTool::mousePressEvent( QMouseEvent *event )
{
    mCurrentWidth = 10;

    if ( event->button() == Qt::LeftButton )
    {
        mEditor->backup( typeName() );
        mScribbleArea->setAllDirty();
    }

    startStroke();
}

void BucketTool::mouseReleaseEvent( QMouseEvent *event )
{
    Layer* layer = mEditor->layers()->currentLayer();
    if ( layer == NULL ) { return; }

    if ( event->button() == Qt::LeftButton )
    {
        if ( layer->type() == Layer::BITMAP )
        {
            BitmapImage *sourceImage = ( ( LayerBitmap * )layer )->getLastBitmapImageAtFrame( mEditor->currentFrame(), 0 );
            Layer *targetLayer = layer; // by default
            int layerNumber = mEditor->layers()->currentLayerIndex(); // by default
            if ( mEditor->layers()->currentLayerIndex() > 0 )
            {
                Layer *layer2 = mEditor->layers()->currentLayer( -1 );
                if ( layer2->type() == Layer::BITMAP )
                {
                    targetLayer = layer2;
                    layerNumber = layerNumber - 1;
                }
            }
            BitmapImage *targetImage = ( ( LayerBitmap * )targetLayer )->getLastBitmapImageAtFrame( mEditor->currentFrame(), 0 );

            BitmapImage::floodFill( sourceImage,
                                    targetImage,
                                    getLastPoint().toPoint(),
                                    qRgba( 0, 0, 0, 0 ),
                                    mEditor->color()->frontColor().rgba(),
                                    10 * 10,
                                    true );

            mScribbleArea->setModified( layerNumber, mEditor->currentFrame() );
            mScribbleArea->setAllDirty();
        }
        else if ( layer->type() == Layer::VECTOR )
        {
            mScribbleArea->clearBitmapBuffer();
            VectorImage *vectorImage = ( ( LayerVector * )layer )->getLastVectorImageAtFrame( mEditor->currentFrame(), 0 );

            if ( event->modifiers() == Qt::AltModifier )
            {
                vectorImage->removeArea( getLastPoint() );
            }
            else
            {
                QList<QPointF> path = mStrokePoints;
                if (path.size() < 10) {
                    vectorImage->fill( getLastPoint(),
                                       mEditor->color()->frontColorNumber(),
                                       mEditor->view()->getView(),
                                       mScribbleArea->size(),
                                       mEditor->view()->scaling(),
                                       3);
                }
                else {
                    vectorImage->fill(path, mEditor->color()->frontColorNumber(), mEditor->view()->scaling(), 10.0);
                }
            }
            mScribbleArea->setModified( mEditor->layers()->currentLayerIndex(), mEditor->currentFrame() );
            mScribbleArea->setAllDirty();
        }
    }
    endStroke();
}

void BucketTool::mouseMoveEvent( QMouseEvent *event )
{
    Layer* layer = mEditor->layers()->currentLayer();
    if ( layer->type() == Layer::BITMAP) {
        Q_UNUSED( event );
    }
    else if (layer->type() == Layer::VECTOR )
    {
        if ( event->buttons() & Qt::LeftButton )
        {
            drawStroke();
            qDebug() << "DrawStroke" << event->pos() ;
        }
    }

    Q_UNUSED( event );
}

void BucketTool::drawStroke()
{
    StrokeTool::drawStroke();
    QList<QPointF> p = m_pStrokeManager->interpolateStroke();

    Layer* layer = mEditor->layers()->currentLayer();

    if ( layer->type() == Layer::BITMAP )
    {
        // No stroke in Bitmap layer
    }
    else if ( layer->type() == Layer::VECTOR )
    {
        int rad = qRound( ( mCurrentWidth / 2 + 2 ) * mEditor->view()->scaling() );

        QColor pathColor = mEditor->color()->frontColor();
        pathColor.setAlpha(50);

        QPen pen( pathColor,
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
