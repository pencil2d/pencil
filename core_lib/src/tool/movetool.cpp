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

#include <cassert>
#include <QMouseEvent>
#include <QMessageBox>

#include "editor.h"

#include "toolmanager.h"
#include "viewmanager.h"
#include "strokemanager.h"
#include "backupmanager.h"
#include "layermanager.h"

#include "scribblearea.h"
#include "layervector.h"
#include "vectorimage.h"

#include "bitmapimage.h"
#include "layerbitmap.h"

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
    MoveMode mode = mScribbleArea->getMoveModeForSelectionAnchor();
    return mScribbleArea->currentTool()->selectMoveCursor(mode, type());
}

void MoveTool::tabletPressEvent(QTabletEvent *event)
{
    mCurrentLayer = currentPaintableLayer();

	if (mScribbleArea->isSomethingSelected()) {
        mEditor->backups()->saveStates();
    }
    beginInteraction(event->modifiers(), mCurrentLayer);
}

void MoveTool::tabletMoveEvent(QTabletEvent *event)
{
    mCurrentLayer = currentPaintableLayer();

    if (m_pStrokeManager->isPenPressed())   // the user is also pressing the mouse (dragging)
    {
        transformSelection(event->modifiers(), mCurrentLayer);
    }
    else
    {
        // the user is hovering the pen over the tablet
        // update cursor to reflect selection corner interaction
        mScribbleArea->updateToolCursor();

        if (mCurrentLayer->type() == Layer::VECTOR)
        {
            storeClosestVectorCurve(mCurrentLayer);
        }
    }
    mScribbleArea->updateCurrentFrame();
}

void MoveTool::tabletReleaseEvent(QTabletEvent*)
{
    if (!mScribbleArea->isSomethingSelected())
        return;

    mRotatedAngle = mScribbleArea->myRotatedAngle;
    updateTransformation();

    mScribbleArea->updateToolCursor();
    mScribbleArea->updateCurrentFrame();

    mScribbleArea->setModified(mEditor->currentLayerIndex(), mEditor->currentFrame());
    mEditor->backups()->transform();
}

void MoveTool::mousePressEvent(QMouseEvent* event)
{

    mCurrentLayer = currentPaintableLayer();
    if (mCurrentLayer == nullptr) return;
    setAnchorToLastPoint();
    
    if (mScribbleArea->isSomethingSelected()) {
        mEditor->backups()->saveStates();
    }
    beginInteraction(event->modifiers(), mCurrentLayer);
}

void MoveTool::mouseReleaseEvent(QMouseEvent*)
{
    if (!mScribbleArea->isSomethingSelected())
        return;

    mRotatedAngle = mScribbleArea->myRotatedAngle;
    updateTransformation();

    mScribbleArea->updateToolCursor();
    mScribbleArea->updateCurrentFrame();

    mScribbleArea->setModified(mEditor->currentLayerIndex(), mEditor->currentFrame());
    mEditor->backups()->transform();
}

void MoveTool::mouseMoveEvent(QMouseEvent* event)
{
    mCurrentLayer = currentPaintableLayer();
    if (mCurrentLayer == nullptr) return;

    if (event->buttons() & Qt::LeftButton)   // the user is also pressing the mouse (dragging)
    {
        transformSelection(event->modifiers(), mCurrentLayer);
    }
    else
    {
        // the user is moving the mouse without pressing it
        // update cursor to reflect selection corner interaction
        mScribbleArea->updateToolCursor();

        if (mCurrentLayer->type() == Layer::VECTOR)
        {
            storeClosestVectorCurve(mCurrentLayer);
        }
    }
    mScribbleArea->updateCurrentFrame();
}

void MoveTool::updateTransformation()
{
    // update the transformed selection
    mScribbleArea->myTransformedSelection = mScribbleArea->myTempTransformedSelection;

    // make sure transform is correct
    mScribbleArea->calculateSelectionTransformation();

    // paint the transformation
    paintTransformedSelection();
}

