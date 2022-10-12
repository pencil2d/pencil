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
#include "movemode.h"
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
    connect(mEditor->layers(), &LayerManager::currentLayerChanged, this, &CameraTool::onDidChangeLayer);
    connect(mEditor, &Editor::objectLoaded, this, &CameraTool::onDidLoadObject);

    mRotationIncrement = mEditor->preference()->getInt(SETTING::ROTATION_INCREMENT);

    connect(mEditor->preference(), &PreferenceManager::optionChanged, this, &CameraTool::updateSettings);
}

void CameraTool::onDidLoadObject()
{
    onDidChangeLayer(mEditor->currentLayerIndex());
}

void CameraTool::onDidChangeLayer(int index)
{
    Layer* layer = mEditor->layers()->getLayer(index);
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
    // When pointer is in use, we can't change state, so keep the last image.
    if (mScribbleArea->isPointerInUse()) { return cursorCache; }

    QPixmap cursorPixmap = QPixmap(24, 24);
    cursorPixmap.fill(QColor(255, 255, 255, 0));
    QPainter cursorPainter(&cursorPixmap);
    cursorPainter.setRenderHint(QPainter::HighQualityAntialiasing);

    switch(moveMode())
    {
    case MoveMode::PERSP_LEFT:
    case MoveMode::PERSP_RIGHT:
    case MoveMode::PERSP_MIDDLE:
    case MoveMode::PERSP_SINGLE:
    {
        cursorPainter.drawImage(QPoint(6,6),QImage("://icons/new/arrow-selectmove.png"));
        break;
    }
    case MoveMode::TOPLEFT:
    case MoveMode::BOTTOMRIGHT:
    {
        cursorPainter.drawImage(QPoint(6,6),QImage("://icons/new/arrow-diagonalleft.png"));
        break;
    }
    case MoveMode::TOPRIGHT:
    case MoveMode::BOTTOMLEFT:
    {
        cursorPainter.drawImage(QPoint(6,6),QImage("://icons/new/arrow-diagonalright.png"));
        break;
    }
    case MoveMode::ROTATIONLEFT:
    case MoveMode::ROTATIONRIGHT:
    case MoveMode::ROTATION:
    {
        cursorPainter.drawImage(QPoint(6,6),QImage("://icons/new/arrow-rotate.png"));
        break;
    }
    case MoveMode::MIDDLE:
    case MoveMode::CENTER:
    {
        cursorPainter.drawImage(QPoint(6,6),QImage("://icons/new/arrow-selectmove.png"));
        break;
    }
    default:
        return Qt::ArrowCursor;
    }
    cursorPainter.end();

    cursorCache = QCursor(cursorPixmap);

    return cursorCache;
}

MoveMode CameraTool::moveMode()
{
    Layer* layer = mEditor->layers()->currentLayer();
    MoveMode mode = MoveMode::NONE;
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
    } else {
        int keyPos = cam->firstKeyFramePosition();
        while (keyPos <= cam->getMaxKeyFramePosition())
        {
            mode = getPathMoveMode(cam,
                                   keyPos,
                                   currentPoint,
                                   selectionTolerance);
            mCamPathMoveMode = mode;
            if (mode != MoveMode::NONE && !cam->hasSameTranslation(keyPos, cam->getPreviousKeyFramePosition(keyPos)))
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

    layer->setDotColorType(pathDotColor);
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

    layer->resetCameraAtFrame(option, mEditor->currentFrame());
    emit mEditor->frameModified(mEditor->currentFrame());
}

void CameraTool::transformCamera(Qt::KeyboardModifiers keyMod)
{
    Q_ASSERT(editor()->layers()->currentLayer()->type() == Layer::CAMERA);
    LayerCamera* layer = static_cast<LayerCamera*>(editor()->layers()->currentLayer());

    QRectF viewRect = layer->getViewAtFrame(mEditor->currentFrame()).inverted().mapRect(layer->getViewRect());
    qreal angleRad = mCamMoveMode == MoveMode::ROTATIONLEFT ? MathUtils::getDifferenceAngle(getCurrentPoint(),viewRect.center()) : MathUtils::getDifferenceAngle(viewRect.center(), getCurrentPoint());
    qreal angle = qRadiansToDegrees(angleRad);
    if (keyMod == Qt::ShiftModifier && (mCamMoveMode == MoveMode::ROTATIONLEFT || mCamMoveMode == MoveMode::ROTATIONRIGHT)) {
        angle = constrainedRotation(angle, mRotationIncrement);
    }

    transformView(layer, mCamMoveMode, getCurrentPoint(), mTransformOffset, -angle, mEditor->currentFrame());

    emit mEditor->frameModified(mEditor->currentFrame());
    mTransformOffset = getCurrentPoint();
}

void CameraTool::transformCameraPath()
{
    Q_ASSERT(editor()->layers()->currentLayer()->type() == Layer::CAMERA);
    LayerCamera* layer = static_cast<LayerCamera*>(editor()->layers()->currentLayer());
    layer->updatePathControlPointAtFrame(getCurrentPoint(), mDragPathFrame);
    emit mEditor->frameModified(mEditor->currentFrame());
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
        else if (mCamPathMoveMode == MoveMode::MIDDLE)
        {
            transformCameraPath();
        }
    }
    else
    {
        // the user is moving the mouse without pressing it
        // update cursor to reflect selection corner interaction
        mScribbleArea->updateToolCursor();
    }
    mEditor->view()->forceUpdateViewTransform();
    mEditor->updateCurrentFrame();
}

