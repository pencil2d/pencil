#include <QPixmap>
#include <QPainter>
#include <QMouseEvent>

#include "layer.h"
#include "layervector.h"
#include "layerbitmap.h"
#include "layermanager.h"
#include "colormanager.h"

#include "pencilsettings.h"
#include "editor.h"
#include "scribblearea.h"

#include "buckettool.h"

BucketTool::BucketTool( QObject *parent ) :
BaseTool( parent )
{
}


ToolType BucketTool::type()
{
    return BUCKET;
}

void BucketTool::loadSettings()
{
    properties.width = -1;
    properties.feather = -1;
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
    if ( event->button() == Qt::LeftButton )
    {
        mEditor->backup( typeName() );
        mScribbleArea->setAllDirty();
    }
}

void BucketTool::mouseReleaseEvent( QMouseEvent *event )
{
    Layer* layer = mEditor->layers()->currentLayer();
    if ( layer == NULL ) { return; }

    if ( event->button() == Qt::LeftButton )
    {
        if ( layer->type() == Layer::BITMAP )
        {
            BitmapImage *sourceImage = ( ( LayerBitmap * )layer )->getLastBitmapImageAtFrame( mEditor->layers()->currentFramePosition(), 0 );
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
            BitmapImage *targetImage = ( ( LayerBitmap * )targetLayer )->getLastBitmapImageAtFrame( mEditor->layers()->currentFramePosition(), 0 );

            BitmapImage::floodFill( sourceImage,
                                    targetImage,
                                    getLastPoint().toPoint(),
                                    qRgba( 0, 0, 0, 0 ),
                                    mEditor->color()->frontColor().rgba(),
                                    10 * 10,
                                    true );

            mScribbleArea->setModified( layerNumber, mEditor->layers()->currentFramePosition() );
            mScribbleArea->setAllDirty();
        }
        else if ( layer->type() == Layer::VECTOR )
        {
            VectorImage *vectorImage = ( ( LayerVector * )layer )->getLastVectorImageAtFrame( mEditor->layers()->currentFramePosition(), 0 );

            if ( event->modifiers() == Qt::AltModifier )
            {
                vectorImage->removeArea( getLastPoint() );
            }
            else
            {
                mScribbleArea->floodFill( vectorImage, getLastPixel().toPoint(), qRgba( 0, 0, 0, 0 ), qRgb( 200, 200, 200 ), 100 * 100 );
            }
            mScribbleArea->setModified( mEditor->layers()->currentLayerIndex(), mEditor->layers()->currentFramePosition() );
            mScribbleArea->setAllDirty();
        }
    }
}

void BucketTool::mouseMoveEvent( QMouseEvent *event )
{
    Q_UNUSED( event );
}