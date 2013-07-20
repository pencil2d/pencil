#include "editor.h"
#include "layer.h"
#include "scribblearea.h"

#include "edittool.h"

EditTool::EditTool()
{

}

ToolType EditTool::type()
{
    return EDIT;
}

void EditTool::loadSettings()
{
    properties.width = -1;
    properties.feather = -1;
}

void EditTool::mousePressEvent(QMouseEvent *event)
{
    Layer *layer = m_pEditor->getCurrentLayer();
    if (layer == NULL) { return; }

    if (event->button() == Qt::LeftButton)
    {
        if (layer->type == Layer::VECTOR)
        {
            m_pScribbleArea->closestCurves = ((LayerVector *)layer)->getLastVectorImageAtFrame(m_pEditor->m_nCurrentFrameIndex, 0)
                    ->getCurvesCloseTo(m_pScribbleArea->currentPoint, m_pScribbleArea->tol / m_pScribbleArea->myTempView.m11());
            m_pScribbleArea->closestVertices = ((LayerVector *)layer)->getLastVectorImageAtFrame(m_pEditor->m_nCurrentFrameIndex, 0)
                    ->getVerticesCloseTo(m_pScribbleArea->currentPoint, m_pScribbleArea->tol / m_pScribbleArea->myTempView.m11());

            if (m_pScribbleArea->closestVertices.size() > 0 || m_pScribbleArea->closestCurves.size() > 0)      // the user clicks near a vertex or a curve
            {
                //qDebug() << "closestCurves:" << closestCurves << " | closestVertices" << closestVertices;
                m_pEditor->backup(typeName());
                VectorImage *vectorImage = ((LayerVector *)layer)->getLastVectorImageAtFrame(m_pEditor->m_nCurrentFrameIndex, 0);

                if (event->modifiers() != Qt::ShiftModifier && !vectorImage->isSelected(m_pScribbleArea->closestVertices))
                {
                    m_pScribbleArea->paintTransformedSelection();
                    m_pScribbleArea->deselectAll();
                }

                vectorImage->setSelected(m_pScribbleArea->closestVertices, true);
                m_pScribbleArea->vectorSelection.add(m_pScribbleArea->closestCurves);
                m_pScribbleArea->vectorSelection.add(m_pScribbleArea->closestVertices);

                m_pScribbleArea->update();
            }
            else
            {
                m_pScribbleArea->deselectAll();
            }
        }
    }

}

void EditTool::mouseReleaseEvent(QMouseEvent *event)
{
    Layer *layer = m_pEditor->getCurrentLayer();
    if (layer == NULL) { return; }

    if (event->button() == Qt::LeftButton)
    {
        if (layer->type == Layer::VECTOR)
        {
            VectorImage *vectorImage = ((LayerVector *)layer)->getLastVectorImageAtFrame(m_pEditor->m_nCurrentFrameIndex, 0);
            vectorImage->applySelectionTransformation();
            m_pScribbleArea->selectionTransformation.reset();
            for (int k = 0; k < m_pScribbleArea->vectorSelection.curve.size(); k++)
            {
                int curveNumber = m_pScribbleArea->vectorSelection.curve.at(k);
                vectorImage->curve[curveNumber].smoothCurve();
            }
            m_pScribbleArea->setModified(m_pEditor->m_nCurrentLayerIndex, m_pEditor->m_nCurrentFrameIndex);
        }
    }

}

void EditTool::mouseMoveEvent(QMouseEvent *event)
{
    Layer *layer = m_pEditor->getCurrentLayer();
    if (layer == NULL) { return; }

    if (layer->type == Layer::BITMAP || layer->type == Layer::VECTOR)
    {
        if (event->buttons() & Qt::LeftButton)   // the user is also pressing the mouse (dragging) {
        {
            if (layer->type == Layer::VECTOR)
            {
                if (event->modifiers() != Qt::ShiftModifier)    // (and the user doesn't press shift)
                {
                    // transforms the selection
                    m_pScribbleArea->selectionTransformation = QMatrix().translate(m_pScribbleArea->offset.x(), m_pScribbleArea->offset.y());
                    ((LayerVector *)layer)->getLastVectorImageAtFrame(m_pEditor->m_nCurrentFrameIndex, 0)->setSelectionTransformation(m_pScribbleArea->selectionTransformation);
                }
            }
        }
        else     // the user is moving the mouse without pressing it
        {
            if (layer->type == Layer::VECTOR)
            {
                m_pScribbleArea->closestVertices = ((LayerVector *)layer)->getLastVectorImageAtFrame(m_pEditor->m_nCurrentFrameIndex, 0)
                        ->getVerticesCloseTo(m_pScribbleArea->currentPoint, m_pScribbleArea->tol / m_pScribbleArea->myTempView.m11());
            }
        }
        m_pScribbleArea->update();
        m_pScribbleArea->setAllDirty();
    }

}
