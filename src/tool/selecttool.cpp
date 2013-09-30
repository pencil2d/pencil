#include "editor.h"
#include "layer.h"
#include "layervector.h"
#include "scribblearea.h"

#include "selecttool.h"

SelectTool::SelectTool()
{

}

ToolType SelectTool::type()
{
    return SELECT;
}

void SelectTool::loadSettings()
{
    properties.width = -1;
    properties.feather = -1;
}

QCursor SelectTool::cursor()
{
    return Qt::CrossCursor;
}

void SelectTool::mousePressEvent(QMouseEvent *event)
{
    Layer *layer = m_pEditor->getCurrentLayer();
    if (layer == NULL) { return; }

    m_pScribbleArea->myRotatedAngle = 0;

    if (event->button() == Qt::LeftButton)
    {
        if (layer->type == Layer::BITMAP || layer->type == Layer::VECTOR)
        {
            if (layer->type == Layer::VECTOR)
            {
                ((LayerVector *)layer)->getLastVectorImageAtFrame(m_pEditor->m_nCurrentFrameIndex, 0)->deselectAll();
            }
            m_pScribbleArea->setMoveMode(ScribbleArea::MIDDLE);
            m_pEditor->backup(typeName());

            if (m_pScribbleArea->somethingSelected)      // there is something selected
            {
                if (BezierCurve::mLength(getLastPoint() - m_pScribbleArea->myTransformedSelection.topLeft()) < 6)
                {
                    m_pScribbleArea->setMoveMode(ScribbleArea::TOPLEFT);
                }
                if (BezierCurve::mLength(getLastPoint() - m_pScribbleArea->myTransformedSelection.topRight()) < 6)
                {
                    m_pScribbleArea->setMoveMode(ScribbleArea::TOPRIGHT);
                }
                if (BezierCurve::mLength(getLastPoint() - m_pScribbleArea->myTransformedSelection.bottomLeft()) < 6)
                {
                    m_pScribbleArea->setMoveMode(ScribbleArea::BOTTOMLEFT);
                }
                if (BezierCurve::mLength(getLastPoint() - m_pScribbleArea->myTransformedSelection.bottomRight()) < 6)
                {
                    m_pScribbleArea->setMoveMode(ScribbleArea::BOTTOMRIGHT);
                }
                if (m_pScribbleArea->getMoveMode() == ScribbleArea::MIDDLE)
                {
                    m_pScribbleArea->paintTransformedSelection();
                    m_pScribbleArea->deselectAll();
                } // the user did not click on one of the corners
            }
            else     // there is nothing selected
            {
                m_pScribbleArea->mySelection.setTopLeft(getLastPoint());
                m_pScribbleArea->mySelection.setBottomRight(getLastPoint());
                m_pScribbleArea->setSelection(m_pScribbleArea->mySelection, true);
            }
            m_pScribbleArea->update();
        }
    }

}

void SelectTool::mouseReleaseEvent(QMouseEvent *event)
{
    Layer *layer = m_pEditor->getCurrentLayer();
    if (layer == NULL) { return; }

    if (event->button() == Qt::LeftButton)
    {
        if (layer->type == Layer::VECTOR)
        {
            if (m_pScribbleArea->somethingSelected)
            {
                m_pScribbleArea->switchTool(MOVE);
                VectorImage *vectorImage = ((LayerVector *)layer)->getLastVectorImageAtFrame(m_pEditor->m_nCurrentFrameIndex, 0);
                m_pScribbleArea->setSelection(vectorImage->getSelectionRect(), true);
                if (m_pScribbleArea->mySelection.size() == QSizeF(0, 0))
                {
                    m_pScribbleArea->somethingSelected = false;
                }
            }
            m_pScribbleArea->updateFrame();
            m_pScribbleArea->setAllDirty();
        }
        else if (layer->type == Layer::BITMAP)
        {
            m_pScribbleArea->updateFrame();
            m_pScribbleArea->setAllDirty();
        }
    }

}

void SelectTool::mouseMoveEvent(QMouseEvent *event)
{
    Layer *layer = m_pEditor->getCurrentLayer();
    if (layer == NULL) { return; }

    if ((event->buttons() & Qt::LeftButton) && m_pScribbleArea->somethingSelected && (layer->type == Layer::BITMAP || layer->type == Layer::VECTOR))
    {
        switch (m_pScribbleArea->getMoveMode())
        {
        case ScribbleArea::MIDDLE:
            m_pScribbleArea->mySelection.setBottomRight(getCurrentPoint());
            break;

        case ScribbleArea::TOPLEFT:
            m_pScribbleArea->mySelection.setTopLeft(getCurrentPoint());
            break;

        case ScribbleArea::TOPRIGHT:
            m_pScribbleArea->mySelection.setTopRight(getCurrentPoint());
            break;

        case ScribbleArea::BOTTOMLEFT:
            m_pScribbleArea->mySelection.setBottomLeft(getCurrentPoint());
            break;

        case ScribbleArea::BOTTOMRIGHT:
            m_pScribbleArea->mySelection.setBottomRight(getCurrentPoint());
            break;
        }

        m_pScribbleArea->myTransformedSelection = m_pScribbleArea->mySelection.adjusted(0, 0, 0, 0);
        m_pScribbleArea->myTempTransformedSelection = m_pScribbleArea->mySelection.adjusted(0, 0, 0, 0);

        if (layer->type == Layer::VECTOR)
        {
            ((LayerVector *)layer)->getLastVectorImageAtFrame(m_pEditor->m_nCurrentFrameIndex, 0)->select(m_pScribbleArea->mySelection);
        }
        m_pScribbleArea->update();
    }

}
