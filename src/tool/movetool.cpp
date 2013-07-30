#include "editor.h"
#include "layer.h"
#include "scribblearea.h"

#include "movetool.h"

MoveTool::MoveTool()
{

}

ToolType MoveTool::type()
{
    return MOVE;
}

void MoveTool::loadSettings()
{
    properties.width = -1;
    properties.feather = -1;
}

QCursor MoveTool::cursor()
{
    return Qt::ArrowCursor;
}

void MoveTool::mousePressEvent(QMouseEvent *event)
{
    Layer *layer = m_pEditor->getCurrentLayer();
    if (layer == NULL) { return; }

    if (event->button() == Qt::LeftButton)
    {
        // ----------------------------------------------------------------------
        if ((layer->type == Layer::BITMAP || layer->type == Layer::VECTOR))
        {
            m_pEditor->backup(tr("Move"));
            m_pScribbleArea->setMoveMode(ScribbleArea::MIDDLE);

            if (m_pScribbleArea->somethingSelected)      // there is an area selection
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
            }

            if (m_pScribbleArea->getMoveMode() == ScribbleArea::MIDDLE)
            {
                if (layer->type == Layer::BITMAP)
                {
                    if (!(m_pScribbleArea->myTransformedSelection.contains(getLastPoint())))    // click is outside the transformed selection with the MOVE tool
                    {
                        m_pScribbleArea->paintTransformedSelection();
                        m_pScribbleArea->deselectAll();
                    }
                }
                if (layer->type == Layer::VECTOR)
                {
                    VectorImage *vectorImage = ((LayerVector *)layer)->getLastVectorImageAtFrame(m_pEditor->m_nCurrentFrameIndex, 0);
                    if (m_pScribbleArea->closestCurves.size() > 0)     // the user clicks near a curve
                    {
                        //  editor->backup();
                        if (!vectorImage->isSelected(m_pScribbleArea->closestCurves))
                        {
                            m_pScribbleArea->paintTransformedSelection();
                            if (event->modifiers() != Qt::ShiftModifier)
                            {
                                m_pScribbleArea->deselectAll();
                            }
                            vectorImage->setSelected(m_pScribbleArea->closestCurves, true);
                            m_pScribbleArea->setSelection(vectorImage->getSelectionRect(), true);
                            m_pScribbleArea->update();
                        }
                    }
                    else
                    {
                        int areaNumber = vectorImage->getLastAreaNumber(getLastPoint());
                        if (areaNumber != -1)   // the user clicks on an area
                        {
                            if (!vectorImage->isAreaSelected(areaNumber))
                            {
                                if (event->modifiers() != Qt::ShiftModifier)
                                {
                                    m_pScribbleArea->deselectAll();
                                }
                                vectorImage->setAreaSelected(areaNumber, true);
                                //setSelection( vectorImage->getSelectionRect() );
                                m_pScribbleArea->setSelection(QRectF(0, 0, 0, 0), true);
                                m_pScribbleArea->update();
                            }
                        }
                        else     // the user doesn't click near a curve or an area
                        {
                            if (!(m_pScribbleArea->myTransformedSelection.contains(getLastPoint())))    // click is outside the transformed selection with the MOVE tool
                            {
                                m_pScribbleArea->paintTransformedSelection();
                                m_pScribbleArea->deselectAll();
                            }
                        }
                    }
                }
            }
        }
    }

}

void MoveTool::mouseReleaseEvent(QMouseEvent *event)
{
    Layer *layer = m_pEditor->getCurrentLayer();
    if (layer == NULL) { return; }

    if (event->button() == Qt::LeftButton)
    {
        if (layer->type == Layer::BITMAP || layer->type == Layer::VECTOR)
        {
            m_pScribbleArea->offset.setX(0);
            m_pScribbleArea->offset.setY(0);
            m_pScribbleArea->calculateSelectionTransformation();

            m_pScribbleArea->myTransformedSelection = m_pScribbleArea->myTempTransformedSelection;
            m_pScribbleArea->setModified(m_pEditor->m_nCurrentLayerIndex, m_pEditor->m_nCurrentFrameIndex);
            m_pScribbleArea->setAllDirty();
        }
    }

}

