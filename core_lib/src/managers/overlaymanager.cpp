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

#include <QTransform>

OverlayManager::OverlayManager(Editor *editor): BaseManager(editor, __FUNCTION__)
{
    mEditor = editor;

    mSinglePerspectivePoint = QPointF(0.1, 0.1);
    mLeftPerspectivePoint = QPointF(-300.0, 0.0);
    mRightPerspectivePoint = QPointF(300.0, 0.0);
    mMiddlePerspectivePoint = QPointF(0.0, 200.0);
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

void OverlayManager::settingsUpdated(SETTING setting, bool state)
{
    switch (setting) {
        case SETTING::OVERLAY_PERSPECTIVE1:
            mSinglePerspectiveEnabled = state;
            break;
        case SETTING::OVERLAY_PERSPECTIVE2:
            mTwoPointPerspectiveEnabled = state;
            break;
        case SETTING::OVERLAY_PERSPECTIVE3:
            mThreePointPerspectiveEnabled = state;
            break;
        default:
            // We intentiallly leave everything else out as only overlay settings are important
            break;
    }
}

MoveMode OverlayManager::getMoveModeForPoint(const QPointF& pos, const QTransform& transform)
{
    const double calculatedSelectionTol = selectionTolerance();
    MoveMode mode = MoveMode::NONE;

    if (mSinglePerspectiveEnabled && QLineF(pos, transform.inverted().map(mSinglePerspectivePoint)).length() < calculatedSelectionTol)
    {
        mode = MoveMode::PERSP_SINGLE;
    }
    else if ((mTwoPointPerspectiveEnabled || mThreePointPerspectiveEnabled) && QLineF(pos, transform.inverted().map(mLeftPerspectivePoint)).length() < calculatedSelectionTol)
    {
        mode = MoveMode::PERSP_LEFT;
    }
    else if ((mTwoPointPerspectiveEnabled || mThreePointPerspectiveEnabled) && QLineF(pos, transform.inverted().map(mRightPerspectivePoint)).length() < calculatedSelectionTol)
    {
        mode = MoveMode::PERSP_RIGHT;
    }
    else if (mThreePointPerspectiveEnabled && QLineF(pos, transform.inverted().map(mMiddlePerspectivePoint)).length() < calculatedSelectionTol)
    {
        mode = MoveMode::PERSP_MIDDLE;
    }

    return mode;
}

double OverlayManager::selectionTolerance()
{
    return qAbs(mSelectionTolerance * mEditor->viewScaleInversed());
}

void OverlayManager::updatePerspective(const QPointF& point)
{
    switch (mMoveMode) {
    case MoveMode::PERSP_SINGLE:
        mSinglePerspectivePoint = point;
        break;
    case MoveMode::PERSP_LEFT:
        mLeftPerspectivePoint = point;
        mRightPerspectivePoint = QPointF(getRightPerspectivePoint().x(), point.y());
        break;
    case MoveMode::PERSP_RIGHT:
        mRightPerspectivePoint = point;
        mLeftPerspectivePoint = QPointF(getLeftPerspectivePoint().x(), point.y());
        break;
    case MoveMode::PERSP_MIDDLE:
        mMiddlePerspectivePoint = point;
        break;
    default:
        break;
    }
}
