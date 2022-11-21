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

#ifndef OVERLAYMANAGER_H
#define OVERLAYMANAGER_H

#include "pencildef.h"
#include "movemode.h"
#include "basemanager.h"

#include <QPointF>

class Editor;

class OverlayManager : public BaseManager
{
    Q_OBJECT

public:
    explicit OverlayManager(Editor* editor);
    ~OverlayManager() override;

    bool init() override;
    Status load(Object *o) override;
    Status save(Object *o) override;

    void workingLayerChanged(Layer *) override;

    MoveMode getMoveModeForPoint(const QPointF& pos, const QTransform& transform);
    double selectionTolerance();

    void updatePerspective(const int persp);
    void updatePerspective(const QPointF& point);

    MoveMode getMoveMode() const { return mMoveMode; }
    void setMoveMode(MoveMode mode) { mMoveMode = mode; }
    void setSinglePerspectivePoint(const QPointF& point) { mSinglePerspectivePoint = point; }
    QPointF getSinglePerspectivePoint() const { return mSinglePerspectivePoint; }
    void setLeftPerspectivePoint(const QPointF& point) { mLeftPerspectivePoint = point; }
    QPointF getLeftPerspectivePoint() const { return mLeftPerspectivePoint; }
    void setRightPerspectivePoint(const QPointF& point) { mRightPerspectivePoint = point; }
    QPointF getRightPerspectivePoint() const { return mRightPerspectivePoint; }
    void setMiddlePerspectivePoint(const QPointF& point) { mMiddlePerspectivePoint = point; }
    QPointF getMiddlePerspectivePoint() const { return mMiddlePerspectivePoint; }

private:
    Editor* mEditor = nullptr;

    QPointF mSinglePerspectivePoint;   // for single point perspective.
    QPointF mLeftPerspectivePoint;
    QPointF mRightPerspectivePoint;    // Left, right and middle are for
    QPointF mMiddlePerspectivePoint;   // two and three point perspective

    MoveMode mMoveMode = MoveMode::NONE;

    const qreal mSelectionTolerance = 8.0;
};

#endif // OVERLAYMANAGER_H
