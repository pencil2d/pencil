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
#include "selecttool.h"
#include <QSettings>
#include "pointerevent.h"
#include "vectorimage.h"
#include "editor.h"
#include "layervector.h"
#include "scribblearea.h"
#include "layermanager.h"
#include "toolmanager.h"
#include "selectionmanager.h"
#include "undoredomanager.h"

SelectTool::SelectTool(QObject* parent) : TransformTool(parent)
{
}

void SelectTool::loadSettings()
{
    QSettings pencilSettings(PENCIL2D, PENCIL2D);

    QHash<int, PropertyInfo> info;

    mPropertyUsed[TransformToolProperties::SHOWSELECTIONINFO_ENABLED] = { Layer::BITMAP, Layer::VECTOR };

    info[TransformToolProperties::SHOWSELECTIONINFO_ENABLED] = false;
    toolProperties().insertProperties(info);
    toolProperties().loadFrom(typeName(), pencilSettings);

    if (toolProperties().requireMigration(pencilSettings, ToolProperties::VERSION_1)) {
        toolProperties().setBaseValue(TransformToolProperties::SHOWSELECTIONINFO_ENABLED, pencilSettings.value("ShowSelectionInfo", false).toBool());
    }
}

QCursor SelectTool::cursor()
{
    // Don't update cursor while we're moving the selection
    if (mScribbleArea->isPointerInUse()) { return QCursor(mCursorPixmap); }

    MoveMode mode = mEditor->select()->getMoveMode();

    mCursorPixmap.fill(QColor(255, 255, 255, 0));
    QPainter cursorPainter(&mCursorPixmap);
    cursorPainter.setRenderHint(QPainter::Antialiasing);

    switch(mode)
    {
    case MoveMode::TOPLEFT:
    case MoveMode::BOTTOMRIGHT:
    {
        cursorPainter.drawPixmap(QPoint(6,6),QPixmap("://icons/general/cursor-diagonal-left.svg"));
        break;
    }
    case MoveMode::TOPRIGHT:
    case MoveMode::BOTTOMLEFT:
    {
        cursorPainter.drawPixmap(QPoint(6,6),QPixmap("://icons/general/cursor-diagonal-right.svg"));
        break;
    }
    case MoveMode::MIDDLE:
    {
        cursorPainter.drawPixmap(QPoint(6,6),QPixmap("://icons/general/cursor-move.svg"));
        break;
    }
    case MoveMode::NONE:
        cursorPainter.drawPixmap(QPoint(3,3), QPixmap(":icons/general/cross.png"));
        break;
    default:
        Q_UNREACHABLE();
        break;
    }
    return QCursor(mCursorPixmap);
}

void SelectTool::beginSelection(Layer* currentLayer, const QPointF& pos)
{
    auto selectMan = mEditor->select();

    if (selectMan->somethingSelected() && mMoveMode != MoveMode::NONE) // there is something selected
    {
        if (currentLayer->type() == Layer::VECTOR)
        {
            VectorImage* vectorImage = static_cast<LayerVector*>(currentLayer)->getLastVectorImageAtFrame(mEditor->currentFrame(), 0);
            if (vectorImage != nullptr) {
                vectorImage->deselectAll();
            }
        }
        mSelectionRect = mEditor->select()->mapToSelection(mEditor->select()->mySelectionRect()).boundingRect();
    }
    else
    {
        selectMan->setSelection(QRectF(pos.x(), pos.y(), 0, 0), mEditor->layers()->currentLayer()->type() == Layer::BITMAP);
        mAnchorOriginPoint = pos;
    }

    mScribbleArea->updateFrame();
}

void SelectTool::pointerPressEvent(PointerEvent* event)
{
    Layer* currentLayer = mEditor->layers()->currentLayer();
    if (currentLayer == nullptr) return;
    if (!currentLayer->isPaintable()) { return; }
    if (event->button() != Qt::LeftButton) { return; }
    auto selectMan = mEditor->select();

    mUndoState = mEditor->undoRedo()->state(UndoRedoRecordType::KEYFRAME_MODIFY);

    mPressPoint = event->canvasPos();

    if (currentLayer->type() == Layer::BITMAP) {
        mPressPoint = mPressPoint.toPoint();
    }

    selectMan->setMoveModeForAnchorInRange(mPressPoint);
    mMoveMode = selectMan->getMoveMode();
    mStartMoveMode = mMoveMode;

    beginSelection(currentLayer, mPressPoint);
}

