#include "polylinetool.h"

#include "layer.h"
#include "editor.h"
#include "scribblearea.h"

#include "strokemanager.h"


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

void PolylineTool::clear()
{
    points.clear();
}

void PolylineTool::mousePressEvent(QMouseEvent *event)
{
    Layer *layer = m_pEditor->getCurrentLayer();

    if (event->button() == Qt::LeftButton)
    {
        if (layer->type == Layer::BITMAP || layer->type == Layer::VECTOR)
        {
            if (points.size() == 0)
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
            points << m_pScribbleArea->pixelToPoint(m_pStrokeManager->getCurrentPixel());
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

    if (layer->type == Layer::BITMAP || layer->type == Layer::VECTOR)
    {
        m_pScribbleArea->drawPolyline(points, m_pScribbleArea->pixelToPoint(m_pStrokeManager->getCurrentPixel()));
    }
}

void PolylineTool::mouseDoubleClickEvent(QMouseEvent *event)
{
    // XXX highres position ??
    if (BezierCurve::eLength(m_pStrokeManager->getLastPressPixel() - event->pos()) < 2.0)
    {
        m_pScribbleArea->endPolyline(points);
        clear();
    }
}

bool PolylineTool::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Return:
        if (points.size() > 0 )
        {
            m_pScribbleArea->endPolyline(points);
            clear();
            return true;
        }
        break;

    case Qt::Key_Escape:
        if (points.size() > 0) {
            clear();
            return true;
        }
        break;

    default:
        return false;
    }

    return false;
}
