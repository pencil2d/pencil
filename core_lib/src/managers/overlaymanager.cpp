#include "overlaymanager.h"
#include <QtMath>

OverlayManager::OverlayManager(Editor *editor): BaseManager(editor)
{
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

void OverlayManager::resetPerspectiveOverlays()
{

}

double OverlayManager::getAngleLeftRight(QPointF left, QPointF right)
{
    double deltaX = right.x() - left.x();
    double deltaY = right.y() - left.y();
    return qRadiansToDegrees(qAtan(deltaY/deltaX));
}
