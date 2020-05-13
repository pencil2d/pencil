#include "editor.h"
#include "scribblearea.h"
#include "overlaymanager.h"
#include "overlaypainter.h"

OverlayManager::OverlayManager(Editor *editor): BaseManager(editor)
{
    mEditor = editor;
    op = new OverlayPainter();
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

MoveMode OverlayManager::getMoveModeForOverlayAnchor(QPointF pos)
{
    const double calculatedSelectionTol = selectionTolerance();

    if (QLineF(pos, mSinglePerspPoint).length() < calculatedSelectionTol)
    {
//        mMoveMode = MoveMode::PERSP_SINGLE;
        return mMoveMode = MoveMode::PERSP_SINGLE;
    }
    else if (QLineF(pos, mLeftPerspPoint).length() < calculatedSelectionTol)
    {
//        mMoveMode = MoveMode::PERSP_LEFT;
        return mMoveMode = MoveMode::PERSP_LEFT;
    }
    else if (QLineF(pos, mRightPerspPoint).length() < calculatedSelectionTol)
    {
//        mMoveMode = MoveMode::PERSP_RIGHT;
        return mMoveMode = MoveMode::PERSP_RIGHT;
    }
    else if (QLineF(pos, mMiddlePerspPoint).length() < calculatedSelectionTol)
    {
//        mMoveMode = MoveMode::PERSP_MIDDLE;
        return mMoveMode = MoveMode::PERSP_MIDDLE;
    }

//    mMoveMode = MoveMode::NONE;
    return mMoveMode = MoveMode::NONE;
}

double OverlayManager::selectionTolerance()
{
    return qAbs(mSelectionTolerance * mEditor->viewScaleInversed());
}

void OverlayManager::updatePerspOverlay(int i, QPointF point)
{
    switch (i) {
    case 1:
        setSinglePerspPoint(point);
        mEditor->getScribbleArea()->prepOverlays();
        mEditor->getScribbleArea()->renderOverlays();
        break;

    }
}

void OverlayManager::updatePerspOverlay(MoveMode mode, QPoint point)
{
    switch (mode) {
    case MoveMode::PERSP_SINGLE:
        setSinglePerspPoint(point);
        mEditor->getScribbleArea()->prepOverlays();
        mEditor->getScribbleArea()->renderOverlays();
        break;

    }
}

void OverlayManager::resetPerspectiveOverlays()
{

}

void OverlayManager::setOverlayCenter(bool b)
{
    if (b != mOverlayCenter)
        mOverlayCenter = b;
}

void OverlayManager::setOverlayThirds(bool b)
{
    if (b != mOverlayThirds)
        mOverlayThirds = b;
}

void OverlayManager::setOverlayGoldenRatio(bool b)
{
    if (b != mOverlayGoldenRatio)
        mOverlayGoldenRatio = b;
}

void OverlayManager::setOverlaySafeAreas(bool b)
{
    if (b != mOverlaySafeAreas)
        mOverlaySafeAreas = b;
}

void OverlayManager::setOverlayPerspective1(bool b)
{
    if (b != mOverlayPerspective1)
        mOverlayPerspective1 = b;
}

void OverlayManager::setOverlayPerspective2(bool b)
{
    if (b != mOverlayPerspective2)
        mOverlayPerspective2 = b;
}

void OverlayManager::setOverlayPerspective3(bool b)
{
    if (b != mOverlayPerspective3)
        mOverlayPerspective3 = b;
}
