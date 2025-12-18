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

#ifndef SELECTTOOL_H
#define SELECTTOOL_H

#include "transformtool.h"
#include "movemode.h"
#include "undoredomanager.h"

#include "layer.h"

#include <QRectF>

class Layer;
class SelectionManager;

class SelectTool : public TransformTool
{
    Q_OBJECT

public:
    explicit SelectTool(QObject* parent = nullptr);

    ToolType type() const override { return SELECT; }

    void loadSettings() override;
    QCursor cursor() override;

private:

    void pointerPressEvent(PointerEvent*) override;
    void pointerReleaseEvent(PointerEvent*) override;
    void pointerMoveEvent(PointerEvent*) override;

    bool keyPressEvent(QKeyEvent* event) override;

    void manageSelectionOrigin(QPointF currentPoint, QPointF originPoint, Layer::LAYER_TYPE layerType);
    void controlOffsetOrigin(QPointF currentPoint, QPointF anchorPoint, Layer::LAYER_TYPE layerType);

    void beginSelection(Layer* currentLayer, const QPointF& pos);
    void keepSelection(Layer* currentLayer);

    QPointF offsetFromPressPos(const QPointF& pos);

    inline bool isSelectionPointValid(const QPointF& pos) { return mAnchorOriginPoint != pos; }
    bool maybeDeselect(const QPointF& pos);

    // Store selection origin, so we can calculate
    // the selection rectangle in mousePressEvent.
    QPointF mAnchorOriginPoint;
    QPointF mPressPoint;
    MoveMode mMoveMode;
    MoveMode mStartMoveMode = MoveMode::NONE;
    QRectF mSelectionRect;

    QPixmap mCursorPixmap = QPixmap(24, 24);

    const UndoSaveState* mUndoState = nullptr;
};

#endif