void SelectTool::pointerMoveEvent(PointerEvent* event)
{
    Layer* currentLayer = mEditor->layers()->currentLayer();
    if (currentLayer == nullptr) { return; }
    if (!currentLayer->isPaintable()) { return; }
    auto selectMan = mEditor->select();

    QPointF canvasPos = event->canvasPos();

    if (currentLayer->type() == Layer::BITMAP) {
        canvasPos = canvasPos.toPoint();
    }

    selectMan->setMoveModeForAnchorInRange(canvasPos);
    mMoveMode = selectMan->getMoveMode();
    mScribbleArea->updateToolCursor();

    if (mScribbleArea->isPointerInUse())
    {
        controlOffsetOrigin(canvasPos, mAnchorOriginPoint, currentLayer->type());

        if (currentLayer->type() == Layer::VECTOR)
        {
            VectorImage* vectorImage = static_cast<LayerVector*>(currentLayer)->getLastVectorImageAtFrame(mEditor->currentFrame(), 0);
            if (vectorImage != nullptr) {
                vectorImage->select(selectMan->mapToSelection(QPolygonF(selectMan->mySelectionRect())).boundingRect());
            }
        }
    }

    mScribbleArea->updateFrame();
}

void SelectTool::pointerReleaseEvent(PointerEvent* event)
{
    Layer* currentLayer = mEditor->layers()->currentLayer();
    if (currentLayer == nullptr) return;
    if (event->button() != Qt::LeftButton) return;

    QPointF canvasPos = event->canvasPos();
    if (currentLayer->type() == Layer::BITMAP) {
        canvasPos = canvasPos.toPoint();
    }

    // if there's a small very small distance between current and last point
    // discard the selection...
    // TODO: improve by adding a timer to check if the user is deliberately selecting
    if (QLineF(mAnchorOriginPoint, canvasPos).length() < 1.0)
    {
        mEditor->deselectAll();
    }
    else if (maybeDeselect(canvasPos))
    {
        mEditor->deselectAll();
    }
    else
    {
        mSelectionRect = mEditor->select()->mapToSelection(mEditor->select()->mySelectionRect()).boundingRect();
        keepSelection(currentLayer);
    }

    mEditor->undoRedo()->record(mUndoState, typeName());

    mStartMoveMode = MoveMode::NONE;
    mMoveMode = MoveMode::NONE;

    mScribbleArea->updateToolCursor();
    mScribbleArea->updateFrame();
}

bool SelectTool::maybeDeselect(const QPointF& pos)
{
    return ((!isSelectionPointValid(pos) && mEditor->select()->getMoveMode() == MoveMode::NONE)
            || !mEditor->select()->mySelectionRect().isValid());
}

/**
 * @brief SelectTool::keepSelection
 * Keep selection rect and normalize if invalid
 */
void SelectTool::keepSelection(Layer* currentLayer)
{
    
    if (currentLayer->type() == Layer::VECTOR)
    {
        VectorImage* vectorImage = static_cast<LayerVector*>(currentLayer)->getLastVectorImageAtFrame(mEditor->currentFrame(), 0);
        if (vectorImage == nullptr) { return; }
        auto selectMan = mEditor->select();
        selectMan->setSelection(vectorImage->getSelectionRect(), false);
    }
}

void SelectTool::controlOffsetOrigin(QPointF currentPoint, QPointF anchorPoint, Layer::LAYER_TYPE layerType)
{
    QRectF newSelection;
    if (mStartMoveMode == MoveMode::NONE) {
        // When the selection is none, manage the selection Origin
        newSelection = QRectF(currentPoint, anchorPoint);
    } else {
        newSelection = mSelectionRect;

        QPointF offset = offsetFromPressPos(currentPoint);
        if (mStartMoveMode == MoveMode::TOPLEFT) {
            newSelection.adjust(offset.x(), offset.y(), 0, 0);
        } else if (mStartMoveMode == MoveMode::TOPRIGHT) {
            newSelection.adjust(0, offset.y(), offset.x(), 0);
        } else if (mStartMoveMode == MoveMode::BOTTOMRIGHT) {
            newSelection.adjust(0, 0, offset.x(), offset.y());
        } else if (mStartMoveMode == MoveMode::BOTTOMLEFT) {
            newSelection.adjust(offset.x(), 0, 0, offset.y());
        } else {
            newSelection.translate(offset.x(), offset.y());
        }
    }
    newSelection = newSelection.normalized();
    mEditor->select()->setSelection(newSelection, layerType == Layer::BITMAP);
}

bool SelectTool::keyPressEvent(QKeyEvent* event)
{
    switch (event->key())
    {
    case Qt::Key_Alt:
        if (mEditor->tools()->setTemporaryTool(MOVE, {}, Qt::AltModifier))
        {
            return true;
        }
        break;
    default:
        break;
    }

    // Follow the generic behavior anyway
    return TransformTool::keyPressEvent(event);
}

QPointF SelectTool::offsetFromPressPos(const QPointF& pos)
{
    return pos - mPressPoint;
}
