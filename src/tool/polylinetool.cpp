#include "polylinetool.h"

#include "layer.h"
#include "editor.h"
#include "scribblearea.h"


PolylineTool::PolylineTool(QObject *parent) :
    BaseTool(parent)
{
}

ToolType PolylineTool::type()
{
    return POLYLINE;
}

void PolylineTool::loadSettings()
{
    properties.width = 1;
    properties.feather = -1;
}

QCursor PolylineTool::cursor()
{
    return Qt::CrossCursor;
}

void PolylineTool::mousePressEvent(QMouseEvent *event)
{
    Layer *layer = m_pEditor->getCurrentLayer();
    if (layer == NULL) { return; }

    if (event->button() == Qt::LeftButton)
    {
        if (layer->type == Layer::BITMAP || layer->type == Layer::VECTOR)
        {
            if (m_pScribbleArea->mousePoints.size() == 0)
            {
                m_pEditor->backup(tr("Line"));
            }

            if (layer->type == Layer::VECTOR)
            {
                ((LayerVector *)layer)->getLastVectorImageAtFrame(m_pEditor->m_nCurrentFrameIndex, 0)->deselectAll();
                if (m_pScribbleArea->makeInvisible && !m_pScribbleArea->showThinLines)
                {
                    m_pScribbleArea->toggleThinLines();
                }
            }
            m_pScribbleArea->mousePoints << m_pScribbleArea->lastPoint;
            m_pScribbleArea->updateAll = true;
        }
    }

}

void PolylineTool::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
}

void PolylineTool::mouseMoveEvent(QMouseEvent *event)
{
    Layer *layer = m_pEditor->getCurrentLayer();
    // ---- checks ------
    if (layer == NULL) { return; }

    if (layer->type == Layer::BITMAP || layer->type == Layer::VECTOR)
    {
        m_pScribbleArea->drawPolyline();
    }
}

void PolylineTool::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (BezierCurve::eLength(m_pScribbleArea->lastPixel.toPoint() - event->pos()) < 2.0)
    {
        m_pScribbleArea->endPolyline();
    }
}
