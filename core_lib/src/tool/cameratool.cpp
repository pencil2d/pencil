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


#include "cameratool.h"

#include "editor.h"
#include "pointerevent.h"
#include "layermanager.h"
#include "selectionmanager.h"
#include "viewmanager.h"
#include "layercamera.h"
#include "mathutils.h"
#include "layercamera.h"
#include "camera.h"

#include "scribblearea.h"

#include <QPixmap>
#include <QPainter>
#include <QSettings>

CameraTool::CameraTool(QObject* object) : BaseTool(object)
{

}

CameraTool::~CameraTool()
{
}

void CameraTool::loadSettings()
{
    mPropertyEnabled[CAMERAPATH] = true;
    connect(mEditor->layers(), &LayerManager::currentLayerChanged, this, &CameraTool::updateProperties);
    connect(mEditor, &Editor::objectLoaded, this, &CameraTool::updateProperties);

    mRotationIncrement = mEditor->preference()->getInt(SETTING::ROTATION_INCREMENT);

    connect(mEditor->preference(), &PreferenceManager::optionChanged, this, &CameraTool::updateSettings);
}

void CameraTool::updateProperties()
{
    Layer* layer = mEditor->layers()->getLayer(mEditor->currentLayerIndex());
    if (!layer || layer->type() != Layer::CAMERA) { return; }

    LayerCamera* layerCam = static_cast<LayerCamera*>(layer);
    properties.cameraPathDotColorType = layerCam->getDotColorType();
    properties.cameraShowPath = layerCam->getShowCameraPath();
}

void CameraTool::updateSettings(const SETTING setting)
{
    switch (setting)
    {
    case SETTING::ROTATION_INCREMENT:
    {
        mRotationIncrement = mEditor->preference()->getInt(SETTING::ROTATION_INCREMENT);
        break;
    }
    default:
        break;

    }
}

QCursor CameraTool::cursor()
{
    QPixmap cursorPixmap = QPixmap(24, 24);
    cursorPixmap.fill(Qt::transparent);
    QPainter cursorPainter(&cursorPixmap);

    QImage moveTypeImage;
    QPoint offset = QPoint(6, 6);
    switch(moveMode())
    {
    case CameraMoveType::TOPLEFT:
    case CameraMoveType::BOTTOMRIGHT:
    {
        moveTypeImage = QImage("://icons/new/arrow-diagonalleft.png");
        break;
    }
    case CameraMoveType::TOPRIGHT:
    case CameraMoveType::BOTTOMLEFT:
    {
        moveTypeImage = QImage("://icons/new/arrow-diagonalright.png");
        break;
    }
    case CameraMoveType::ROTATION:
    {
        moveTypeImage = QImage("://icons/new/arrow-rotate.png");
        break;
    }
    case CameraMoveType::PATH:
    case CameraMoveType::CENTER:
    {
        moveTypeImage = QImage("://icons/new/arrow-selectmove.png");
        break;
    }
    default:
        return Qt::ArrowCursor;
    }

    QTransform rotT;
    QPointF center = QPointF(moveTypeImage.size().width()*0.5, moveTypeImage.size().height()*0.5);

    // rotate around center
    rotT.translate(center.x() + offset.x(), center.y() + offset.y());
    rotT.rotate(mCurrentAngle);
    rotT.translate(-center.x() - offset.x(), -center.y() - offset.y());
    cursorPainter.setTransform(rotT);
    cursorPainter.drawImage(offset, moveTypeImage);
    cursorPainter.end();

    return QCursor(cursorPixmap);
}

CameraMoveType CameraTool::moveMode()
{

    if (mScribbleArea->isPointerInUse()) {
        // Pointer in use, return previous used mode
        return mCamMoveMode;
    }

    Layer* layer = mEditor->layers()->currentLayer();
    CameraMoveType mode = CameraMoveType::NONE;
    qreal selectionTolerance = mEditor->select()->selectionTolerance();
    QPointF currentPoint = getCurrentPoint();

    Q_ASSERT(layer->type() == Layer::CAMERA);
    LayerCamera* cam = static_cast<LayerCamera*>(layer);
    if (layer->keyExists(mEditor->currentFrame()))
    {
        mode = getCameraMoveMode(cam,
                           mEditor->currentFrame(),
                           currentPoint,
                           selectionTolerance);
        mCamMoveMode = mode;
    } else if (properties.cameraShowPath) {
        int keyPos = cam->firstKeyFramePosition();
        while (keyPos <= cam->getMaxKeyFramePosition())
        {
            mode = getPathMoveMode(cam,
                                   keyPos,
                                   currentPoint,
                                   selectionTolerance);
            mCamPathMoveMode = mode;
            if (mode != CameraMoveType::NONE && !cam->hasSameTranslation(keyPos, cam->getPreviousKeyFramePosition(keyPos)))
            {
                mDragPathFrame = keyPos;
                break;
            }

            if (keyPos == cam->getNextKeyFramePosition(keyPos)) {
                break;
            }

            keyPos = cam->getNextKeyFramePosition(keyPos);
        }
    }
    return mode;
}

