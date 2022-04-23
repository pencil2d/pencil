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
#include "toolmanager.h"
#include "selectionmanager.h"
#include "viewmanager.h"
#include "camera.h"
#include "layercamera.h"
#include "movemode.h"
#include "mathutils.h"

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

    QSettings settings(PENCIL2D, PENCIL2D);

    mRotationIncrement = mEditor->preference()->getInt(SETTING::ROTATION_INCREMENT);

    connect(mEditor->preference(), &PreferenceManager::optionChanged, this, &CameraTool::updateSettings);
}

void CameraTool::onDidLoadObject()
{
    Layer* layer = mEditor->layers()->getLayer(mEditor->currentLayerIndex());
    if (!layer || layer->type() != Layer::CAMERA) { return ; }

    LayerCamera* layerCam = static_cast<LayerCamera*>(layer);
    properties.cameraPathDotColorType = static_cast<int>(layerCam->getDotColorType());
    properties.cameraShowPath = layerCam->getShowCameraPath();
}

void CameraTool::onDidChangeLayer(int index)
{
    Layer* layer = mEditor->layers()->getLayer(index);
    if (layer->type() != Layer::CAMERA) { return ; }

    LayerCamera* layerCam = static_cast<LayerCamera*>(layer);
    properties.cameraPathDotColorType = static_cast<int>(layerCam->getDotColorType());
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
    if (!cursorPixmap.isNull())
    {
        cursorPixmap.fill(QColor(255, 255, 255, 0));
        QPainter cursorPainter(&cursorPixmap);
        cursorPainter.setRenderHint(QPainter::HighQualityAntialiasing);

        switch(cursorForMoveMode())
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
    }
    return QCursor(cursorPixmap);
}

MoveMode CameraTool::cursorForMoveMode()
{
    Layer* layer = mEditor->layers()->currentLayer();
    MoveMode mode = MoveMode::NONE;
    qreal selectionTolerance = mEditor->select()->selectionTolerance();
    QPointF currentPoint = getCurrentPoint();

    LayerCamera* cam = static_cast<LayerCamera*>(layer);
    if (layer->keyExists(mEditor->currentFrame()))
    {
        mode = cam->getMoveModeForCamera(mEditor->currentFrame(),
                                         currentPoint,
                                         selectionTolerance);
        mCamMoveMode = mode;
    } else {
        int keyPos = cam->firstKeyFramePosition();
        while (keyPos <= cam->getMaxKeyFramePosition())
        {
            mode = cam->getMoveModeForCameraPath(keyPos,
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

void CameraTool::setPathDotColorType(const int pathDotColor)
{
    LayerCamera* layer = static_cast<LayerCamera*>(editor()->layers()->currentLayer());
    Q_ASSERT(layer->type() == Layer::CAMERA);

    DotColorType color = static_cast<DotColorType>(pathDotColor);
    layer->setDotColorType(color);
}

void CameraTool::resetCameraPath()
{
    LayerCamera* layer = static_cast<LayerCamera*>(editor()->layers()->currentLayer());
    Q_ASSERT(layer->type() == Layer::CAMERA);

    layer->centerPathControlPoint(mEditor->currentFrame());
    layer->setPathMovedAtFrame(mEditor->currentFrame(), false);
}

void CameraTool::resetTransform(CameraFieldOption option)
{
    LayerCamera* layer = static_cast<LayerCamera*>(editor()->layers()->currentLayer());
    Q_ASSERT(layer->type() == Layer::CAMERA);

    layer->setCameraReset(option, mEditor->currentFrame());
    mEditor->frameModified(mEditor->currentFrame());
}

void CameraTool::transformCamera(Qt::KeyboardModifiers keyMod)
{
    LayerCamera* layer = static_cast<LayerCamera*>(editor()->layers()->currentLayer());

    QRectF viewRect = layer->getViewAtFrame(mEditor->currentFrame()).inverted().mapRect(layer->getViewRect());
    qreal angleRad = mCamMoveMode == MoveMode::ROTATIONLEFT ? MathUtils::getDifferenceAngle(getCurrentPoint(),viewRect.center()) : MathUtils::getDifferenceAngle(viewRect.center(), getCurrentPoint());
    qreal angle = qRadiansToDegrees(angleRad);
    if (keyMod == Qt::ShiftModifier && (mCamMoveMode == MoveMode::ROTATIONLEFT || mCamMoveMode == MoveMode::ROTATIONRIGHT)) {
        angle = constrainedRotation(angle, mRotationIncrement);
    }

    layer->transformCameraView(mCamMoveMode, getCurrentPoint(), mTransformOffset, -angle, mEditor->currentFrame());

    mEditor->frameModified(mEditor->currentFrame());
    mTransformOffset = getCurrentPoint();
}

void CameraTool::transformCameraPath()
{
    LayerCamera* layer = static_cast<LayerCamera*>(editor()->layers()->currentLayer());
    layer->updatePathControlPointAtFrame(getCurrentPoint(), mDragPathFrame);
    mEditor->frameModified(mEditor->currentFrame());
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
    LayerCamera* layer = static_cast<LayerCamera*>(editor()->layers()->currentLayer());
    if (layer == nullptr) return;

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
