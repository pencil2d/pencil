/*

Pencil2D - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2012-2020 Matthew Chiawen Chang

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
#include <QMessageBox>
#include <QSettings>

#include "pointerevent.h"
#include "editor.h"
#include "toolmanager.h"
#include "viewmanager.h"
#include "strokemanager.h"
#include "selectionmanager.h"
#include "overlaymanager.h"
#include "scribblearea.h"
#include "layervector.h"
#include "layermanager.h"
#include "mathutils.h"
#include "vectorimage.h"


MoveTool::MoveTool(QObject* parent) : BaseTool(parent)
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
    mRotationIncrement = mEditor->preference()->getInt(SETTING::ROTATION_INCREMENT);
    QSettings settings(PENCIL2D, PENCIL2D);
    properties.showSelectionInfo = settings.value("ShowSelectionInfo").toBool();
    mPropertyEnabled[SHOWSELECTIONINFO] = true;

    connect(mEditor->preference(), &PreferenceManager::optionChanged, this, &MoveTool::updateSettings);
}

QCursor MoveTool::cursor()
{
    MoveMode mode = MoveMode::NONE;
    SelectionManager* selectMan = mEditor->select();
    if (selectMan->somethingSelected())
    {
        mode = selectMan->getMoveMode();
        return mScribbleArea->currentTool()->selectMoveCursor(mode, type());
    }
    if (mEditor->overlays()->isPerspOverlaysActive())
    {
        mode = mEditor->overlays()->getMoveModeForOverlayAnchor(getCurrentPoint());
        mPerspMode = mode;
        return mScribbleArea->currentTool()->selectMoveCursor(mode, type());
    }
    return mScribbleArea->currentTool()->selectMoveCursor(mode, type());
}

void MoveTool::updateSettings(const SETTING setting)
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

void MoveTool::pointerPressEvent(PointerEvent* event)
{
    mCurrentLayer = currentPaintableLayer();
    if (mCurrentLayer == nullptr) return;

    if (mEditor->select()->somethingSelected())
    {
        beginInteraction(event->modifiers(), mCurrentLayer);
    }
    if (mEditor->overlays()->isPerspOverlaysActive())
    {
        QPointF point = mEditor->view()->mapScreenToCanvas(event->posF());
        mEditor->overlays()->setMoveMode(mPerspMode);
        mEditor->overlays()->updatePerspOverlay(point);
    }
    mOffset = getCurrentPoint();

    mEditor->frameModified(mEditor->currentFrame());
}

void MoveTool::pointerMoveEvent(PointerEvent* event)
{
    mCurrentLayer = currentPaintableLayer();
    if (mCurrentLayer == nullptr) return;

    if (mScribbleArea->isPointerInUse())   // the user is also pressing the mouse (dragging)
    {
        transformSelection(event->modifiers(), mCurrentLayer);
        if (mEditor->overlays()->isPerspOverlaysActive())
        {
            QPointF mapped = mEditor->view()->mapScreenToCanvas(event->pos());
            mEditor->overlays()->updatePerspOverlay(mapped);
        }
    }
    else
    {
        // the user is moving the mouse without pressing it
        // update cursor to reflect selection corner interaction
        mEditor->select()->setMoveModeForAnchorInRange(getCurrentPoint());
        mScribbleArea->updateToolCursor();

        if (mCurrentLayer->type() == Layer::VECTOR)
        {
            storeClosestVectorCurve(mCurrentLayer);
        }
        mEditor->getScribbleArea()->prepOverlays();
    }
    mEditor->frameModified(mEditor->currentFrame());
}

void MoveTool::pointerReleaseEvent(PointerEvent*)
{
    if (mEditor->overlays()->isPerspOverlaysActive())
    {
        mEditor->overlays()->setMoveMode(MoveMode::NONE);
        mPerspMode = MoveMode::NONE;
    }

    auto selectMan = mEditor->select();
    if (!selectMan->somethingSelected())
        return;

    mOffset = getCurrentPoint();

    mScribbleArea->updateToolCursor();
    mEditor->frameModified(mEditor->currentFrame());
}

void MoveTool::transformSelection(Qt::KeyboardModifiers keyMod, Layer* layer)
{
    auto selectMan = mEditor->select();
    if (selectMan->somethingSelected())
    {

        QPointF offset = offsetFromPressPos();

        int rotationIncrement = 0;
        if (selectMan->getMoveMode() == MoveMode::ROTATION && keyMod & Qt::ShiftModifier)
        {
            rotationIncrement = mRotationIncrement;
        }

        selectMan->maintainAspectRatio(keyMod == Qt::ShiftModifier);

        if(layer->type() == Layer::BITMAP)
        {
            offset = offset.toPoint();
        }

        qreal newAngle = 0;
        if (selectMan->getMoveMode() == MoveMode::ROTATION) {
            QPointF anchorPoint = selectMan->currentTransformAnchor();
            newAngle = selectMan->angleFromPoint(getCurrentPoint(), anchorPoint) - mRotatedAngle;
            mPreviousAngle = newAngle;
        }

        selectMan->adjustSelection(getCurrentPoint(), mOffset, newAngle, rotationIncrement);

        mOffset = getCurrentPoint();
        emit mEditor->frameModified(mEditor->currentFrame());
    }
    else // there is nothing selected
    {
        selectMan->setMoveMode(MoveMode::NONE);
    }
}

void MoveTool::beginInteraction(Qt::KeyboardModifiers keyMod, Layer* layer)
{
    auto selectMan = mEditor->select();
    QRectF selectionRect = selectMan->mySelectionRect();
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

    if (selectMan->getMoveMode() == MoveMode::MIDDLE)
    {
        if (keyMod == Qt::ControlModifier) // --- rotation
        {
            selectMan->setMoveMode(MoveMode::ROTATION);
        }
    }

    if (layer->type() == Layer::VECTOR)
    {
        createVectorSelection(keyMod, layer);
    }

    selectMan->setTransformAnchor(selectMan->getSelectionAnchorPoint());
    if(selectMan->getMoveMode() == MoveMode::ROTATION) {
        mRotatedAngle = selectMan->angleFromPoint(getCurrentPoint(), selectMan->currentTransformAnchor()) - mPreviousAngle;
    }
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
    if (vectorImage == nullptr) { return; }

    if (!mEditor->select()->closestCurves().empty()) // the user clicks near a curve
    {
        setCurveSelected(vectorImage, keyMod);
    }
    else if (vectorImage->getLastAreaNumber(getLastPoint()) > -1)
    {
        setAreaSelected(vectorImage, keyMod);
    }
}

void MoveTool::setCurveSelected(VectorImage* vectorImage, Qt::KeyboardModifiers keyMod)
{
    auto selectMan = mEditor->select();
    if (!vectorImage->isSelected(selectMan->closestCurves()))
    {
        if (keyMod != Qt::ShiftModifier)
        {
            applyTransformation();
        }
        vectorImage->setSelected(selectMan->closestCurves(), true);
        selectMan->setSelection(vectorImage->getSelectionRect(), false);
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
        mEditor->select()->setSelection(vectorImage->getSelectionRect(), false);
    }
}

/**
 * @brief MoveTool::storeClosestVectorCurve
 * stores the curves closest to the mouse position in mClosestCurves
 */
