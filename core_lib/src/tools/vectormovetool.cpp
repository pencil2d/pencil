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

#include "vectormovetool.h"

#include <cassert>
#include <QMessageBox>

#include "pointerevent.h"
#include "editor.h"
#include "toolmanager.h"
#include "viewmanager.h"
#include "strokemanager.h"
#include "selectionmanager.h"
#include "scribblearea.h"
#include "layervector.h"
#include "layermanager.h"
#include "mathutils.h"
#include "vectorimage.h"


VectorMoveTool::VectorMoveTool(QObject* parent) : BaseTool(parent)
{
}

void VectorMoveTool::loadSettings()
{
    properties.width = -1;
    properties.stabilizerLevel = -1;
    mRotationIncrement = mEditor->preference()->getInt(SETTING::ROTATION_INCREMENT);

    connect(mEditor->preference(), &PreferenceManager::optionChanged, this, &VectorMoveTool::updateSettings);
}

QCursor VectorMoveTool::cursor()
{
    MoveMode mode = mEditor->select()->getMoveModeForSelectionAnchor(getCurrentPoint());
    return mScribbleArea->currentTool()->selectMoveCursor(mode, type());
}

void VectorMoveTool::updateSettings(const SETTING setting)
{
    switch (setting)
    {
    case SETTING::ROTATION_INCREMENT:
    {
        mRotationIncrement = mEditor->preference()->getInt(SETTING::ROTATION_INCREMENT);
        break;
    }
    default:
        break;

    }
}

void VectorMoveTool::pointerPressEvent(PointerEvent* event)
{
    mCurrentLayer = currentPaintableLayer();
    if (mCurrentLayer == nullptr) return;

    mEditor->select()->updatePolygons();

    Q_ASSERT(mEditor->layers()->currentLayer()->type() == Layer::VECTOR);

    setAnchorToLastPoint();
    beginInteraction(event->modifiers(), mCurrentLayer);
}

void VectorMoveTool::pointerMoveEvent(PointerEvent* event)
{
    mCurrentLayer = currentPaintableLayer();
    if (mCurrentLayer == nullptr) return;

    mEditor->select()->updatePolygons();

    if (mScribbleArea->isPointerInUse())   // the user is also pressing the mouse (dragging)
    {
        transformSelection(event->modifiers());
    }
    else
    {
        // the user is moving the mouse without pressing it
        // update cursor to reflect selection corner interaction
        mScribbleArea->updateToolCursor();

        storeClosestVectorCurve(mCurrentLayer);
    }
    mScribbleArea->updateCurrentFrame();
}

void VectorMoveTool::pointerReleaseEvent(PointerEvent*)
{
    auto selectMan = mEditor->select();
    if (!selectMan->somethingSelected())
        return;

    mRotatedAngle = selectMan->myRotation();
    updateTransformation();
    applyTransformation();

    selectMan->updatePolygons();

    mScribbleArea->updateToolCursor();
    mScribbleArea->updateCurrentFrame();
}

void VectorMoveTool::updateTransformation()
{
    auto selectMan = mEditor->select();
    selectMan->updateTransformedSelection();

    // make sure transform is correct
    selectMan->calculateSelectionTransformation();

    // paint the transformation
    paintTransformedSelection();
}

void VectorMoveTool::transformSelection(Qt::KeyboardModifiers keyMod)
{
    auto selectMan = mEditor->select();
    if (selectMan->somethingSelected())
    {

        QPointF offset = offsetFromPressPos();

        // maintain aspect ratio
        if (keyMod == Qt::ShiftModifier)
        {
            offset = selectMan->offsetFromAspectRatio(offset.x(), offset.y());
        }

        int rotationIncrement = 0;
        if (selectMan->getMoveMode() == MoveMode::ROTATION && keyMod & Qt::ShiftModifier)
        {
            rotationIncrement = mRotationIncrement;
        }

        selectMan->adjustSelection(getCurrentPoint(), offset.x(), offset.y(), mRotatedAngle, rotationIncrement);

        selectMan->calculateSelectionTransformation();
        paintTransformedSelection();

    }
    else // there is nothing selected
    {
        selectMan->setMoveMode(MoveMode::NONE);
    }
}

void VectorMoveTool::beginInteraction(Qt::KeyboardModifiers keyMod, Layer* layer)
{
    auto selectMan = mEditor->select();
    QRectF selectionRect = selectMan->myTransformedSelectionRect();
    if (!selectionRect.isNull())
    {
        mEditor->backup(typeName());
    }

    if (keyMod != Qt::ShiftModifier)
    {
        if (selectMan->isOutsideSelectionArea(getCurrentPoint()))
        {
            applyTransformation();
            mEditor->deselectAll();
        }
    }

    if (selectMan->validateMoveMode(getLastPoint()) == MoveMode::MIDDLE)
    {
        if (keyMod == Qt::ControlModifier) // --- rotation
        {
            selectMan->setMoveMode(MoveMode::ROTATION);
        }
    }

    createVectorSelection(keyMod, layer);

    if(selectMan->getMoveMode() == MoveMode::ROTATION) {
        QPointF curPoint = getCurrentPoint();
        QPointF anchorPoint = selectionRect.center();
        mRotatedAngle = MathUtils::radToDeg(MathUtils::getDifferenceAngle(anchorPoint, curPoint)) - selectMan->myRotation();
    }
}

