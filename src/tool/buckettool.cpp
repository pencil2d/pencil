#include <QPixmap>
#include <QPainter>

#include "layer.h"
#include "layervector.h"
#include "layerbitmap.h"
#include "colormanager.h"

#include "pencilsettings.h"
#include "editor.h"
#include "scribblearea.h"

#include "buckettool.h"

BucketTool::BucketTool(QObject *parent) :
    BaseTool(parent)
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
        QPixmap pixmap(":icons/bucketTool.png");
        QPainter painter(&pixmap);
        painter.setPen( Qt::blue );   // FIXME: need to get current color
        painter.drawLine( QPoint(5, 16), QPoint(5, 18) );
        painter.end();

        return QCursor(pixmap, 4, 20);
    }
    else
    {
        return Qt::CrossCursor;
    }
}

void BucketTool::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        m_pEditor->backup(typeName());
        m_pScribbleArea->setAllDirty();
    }
}

void BucketTool::mouseReleaseEvent(QMouseEvent *event)
{
    Layer *layer = m_pEditor->getCurrentLayer();
    if (layer == NULL) { return; }

    if (event->button() == Qt::LeftButton)
    {
        if (layer->type() == Layer::BITMAP)
        {
            BitmapImage *sourceImage = ((LayerBitmap *)layer)->getLastBitmapImageAtFrame(m_pEditor->m_nCurrentFrameIndex, 0);
            Layer *targetLayer = layer; // by default
            int layerNumber = m_pEditor->m_nCurrentLayerIndex; // by default
            if (m_pEditor->m_nCurrentLayerIndex > 0)
            {
                Layer *layer2 = m_pEditor->getCurrentLayer(-1);
                if (layer2->type() == Layer::BITMAP)
                {
                    targetLayer = layer2;
                    layerNumber = layerNumber - 1;
                }
            }
            BitmapImage *targetImage = ((LayerBitmap *)targetLayer)->getLastBitmapImageAtFrame(m_pEditor->m_nCurrentFrameIndex, 0);

            BitmapImage::floodFill(sourceImage,
                                   targetImage,
                                   getLastPoint().toPoint(),
                                   qRgba(0, 0, 0, 0),
                                   m_pEditor->colorManager()->frontColor().rgba(),
                                   10 * 10,
                                   true);

            m_pScribbleArea->setModified(layerNumber, m_pEditor->m_nCurrentFrameIndex);
            m_pScribbleArea->setAllDirty();
        }
        else if (layer->type() == Layer::VECTOR)
        {
            VectorImage *vectorImage = ((LayerVector *)layer)->getLastVectorImageAtFrame(m_pEditor->m_nCurrentFrameIndex, 0);

            if (event->modifiers() == Qt::AltModifier)
            {
                vectorImage->removeArea(getLastPoint());
            }
            else
            {
                m_pScribbleArea->floodFill(vectorImage, getLastPixel().toPoint(), qRgba(0, 0, 0, 0), qRgb(200, 200, 200), 100 * 100);
            }
            m_pScribbleArea->setModified(m_pEditor->m_nCurrentLayerIndex, m_pEditor->m_nCurrentFrameIndex);
            m_pScribbleArea->setAllDirty();
        }
    }

}

void BucketTool::mouseMoveEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
}
