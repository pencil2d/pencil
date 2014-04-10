
#include <QPixmap>
#include "handtool.h"

#include "layer.h"
#include "editor.h"
#include "scribblearea.h"
#include "layercamera.h"

HandTool::HandTool()
{

}

ToolType HandTool::type()
{
    return HAND;
}

void HandTool::loadSettings()
{
    properties.width = -1;
    properties.feather = -1;
}

QCursor HandTool::cursor()
{
    return QPixmap(":icons/hand.png");
}

void HandTool::mousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
}

void HandTool::mouseReleaseEvent(QMouseEvent *event)
{
    m_pScribbleArea->applyTransformationMatrix();

    //---- stop the hand tool if this was mid button
    if (event->button() == Qt::MidButton)
    {
        //qDebug("Stop Hand Tool");
        m_pScribbleArea->setPrevTool();
    }
}

void HandTool::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() != Qt::NoButton)
    {
        if (event->modifiers() & Qt::ControlModifier || event->modifiers() & Qt::AltModifier || event->buttons() & Qt::RightButton)
        {
            QPoint centralPixel(m_pScribbleArea->width() / 2, m_pScribbleArea->height() / 2);
            if (getLastPressPixel().x() != centralPixel.x())
            {
                qreal scale = 1.0;
                qreal cosine = 1.0;
                qreal sine = 0.0;
                if (event->modifiers() & Qt::AltModifier)    // rotation
                {
                    QPointF V1 = getLastPressPixel() - centralPixel;
                    QPointF V2 = getCurrentPixel() - centralPixel;
                    cosine = (V1.x() * V2.x() + V1.y() * V2.y()) / (BezierCurve::eLength(V1) * BezierCurve::eLength(V2));
                    sine = (-V1.x() * V2.y() + V1.y() * V2.x()) / (BezierCurve::eLength(V1) * BezierCurve::eLength(V2));

                }
                if (event->modifiers() & Qt::ControlModifier || event->buttons() & Qt::RightButton)    // scale
                {
                    scale = exp(0.01 * (getCurrentPixel().y() - getLastPressPixel().y()));
                }
                m_pScribbleArea->setTransformationMatrix(QMatrix(
                                                             scale * cosine, -scale * sine,
                                                             scale * sine,  scale * cosine,
                                                             0.0,
                                                             0.0
                                                             ));
            }
        }
        else     // translation
        {
            m_pScribbleArea->setTransformationMatrix(QMatrix(
                                                         1.0, 0.0, 0.0,
                                                         1.0,
                                                         getCurrentPixel().x() - getLastPressPixel().x(),
                                                         getCurrentPixel().y() - getLastPressPixel().y()));
        }
    }
}

void HandTool::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton)
    {
        m_pScribbleArea->resetView();
    }

}
