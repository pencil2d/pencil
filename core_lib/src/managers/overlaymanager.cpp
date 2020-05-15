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
        return mMoveMode = MoveMode::PERSP_SINGLE;
    }
    else if (QLineF(pos, mLeftPerspPoint).length() < calculatedSelectionTol)
    {
        return mMoveMode = MoveMode::PERSP_LEFT;
    }
    else if (QLineF(pos, mRightPerspPoint).length() < calculatedSelectionTol)
    {
        return mMoveMode = MoveMode::PERSP_RIGHT;
    }
    else if (QLineF(pos, mMiddlePerspPoint).length() < calculatedSelectionTol)
    {
        return mMoveMode = MoveMode::PERSP_MIDDLE;
    }

    return mMoveMode = MoveMode::NONE;
}

double OverlayManager::selectionTolerance()
{
    return qAbs(mSelectionTolerance * mEditor->viewScaleInversed());
}

void OverlayManager::initPerspOverlay(int i)
{
    if (i > 3 || i < 1) return;

    if (i == 1)
    {
        setSinglePerspPoint(getSinglePerspPoint());
        qDebug() << "singlepoint: " << mSinglePerspPoint.toPoint();
        mEditor->getScribbleArea()->prepOverlays();
        mEditor->getScribbleArea()->renderOverlays();
        MoveMode mode = MoveMode::PERSP_SINGLE;
        updatePerspOverlay(mode, mSinglePerspPoint.toPoint());
    }
}

void OverlayManager::updatePerspOverlay(int i, QPointF point)
{
    if (i > 3 || i < 1) return;

    switch (i) {
    case 1:
        setSinglePerspPoint(point);
        mEditor->getScribbleArea()->prepOverlays();
        mEditor->getScribbleArea()->renderOverlays();
        break;
    default:
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
    default:
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
    {
        mOverlayPerspective1 = b;
        updatePerspOverlayActiveList();
    }
}

void OverlayManager::setOverlayPerspective2(bool b)
{
    if (b != mOverlayPerspective2)
    {
        mOverlayPerspective2 = b;
        updatePerspOverlayActiveList();
    }
}

void OverlayManager::setOverlayPerspective3(bool b)
{
    if (b != mOverlayPerspective3)
    {
        mOverlayPerspective3 = b;
        updatePerspOverlayActiveList();
    }
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
