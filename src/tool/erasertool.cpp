
#include <QSettings>
#include <QPixmap>
#include <QPainter>

#include "editor.h"
#include "scribblearea.h"

#include "erasertool.h"
#include "layer.h"


EraserTool::EraserTool(QObject *parent) :
    BaseTool(parent)
{
}

ToolType EraserTool::type()
{
    return ERASER;
}

void EraserTool::loadSettings()
{
    QSettings settings("pencil", "pencil");
    properties.width = settings.value("eraserWidth").toDouble();    
    properties.feather = 0;
    properties.pressure = 1;
    properties.invisibility = -1;
    properties.preserveAlpha = 0;

    if (properties.width == 0)
    {
        properties.width = 24;
        settings.setValue("eraserWidth", properties.width);
    }
}

QCursor EraserTool::cursor()
{
    QPixmap pixmap( properties.width, properties.width );
    pixmap.fill( QColor(255,255,255,0) );

    QPainter painter(&pixmap);
    painter.setPen( QColor(0,0,0,190) );
    painter.setBrush( QColor(255,255,255,100) );
    painter.drawLine( QPointF(properties.width/2 - 2, properties.width/2), QPointF(properties.width/2+2, properties.width/2) );
    painter.drawLine( QPointF(properties.width/2, properties.width/2-2), QPointF(properties.width/2, properties.width/2+2) );
    painter.setRenderHints(QPainter::Antialiasing, true);
    painter.setPen( QColor(0,0,0,100) );
    painter.drawEllipse( QRectF(1, 1, properties.width - 2, properties.width - 2) );
    painter.end();

    return QCursor(pixmap);
}

void EraserTool::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        m_pEditor->backup(typeName(type()));
        m_pScribbleArea->mousePath.append(m_pScribbleArea->lastPoint);
        m_pScribbleArea->updateAll = true;
    }
}

void EraserTool::mouseMoveEvent(QMouseEvent *event)
{
    Layer *layer = m_pEditor->getCurrentLayer();
    if (layer->type == Layer::BITMAP || layer->type == Layer::VECTOR)
    {
        if (event->buttons() & Qt::LeftButton)
        {
            m_pScribbleArea->drawLineTo(m_pScribbleArea->currentPixel, m_pScribbleArea->currentPoint);
        }
    }
}

void EraserTool::mouseReleaseEvent(QMouseEvent *event)
{
    Layer *layer = m_pEditor->getCurrentLayer();
    if (event->button() == Qt::LeftButton)
    {
        if (layer->type == Layer::BITMAP)
        {
            m_pScribbleArea->paintBitmapBuffer();
            m_pScribbleArea->updateAll = true;
        }
        else if (layer->type == Layer::VECTOR)
        {
            VectorImage *vectorImage = ((LayerVector *)layer)->getLastVectorImageAtFrame(m_pEditor->m_nCurrentFrameIndex, 0);
            // Clear the area containing the last point
            //vectorImage->removeArea(lastPoint);
            // Clear the temporary pixel path
            m_pScribbleArea->bufferImg->clear();
            vectorImage->deleteSelectedPoints();
            //update();
            m_pScribbleArea->setModified(m_pEditor->m_nCurrentLayerIndex, m_pEditor->m_nCurrentFrameIndex);
            m_pScribbleArea->updateAll = true;
        }
    }
}

void EraserTool::adjustPressureSensitiveProperties(qreal pressure, bool mouseDevice)
{
    //myPenWidth = static_cast<int>(10.0*tabletPressure);
    if (mouseDevice)
    {
        m_pScribbleArea->currentWidth =  properties.width;
    }
    else
    {
        m_pScribbleArea->currentWidth = properties.width * pressure;
    }

}
