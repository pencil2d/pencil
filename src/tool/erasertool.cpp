
#include <QSettings>
#include <QPixmap>
#include <QPainter>

#include "editor.h"
#include "scribblearea.h"
#include "strokemanager.h"

#include "erasertool.h"
#include "layer.h"
#include "layervector.h"


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
    QSettings settings("Pencil", "Pencil");
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
        m_pScribbleArea->setAllDirty();
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
            qreal radius = (properties.width / 2) / m_pScribbleArea->getTempViewScaleX();
            QList<VertexRef> nearbyVertices = ((LayerVector *)layer)->getLastVectorImageAtFrame(m_pEditor->m_nCurrentFrameIndex, 0)
                    ->getVerticesCloseTo(getCurrentPoint(), radius);
            for (int i = 0; i < nearbyVertices.size(); i++)
            {
                ((LayerVector *)layer)->getLastVectorImageAtFrame(m_pEditor->m_nCurrentFrameIndex, 0)->setSelected(nearbyVertices.at(i), true);
            }
            //update();
            m_pScribbleArea->setAllDirty();
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
            m_pScribbleArea->setAllDirty();
        }
        else if (layer->type == Layer::VECTOR)
        {
            VectorImage *vectorImage = ((LayerVector *)layer)->getLastVectorImageAtFrame(m_pEditor->m_nCurrentFrameIndex, 0);
            // Clear the area containing the last point
            //vectorImage->removeArea(lastPoint);
            // Clear the temporary pixel path
            m_pScribbleArea->clearBitmapBuffer();
            vectorImage->deleteSelectedPoints();
            //update();
            m_pScribbleArea->setModified(m_pEditor->m_nCurrentLayerIndex, m_pEditor->m_nCurrentFrameIndex);
            m_pScribbleArea->setAllDirty();
        }
    }
}

void EraserTool::adjustPressureSensitiveProperties(qreal pressure, bool mouseDevice)
{
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
    StrokeTool::drawStroke();
    QList<QPointF> p = m_pStrokeManager->interpolateStroke(currentWidth);

    Layer *layer = m_pEditor->getCurrentLayer();

    if (layer->type == Layer::BITMAP)
    {
        QPen pen = QPen(QBrush(QColor(255, 255, 255, 255)), currentWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        int rad = qRound(currentWidth / 2) + 3;

        for (int i = 0; i < p.size(); i++) {
            p[i] = m_pScribbleArea->pixelToPoint(p[i]);
        }

        if (p.size() == 4) {
            QSizeF size(2,2);
            QPainterPath path(p[0]);
            path.cubicTo(p[1],
                    p[2],
                    p[3]);
            m_pScribbleArea->drawPath(path, pen, Qt::NoBrush, QPainter::CompositionMode_Source);
            m_pScribbleArea->refreshBitmap(path.boundingRect().toRect(), rad);
        }
    }
    else if (layer->type == Layer::VECTOR)
    {
        QPen pen(Qt::white, currentWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        int rad = qRound((currentWidth / 2 + 2) * (qAbs(m_pScribbleArea->getTempViewScaleX()) + qAbs(m_pScribbleArea->getTempViewScaleY())));

        if (p.size() == 4) {
            QSizeF size(2,2);
            QPainterPath path(p[0]);
            path.cubicTo(p[1],
                    p[2],
                    p[3]);
            m_pScribbleArea->drawPath(path, pen, Qt::NoBrush, QPainter::CompositionMode_Source);
            m_pScribbleArea->refreshVector(path.boundingRect().toRect(), rad);
        }
    }
}
