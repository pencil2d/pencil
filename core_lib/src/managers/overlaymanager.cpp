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

#include "editor.h"
#include "overlaymanager.h"

OverlayManager::OverlayManager(Editor *editor): BaseManager(editor, __FUNCTION__)
{
    mEditor = editor;

    setSinglePerspectivePoint(QPointF(0.1, 0.1));
    setLeftPerspectivePoint(QPointF(-300.0, 0.0));
    setRightPerspectivePoint(QPointF(300.0, 0.0));
    setMiddlePerspectivePoint(QPointF(0.0, 200.0));
}

OverlayManager::~OverlayManager()
{
}

bool OverlayManager::init()
{
    return true;
}

Status OverlayManager::load(Object*)
{
    return Status::OK;
}

Status OverlayManager::save(Object*)
{
    return Status::OK;
}

void OverlayManager::workingLayerChanged(Layer *)
{
}

MoveMode OverlayManager::getMoveModeForPoint(const QPointF& pos, const QTransform& transform)
{
    const double calculatedSelectionTol = selectionTolerance();
    MoveMode mode = MoveMode::NONE;

    if (QLineF(pos, transform.inverted().map(mSinglePerspectivePoint)).length() < calculatedSelectionTol)
    {
        mode = MoveMode::PERSP_SINGLE;
    }
    else if (QLineF(pos, transform.inverted().map(mLeftPerspectivePoint)).length() < calculatedSelectionTol)
    {
        mode = MoveMode::PERSP_LEFT;
    }
    else if (QLineF(pos, transform.inverted().map(mRightPerspectivePoint)).length() < calculatedSelectionTol)
    {
        mode = MoveMode::PERSP_RIGHT;
    }
    else if (QLineF(pos, transform.inverted().map(mMiddlePerspectivePoint)).length() < calculatedSelectionTol)
    {
        mode = MoveMode::PERSP_MIDDLE;
    }

    return mode;
}

double OverlayManager::selectionTolerance()
{
    return qAbs(mSelectionTolerance * mEditor->viewScaleInversed());
}

void OverlayManager::updatePerspective(int persp)
{
    switch (persp) {
    case 1:
        setMoveMode(MoveMode::PERSP_SINGLE);
        break;
    case 2:
        setMoveMode(MoveMode::PERSP_LEFT);
        break;
    case 3:
        setMoveMode(MoveMode::PERSP_LEFT);
        break;
    default:
        break;
    }
}

void OverlayManager::updatePerspective(const QPointF& point)
{
    switch (mMoveMode) {
    case MoveMode::PERSP_SINGLE:
        setSinglePerspectivePoint(point);
        break;
    case MoveMode::PERSP_LEFT:
        setLeftPerspectivePoint(point);
        setRightPerspectivePoint(QPointF(getRightPerspectivePoint().x(), point.y()));
        break;
    case MoveMode::PERSP_RIGHT:
        setRightPerspectivePoint(point);
        setLeftPerspectivePoint(QPointF(getLeftPerspectivePoint().x(), point.y()));
        break;
    case MoveMode::PERSP_MIDDLE:
        setMiddlePerspectivePoint(point);
        break;
    default:
        break;
    }
}

void OverlayManager::setOnePointPerspectiveEnabled(bool b)
{
    mOverlayPerspective1 = b;
}

void OverlayManager::setTwoPointPerspectiveEnabled(bool b)
{
    mOverlayPerspective2 = b;
}

void OverlayManager::setThreePointPerspectiveEnabled(bool b)
{
    mOverlayPerspective3 = b;
}