void CameraTool::setShowCameraPath(const bool showCameraPath)
{
    LayerCamera* layer = static_cast<LayerCamera*>(editor()->layers()->currentLayer());

    Q_ASSERT(layer->type() == Layer::CAMERA);
    layer->setShowCameraPath(showCameraPath);

    properties.cameraShowPath = showCameraPath;
}

void CameraTool::setPathDotColorType(const DotColorType pathDotColor)
{
    LayerCamera* layer = static_cast<LayerCamera*>(editor()->layers()->currentLayer());
    Q_ASSERT(layer->type() == Layer::CAMERA);

    layer->updateDotColor(pathDotColor);
}

void CameraTool::resetCameraPath()
{
    LayerCamera* layer = static_cast<LayerCamera*>(editor()->layers()->currentLayer());
    Q_ASSERT(layer->type() == Layer::CAMERA);

    layer->centerPathControlPointAtFrame(mEditor->currentFrame());
    layer->setPathMovedAtFrame(mEditor->currentFrame(), false);
}

void CameraTool::resetTransform(CameraFieldOption option)
{
    LayerCamera* layer = static_cast<LayerCamera*>(editor()->layers()->currentLayer());
    Q_ASSERT(layer->type() == Layer::CAMERA);

    if (option == CameraFieldOption::RESET_ROTATION || option == CameraFieldOption::RESET_FIELD) {
        mCurrentAngle = 0;
    }

    layer->resetCameraAtFrame(option, mEditor->currentFrame());
    emit mEditor->frameModified(mEditor->currentFrame());
}

void CameraTool::transformCamera(Qt::KeyboardModifiers keyMod)
{
    Q_ASSERT(editor()->layers()->currentLayer()->type() == Layer::CAMERA);
    LayerCamera* layer = static_cast<LayerCamera*>(editor()->layers()->currentLayer());

    qreal angleDeg = 0;
    if (mCamMoveMode == CameraMoveType::ROTATION) {
        QTransform cameraT = layer->getViewAtFrame(mEditor->currentFrame()).inverted();
        QRectF viewRect = cameraT.mapRect(layer->getViewRect());
        angleDeg = getAngleBetween(getCurrentPoint(), viewRect.center()) - mStartAngle;
        if (keyMod == Qt::ShiftModifier) {
            angleDeg = constrainedRotation(angleDeg, mRotationIncrement);
        }
        mCurrentAngle = angleDeg;
    }

    transformView(layer, mCamMoveMode, getCurrentPoint(), mTransformOffset, -angleDeg, mEditor->currentFrame());

    mEditor->updateCurrentFrame();
    mTransformOffset = getCurrentPoint();
}

void CameraTool::transformCameraPath()
{
    Q_ASSERT(editor()->layers()->currentLayer()->type() == Layer::CAMERA);
    LayerCamera* layer = static_cast<LayerCamera*>(editor()->layers()->currentLayer());
    layer->updatePathControlPointAtFrame(getCurrentPoint(), mDragPathFrame);
    mEditor->updateCurrentFrame();
}

int CameraTool::constrainedRotation(const qreal rotatedAngle, const int rotationIncrement) const
{
    return qRound(rotatedAngle / rotationIncrement) * rotationIncrement;
}

void CameraTool::pointerPressEvent(PointerEvent*)
{
    LayerCamera* layer = static_cast<LayerCamera*>(editor()->layers()->currentLayer());

    Q_ASSERT(layer->type() == Layer::CAMERA);

    if (!layer->keyExists(mEditor->currentFrame())) { return; }

    QTransform cameraT = layer->getViewAtFrame(mEditor->currentFrame()).inverted();
    QRectF projectedViewRect = cameraT.mapRect(layer->getViewRect());
    mStartAngle = getAngleBetween(getCurrentPoint(), projectedViewRect.center()) - mCurrentAngle;

    mDragPathFrame = mEditor->currentFrame();
    mTransformOffset = getCurrentPoint();
}

void CameraTool::pointerMoveEvent(PointerEvent* event)
{
    Q_ASSERT(editor()->layers()->currentLayer()->type() == Layer::CAMERA);
    LayerCamera* layer = static_cast<LayerCamera*>(editor()->layers()->currentLayer());

    if (mScribbleArea->isPointerInUse())   // the user is also pressing the mouse (dragging)
    {
        if (layer->keyExists(mEditor->currentFrame())) {
            transformCamera(event->modifiers());
        }
        else if (mCamPathMoveMode == CameraMoveType::PATH)
        {
            transformCameraPath();
        }
    }
    mScribbleArea->updateToolCursor();
    mEditor->view()->forceUpdateViewTransform();
    mEditor->updateCurrentFrame();
}

