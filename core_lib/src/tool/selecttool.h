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

#include "basetool.h"
#include "movemode.h"

#include <QRectF>

class Layer;
class SelectionManager;

class SelectTool : public BaseTool
{
    Q_OBJECT

public:
    explicit SelectTool(QObject* parent = nullptr);
    ToolType type() override { return SELECT; }
    void loadSettings() override;
    QCursor cursor() override;

    void resetToDefault() override;
    void setShowSelectionInfo(const bool b) override;

private:

    void pointerPressEvent(PointerEvent*) override;
    void pointerReleaseEvent(PointerEvent*) override;
    void pointerMoveEvent(PointerEvent*) override;

    bool keyPressEvent(QKeyEvent* event) override;

    void manageSelectionOrigin(QPointF currentPoint, QPointF originPoint);
    void controlOffsetOrigin(QPointF currentPoint, QPointF anchorPoint);

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
};

#endif
