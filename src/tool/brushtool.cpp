
#include <QSettings>
#include <QPixmap>
#include <QPainter>

#include "layer.h"
#include "scribblearea.h"
#include "editor.h"
#include "strokemanager.h"

#include "brushtool.h"
#include "blitrect.h"

BrushTool::BrushTool(QObject *parent) :
    StrokeTool(parent)
{
}

ToolType BrushTool::type()
{
    return BRUSH;
}

void BrushTool::loadSettings()
{
    QSettings settings("pencil", "pencil");

    properties.width = settings.value("brushWidth").toDouble();        
    properties.colourNumber = 1;
    properties.feather = settings.value("brushFeather").toDouble();    
    properties.opacity = 0.5;

    properties.pressure = ON;
    properties.invisibility = DISABLED;
    properties.preserveAlpha = OFF;

    if (properties.feather == 0)
    {
        properties.feather = 70;
        settings.setValue("brushFeather", properties.feather);
    }
    if (properties.width == 0)
    {
        properties.width = 48;
        settings.setValue("brushWidth", properties.width);
    }
}

QCursor BrushTool::cursor()
{
    qreal width = properties.width + 0.5 * properties.feather;
    QPixmap pixmap(width,width);
    if (!pixmap.isNull())
    {
        pixmap.fill( QColor(255,255,255,0) );
        QPainter painter(&pixmap);
        painter.setPen( QColor(0,0,0,190) );
        painter.setBrush( Qt::NoBrush );
        painter.drawLine( QPointF(width/2-2,width/2), QPointF(width/2+2,width/2) );
        painter.drawLine( QPointF(width/2,width/2-2), QPointF(width/2,width/2+2) );
        painter.setRenderHints(QPainter::Antialiasing, true);
        painter.setPen( QColor(0,0,0,100) );
        painter.drawEllipse( QRectF( 
            1 + properties.feather/2, 
            1 + properties.feather/2,
            qMax(0.0, properties.width - properties.feather/2-2),
            qMax(0.0, properties.width - properties.feather/2-2)) );
        painter.setPen( QColor(0,0,0,50) );
        painter.drawEllipse( QRectF(1+properties.feather/8,1+properties.feather/8,qMax(0.0,width-properties.feather/4-2),qMax(0.0,width-properties.feather/4-2)) );
        painter.end();
    }
    return QCursor(pixmap);

}


void BrushTool::adjustPressureSensitiveProperties(qreal pressure, bool mouseDevice)
{
    if (m_pScribbleArea->usePressure && !mouseDevice)
    {
        currentWidth = properties.width * pressure;
    }
    else
    {
        currentWidth = properties.width;
    }
    // we choose the "normal" width to correspond to a pressure 0.5
}

void BrushTool::mousePressEvent(QMouseEvent *event)
{
    Layer *layer = m_pEditor->getCurrentLayer();

    if (layer->type == Layer::VECTOR)
    {
        m_pEditor->selectVectorColourNumber(properties.colourNumber);
    }

    if (event->button() == Qt::LeftButton)
    {
        m_pEditor->backup(typeName());
        m_pScribbleArea->updateAll = true;
    }

    startStroke();
}

void BrushTool::mouseReleaseEvent(QMouseEvent *event)
{
    Layer *layer = m_pEditor->getCurrentLayer();

    if (event->button() == Qt::LeftButton)
    {
        if (m_pScribbleArea->isLayerPaintable())
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
        }
    }
}

void BrushTool::mouseMoveEvent(QMouseEvent *event)
{
    Layer *layer = m_pEditor->getCurrentLayer();

    if (layer->type == Layer::BITMAP || layer->type == Layer::VECTOR)
    {
        if (event->buttons() & Qt::LeftButton)
        {
            drawStroke();
        }
    }
}

void BrushTool::drawStroke()
{
    Layer *layer = m_pEditor->getCurrentLayer();

    if (layer->type == Layer::BITMAP)
    {
//        currentWidth = properties.width;

        QPointF a = m_pScribbleArea->pixelToPoint(lastPixel);
        QPointF b = m_pScribbleArea->pixelToPoint(m_pStrokeManager->getCurrentPixel());

        qreal opacity = 1.0;
        qreal brushWidth = currentWidth +  0.5 * properties.feather;
        qreal offset = qMax(0.0, currentWidth - 0.5 * properties.feather) / brushWidth;
        //            if (tabletInUse) { opacity = tabletPressure; }
        //            if (usePressure) { brushWidth = brushWidth * tabletPressure; }

        qreal brushStep = 0.5 * currentWidth + 0.5 * properties.feather;
        //            if (usePressure) { brushStep = brushStep * tabletPressure; }
        brushStep = qMax(1.0, brushStep);

        qreal distance = 4 * QLineF(b, a).length();
        int steps = qRound(distance) / brushStep + 1;

        QList<QPoint> pixels =  m_pStrokeManager->interpolateStrokeInSteps(steps);

        BlitRect rect;

        foreach (QPointF pixel, pixels) {
            QPointF point = m_pScribbleArea->pixelToPoint(pixel);
            rect.extend(point.toPoint());
            m_pScribbleArea->drawBrush(point, brushWidth, offset, m_pEditor->currentColor, opacity);
        }

        int rad = qRound(brushWidth) / 2 + 2;
        m_pScribbleArea->update(m_pScribbleArea->myTempView.mapRect(rect).normalized().adjusted(-rad, -rad, +rad, +rad));
    }
    else if (layer->type == Layer::VECTOR)
    {
        QPen pen(Qt::gray, 1, Qt::DashLine, Qt::RoundCap, Qt::RoundJoin);
        int rad = qRound((currentWidth / 2 + 2) * qAbs(m_pScribbleArea->myTempView.m11()));

        foreach (QSegment segment, calculateStroke(currentWidth))
        {
            QPointF a = segment.first;
            QPointF b = segment.second;
            m_pScribbleArea->bufferImg->drawLine(a, b, pen, QPainter::CompositionMode_SourceOver, m_pScribbleArea->antialiasing);
            m_pScribbleArea->update(QRect(a.toPoint(), b.toPoint()).normalized().adjusted(-rad, -rad, +rad, +rad));
        }
    }
}