void MoveTool::mouseMoveEvent(QMouseEvent *event)
{
    Layer *layer = m_pEditor->getCurrentLayer();
    if (layer == NULL) { return; }

    if (layer->type == Layer::BITMAP || layer->type == Layer::VECTOR)
    {
        if (event->buttons() & Qt::LeftButton)   // the user is also pressing the mouse (dragging)
        {
            if (m_pScribbleArea->somethingSelected)     // there is something selected
            {
                if (event->modifiers() != Qt::ShiftModifier)    // (and the user doesn't press shift)
                {
                    switch (m_pScribbleArea->m_moveMode)
                    {
                    case ScribbleArea::MIDDLE:
                        if (QLineF(getLastPixel(), getCurrentPixel()).length() > 4)
                        {
                            m_pScribbleArea->myTempTransformedSelection = m_pScribbleArea->myTransformedSelection.translated(m_pScribbleArea->offset);
                        }
                        break;

                    case ScribbleArea::TOPRIGHT:
                        m_pScribbleArea->myTempTransformedSelection =
                                m_pScribbleArea->myTransformedSelection.adjusted(0, m_pScribbleArea->offset.y(), m_pScribbleArea->offset.x(), 0);
                        break;


                    case ScribbleArea::TOPLEFT:
                        m_pScribbleArea->myTempTransformedSelection =
                                m_pScribbleArea->myTransformedSelection.adjusted(m_pScribbleArea->offset.x(), m_pScribbleArea->offset.y(), 0, 0);
                        break;

                        // TOPRIGHT XXX

                    case ScribbleArea::BOTTOMLEFT:
                        m_pScribbleArea->myTempTransformedSelection =
                                m_pScribbleArea->myTransformedSelection.adjusted(m_pScribbleArea->offset.x(), 0, 0, m_pScribbleArea->offset.y());
                        break;

                    case ScribbleArea::BOTTOMRIGHT:
                        m_pScribbleArea->myTempTransformedSelection =
                                m_pScribbleArea->myTransformedSelection.adjusted(0, 0, m_pScribbleArea->offset.x(), m_pScribbleArea->offset.y());
                        break;
                    }

                    m_pScribbleArea->calculateSelectionTransformation();
                    m_pScribbleArea->update();
                    m_pScribbleArea->setAllDirty();
                }
            }
            else     // there is nothing selected
            {
                //selectionTransformation = selectionTransformation.translate(offset.x(), offset.y());
                //if (layer->type == Layer::VECTOR) ((LayerVector*)layer)->getLastVectorImageAtFrame(editor->currentFrame, 0)->setSelectionTransformation(selectionTransformation);

                //VectorImage* vectorImage = ((LayerVector*)layer)->getLastVectorImageAtFrame(editor->currentFrame, 0);
                //setSelection( vectorImage->getSelectionRect() );

                // we switch to the select tool
                m_pScribbleArea->switchTool(SELECT);
                m_pScribbleArea->m_moveMode = ScribbleArea::MIDDLE;
                m_pScribbleArea->mySelection.setTopLeft(getLastPoint());
                m_pScribbleArea->mySelection.setBottomRight(getLastPoint());
                m_pScribbleArea->setSelection(m_pScribbleArea->mySelection, true);
            }
        }
        else     // the user is moving the mouse without pressing it
        {
            if (layer->type == Layer::VECTOR)
            {
                m_pScribbleArea->closestCurves =
                        ((LayerVector *)layer)->getLastVectorImageAtFrame(m_pEditor->m_nCurrentFrameIndex, 0)
                        ->getCurvesCloseTo(getCurrentPoint(), m_pScribbleArea->tol / m_pScribbleArea->getTempViewScaleX());
            }
            m_pScribbleArea->update();
        }
    }

}
