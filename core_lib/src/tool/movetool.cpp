/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2012-2018 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/

#include "movetool.h"

#include <QMouseEvent>

#include "editor.h"
#include "toolmanager.h"
#include "viewmanager.h"
#include "scribblearea.h"
#include "layervector.h"
#include "layermanager.h"
#include "vectorimage.h"


MoveTool::MoveTool(QObject* parent) :
    BaseTool(parent)
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
    properties.useFeather = false;
    properties.stabilizerLevel = -1;
    properties.useAA = -1;
}

QCursor MoveTool::cursor()
{
    return Qt::ArrowCursor;
}

void MoveTool::mousePressEvent(QMouseEvent* event)
{
    Layer* layer = mEditor->layers()->currentLayer();
    if (layer == NULL) { return; }

    if (event->button() == Qt::LeftButton)
    {
        pressOperation(event, layer);
    }
}

void MoveTool::mouseReleaseEvent(QMouseEvent*)
{
    if (!mScribbleArea->somethingSelected)
        return;

    // update selection position
    mScribbleArea->myTransformedSelection = mScribbleArea->myTempTransformedSelection;

    // make sure transform is correct
    mScribbleArea->calculateSelectionTransformation();

    // paint and apply
    paintTransformedSelection();
    applyChanges();

    // set selection again to avoid scaling issues.
    mScribbleArea->setSelection(mScribbleArea->myTransformedSelection, true);
    resetSelectionProperties();
}

void MoveTool::mouseMoveEvent(QMouseEvent* event)
{
    Layer* layer = mEditor->layers()->currentLayer();
    if (layer == NULL) { return; }
    if (!layer->isPaintable()) { return; }

    if (event->buttons() & Qt::LeftButton)   // the user is also pressing the mouse (dragging)
    {
        if (mScribbleArea->somethingSelected)
        {
            QPointF offset = QPointF(mScribbleArea->mOffset.x(),
                                     mScribbleArea->mOffset.y());

            if (event->modifiers() == Qt::ShiftModifier)    // maintain aspect ratio
            {
                offset = maintainAspectRatio(offset.x(), offset.y());
            }

            transformSelection(offset.x(), offset.y());

            mScribbleArea->calculateSelectionTransformation();
            paintTransformedSelection();
        }
        else // there is nothing selected
        {
            mScribbleArea->deselectAll();
            mScribbleArea->mMoveMode = ScribbleArea::NONE;
        }
    }
    else // the user is moving the mouse without pressing it
    {
        if (layer->type() == Layer::VECTOR)
        {
            storeClosestVectorCurve();
        }
        mScribbleArea->update();
    }
}

void MoveTool::pressOperation(QMouseEvent* event, Layer* layer)
{
    if (layer->isPaintable())
    {
        mScribbleArea->setMoveMode(ScribbleArea::MIDDLE); // was MIDDLE

        QRectF selectionRect = mScribbleArea->myTransformedSelection;
        if (!selectionRect.isEmpty())
        {
            // Hack to "carry over" the selected part of the drawing.
            // Commented out for now, since it doesn't work right for
            // vector layers.

//            bool onEmptyFrame = layer->getKeyFrameAt(mEditor->currentFrame()) == nullptr;
//            bool preferCreateNewKey = mEditor->preference()->getInt(SETTING::DRAW_ON_EMPTY_FRAME_ACTION) == CREATE_NEW_KEY;
//            bool preferDuplicate = mEditor->preference()->getInt(SETTING::DRAW_ON_EMPTY_FRAME_ACTION) == DUPLICATE_PREVIOUS_KEY;

//            if(onEmptyFrame)
//            {
//                if(preferCreateNewKey)
//                {
//                    mEditor->copy();
//                    mScribbleArea->deleteSelection();
//                }
//                else if(preferDuplicate)
//                {
//                    mEditor->copy();
//                }
//            }

//            mScribbleArea->handleDrawingOnEmptyFrame();
            mEditor->backup(typeName());

            // Hack to "carry over" the selected part of the drawing.
//            if(onEmptyFrame)
//            {
//                if(preferCreateNewKey || preferDuplicate)
//                {
//                    mEditor->paste();
//                }
//            }
        }

        if (mScribbleArea->somethingSelected) // there is an area selection
        {
            // Below will return true if a corner point was in range of the click.
            bool cornerPointInRange = whichTransformationPoint();

            if (event->modifiers() != Qt::ShiftModifier && !selectionRect.contains(getCurrentPoint())
                && !cornerPointInRange)
                mScribbleArea->deselectAll();

            // calculate new transformation in case click only
            mScribbleArea->calculateSelectionTransformation();

            paintTransformedSelection();
            applyChanges();

            mScribbleArea->setSelection(mScribbleArea->myTransformedSelection, true);
            resetSelectionProperties();
        } else {
            anchorOriginPoint = getLastPoint();
        }

        if (mScribbleArea->getMoveMode() == ScribbleArea::MIDDLE)
        {
            if (event->modifiers() == Qt::ControlModifier) // --- rotation
            {
                mScribbleArea->setMoveMode(ScribbleArea::ROTATION);
            }
            else if (event->modifiers() == Qt::AltModifier) // --- symmetry
            {
                mScribbleArea->setMoveMode(ScribbleArea::SYMMETRY);
            }

            if (layer->type() == Layer::VECTOR)
            {
                actionOnVector(event, layer);
            }
            if (!(mScribbleArea->myTransformedSelection.isEmpty())
                && !(mScribbleArea->myTransformedSelection.contains(getLastPoint())))    // click is outside the transformed selection with the MOVE tool
            {
                applyChanges();
            }
        }
    }
}

