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

#include "object.h"
#include "editor.h"
#include "pointerevent.h"
#include "layermanager.h"
#include "selectionmanager.h"
#include "playbackmanager.h"
#include "viewmanager.h"
#include "layercamera.h"
#include "mathutils.h"
#include "transform.h"
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

    mHandleColor = Qt::white;
    mHandleDisabledColor = Qt::black;
    mHandleTextColor = QColor(0, 0, 0);

    mHandlePen = QPen();
    mHandlePen.setColor(QColor(0, 0, 0, 255));
    mHandlePen.setWidth(2);
}

void CameraTool::updateUIAssists(const Layer* layer)
{
    const LayerCamera* camLayer = static_cast<const LayerCamera*>(layer);

    Q_ASSERT(layer->type() == Layer::CAMERA);

    int currentFrame = mEditor->currentFrame();
    if (!layer->keyExists(currentFrame)) { return; }

    const QTransform& localCamT = camLayer->getViewAtFrame(currentFrame);
    const QRect& cameraRect = camLayer->getViewRect();

    mCameraRect = Transform::mapFromLocalRect(localCamT, cameraRect);
    mCameraPolygon = Transform::mapFromLocalPolygon(localCamT, cameraRect);

    Camera* cam = camLayer->getLastCameraAtFrame(mEditor->currentFrame(), 0);
    if (cam) {
        mRotationHandlePoint = localRotationHandlePoint(cameraRect.topLeft(), localCamT, cam->scaling(), mEditor->view()->getViewScaleInverse());
    }
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
    switch(mCamMoveMode)
    {
    case CameraMoveType::TOPLEFT:
    case CameraMoveType::BOTTOMRIGHT:
    {
        moveTypeImage = QImage("://icons/general/cursor-diagonal-left.svg");
        break;
    }
    case CameraMoveType::TOPRIGHT:
    case CameraMoveType::BOTTOMLEFT:
    {
        moveTypeImage = QImage("://icons/general/cursor-diagonal-right.svg");
        break;
    }
    case CameraMoveType::ROTATION:
    {
        moveTypeImage = QImage("://icons/general/cursor-rotate.svg");
        break;
    }
    case CameraMoveType::PATH:
    case CameraMoveType::CENTER:
    {
        moveTypeImage = QImage("://icons/general/cursor-move.svg");
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

void CameraTool::updateMoveMode(const QPointF& pos)
{

    if (mScribbleArea->isPointerInUse()) {
        // Pointer in use, keep previous used mode
        return;
    }

    Layer* layer = mEditor->layers()->currentLayer();
    mCamMoveMode = CameraMoveType::NONE;
    qreal selectionTolerance = mEditor->select()->selectionTolerance();

    Q_ASSERT(layer->type() == Layer::CAMERA);
    LayerCamera* cam = static_cast<LayerCamera*>(layer);
    if (layer->keyExists(mEditor->currentFrame()))
    {
        mCamMoveMode = getCameraMoveMode(pos,
                                         selectionTolerance);
    } else if (properties.cameraShowPath) {
        int keyPos = cam->firstKeyFramePosition();
        while (keyPos <= cam->getMaxKeyFramePosition())
        {
            mCamMoveMode = getPathMoveMode(cam,
                                           keyPos,
                                           pos,
                                           selectionTolerance);
            if (mCamMoveMode != CameraMoveType::NONE)
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

    layer->setPathMovedAtFrame(mEditor->currentFrame(), false);
    mEditor->updateFrame();
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

void CameraTool::transformCamera(const QPointF& pos, Qt::KeyboardModifiers keyMod)
{
    Q_ASSERT(editor()->layers()->currentLayer()->type() == Layer::CAMERA);
    LayerCamera* layer = static_cast<LayerCamera*>(editor()->layers()->currentLayer());

    qreal angleDeg = 0;
    if (mCamMoveMode == CameraMoveType::ROTATION) {
        angleDeg = getAngleBetween(pos, mCameraRect.center()) - mStartAngle;
        if (keyMod == Qt::ShiftModifier) {
            angleDeg = constrainedRotation(angleDeg, mRotationIncrement);
        }
        mCurrentAngle = angleDeg;
    }

    transformView(layer, mCamMoveMode, pos, mTransformOffset, -angleDeg, mEditor->currentFrame());

    mEditor->updateFrame();
    mTransformOffset = pos;
}

void CameraTool::transformCameraPath(const QPointF& pos)
{
    Q_ASSERT(editor()->layers()->currentLayer()->type() == Layer::CAMERA);
    LayerCamera* layer = static_cast<LayerCamera*>(editor()->layers()->currentLayer());

    layer->updatePathControlPointAtFrame(pos, mDragPathFrame);
    mEditor->updateFrame();
}

int CameraTool::constrainedRotation(const qreal rotatedAngle, const int rotationIncrement) const
{
    return qRound(rotatedAngle / rotationIncrement) * rotationIncrement;
}

void CameraTool::pointerPressEvent(PointerEvent* event)
{
    updateMoveMode(event->canvasPos());
    updateUIAssists(mEditor->layers()->currentLayer());

    mStartAngle = getAngleBetween(event->canvasPos(), mCameraRect.center()) - mCurrentAngle;
    mTransformOffset = event->canvasPos();
}

void CameraTool::pointerMoveEvent(PointerEvent* event)
{
    Layer* currentLayer = mEditor->layers()->currentLayer();
    updateMoveMode(event->canvasPos());
    updateUIAssists(currentLayer);

    if (mScribbleArea->isPointerInUse())   // the user is also pressing the mouse (dragging)
    {
        if (currentLayer->keyExists(mEditor->currentFrame())) {
            transformCamera(event->canvasPos(), event->modifiers());
        }
        else if (mCamMoveMode == CameraMoveType::PATH)
        {
            transformCameraPath(event->canvasPos());
        }
    }
    mScribbleArea->updateToolCursor();
    mEditor->view()->forceUpdateViewTransform();
    mEditor->updateFrame();
}

void CameraTool::pointerReleaseEvent(PointerEvent* event)
{
    Layer* layer = editor()->layers()->currentLayer();
    updateMoveMode(event->canvasPos());
    updateUIAssists(layer);

    int frame = mEditor->currentFrame();
    if (layer->keyExists(frame)) {
        transformCamera(event->canvasPos(), event->modifiers());
        mEditor->view()->forceUpdateViewTransform();
    } else if (mCamMoveMode == CameraMoveType::PATH) {
        transformCameraPath(event->canvasPos());
        mEditor->view()->forceUpdateViewTransform();
    }
    emit mEditor->frameModified(frame);
}

qreal CameraTool::getAngleBetween(const QPointF& pos1, const QPointF& pos2) const
{
    return qRadiansToDegrees(MathUtils::getDifferenceAngle(pos1, pos2));
}

CameraMoveType CameraTool::getCameraMoveMode(const QPointF& point, qreal tolerance) const
{
    QPolygonF camPoly =  mCameraPolygon;

    if (camPoly.count() <= 0) { return CameraMoveType::NONE; }

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
    else if (QLineF(point, mRotationHandlePoint).length() < tolerance)
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

    Camera* camera = layerCamera->getLastCameraAtFrame(frameNumber, 0);

    if (camera == nullptr) { return CameraMoveType::NONE; }

    QPointF pathPoint = camera->getPathControlPoint();

    if (!camera->pathControlPointMoved()) {
        pathPoint = layerCamera->getCenteredPathPoint(frameNumber);
    }

    if (QLineF(pathPoint, point).length() < tolerance) {
        return CameraMoveType::PATH;
    }
    return CameraMoveType::NONE;
}


QPointF CameraTool::localRotationHandlePoint(const QPoint& origin, const QTransform& localT, const qreal objectScale, float worldScale) const
{
    // Calculate the perceived distance from the frame to the handle
    // so that it looks like the handle is always x pixels above the origin
    qreal topDis = origin.y() + ((objectScale * origin.y()) * mRotationHandleOffsetPercentage) * worldScale;
    return QPointF(localT.inverted().map(QPointF(0, topDis)));
}

QPointF CameraTool::worldRotationHandlePoint(const QPoint& origin, const QTransform& localT, const qreal objectScale, const QTransform& worldT, float worldScale) const
{
    return worldT.map(localRotationHandlePoint(origin, localT, objectScale, worldScale));
}

void CameraTool::transformView(LayerCamera* layerCamera, CameraMoveType mode, const QPointF& point, const QPointF& offset, qreal angle, int frameNumber) const
{
    QPolygonF curPoly = mCameraPolygon;
    QPointF curCenter = QLineF(curPoly.at(0), curPoly.at(2)).pointAt(0.5).toPoint();
    QLineF lineOld(curCenter, point);
    QLineF lineNew(curCenter, point);
    Camera* curCam = layerCamera->getCameraAtFrame(frameNumber);

    switch (mode)
    {
    case CameraMoveType::CENTER: {
        curCam->translate(curCam->translation() - (point - offset));
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

void CameraTool::paint(QPainter& painter, const QRect& blitRect)
{
    int frameIndex = mEditor->currentFrame();
    LayerCamera* cameraLayerBelow = static_cast<LayerCamera*>(mEditor->object()->getLayerBelow(mEditor->currentLayerIndex(), Layer::CAMERA));

    const QTransform& camTransform = cameraLayerBelow->getViewAtFrame(frameIndex);
    const QRect& cameraRect = cameraLayerBelow->getViewRect();
    const QTransform& worldT = mEditor->view()->getView();

    bool isPlaying = mEditor->playback()->isPlaying();


    // Show handles while we're on a camera layer and not doing playback
    if (!isPlaying) {
        int frame = cameraLayerBelow->getPreviousKeyFramePosition(frameIndex);
        Camera* cam = cameraLayerBelow->getLastCameraAtFrame(qMax(frame, frameIndex), 0);
        Q_ASSERT(cam);
        qreal scale = cam->scaling();
        qreal rotation = cam->rotation();
        QPointF translation = cam->translation();
        paintHandles(painter, worldT, camTransform, cameraRect, translation, scale, rotation, !cameraLayerBelow->keyExists(frameIndex));
    }

    cameraLayerBelow->foreachKeyFrame([&] (const KeyFrame* keyframe) {
        paintInterpolations(painter, worldT, frameIndex, cameraLayerBelow, static_cast<const Camera*>(keyframe), isPlaying);
    });
}


void CameraTool::paintHandles(QPainter& painter, const QTransform& worldTransform, const QTransform& camTransform, const QRect& cameraRect, const QPointF translation, const qreal scale, const qreal rotation, bool hollowHandles) const
{
    painter.save();

    // if the current view is narrower than the camera field
    // Indicates that the quality of the output will be degraded
    if (scale > 1)
    {
        painter.setPen(Qt::red);
    }
    else
    {
        painter.setPen(QColor(0, 0, 0, 255));
    }

    const QPolygonF& camPolygon = Transform::mapToWorldPolygon(camTransform, worldTransform, cameraRect);
    painter.drawPolygon(camPolygon);

    QTransform scaleT;
    scaleT.scale(1, 1);
    scaleT.rotate(rotation);
    scaleT.translate(translation.x(), translation.y());

    const QPolygonF& nonScaledCamPoly = Transform::mapToWorldPolygon(scaleT, worldTransform, cameraRect);
    painter.drawPolygon(nonScaledCamPoly);
    painter.drawText(nonScaledCamPoly[0]-QPoint(0, 2), "100%");

    if (hollowHandles) {
        painter.setPen(mHandleDisabledColor);
        painter.setBrush(Qt::gray);
    } else {
        painter.setPen(mHandlePen);
        painter.setBrush(mHandleColor);
    }
    int handleW = mHandleWidth;
    int radius = handleW / 2;

    const QRectF& topRightCorner = QRectF(camPolygon.at(1).x() - radius,
                                            camPolygon.at(1).y() - radius,
                                            handleW, handleW);
    painter.drawRect(topRightCorner);

    const QRectF& bottomRightCorner = QRectF(camPolygon.at(2).x() - radius,
                                            camPolygon.at(2).y() - radius,
                                            handleW, handleW);
    painter.drawRect(bottomRightCorner);
    const QRectF& topLeftCorner = QRectF(camPolygon.at(0).x() - radius,
                                            camPolygon.at(0).y() - radius,
                                            handleW, handleW);
    painter.drawRect(topLeftCorner);

    const QRectF& bottomLeftCorner = QRectF(camPolygon.at(3).x() - radius,
                                            camPolygon.at(3).y() - radius,
                                            handleW, handleW);
    painter.drawRect(bottomLeftCorner);

    // Paint rotation handle
    const QPointF& topCenter = QLineF(camPolygon.at(0), camPolygon.at(1)).pointAt(.5);
    const QPointF& rotationHandle = worldRotationHandlePoint(cameraRect.topLeft(), camTransform, scale, worldTransform, mEditor->viewScaleInversed());

    painter.drawLine(topCenter, rotationHandle);

    painter.drawEllipse(QRectF((rotationHandle.x() - handleW*0.5),
                               (rotationHandle.y() - handleW*0.5),
                               handleW, handleW));

    painter.restore();
}

void CameraTool::paintInterpolations(QPainter& painter, const QTransform& worldTransform, int currentFrame, const LayerCamera* cameraLayer, const Camera* keyframe, bool isPlaying) const
{
    QColor cameraDotColor = cameraLayer->getDotColor();
    int frame = keyframe->pos();
    int nextFrame = cameraLayer->getNextKeyFramePosition(frame);

    if (cameraLayer->getShowCameraPath() && !cameraLayer->hasSameTranslation(frame, nextFrame)) {
        painter.save();

        painter.setBrush(cameraDotColor);

        // Highlight current dot
        QPen pen(Qt::black);
        pen.setWidth(2);
        painter.setPen(pen);
        const QRect& cameraRect = cameraLayer->getViewRect();
        const QTransform& cameraTransform = cameraLayer->getViewAtFrame(currentFrame);
        const QPointF& centerDot = Transform::mapToWorldRect(cameraTransform, worldTransform, cameraRect).center();
        painter.drawEllipse(centerDot, mDotWidth/2., mDotWidth/2.);

        QPointF cameraPathPoint;
        if (!keyframe->pathControlPointMoved()) {
            cameraPathPoint = worldTransform.map(cameraLayer->getCenteredPathPoint(frame + 1));
        } else {
            cameraPathPoint = worldTransform.map(cameraLayer->getPathControlPointAtFrame(frame + 1));
        }

        painter.save();
        QColor color = cameraDotColor;
        if (currentFrame > frame && currentFrame < nextFrame)
            color.setAlphaF(.5f);
        else
            color.setAlphaF(.2f);
        painter.setPen(Qt::black);
        painter.setBrush(color);

        for (int frameInBetween = frame; frameInBetween <= nextFrame ; frameInBetween++)
        {
            const QTransform& transform = cameraLayer->getViewAtFrame(frameInBetween);
            const QPointF& center = Transform::mapToWorldRect(transform, worldTransform, cameraRect).center();
            painter.drawEllipse(center, mDotWidth/2., mDotWidth/2.);
        }
        painter.restore();

        int distance = nextFrame - frame;
        // It makes no sense to paint the path when there's no interpolation.
        if (distance >= 2 && !isPlaying) {
            paintControlPoint(painter, worldTransform, cameraLayer, frame, cameraPathPoint, cameraLayer->keyExists(currentFrame));
        }

        painter.restore();
    }
}

void CameraTool::paintControlPoint(QPainter& painter, const QTransform& worldTransform, const LayerCamera* cameraLayer, const int frameIndex, const QPointF& pathPoint, bool hollowHandle) const
{
    painter.save();

    // if active path, draw bezier help lines for active path
    const QList<QPointF>& points = cameraLayer->getBezierPointsAtFrame(frameIndex + 1);

    if (!points.empty())
    {
        Q_ASSERT(points.size() == 3);
        QPointF p0 = worldTransform.map(points.at(0));
        QPointF p1 = worldTransform.map(points.at(1));
        QPointF p2 = worldTransform.map(points.at(2));

        painter.save();
        QPen pen (Qt::black, 0.5, Qt::PenStyle::DashLine);
        painter.setPen(pen);
        painter.drawLine(p0, p1);
        painter.drawLine(p1, p2);
        painter.restore();
    }

    // draw movemode in text
    painter.setPen(Qt::black);
    const QString& pathType = cameraLayer->getInterpolationTextAtFrame(frameIndex);

    // Space text according to path point so it doesn't overlap
    painter.drawText(pathPoint - QPoint(0, mHandleWidth), pathType);
    painter.restore();

    // if active path, draw move handle
    painter.save();
    painter.setPen(mHandleTextColor);

    if (hollowHandle) {
        painter.setPen(mHandleDisabledColor);
        painter.setBrush(Qt::gray);
    } else {
        painter.setPen(mHandlePen);
        painter.setBrush(mHandleColor);
    }
    painter.drawRect(static_cast<int>(pathPoint.x() - mHandleWidth/2),
                     static_cast<int>(pathPoint.y() - mHandleWidth/2),
                     mHandleWidth, mHandleWidth);
    painter.restore();
}
