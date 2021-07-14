#include "editor.h"
#include "scribblearea.h"
#include "overlaymanager.h"
#include "overlaypainter.h"

OverlayManager::OverlayManager(Editor *editor): BaseManager(editor, __FUNCTION__)
{
    mEditor = editor;
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
    MoveMode mode = MoveMode::NONE;

    if (QLineF(pos, op.getSinglePoint()).length() < calculatedSelectionTol)
    {
        mode = MoveMode::PERSP_SINGLE;
    }
    else if (QLineF(pos, op.getLeftPoint()).length() < calculatedSelectionTol)
    {
        mode = MoveMode::PERSP_LEFT;
    }
    else if (QLineF(pos, op.getRightPoint()).length() < calculatedSelectionTol)
    {
        mode = MoveMode::PERSP_RIGHT;
    }
    else if (QLineF(pos, op.getMiddlePoint()).length() < calculatedSelectionTol)
    {
        mode = MoveMode::PERSP_MIDDLE;
    }

    return mode;
}

double OverlayManager::selectionTolerance()
{
    return qAbs(mSelectionTolerance * mEditor->viewScaleInversed());
}

// Must only be called at startup!
void OverlayManager::initPerspOverlay()
{
    setSinglePerspPoint(QPointF(0.1, 0.1));
    setLeftPerspPoint(QPointF(-300.0, 0.0));
    setRightPerspPoint(QPointF(300.0, 0.0));
    setMiddlePerspPoint(QPointF(0.0, 200.0));
}

void OverlayManager::updatePerspOverlay(int persp)
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

    mEditor->getScribbleArea()->prepOverlays();
    mEditor->getScribbleArea()->renderOverlays();
}

void OverlayManager::updatePerspOverlay(QPointF point)
{
    MoveMode mode = op.getMoveMode();

    switch (mode) {
    case MoveMode::PERSP_SINGLE:
        setSinglePerspPoint(point);
        break;
    case MoveMode::PERSP_LEFT:
        setLeftPerspPoint(point);
        setRightPerspPoint(QPointF(op.getRightPoint().x(), point.y()));
        break;
    case MoveMode::PERSP_RIGHT:
        setRightPerspPoint(point);
        setLeftPerspPoint(QPointF(op.getLeftPoint().x(), point.y()));
        break;
    case MoveMode::PERSP_MIDDLE:
        setMiddlePerspPoint(point);
        break;
    default:
        break;
    }
    mEditor->getScribbleArea()->prepOverlays();
    mEditor->getScribbleArea()->renderOverlays();
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

MoveMode OverlayManager::getMoveMode()
{
    return op.getMoveMode();
}

void OverlayManager::setMoveMode(MoveMode mode)
{
    op.setMoveMode(mode);

    switch (mode) {
    case MoveMode::PERSP_LEFT:
        mLastRightPoint = op.getRightPoint();
        mLastMiddlePoint = op.getMiddlePoint();
        break;
    case MoveMode::PERSP_RIGHT:
        mLastLeftPoint = op.getLeftPoint();
        mLastMiddlePoint = op.getMiddlePoint();
        break;
    case MoveMode::PERSP_MIDDLE:
        mLastRightPoint = op.getRightPoint();
        mLastLeftPoint = op.getLeftPoint();
        break;
    default:
        break;
    }
}

void OverlayManager::setSinglePerspPoint(QPointF point)
{
    op.setSinglePoint(point.toPoint());
}

QPointF OverlayManager::getSinglePerspPoint()
{
    return op.getSinglePoint();
}

void OverlayManager::setLeftPerspPoint(QPointF point)
{
    op.setLeftPoint(point.toPoint());
}

QPointF OverlayManager::getLeftPerspPoint()
{
    return op.getLeftPoint();
}

void OverlayManager::setRightPerspPoint(QPointF point)
{
    op.setRightPoint(point.toPoint());
}

QPointF OverlayManager::getRightPerspPoint()
{
    return op.getRightPoint();
}

void OverlayManager::setMiddlePerspPoint(QPointF point)
{
    op.setMiddlePoint(point.toPoint());
}

QPointF OverlayManager::getMiddlePerspPoint()
{
    return op.getMiddlePoint();
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
