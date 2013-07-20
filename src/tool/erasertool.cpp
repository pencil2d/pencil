
#include <QSettings>
#include <QPixmap>
#include <QPainter>

#include "editor.h"
#include "scribblearea.h"

#include "erasertool.h"
#include "layer.h"


EraserTool::EraserTool(QObject *parent) :
    StrokeTool(parent)
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
        m_pEditor->backup(typeName());
        m_pScribbleArea->updateAll = true;
    }

    startStroke();
}

void EraserTool::mouseMoveEvent(QMouseEvent *event)
{
    Layer *layer = m_pEditor->getCurrentLayer();
    if (event->buttons() & Qt::LeftButton)
    {
        if (layer->type == Layer::BITMAP || layer->type == Layer::VECTOR)
        {
            drawStroke();
        }

        if (layer->type == Layer::VECTOR)
        {
            qreal radius = (properties.width / 2) / m_pScribbleArea->myTempView.m11();
            QList<VertexRef> nearbyVertices = ((LayerVector *)layer)->getLastVectorImageAtFrame(m_pEditor->m_nCurrentFrameIndex, 0)
                    ->getVerticesCloseTo(m_pScribbleArea->currentPoint, radius);
            for (int i = 0; i < nearbyVertices.size(); i++)
            {
                ((LayerVector *)layer)->getLastVectorImageAtFrame(m_pEditor->m_nCurrentFrameIndex, 0)->setSelected(nearbyVertices.at(i), true);
            }
            //update();
            m_pScribbleArea->updateAll = true;
        }
    }
}

void EraserTool::mouseReleaseEvent(QMouseEvent *event)
{
    Layer *layer = m_pEditor->getCurrentLayer();

    if (event->button() == Qt::LeftButton)
    {
        if (layer->type == Layer::BITMAP || layer->type == Layer::VECTOR)
        {
            drawStroke();
        }

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
        currentWidth =  properties.width;
    }
    else
    {
        currentWidth = properties.width * pressure;
    }

}

void EraserTool::drawStroke()
{
    Layer *layer = m_pEditor->getCurrentLayer();

    if (layer->type == Layer::BITMAP)
    {
        QPen pen2 = QPen(QBrush(QColor(255, 255, 255, 255)), currentWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        int rad = qRound(currentWidth / 2) + 3;


        foreach (QSegment segment, calculateStroke(currentWidth))
        {
            QPointF a = m_pScribbleArea->pixelToPoint(segment.first);
            QPointF b = m_pScribbleArea->pixelToPoint(segment.second);

            m_pScribbleArea->bufferImg->drawLine(a, b, pen2, QPainter::CompositionMode_SourceOver, m_pScribbleArea->antialiasing);
            m_pScribbleArea->update(m_pScribbleArea->myTempView.
                                    mapRect(QRect(a.toPoint(), b.toPoint())
                                            .normalized().adjusted(-rad, -rad, +rad, +rad)));
        }
    }
    else if (layer->type == Layer::VECTOR)
    {
        QPen pen(Qt::white, currentWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        int rad = qRound((currentWidth / 2 + 2) * (qAbs(m_pScribbleArea->myTempView.m11()) + qAbs(m_pScribbleArea->myTempView.m22())));

        foreach (QSegment segment, calculateStroke(currentWidth))
        {
            QPointF a = segment.first;
            QPointF b = segment.second;
            m_pScribbleArea->bufferImg->drawLine(a, b, pen, QPainter::CompositionMode_SourceOver, m_pScribbleArea->antialiasing);
            m_pScribbleArea->update(QRect(a.toPoint(), b.toPoint()).normalized().adjusted(-rad, -rad, +rad, +rad));
        }
    }
}