void CameraTool::pointerReleaseEvent(PointerEvent* event)
{
    LayerCamera* layer = static_cast<LayerCamera*>(editor()->layers()->currentLayer());
    Q_ASSERT(layer->type() == Layer::CAMERA);

    int frame = mEditor->currentFrame();
    if (layer->keyExists(frame)) {
        transformCamera(event->modifiers());
        mEditor->view()->forceUpdateViewTransform();
    } else if (mCamPathMoveMode == CameraMoveType::PATH) {
        transformCameraPath();
        layer->setPathMovedAtFrame(frame, true);
        mEditor->view()->forceUpdateViewTransform();
    }
    emit mEditor->frameModified(frame);
}

qreal CameraTool::getAngleBetween(const QPointF& pos1, const QPointF& pos2) const
{
    return qRadiansToDegrees(MathUtils::getDifferenceAngle(pos1, pos2));
}

CameraMoveType CameraTool::getCameraMoveMode(const LayerCamera* layerCamera, int frameNumber, const QPointF& point, qreal tolerance) const
{
    QTransform curCam = layerCamera->getViewAtFrame(frameNumber);
    QPolygon camPoly =  curCam.inverted().mapToPolygon(layerCamera->getViewRect());

    float offsetLimiter = (0.8 * mEditor->viewScaleInversed());
    QPointF rotationHandle = curCam.inverted().map(QPoint(0, (-layerCamera->getViewRect().height()*0.5 - (offsetLimiter) * RotationHandleOffset)));
    if (QLineF(point, camPoly.at(0)).length() < tolerance)
    {
        return CameraMoveType::TOPLEFT;
    }
    else if (QLineF(point, camPoly.at(1)).length() < tolerance)
    {
        return CameraMoveType::TOPRIGHT;
    }
    else if (QLineF(point, camPoly.at(2)).length() < tolerance)
    {
        return CameraMoveType::BOTTOMRIGHT;
    }
    else if (QLineF(point, camPoly.at(3)).length() < tolerance)
    {
        return CameraMoveType::BOTTOMLEFT;
    }
    else if (QLineF(point, rotationHandle).length() < tolerance)
    {
        return CameraMoveType::ROTATION;
    }
    else if (camPoly.containsPoint(point.toPoint(), Qt::FillRule::OddEvenFill))
    {
        return CameraMoveType::CENTER;
    }
    return CameraMoveType::NONE;
}

CameraMoveType CameraTool::getPathMoveMode(const LayerCamera* layerCamera, int frameNumber, const QPointF& point, qreal tolerance) const
{
    int prev = layerCamera->getPreviousKeyFramePosition(frameNumber);
    int next = layerCamera->getNextKeyFramePosition(frameNumber);
    if (layerCamera->hasSameTranslation(prev, next))
        return CameraMoveType::NONE;

    Camera* camera = layerCamera->getCameraAtFrame(prev);

    if (camera == nullptr) { return CameraMoveType::NONE; }

    if (QLineF(camera->getPathControlPoint(), point).length() < tolerance) {
        return CameraMoveType::PATH;
    }
    return CameraMoveType::NONE;
}

void CameraTool::transformView(LayerCamera* layerCamera, CameraMoveType mode, const QPointF& point, const QPointF& offset, qreal angle, int frameNumber) const
{
    QPolygon curPoly = layerCamera->getViewAtFrame(frameNumber).inverted().mapToPolygon(layerCamera->getViewRect());
    QPoint curCenter = QLineF(curPoly.at(0), curPoly.at(2)).pointAt(0.5).toPoint();
    QLineF lineOld(curCenter, point);
    QLineF lineNew(curCenter, point);
    Camera* curCam = layerCamera->getCameraAtFrame(frameNumber);

    switch (mode)
    {
    case CameraMoveType::CENTER: {
        curCam->translate(curCam->translation() - (point - offset));

        int prevFrame = layerCamera->getPreviousKeyFramePosition(frameNumber);
        Camera* prevCam = layerCamera->getCameraAtFrame(prevFrame);

        // Only center a control points if it hasn't been moved
        if (!curCam->pathControlPointMoved()) {
            curCam->setPathControlPoint(layerCamera->getNewPathControlPointAtFrame(frameNumber));
        }
        if (!prevCam->pathControlPointMoved()) {
            prevCam->setPathControlPoint(layerCamera->getNewPathControlPointAtFrame(prevFrame));
        }
        break;
    }
    case CameraMoveType::TOPLEFT:
        lineOld.setP2(curPoly.at(0));
        curCam->scale(curCam->scaling() * (lineOld.length() / lineNew.length()));
        break;
    case CameraMoveType::TOPRIGHT:
        lineOld.setP2(curPoly.at(1));
        curCam->scale(curCam->scaling() * (lineOld.length() / lineNew.length()));
        break;
    case CameraMoveType::BOTTOMRIGHT:
        lineOld.setP2(curPoly.at(2));
        curCam->scale(curCam->scaling() * (lineOld.length() / lineNew.length()));
        break;
    case CameraMoveType::BOTTOMLEFT:
        lineOld.setP2(curPoly.at(3));
        curCam->scale(curCam->scaling() * (lineOld.length() / lineNew.length()));
        break;
    case CameraMoveType::ROTATION:
        curCam->rotate(angle);
        break;
    default:
        break;
    }
    curCam->updateViewTransform();
    curCam->modification();
}

