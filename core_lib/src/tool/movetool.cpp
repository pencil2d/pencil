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
#include "strokeinterpolator.h"
#include "selectionmanager.h"
#include "overlaymanager.h"
#include "scribblearea.h"
#include "layervector.h"
#include "layermanager.h"
#include "layercamera.h"
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
        mode = mEditor->select()->getMoveMode();
    }
    else if (mEditor->overlays()->anyOverlayEnabled())
    {
        mode = mPerspMode;
    }

    return cursor(mode);
}

void MoveTool::updateSettings(const SETTING setting)
{
    switch (setting)
    {
    case SETTING::ROTATION_INCREMENT:
        mRotationIncrement = mEditor->preference()->getInt(SETTING::ROTATION_INCREMENT);
        break;
    case SETTING::OVERLAY_PERSPECTIVE1:
        mEditor->overlays()->settingsUpdated(setting, mEditor->preference()->isOn(setting));
        break;
    case SETTING::OVERLAY_PERSPECTIVE2:
        mEditor->overlays()->settingsUpdated(setting, mEditor->preference()->isOn(setting));
        break;
    case SETTING::OVERLAY_PERSPECTIVE3:
        mEditor->overlays()->settingsUpdated(setting, mEditor->preference()->isOn(setting));
        break;
    default:
        break;
    }
}

void MoveTool::pointerPressEvent(PointerEvent* event)
{
    Layer* currentLayer = currentPaintableLayer();
    if (currentLayer == nullptr) return;

    if (mEditor->select()->somethingSelected())
    {
        beginInteraction(event->canvasPos(), event->modifiers(), currentLayer);
    }
    else if (mEditor->overlays()->anyOverlayEnabled())
    {
        LayerCamera* layerCam = mEditor->layers()->getCameraLayerBelow(mEditor->currentLayerIndex());
        Q_ASSERT(layerCam);

        mPerspMode = mEditor->overlays()->getMoveModeForPoint(event->canvasPos(), layerCam->getViewAtFrame(mEditor->currentFrame()));
        mEditor->overlays()->setMoveMode(mPerspMode);
        QPoint mapped = layerCam->getViewAtFrame(mEditor->currentFrame()).map(event->canvasPos()).toPoint();
        mEditor->overlays()->updatePerspective(mapped);
    }

    mEditor->updateFrame();
}

void MoveTool::pointerMoveEvent(PointerEvent* event)
{
    Layer* currentLayer = currentPaintableLayer();
    if (currentLayer == nullptr) return;

    if (mScribbleArea->isPointerInUse())   // the user is also pressing the mouse (dragging)
    {
        transformSelection(event->canvasPos(), event->modifiers());

        if (mEditor->overlays()->anyOverlayEnabled())
        {
            LayerCamera* layerCam = mEditor->layers()->getCameraLayerBelow(mEditor->currentLayerIndex());
            Q_ASSERT(layerCam);
            mEditor->overlays()->updatePerspective(layerCam->getViewAtFrame(mEditor->currentFrame()).map(event->canvasPos()));
        }
        if (mEditor->select()->somethingSelected())
        {
            transformSelection(event->canvasPos(), event->modifiers());
        }
    }
    else
    {
        // the user is moving the mouse without pressing it
        // update cursor to reflect selection corner interaction
        mEditor->select()->setMoveModeForAnchorInRange(event->canvasPos());
        if (mEditor->overlays()->anyOverlayEnabled())
        {
            LayerCamera *layerCam = mEditor->layers()->getCameraLayerBelow(mEditor->currentLayerIndex());
            Q_ASSERT(layerCam);
            mPerspMode = mEditor->overlays()->getMoveModeForPoint(event->canvasPos(), layerCam->getViewAtFrame(mEditor->currentFrame()));
        }
        mScribbleArea->updateToolCursor();

        if (currentLayer->type() == Layer::VECTOR)
        {
            storeClosestVectorCurve(event->canvasPos(), currentLayer);
        }
    }
    mEditor->updateFrame();
}

void MoveTool::pointerReleaseEvent(PointerEvent*)
{
    if (mEditor->overlays()->anyOverlayEnabled())
    {
        mEditor->overlays()->setMoveMode(MoveMode::NONE);
        mPerspMode = MoveMode::NONE;
    }

    auto selectMan = mEditor->select();
    if (!selectMan->somethingSelected())
        return;

    mScribbleArea->updateToolCursor();
    mEditor->frameModified(mEditor->currentFrame());
}

void MoveTool::transformSelection(const QPointF& pos, Qt::KeyboardModifiers keyMod)
{
    auto selectMan = mEditor->select();
    if (selectMan->somethingSelected())
    {
        int rotationIncrement = 0;
        if (selectMan->getMoveMode() == MoveMode::ROTATION && keyMod & Qt::ShiftModifier)
        {
            rotationIncrement = mRotationIncrement;
        }

        selectMan->maintainAspectRatio(keyMod == Qt::ShiftModifier);
        selectMan->alignPositionToAxis(keyMod == Qt::ShiftModifier);

        qreal newAngle = 0;
        if (selectMan->getMoveMode() == MoveMode::ROTATION) {
            QPointF anchorPoint = selectMan->currentTransformAnchor();
            newAngle = selectMan->angleFromPoint(pos, anchorPoint) - mRotatedAngle;
        }

        selectMan->adjustSelection(pos, mOffset, newAngle, rotationIncrement);
    }
    else // there is nothing selected
    {
        selectMan->setMoveMode(MoveMode::NONE);
    }
}