/**
 * @brief VectorMoveTool::createVectorSelection
 * In vector the selection rectangle is based on the bounding box of the curves
 * We can therefore create a selection just by clicking near/on a curve
 */
void VectorMoveTool::createVectorSelection(Qt::KeyboardModifiers keyMod, Layer* layer)
{
    LayerVector* vecLayer = static_cast<LayerVector*>(layer);
    VectorImage* vectorImage = vecLayer->getLastVectorImageAtFrame(mEditor->currentFrame(), 0);

    if (!mEditor->select()->closestCurves().empty()) // the user clicks near a curve
    {
        setCurveSelected(vectorImage, keyMod);
    }
    else if (vectorImage->getLastAreaNumber(getLastPoint()) > -1)
    {
        setAreaSelected(vectorImage, keyMod);
    }
    mScribbleArea->update();
}

void VectorMoveTool::setCurveSelected(VectorImage* vectorImage, Qt::KeyboardModifiers keyMod)
{
    auto selectMan = mEditor->select();
    if (!vectorImage->isSelected(selectMan->closestCurves()))
    {
        if (keyMod != Qt::ShiftModifier)
        {
            applyTransformation();
        }
        vectorImage->setSelected(selectMan->closestCurves(), true);
        selectMan->setSelection(vectorImage->getSelectionRect());
    }
}

void VectorMoveTool::setAreaSelected(VectorImage* vectorImage, Qt::KeyboardModifiers keyMod)
{
    int areaNumber = vectorImage->getLastAreaNumber(getLastPoint());
    if (!vectorImage->isAreaSelected(areaNumber))
    {
        if (keyMod != Qt::ShiftModifier)
        {
            applyTransformation();
        }
        vectorImage->setAreaSelected(areaNumber, true);
        mEditor->select()->setSelection(vectorImage->getSelectionRect());
    }
}

/**
 * @brief VectorMoveTool::storeClosestVectorCurve
 * stores the curves closest to the mouse position in mClosestCurves
 */
void VectorMoveTool::storeClosestVectorCurve(Layer* layer)
{
    auto selectMan = mEditor->select();
    auto layerVector = static_cast<LayerVector*>(layer);
    VectorImage* pVecImg = layerVector->getLastVectorImageAtFrame(mEditor->currentFrame(), 0);
    selectMan->setCurves(pVecImg->getCurvesCloseTo(getCurrentPoint(), selectMan->selectionTolerance()));
}

void VectorMoveTool::setAnchorToLastPoint()
{
    anchorOriginPoint = getLastPoint();
}

void VectorMoveTool::cancelChanges()
{
    auto selectMan = mEditor->select();
    mScribbleArea->cancelTransformedSelection();
    selectMan->resetSelectionProperties();
    mEditor->deselectAll();
}

void VectorMoveTool::applySelectionChanges()
{
    mEditor->select()->setRotation(0);
    mRotatedAngle = 0;

    mScribbleArea->applySelectionChanges();
}

void VectorMoveTool::applyTransformation()
{
    mScribbleArea->applyTransformedSelection();
}

void VectorMoveTool::paintTransformedSelection()
{
    mScribbleArea->paintTransformedSelection();
}

bool VectorMoveTool::leavingThisTool()
{
    applyTransformation();
    return true;
}

bool VectorMoveTool::switchingLayer()
{
    auto selectMan = mEditor->select();
    if (!selectMan->transformHasBeenModified())
    {
        mEditor->deselectAll();
        return true;
    }

    int returnValue = showTransformWarning();

    if (returnValue == QMessageBox::Yes)
    {
        applyTransformation();
        mEditor->deselectAll();
        return true;
    }
    else if (returnValue == QMessageBox::No)
    {
        cancelChanges();
        return true;
    }
    else if (returnValue == QMessageBox::Cancel)
    {
        return false;
    }
    return true;
}

int VectorMoveTool::showTransformWarning()
{
    int returnValue = QMessageBox::warning(nullptr,
                                           tr("Layer switch", "Windows title of layer switch pop-up."),
                                           tr("You are about to switch away, do you want to apply the transformation?"),
                                           QMessageBox::No | QMessageBox::Cancel | QMessageBox::Yes,
                                           QMessageBox::Yes);
    return returnValue;
}

Layer* VectorMoveTool::currentPaintableLayer()
{
    Layer* layer = mEditor->layers()->currentLayer();
    if (layer == nullptr)
        return nullptr;
    if (!layer->isPaintable())
        return nullptr;
    return layer;
}

QPointF VectorMoveTool::offsetFromPressPos()
{
    return getCurrentPoint() - getCurrentPressPoint();
}