void MoveTool::transformSelection(Qt::KeyboardModifiers keyMod, Layer* layer)
{
    if (mScribbleArea->isSomethingSelected())
    {
        QPointF offset;
        if (layer->type() == Layer::VECTOR)
        {
            offset = mScribbleArea->getTransformOffset();
        }
        else
        {
            offset = QPointF(mScribbleArea->getTransformOffset().x(),
                         mScribbleArea->getTransformOffset().y()).toPoint();
        }

        // maintain aspect ratio
        if (keyMod == Qt::ShiftModifier)
        {
            offset = maintainAspectRatio(offset.x(), offset.y());
        }

        mScribbleArea->adjustSelection(offset.x(),offset.y(), mRotatedAngle);
        mScribbleArea->calculateSelectionTransformation();
        paintTransformedSelection();

    }
    else // there is nothing selected
    {
        mScribbleArea->setMoveMode(MoveMode::NONE);
    }
}

void MoveTool::beginInteraction(Qt::KeyboardModifiers keyMod, Layer* layer)
{

    mScribbleArea->findMoveModeOfCornerInRange();
    mScribbleArea->myRotatedAngle = mRotatedAngle;

    if (keyMod != Qt::ShiftModifier)
    {
        if (canDeselect())
        {
            mEditor->backups()->saveStates();
            applyTransformation();
            mEditor->deselectAllSelections();
        }
    }

    if (mScribbleArea->getMoveMode() == MoveMode::MIDDLE)
    {
        if (keyMod == Qt::ControlModifier) // --- rotation
        {
            mScribbleArea->setMoveMode(MoveMode::ROTATION);
        }
    }

    if (layer->type() == Layer::VECTOR)
    {
        createVectorSelection(keyMod, layer);
    }
}

bool MoveTool::canDeselect()
{
    if (mScribbleArea->isSomethingSelected())
    {
        return (!mScribbleArea->myTransformedSelection.contains(getCurrentPoint())
            && mScribbleArea->getMoveMode() == MoveMode::NONE);
    }
    return false;
}

/**
 * @brief MoveTool::createVectorSelection
 * In vector the selection rectangle is based on the bounding box of the curves
 * We can therefore create a selection just by clicking near/on a curve
 */
void MoveTool::createVectorSelection(Qt::KeyboardModifiers keyMod, Layer* layer)
{
    assert(layer->type() == Layer::VECTOR);
    LayerVector* vecLayer = static_cast<LayerVector*>(layer);
    VectorImage* vectorImage = vecLayer->getLastVectorImageAtFrame(mEditor->currentFrame(), 0);

    if (mScribbleArea->mClosestCurves.size() > 0) // the user clicks near a curve
    {
        setCurveSelected(vectorImage, keyMod);
    }
    else if (vectorImage->getLastAreaNumber(getLastPoint()) > -1)
    {
        setAreaSelected(vectorImage, keyMod);
    }
    mScribbleArea->update();
}

void MoveTool::setCurveSelected(VectorImage* vectorImage, Qt::KeyboardModifiers keyMod)
{
    if (!vectorImage->isSelected(mScribbleArea->mClosestCurves))
    {
        if (keyMod != Qt::ShiftModifier)
        {
            applyTransformation();
        }
        vectorImage->setSelected(mScribbleArea->mClosestCurves, true);
        mScribbleArea->setSelection(vectorImage->getSelectionRect());
    }
}

void MoveTool::setAreaSelected(VectorImage* vectorImage, Qt::KeyboardModifiers keyMod)
{
    int areaNumber = vectorImage->getLastAreaNumber(getLastPoint());
    if (!vectorImage->isAreaSelected(areaNumber))
    {
        if (keyMod != Qt::ShiftModifier)
        {
            applyTransformation();
        }
        vectorImage->setAreaSelected(areaNumber, true);
        mScribbleArea->setSelection(vectorImage->getSelectionRect());
    }
}

