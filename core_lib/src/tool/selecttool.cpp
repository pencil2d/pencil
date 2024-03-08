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

SelectTool::SelectTool(QObject* parent) : BaseTool(parent)
{
}

void SelectTool::loadSettings()
{
    properties.width = -1;
    properties.feather = -1;
    properties.stabilizerLevel = -1;
    properties.useAA = -1;
    QSettings settings(PENCIL2D, PENCIL2D);
    properties.showSelectionInfo = settings.value("ShowSelectionInfo").toBool();
    mPropertyEnabled[SHOWSELECTIONINFO] = true;
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

void SelectTool::resetToDefault()
{
    setShowSelectionInfo(false);
}

void SelectTool::setShowSelectionInfo(const bool b)
{
    properties.showSelectionInfo = b;

    QSettings settings(PENCIL2D, PENCIL2D);
    settings.setValue("ShowSelectionInfo", b);
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
        selectMan->setSelection(QRectF(pos.x(), pos.y(), 1, 1), mEditor->layers()->currentLayer()->type() == Layer::BITMAP);
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

    mPressPoint = event->canvasPos();
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

    if (!selectMan->somethingSelected()) { return; }

    selectMan->setMoveModeForAnchorInRange(event->canvasPos());
    mMoveMode = selectMan->getMoveMode();
    mScribbleArea->updateToolCursor();

    if (mScribbleArea->isPointerInUse())
    {
        controlOffsetOrigin(event->canvasPos(), mAnchorOriginPoint);

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

    // if there's a small very small distance between current and last point
    // discard the selection...
    // TODO: improve by adding a timer to check if the user is deliberately selecting
    if (QLineF(mAnchorOriginPoint, event->canvasPos()).length() < 5.0)
    {
        mEditor->deselectAll();
    }
    if (maybeDeselect(event->canvasPos()))
    {
        mEditor->deselectAll();
    }
    else
    {
        keepSelection(currentLayer);
    }

    mStartMoveMode = MoveMode::NONE;
    mSelectionRect = mEditor->select()->mapToSelection(mEditor->select()->mySelectionRect()).boundingRect();

    mScribbleArea->updateToolCursor();
    mScribbleArea->updateFrame();
}

bool SelectTool::maybeDeselect(const QPointF& pos)
{
    return (!isSelectionPointValid(pos) && mEditor->select()->getMoveMode() == MoveMode::NONE);
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

void SelectTool::controlOffsetOrigin(QPointF currentPoint, QPointF anchorPoint)
{
    // when the selection is none, manage the selection Origin
    if (mStartMoveMode != MoveMode::NONE) {
        QRectF rect = mSelectionRect;

        QPointF offset = offsetFromPressPos(currentPoint);
        if (mStartMoveMode == MoveMode::TOPLEFT) {
            rect.adjust(offset.x(), offset.y(), 0, 0);
        } else if (mStartMoveMode == MoveMode::TOPRIGHT) {
            rect.adjust(0, offset.y(), offset.x(), 0);
        } else if (mStartMoveMode == MoveMode::BOTTOMRIGHT) {
            rect.adjust(0, 0, offset.x(), offset.y());
        } else if (mStartMoveMode == MoveMode::BOTTOMLEFT) {
            rect.adjust(offset.x(), 0, 0, offset.y());
        } else {
            rect.translate(offset.x(), offset.y());
        }

        rect = rect.normalized();
        if (rect.isValid()) {
            mEditor->select()->setSelection(rect, true);
        }
    } else {
        manageSelectionOrigin(currentPoint, anchorPoint);
    }
}

/**
 * @brief SelectTool::manageSelectionOrigin
 * switches anchor point when crossing threshold
 */
void SelectTool::manageSelectionOrigin(QPointF currentPoint, QPointF originPoint)
{
    qreal mouseX = currentPoint.x();
    qreal mouseY = currentPoint.y();

    QRectF selectRect = mSelectionRect;

    if (mouseX <= originPoint.x())
    {
        selectRect.setLeft(mouseX);
        selectRect.setRight(originPoint.x());
    }
    else
    {
        selectRect.setLeft(originPoint.x());
        selectRect.setRight(mouseX);
    }

    if (mouseY <= originPoint.y())
    {
        selectRect.setTop(mouseY);
        selectRect.setBottom(originPoint.y());
    }
    else
    {
        selectRect.setTop(originPoint.y());
        selectRect.setBottom(mouseY);
    }

    if (selectRect.width() <= 0) {
        selectRect.setWidth(1);
    }
    if (selectRect.height() <= 0) {
        selectRect.setHeight(1);
    }

    editor()->select()->setSelection(selectRect);
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
    return BaseTool::keyPressEvent(event);
}

QPointF SelectTool::offsetFromPressPos(const QPointF& pos)
{
    return pos - mPressPoint;
}
