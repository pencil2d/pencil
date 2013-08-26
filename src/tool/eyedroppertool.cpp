#include <QPainter>
#include <QPixmap>
#include <QBitmap>

#include "pencilsettings.h"

#include "layer.h"
#include "layervector.h"
#include "layerbitmap.h"
#include "colormanager.h"

#include "editor.h"
#include "scribblearea.h"

#include "eyedroppertool.h"

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
    if ( pencilSettings()->value( SETTING_TOOL_CURSOR ).toBool() )
    {
        return QCursor(QPixmap(":icons/eyedropper.png"), 0, 15);
    } else {
        return Qt::CrossCursor;
    }
}

QCursor EyedropperTool::cursor(const QColor colour)
{
    QPixmap icon(":icons/eyedropper.png");

    QPixmap pixmap(32, 32);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    painter.drawPixmap(0, 0, icon);
    painter.setPen(QPen(Qt::black, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter.setBrush(colour);
    painter.drawRect(16, 16, 15, 15);
    painter.end();

    return QCursor(pixmap, 0, 15);
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
            //QColor pickedColour = targetImage->pixel(getLastPoint().x(), getLastPoint().y());
            QColor pickedColour;
            pickedColour.setRgba(targetImage->pixel(getLastPoint().x(), getLastPoint().y()));
            if (pickedColour.alpha() != 0)
            {
                m_pEditor->colorManager()->pickColor(pickedColour);
            }
        }
        else if (layer->type == Layer::VECTOR)
        {
            VectorImage *vectorImage = ((LayerVector *)layer)->getLastVectorImageAtFrame(m_pEditor->m_nCurrentFrameIndex, 0);
            int colourNumber = vectorImage->getColourNumber(getLastPoint());
            if (colourNumber != -1)
            {
                m_pEditor->colorManager()->pickColorNumber(colourNumber);
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
            }
            else
            {
                m_pScribbleArea->setCursor(cursor());
            }
        }
        else
        {
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
        }
        else
        {
            m_pScribbleArea->setCursor(cursor());
        }
    }
}