QPointF MoveTool::maintainAspectRatio(qreal offsetX, qreal offsetY)
{
    qreal factor = mScribbleArea->myTransformedSelection.width() / mScribbleArea->myTransformedSelection.height();

    if (mScribbleArea->mMoveMode == MoveMode::TOPLEFT || mScribbleArea->mMoveMode == MoveMode::BOTTOMRIGHT)
    {
        offsetY = offsetX / factor;
    }
    else if (mScribbleArea->mMoveMode == MoveMode::TOPRIGHT || mScribbleArea->mMoveMode == MoveMode::BOTTOMLEFT)
    {
        offsetY = -(offsetX / factor);
    }
    else if (mScribbleArea->mMoveMode == MoveMode::MIDDLE)
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
void MoveTool::storeClosestVectorCurve(Layer* layer)
{
    auto layerVector = static_cast<LayerVector*>(layer);
    VectorImage* pVecImg = layerVector->getLastVectorImageAtFrame(mEditor->currentFrame(), 0);
    mScribbleArea->mClosestCurves = pVecImg->getCurvesCloseTo(getCurrentPoint(),
                                                              mScribbleArea->selectionTolerance / mEditor->view()->scaling());
}

void MoveTool::whichAnchorPoint()
{
    anchorOriginPoint = mScribbleArea->whichAnchorPoint(anchorOriginPoint);
}

void MoveTool::setAnchorToLastPoint()
{
    anchorOriginPoint = getLastPoint();
}

void MoveTool::cancelChanges()
{
    mScribbleArea->cancelTransformedSelection();
//    mScribbleArea->deselectAll();
}

void MoveTool::applySelectionChanges()
{
    mScribbleArea->myRotatedAngle = 0;
    mRotatedAngle = 0;

    mScribbleArea->applySelectionChanges();
}

void MoveTool::applyTransformation()
{
    mScribbleArea->applyTransformedSelection();
}

void MoveTool::paintTransformedSelection()
{
    mScribbleArea->paintTransformedSelection();
}

bool MoveTool::leavingThisTool()
{
    if (mCurrentLayer)
    {
        switch (mCurrentLayer->type())
        {
        case Layer::BITMAP:
        {
            if (transformHasBeenModified()) {
                mEditor->backups()->saveStates();
                mEditor->backups()->transform();
            }
            break;
        }
        case Layer::VECTOR: applyTransformation(); break;
        default: break;
        }
    }
    return true;
}

bool MoveTool::transformHasBeenModified()
{
    return mScribbleArea->transformHasBeenModified();
}

bool MoveTool::switchingLayer()
{
    if (!transformHasBeenModified())
    {
        mScribbleArea->deselectAll();
        return true;
    }

    int returnValue = showTransformWarning();

    if (returnValue == QMessageBox::Yes)
    {
        if (mCurrentLayer->type() == Layer::BITMAP)
        {
            applyTransformation();

        } else if (mCurrentLayer->type() == Layer::VECTOR) {
            applyTransformation();
        }

        mScribbleArea->deselectAll();
        return true;
    }
    else if (returnValue == QMessageBox::No)
    {
        cancelChanges();
        return true;
    }
    else if (returnValue == QMessageBox::Cancel) {
        return false;
    }
    return true;
}

int MoveTool::showTransformWarning()
{
    int returnValue = QMessageBox::warning(nullptr,
                                   tr("Layer switch", "Windows title of layer switch pop-up."),
                                   tr("You are about to switch layer, do you want to apply the transformation?"),
                                   QMessageBox::No | QMessageBox::Cancel | QMessageBox::Yes,
                                   QMessageBox::Yes);
    return returnValue;
}

void MoveTool::resetSelectionProperties()
{
    mScribbleArea->resetSelectionProperties();
}

Layer* MoveTool::currentPaintableLayer()
{
    Layer* layer = mEditor->layers()->currentLayer();
    if (layer == nullptr) 
        return nullptr;
    if (!layer->isPaintable())
        return nullptr;
    return layer;
}