void MoveTool::storeClosestVectorCurve(Layer* layer)
{
    auto selectMan = mEditor->select();
    auto layerVector = static_cast<LayerVector*>(layer);
    VectorImage* pVecImg = layerVector->getLastVectorImageAtFrame(mEditor->currentFrame(), 0);
    if (pVecImg == nullptr) { return; }
    selectMan->setCurves(pVecImg->getCurvesCloseTo(getCurrentPoint(), selectMan->selectionTolerance()));
}

void MoveTool::setAnchorToLastPoint()
{
    anchorOriginPoint = getLastPoint();
}

void MoveTool::cancelChanges()
{
    mScribbleArea->cancelTransformedSelection();
    mEditor->deselectAll();
}

void MoveTool::applyTransformation()
{
    SelectionManager* selectMan = mEditor->select();
    mScribbleArea->applyTransformedSelection();

    // When the selection has been applied, a new rect is applied based on the bounding box.
    // This ensures that if the selection has been rotated, it will still fit the bounds of the image.
    selectMan->setSelection(selectMan->mapToSelection(QPolygonF(selectMan->mySelectionRect())).boundingRect());
    mRotatedAngle = 0;
    mPreviousAngle = 0;
}

bool MoveTool::leavingThisTool()
{
    if (mCurrentLayer)
    {
        switch (mCurrentLayer->type())
        {
        case Layer::BITMAP: applyTransformation(); break;
        case Layer::VECTOR: applyTransformation(); break;
        default: break;
        }
    }
    return true;
}

void MoveTool::resetToDefault()
{
    setShowSelectionInfo(false);
}

void MoveTool::setShowSelectionInfo(const bool b)
{
    properties.showSelectionInfo = b;

    QSettings settings(PENCIL2D, PENCIL2D);
    settings.setValue("ShowSelectionInfo", b);

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

QPointF MoveTool::offsetFromPressPos()
{
    return getCurrentPoint() - getCurrentPressPoint();
}
