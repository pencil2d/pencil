
#include <QSettings>
#include <QPixmap>
#include <QPainter>

#include "layer.h"
#include "scribblearea.h"
#include "editor.h"

#include "brushtool.h"

BrushTool::BrushTool(QObject *parent) :
    BaseTool(parent)
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

void BrushTool::mousePressEvent(QMouseEvent *event)
{
    Layer *layer = m_pEditor->getCurrentLayer();
    // ---- checks ------
    if (layer == NULL) { return; }

    if (layer->type == Layer::VECTOR)
    {
        VectorImage *vectorImage = ((LayerVector *)layer)->getLastVectorImageAtFrame(m_pEditor->m_nCurrentFrameIndex, 0);
        if (vectorImage == NULL) { return; }
        m_pEditor->selectVectorColourNumber(properties.colourNumber);
    }

    if (event->button() == Qt::LeftButton)
    {
        m_pEditor->backup(typeName());
        m_pScribbleArea->mousePath.append(m_pScribbleArea->lastPoint);
        m_pScribbleArea->updateAll = true;
    }

}

void BrushTool::mouseReleaseEvent(QMouseEvent *event)
{
    Layer *layer = m_pEditor->getCurrentLayer();
    // ---- checks ------
    if (layer == NULL) { return; }

    if (event->button() == Qt::LeftButton)
    {
        if (layer->type == Layer::BITMAP)
        {
            m_pScribbleArea->paintBitmapBuffer();
            m_pScribbleArea->updateAll = true;
        }
        else if (layer->type == Layer::VECTOR)
        {
            // Clear the temporary pixel path
            m_pScribbleArea->bufferImg->clear();
            ((LayerVector *)layer)->getLastVectorImageAtFrame(m_pEditor->m_nCurrentFrameIndex, 0)->colour(m_pScribbleArea->mousePath, properties.colourNumber);
            m_pScribbleArea->setModified(m_pEditor->m_nCurrentLayerIndex, m_pEditor->m_nCurrentFrameIndex);
            m_pScribbleArea->updateAll = true;
        }
    }
}

void BrushTool::mouseMoveEvent(QMouseEvent *event)
{
    Layer *layer = m_pEditor->getCurrentLayer();
    // ---- checks ------
    if (layer == NULL) { return; }

    if (layer->type == Layer::BITMAP || layer->type == Layer::VECTOR)
    {
        if (event->buttons() & Qt::LeftButton)
        {
            m_pScribbleArea->drawLineTo(m_pScribbleArea->currentPixel, m_pScribbleArea->currentPoint);
        }
    }

}