void MoveTool::actionOnVector(QMouseEvent* event, Layer* layer)
{
    VectorImage* vectorImage = ((LayerVector*)layer)->getLastVectorImageAtFrame(mEditor->currentFrame(), 0);
    if (mScribbleArea->mClosestCurves.size() > 0) // the user clicks near a curve
    {
        if (!vectorImage->isSelected(mScribbleArea->mClosestCurves))
        {
            if (event->modifiers() != Qt::ShiftModifier)
            {
                applyChanges();
            }
            vectorImage->setSelected(mScribbleArea->mClosestCurves, true);
            mScribbleArea->setSelection(vectorImage->getSelectionRect(), true);
            mScribbleArea->update();
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
                    applyChanges();
                }
                vectorImage->setAreaSelected(areaNumber, true);
                mScribbleArea->setSelection(QRectF(0, 0, 0, 0), true);
                mScribbleArea->update();
            }
        }
    }
}

bool MoveTool::whichTransformationPoint()
{
    QRectF transformPoint = mScribbleArea->myTransformedSelection;

    // Give the user a margin to select a corner point.
    bool cornerInRange = false;
    const double marginInPixels = 12;
    const double scale = mEditor->view()->getView().inverted().m11();
    const double scaledMargin = fabs(marginInPixels * scale);

    if (QLineF(getLastPoint(), transformPoint.topLeft()).length() < scaledMargin)
    {
        mScribbleArea->setMoveMode(ScribbleArea::TOPLEFT);
        anchorOriginPoint = mScribbleArea->mySelection.bottomRight();
        cornerInRange = true;
    }
    if (QLineF(getLastPoint(), transformPoint.topRight()).length() < scaledMargin)
    {
        mScribbleArea->setMoveMode(ScribbleArea::TOPRIGHT);
        anchorOriginPoint = mScribbleArea->mySelection.bottomLeft();
        cornerInRange = true;
    }
    if (QLineF(getLastPoint(), transformPoint.bottomLeft()).length() < scaledMargin)
    {
        mScribbleArea->setMoveMode(ScribbleArea::BOTTOMLEFT);
        anchorOriginPoint = mScribbleArea->mySelection.topRight();
        cornerInRange = true;
    }
    if (QLineF(getLastPoint(), transformPoint.bottomRight()).length() < scaledMargin)
    {
        mScribbleArea->setMoveMode(ScribbleArea::BOTTOMRIGHT);
        anchorOriginPoint = mScribbleArea->mySelection.topLeft();
        cornerInRange = true;
    }
    return cornerInRange;
}

