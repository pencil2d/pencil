#include <QPainter>
#include <QPixmap>
#include <QBitmap>
#include "eyedroppertool.h"

#include "editor.h"
#include "layer.h"
#include "scribblearea.h"


EyedropperTool::EyedropperTool(QObject *parent) :
    BaseTool(parent)
{
}

ToolType EyedropperTool::type()
{
    return EYEDROPPER;
}

void EyedropperTool::loadSettings()
{
    properties.width = -1;
    properties.feather = -1;
}

QCursor EyedropperTool::cursor()
{
    QPixmap pixmap(32, 32);
    pixmap.fill(Qt::white);
    
    QPainter painter(&pixmap);
    painter.drawLine(5,0,5,10);
    painter.drawLine(0,5,10,5);
    painter.end();

    QPixmap mask(32,32);
    mask.fill(Qt::color0);
    
    painter.begin(&mask);
    painter.setBrush(Qt::color1);
    painter.setPen(Qt::color1);
    painter.drawLine(5,0,5,10);
    painter.drawLine(0,5,10,5);
//    painter.drawRect(10,10,20,20);
    painter.end();
    pixmap.setMask(QBitmap(mask));

    return QCursor(pixmap, 5, 5);
}

QCursor EyedropperTool::cursor(const QColor colour)
{
    QPixmap pixmap(32, 32);
    pixmap.fill(Qt::white);

    QPainter painter(&pixmap);
    painter.drawLine(5,0,5,10);
    painter.drawLine(0,5,10,5);

    painter.setPen(QPen(Qt::black, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter.setBrush(colour);
    painter.drawRect(10, 10, 20, 20);

    painter.end();

    QPixmap mask(32,32);
    mask.fill(Qt::color0);

    painter.begin(&mask);
    painter.setBrush(Qt::color1);
    painter.setPen(Qt::color1);
    painter.drawLine(5,0,5,10);
    painter.drawLine(0,5,10,5);
    painter.drawRect(10,10,20,20);
    painter.end();
    pixmap.setMask(QBitmap(mask));

    return QCursor(pixmap, 5, 5);
}


void EyedropperTool::mousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
}

void EyedropperTool::mouseReleaseEvent(QMouseEvent *event)
{
    Layer *layer = m_pEditor->getCurrentLayer();
    if (layer == NULL) { return; }

    if (event->button() == Qt::LeftButton)
    {
        if (layer->type == Layer::BITMAP)
        {
            BitmapImage *targetImage = ((LayerBitmap *)layer)->getLastBitmapImageAtFrame(m_pEditor->m_nCurrentFrameIndex, 0);
            QColor pickedColour = targetImage->pixel(getLastPoint().x(), getLastPoint().y());
            if (pickedColour.alpha() != 0)
            {
                m_pEditor->setBitmapColour(pickedColour);
            }
        }
        else if (layer->type == Layer::VECTOR)
        {
            VectorImage *vectorImage = ((LayerVector *)layer)->getLastVectorImageAtFrame(m_pEditor->m_nCurrentFrameIndex, 0);
            int colourNumber = vectorImage->getColourNumber(getLastPoint());
            if (colourNumber != -1)
            {
                m_pEditor->selectVectorColourNumber(colourNumber);
            }
        }
    }
}

void EyedropperTool::mouseMoveEvent(QMouseEvent *event)
{
    Q_UNUSED(event);

    Layer *layer = m_pEditor->getCurrentLayer();
    if (layer == NULL) { return; }

    if (layer->type == Layer::BITMAP)
    {
        BitmapImage *targetImage = ((LayerBitmap *)layer)->getLastBitmapImageAtFrame(m_pEditor->m_nCurrentFrameIndex, 0);
        if (targetImage->contains(getCurrentPoint()))
        {
            QColor pickedColour;
            pickedColour.setRgba(targetImage->pixel(getCurrentPoint().x(), getCurrentPoint().y()));
            if (pickedColour.alpha() != 0)
            {
                m_pScribbleArea->setCursor(cursor(pickedColour));
            } else {
                m_pScribbleArea->setCursor(cursor());
            }
        } else {
            m_pScribbleArea->setCursor(cursor());
        }
    }
    if (layer->type == Layer::VECTOR)
    {
        VectorImage *vectorImage = ((LayerVector *)layer)->getLastVectorImageAtFrame(m_pEditor->m_nCurrentFrameIndex, 0);
        int colourNumber = vectorImage->getColourNumber(getCurrentPoint());
        if (colourNumber != -1)
        {
            m_pScribbleArea->setCursor(cursor(m_pEditor->object->getColour(colourNumber).colour));
        } else {
            m_pScribbleArea->setCursor(cursor());
        }
    }
}