void MoveTool::beginInteraction(const QPointF& pos, Qt::KeyboardModifiers keyMod, Layer* layer)
{
    auto selectMan = mEditor->select();
    QRectF selectionRect = selectMan->mySelectionRect();
    if (!selectionRect.isNull())
    {
        mEditor->backup(typeName());
    }

    if (keyMod != Qt::ShiftModifier)
    {
        if (selectMan->isOutsideSelectionArea(pos))
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
        createVectorSelection(pos, keyMod, layer);
    }

    selectMan->setTransformAnchor(selectMan->getSelectionAnchorPoint());
    selectMan->setDragOrigin(pos);
    mOffset = selectMan->myTranslation();

    if(selectMan->getMoveMode() == MoveMode::ROTATION) {
        mRotatedAngle = selectMan->angleFromPoint(pos, selectMan->currentTransformAnchor()) - selectMan->myRotation();
    }
}

/**
 * @brief MoveTool::createVectorSelection
 * In vector the selection rectangle is based on the bounding box of the curves
 * We can therefore create a selection just by clicking near/on a curve
 */
void MoveTool::createVectorSelection(const QPointF& pos, Qt::KeyboardModifiers keyMod, Layer* layer)
{
    assert(layer->type() == Layer::VECTOR);
    LayerVector* vecLayer = static_cast<LayerVector*>(layer);
    VectorImage* vectorImage = vecLayer->getLastVectorImageAtFrame(mEditor->currentFrame(), 0);
    if (vectorImage == nullptr) { return; }

    if (!mEditor->select()->closestCurves().empty()) // the user clicks near a curve
    {
        setCurveSelected(vectorImage, keyMod);
    }
    else if (vectorImage->getLastAreaNumber(pos) > -1)
    {
        setAreaSelected(pos, vectorImage, keyMod);
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

void MoveTool::setAreaSelected(const QPointF& pos, VectorImage* vectorImage, Qt::KeyboardModifiers keyMod)
{
    int areaNumber = vectorImage->getLastAreaNumber(pos);
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
void MoveTool::storeClosestVectorCurve(const QPointF& pos, Layer* layer)
{
    auto selectMan = mEditor->select();
    auto layerVector = static_cast<LayerVector*>(layer);
    VectorImage* pVecImg = layerVector->getLastVectorImageAtFrame(mEditor->currentFrame(), 0);
    if (pVecImg == nullptr) { return; }
    selectMan->setCurves(pVecImg->getCurvesCloseTo(pos, selectMan->selectionTolerance()));
}

void MoveTool::applyTransformation()
{
    SelectionManager* selectMan = mEditor->select();
    mScribbleArea->applyTransformedSelection();

    // When the selection has been applied, a new rect is applied based on the bounding box.
    // This ensures that if the selection has been rotated, it will still fit the bounds of the image.
    selectMan->setSelection(selectMan->mapToSelection(QPolygonF(selectMan->mySelectionRect())).boundingRect());
    mRotatedAngle = 0;
}

bool MoveTool::leavingThisTool()
{
    BaseTool::leavingThisTool();

    if (currentPaintableLayer())
    {
        applyTransformation();
    }
    return true;
}

bool MoveTool::isActive() const {
    return mScribbleArea->isPointerInUse() &&
           (mEditor->select()->somethingSelected() || mEditor->overlays()->getMoveMode() != MoveMode::NONE);
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

QCursor MoveTool::cursor(MoveMode mode) const
{
    QPixmap cursorPixmap = QPixmap(24, 24);

    cursorPixmap.fill(QColor(255, 255, 255, 0));
    QPainter cursorPainter(&cursorPixmap);
    cursorPainter.setRenderHint(QPainter::Antialiasing);

    switch(mode)
    {
    case MoveMode::PERSP_LEFT:
    case MoveMode::PERSP_RIGHT:
    case MoveMode::PERSP_MIDDLE:
    case MoveMode::PERSP_SINGLE:
    {
        cursorPainter.drawImage(QPoint(6,6),QImage("://icons/general/cursor-move.svg"));
        break;
    }
    case MoveMode::TOPLEFT:
    case MoveMode::BOTTOMRIGHT:
    {
        cursorPainter.drawImage(QPoint(6,6),QImage("://icons/general/cursor-diagonal-left.svg"));
        break;
    }
    case MoveMode::TOPRIGHT:
    case MoveMode::BOTTOMLEFT:
    {
        cursorPainter.drawImage(QPoint(6,6),QImage("://icons/general/cursor-diagonal-right.svg"));
        break;
    }
    case MoveMode::ROTATIONLEFT:
    case MoveMode::ROTATIONRIGHT:
    case MoveMode::ROTATION:
    {
        cursorPainter.drawImage(QPoint(6,6),QImage("://icons/general/cursor-rotate.svg"));
        break;
    }
    case MoveMode::MIDDLE:
    case MoveMode::CENTER:
    {
        cursorPainter.drawImage(QPoint(6,6),QImage("://icons/general/cursor-move.svg"));
        break;
    }
    default:
        return Qt::ArrowCursor;
    }
    cursorPainter.end();

    return QCursor(cursorPixmap);
}