void CameraTool::pointerReleaseEvent(PointerEvent* event)
{
    LayerCamera* layer = static_cast<LayerCamera*>(editor()->layers()->currentLayer());
    Q_ASSERT(layer->type() == Layer::CAMERA);

    if (layer->keyExists(mEditor->currentFrame())) {
        transformCamera(event->modifiers());
        mEditor->view()->forceUpdateViewTransform();
        mEditor->updateCurrentFrame();
    } else if (mCamPathMoveMode == MoveMode::MIDDLE) {
        transformCameraPath();
        layer->setPathMovedAtFrame(mEditor->currentFrame(), true);
        mEditor->view()->forceUpdateViewTransform();
        mEditor->updateCurrentFrame();
    }
}

MoveMode CameraTool::getCameraMoveMode(const LayerCamera* layerCamera, int frameNumber, const QPointF& point, qreal tolerance) const
{
    QTransform curCam = layerCamera->getViewAtFrame(frameNumber);
    QPolygon camPoly = curCam.inverted().mapToPolygon(layerCamera->getViewRect());
    if (QLineF(point, camPoly.at(0)).length() < tolerance)
    {
        return MoveMode::TOPLEFT;
    }
    else if (QLineF(point, camPoly.at(1)).length() < tolerance)
    {
        return MoveMode::TOPRIGHT;
    }
    else if (QLineF(point, camPoly.at(2)).length() < tolerance)
    {
        return MoveMode::BOTTOMRIGHT;
    }
    else if (QLineF(point, camPoly.at(3)).length() < tolerance)
    {
        return MoveMode::BOTTOMLEFT;
    }
    else if (QLineF(point, QPoint(camPoly.at(1) + (camPoly.at(2) - camPoly.at(1)) / 2)).length() < tolerance)
    {
        return MoveMode::ROTATIONRIGHT;
    }
    else if (QLineF(point, QPoint(camPoly.at(0) + (camPoly.at(3) - camPoly.at(0)) / 2)).length() < tolerance)
    {
        return MoveMode::ROTATIONLEFT;
    }
    else if (camPoly.containsPoint(point.toPoint(), Qt::FillRule::OddEvenFill))
    {
        return MoveMode::CENTER;
    }
    return MoveMode::NONE;
}

MoveMode CameraTool::getPathMoveMode(const LayerCamera* layerCamera, int frameNumber, const QPointF& point, qreal tolerance) const
{
    int prev = layerCamera->getPreviousKeyFramePosition(frameNumber);
    int next = layerCamera->getNextKeyFramePosition(frameNumber);
    if (layerCamera->hasSameTranslation(prev, next))
        return MoveMode::NONE;

    Camera* camera = layerCamera->getCameraAtFrame(prev);
    Q_ASSERT(camera);

    if (QLineF(camera->getPathControlPoint(), point).length() < tolerance)
        return MoveMode::MIDDLE;
    return MoveMode::NONE;
}

void CameraTool::transformView(LayerCamera* layerCamera, MoveMode mode, const QPointF& point, const QPointF& offset, qreal angle, int frameNumber) const
{
    QPolygon curPoly = layerCamera->getViewAtFrame(frameNumber).inverted().mapToPolygon(layerCamera->getViewRect());
    QPoint curCenter = QLineF(curPoly.at(0), curPoly.at(2)).pointAt(0.5).toPoint();
    QLineF lineOld(curCenter, point);
    QLineF lineNew(curCenter, point);
    Camera* curCam = layerCamera->getCameraAtFrame(frameNumber);

    switch (mode)
    {
    case MoveMode::CENTER: {
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
    case MoveMode::TOPLEFT:
        lineOld.setP2(curPoly.at(0));
        curCam->scale(curCam->scaling() * (lineOld.length() / lineNew.length()));
        break;
    case MoveMode::TOPRIGHT:
        lineOld.setP2(curPoly.at(1));
        curCam->scale(curCam->scaling() * (lineOld.length() / lineNew.length()));
        break;
    case MoveMode::BOTTOMRIGHT:
        lineOld.setP2(curPoly.at(2));
        curCam->scale(curCam->scaling() * (lineOld.length() / lineNew.length()));
        break;
    case MoveMode::BOTTOMLEFT:
        lineOld.setP2(curPoly.at(3));
        curCam->scale(curCam->scaling() * (lineOld.length() / lineNew.length()));
        break;
    case MoveMode::ROTATIONRIGHT:
    case MoveMode::ROTATIONLEFT: {
        curCam->rotate(angle);
        break;
    }
    default:
        break;
    }
    curCam->updateViewTransform();
    curCam->modification();
}