void MoveTool::transformSelection(qreal offsetX, qreal offsetY)
{
    QRectF& transformedSelection =  mScribbleArea->myTransformedSelection;
    switch (mScribbleArea->mMoveMode)
    {
    case ScribbleArea::MIDDLE:
        if (QLineF(getLastPressPixel(), getCurrentPixel()).length() > 4)
        {
            mScribbleArea->myTempTransformedSelection =
                transformedSelection.translated(QPointF(offsetX, offsetY));
        }
        break;

    case ScribbleArea::TOPRIGHT:
    {
        mScribbleArea->myTempTransformedSelection =
            transformedSelection.adjusted(0, offsetY, offsetX, 0);

        mScribbleArea->manageSelectionOrigin(QPointF(transformedSelection.topRight().x()+offsetX,
                                                     transformedSelection.topRight().y()+offsetY), anchorOriginPoint);
        break;
    }
    case ScribbleArea::TOPLEFT:
    {
        mScribbleArea->myTempTransformedSelection =
            transformedSelection.adjusted(offsetX, offsetY, 0, 0);

        mScribbleArea->manageSelectionOrigin(QPointF(transformedSelection.x()+offsetX,
                                                     transformedSelection.y()+offsetY), anchorOriginPoint);
        break;
    }
    case ScribbleArea::BOTTOMLEFT:
    {
        mScribbleArea->myTempTransformedSelection =
            transformedSelection.adjusted(offsetX, 0, 0, offsetY);

        mScribbleArea->manageSelectionOrigin(QPointF(transformedSelection.bottomLeft().x()+offsetX,
                                                     transformedSelection.bottomLeft().y()+offsetY), anchorOriginPoint);
        break;
    }
    case ScribbleArea::BOTTOMRIGHT:
    {
        mScribbleArea->myTempTransformedSelection =
            transformedSelection.adjusted(0, 0, offsetX, offsetY);

        mScribbleArea->manageSelectionOrigin(QPointF(transformedSelection.bottomRight().x()+offsetX,
                                                     transformedSelection.bottomRight().y()+offsetY), anchorOriginPoint);
        break;

    }
    case ScribbleArea::ROTATION:
    {
        mScribbleArea->myTempTransformedSelection =
            transformedSelection; // @ necessary?
        mScribbleArea->myRotatedAngle = getCurrentPixel().x() - getLastPressPixel().x();
        break;
    }
    default:
        break;
    }
    mScribbleArea->update();

}

QPointF MoveTool::maintainAspectRatio(qreal offsetX, qreal offsetY)
{
    qreal factor = mScribbleArea->mySelection.width() / mScribbleArea->mySelection.height();

    if (mScribbleArea->mMoveMode == ScribbleArea::TOPLEFT || mScribbleArea->mMoveMode == ScribbleArea::BOTTOMRIGHT)
    {
        offsetY = offsetX / factor;
    }
    else if (mScribbleArea->mMoveMode == ScribbleArea::TOPRIGHT || mScribbleArea->mMoveMode == ScribbleArea::BOTTOMLEFT)
    {
        offsetY = -(offsetX / factor);
    }
    else if (mScribbleArea->mMoveMode == ScribbleArea::MIDDLE)
    {
        qreal absX = offsetX;
        if (absX < 0) { absX = -absX; }

        qreal absY = offsetY;
        if (absY < 0) { absY = -absY; }

        if (absX > absY) { offsetY = 0; }
        if (absY > absX) { offsetX = 0; }
    }
    return QPointF(offsetX, offsetY);
}

/**
 * @brief MoveTool::storeClosestVectorCurve
 * stores the curves closest to the mouse position in mClosestCurves
 */
void MoveTool::storeClosestVectorCurve()
{
    Layer* layer = mEditor->layers()->currentLayer();
    auto layerVector = static_cast<LayerVector*>(layer);
    VectorImage* pVecImg = layerVector->getLastVectorImageAtFrame(mEditor->currentFrame(), 0);
    mScribbleArea->mClosestCurves = pVecImg->getCurvesCloseTo(getCurrentPoint(),
                                                              mScribbleArea->selectionTolerance / mEditor->view()->scaling());
}

void MoveTool::cancelChanges()
{
    mScribbleArea->cancelTransformedSelection();
}

void MoveTool::applyChanges()
{
    mScribbleArea->applyTransformedSelection();
}

void MoveTool::paintTransformedSelection()
{
    mScribbleArea->paintTransformedSelection();
}

void MoveTool::leavingThisTool()
{
    // make sure correct transformation is applied
    // before leaving
    mScribbleArea->calculateSelectionTransformation();
    applyChanges();
}

void MoveTool::switchingLayers()
{
    applyChanges();
}

void MoveTool::resetSelectionProperties()
{
    mScribbleArea->resetSelectionProperties();
}
