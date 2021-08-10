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
#include "scribblearea.h"
#include "overlaymanager.h"
#include "overlaypainter.h"

OverlayManager::OverlayManager(Editor *editor): BaseManager(editor, "OverlayManager")
{
    mEditor = editor;

    setSinglePerspPoint(QPointF(0.1, 0.1));
    setLeftPerspPoint(QPointF(-300.0, 0.0));
    setRightPerspPoint(QPointF(300.0, 0.0));
    setMiddlePerspPoint(QPointF(0.0, 200.0));
}

OverlayManager::~OverlayManager()
{
}

bool OverlayManager::init()
{
    mActivePerspOverlays.clear();
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

MoveMode OverlayManager::getMoveModeForPoint(const QPointF& pos, QTransform transform)
{
    const double calculatedSelectionTol = selectionTolerance();
    MoveMode mode = MoveMode::NONE;

    if (QLineF(pos, transform.inverted().map(mSinglePerspPoint)).length() < calculatedSelectionTol)
    {
        mode = MoveMode::PERSP_SINGLE;
    }
    else if (QLineF(pos, transform.inverted().map(mLeftPerspPoint)).length() < calculatedSelectionTol)
    {
        mode = MoveMode::PERSP_LEFT;
    }
    else if (QLineF(pos, transform.inverted().map(mRightPerspPoint)).length() < calculatedSelectionTol)
    {
        mode = MoveMode::PERSP_RIGHT;
    }
    else if (QLineF(pos, transform.inverted().map(mMiddlePerspPoint)).length() < calculatedSelectionTol)
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
        setSinglePerspPoint(point);
        break;
    case MoveMode::PERSP_LEFT:
        setLeftPerspPoint(point);
        setRightPerspPoint(QPointF(getRightPerspPoint().x(), point.y()));
        break;
    case MoveMode::PERSP_RIGHT:
        setRightPerspPoint(point);
        setLeftPerspPoint(QPointF(getLeftPerspPoint().x(), point.y()));
        break;
    case MoveMode::PERSP_MIDDLE:
        setMiddlePerspPoint(point);
        break;
    default:
        break;
    }
}

void OverlayManager::setOnePointPerspectiveEnabled(bool b)
{
    mOverlayPerspective1 = b;
    updatePerspOverlayActiveList();
}

void OverlayManager::setTwoPointPerspectiveEnabled(bool b)
{
    mOverlayPerspective2 = b;
    updatePerspOverlayActiveList();
}

void OverlayManager::setThreePointPerspectiveEnabled(bool b)
{
    mOverlayPerspective3 = b;
    updatePerspOverlayActiveList();
}

void OverlayManager::updatePerspOverlayActiveList()
{
    mActivePerspOverlays.clear();
    if (mOverlayPerspective1)
        mActivePerspOverlays.append(1);
    if (mOverlayPerspective2)
        mActivePerspOverlays.append(2);
    if (mOverlayPerspective3)
        mActivePerspOverlays.append(3);
}
