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
#include "layercamera.h"

#include <QSettings>
#include <QEasingCurve>

#include "camera.h"
#include "pencildef.h"
#include "mathutils.h"


LayerCamera::LayerCamera(Object* object) : Layer(object, Layer::CAMERA)
{
    setName(tr("Camera Layer"));

    QSettings settings(PENCIL2D, PENCIL2D);
    mFieldW = settings.value("FieldW").toInt();
    mFieldH = settings.value("FieldH").toInt();
    if (mFieldW < 2 || mFieldH < 2)
    {
        mFieldW = 800;
        mFieldH = 600;
    }
    viewRect = QRect(QPoint(-mFieldW / 2, -mFieldH / 2), QSize(mFieldW, mFieldH));

    connect(this, &LayerCamera::keyframeDeleted, this, &LayerCamera::updateOnDeleteFrame);
    connect(this, &LayerCamera::keyframeAdded, this, &LayerCamera::updateOnAddFrame);
}

LayerCamera::~LayerCamera()
{
}

Camera* LayerCamera::getCameraAtFrame(int frameNumber)
{
    return static_cast<Camera*>(getKeyFrameAt(frameNumber));
}

Camera* LayerCamera::getLastCameraAtFrame(int frameNumber, int increment)
{
    return static_cast<Camera*>(getLastKeyFrameAtPosition(frameNumber + increment));
}

QTransform LayerCamera::getViewAtFrame(int frameNumber) const
{
    if (keyFrameCount() == 0)
    {
        return QTransform();
    }

    // IMO: There should always be a keyframe on frame 1 on the Camera layer! (David)
    if (frameNumber < firstKeyFramePosition())
    {
        frameNumber = firstKeyFramePosition();
    }

    Camera* camera1 = static_cast<Camera*>(getLastKeyFrameAtPosition(frameNumber));
    if (camera1)
        camera1->setEasingType(camera1->getEasingType());
    int nextFrame = getNextKeyFramePosition(frameNumber);
    Camera* camera2 = static_cast<Camera*>(getLastKeyFrameAtPosition(nextFrame));
    if (camera2)
        camera2->setEasingType(camera2->getEasingType());

    if (camera1 == nullptr && camera2 == nullptr)
    {
        return QTransform();
    }
    else if (camera1 == nullptr && camera2 != nullptr)
    {
        return camera2->view;
    }
    else if (camera2 == nullptr && camera1 != nullptr)
    {
        return camera1->view;
    }

    if (camera1 == camera2)
    {
        return camera1->view;
    }

    double frame1 = camera1->pos();
    double frame2 = camera2->pos();

    // interpolation
    Camera* cam = camera1->clone();
    cam->setEasingType(camera1->getEasingType());
    qreal percent = getInterpolationPercent(cam->getEasingType(), (frameNumber - frame1)/ (frame2 - frame1));
    auto lerp = [](double f1, double f2, double percent) -> double
    {
        return f1 * (1.0 - percent) + f2 * percent;
    };
    QPointF point = getBezierPoint(camera1->translation(), camera2->translation(),
                                   -camera1->getPathMidPoint(), percent);
    double dx = point.x();
    double dy = point.y();
    double r = lerp(camera1->rotation(), camera2->rotation(), percent);
    double s = lerp(camera1->scaling(), camera2->scaling(), percent);

    cam->translate(dx, dy);
    cam->rotate(r);
    cam->scale(s);
    cam->updateViewTransform();
    return cam->getView();

}

MoveMode LayerCamera::getMoveModeForCamera(int frameNumber, QPointF point, qreal tolerance)
{
    QTransform curCam = getViewAtFrame(frameNumber);
    QPolygon camPoly = curCam.inverted().mapToPolygon(viewRect);
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

MoveMode LayerCamera::getMoveModeForCameraPath(int frameNumber, QPointF point, qreal tolerance)
{
    int prev = getPreviousKeyFramePosition(frameNumber);
    int next = getNextKeyFramePosition(frameNumber);
    if (hasSameTranslation(prev, next))
        return MoveMode::NONE;

    Camera* camera = getCameraAtFrame(prev);
    Q_ASSERT(camera);

    if (QLineF(camera->getPathMidPoint(), point).length() < tolerance)
        return MoveMode::MIDDLE;

    return MoveMode::NONE;
}

void LayerCamera::transformCameraView(MoveMode mode, QPointF point, int frameNumber)
{
    QPolygon curPoly = getViewAtFrame(frameNumber).inverted().mapToPolygon(viewRect);
    QPoint curCenter = QLineF(curPoly.at(0), curPoly.at(2)).pointAt(0.5f).toPoint();
    QLineF lineOld;
    QLineF lineNew(curCenter, point);
    qreal degree;
    Camera* curCam = getCameraAtFrame(frameNumber);
    QPointF mid = curCam->getPathMidPoint();
    switch (mode)
    {
    case MoveMode::CENTER:
        curCam->translate(curCam->translation() - (point - mOffsetPoint));
        if (!curCam->getIsMidPointSet())
            resetPath(frameNumber);
        break;
    case MoveMode::TOPLEFT:
        lineOld = QLineF(curCenter, curPoly.at(0));
        curCam->scale(curCam->scaling() * (lineOld.length() / lineNew.length()));
        break;
    case MoveMode::TOPRIGHT:
        lineOld = QLineF(curCenter, curPoly.at(1));
        curCam->scale(curCam->scaling() * (lineOld.length() / lineNew.length()));
        break;
    case MoveMode::BOTTOMRIGHT:
        lineOld = QLineF(curCenter, curPoly.at(2));
        curCam->scale(curCam->scaling() * (lineOld.length() / lineNew.length()));
        break;
    case MoveMode::BOTTOMLEFT:
        lineOld = QLineF(curCenter, curPoly.at(3));
        curCam->scale(curCam->scaling() * (lineOld.length() / lineNew.length()));
        break;
    case MoveMode::ROTATIONRIGHT:
        degree = -qRadiansToDegrees(MathUtils::getDifferenceAngle(curCenter, point));
        curCam->translate(curCenter);
        curCam->rotate(curCam->rotation() + (degree - curCam->rotation()));
        curCam->translate(-curCenter);
        // since rotations can move midpoint slightly
        curCam->setPathMidPoint(mid);
        break;
    case MoveMode::ROTATIONLEFT:
        degree = -qRadiansToDegrees(MathUtils::getDifferenceAngle(point, curCenter));
        curCam->translate(curCenter);
        curCam->rotate(curCam->rotation() + (degree - curCam->rotation()));
        curCam->translate(-curCenter);
        // since rotations can move midpoint slightly
        curCam->setPathMidPoint(mid);
        break;
    default:
        break;
    }
    setOffsetPoint(point);
    curCam->updateViewTransform();
    curCam->modification();
}

void LayerCamera::linearInterpolateTransform(Camera* cam)
{
    if (keyFrameCount() == 0)
        return;

    int frameNumber = cam->pos();
    Camera* camera1 = static_cast<Camera*>(getLastKeyFrameAtPosition(frameNumber - 1));
    if (camera1)
    {
        camera1->setEasingType(camera1->getEasingType());
    }

    int nextFrame = getNextKeyFramePosition(frameNumber);
    Camera* camera2 = static_cast<Camera*>(getLastKeyFrameAtPosition(nextFrame));
    if (camera2)
    {
        camera2->setEasingType(camera2->getEasingType());
    }

    if (camera1 == nullptr && camera2 == nullptr)
    {
        return; // do nothing
    }

    else if (camera1 == nullptr && camera2 != nullptr)
    {
        cam->setPathMidPoint(camera2->translation());
        cam->setIsMidPointSet(false);
        return cam->assign(*camera2);
    }

    else if (camera2 == nullptr && camera1 != nullptr)
    {
        cam->setPathMidPoint(camera1->translation());
        cam->setIsMidPointSet(false);
        return cam->assign(*camera1);
    }

    if (camera1 == camera2)
    {
        cam->setPathMidPoint(-camera1->translation());
        cam->setIsMidPointSet(false);
        return cam->assign(*camera1);
    }

    double frame1 = camera1->pos();
    double frame2 = camera2->pos();

    // interpolation
    qreal percent = getInterpolationPercent(camera1->getEasingType(), (frameNumber - frame1)/ (frame2 - frame1));

    auto lerp = [](double f1, double f2, double percent) -> double
    {
        return f1 * (1.0 - percent) + f2 * percent;
    };

    QPointF point = getBezierPoint(camera1->translation(), camera2->translation(),
                                   -camera1->getPathMidPoint(), percent);
    double dx = point.x();
    double dy = point.y();
    double r = lerp(camera1->rotation(), camera2->rotation(), percent);
    double s = lerp(camera1->scaling(), camera2->scaling(), percent);

    cam->translate(dx, dy);
    cam->rotate(r);
    cam->scale(s);
}

qreal LayerCamera::getInterpolationPercent(CameraEasingType type, qreal percent) const
{
    QEasingCurve easing;

    switch (type) {
    case CameraEasingType::LINEAR : easing.setType(QEasingCurve::Linear); break;
    case CameraEasingType::INQUAD : easing.setType(QEasingCurve::InQuad); break;
    case CameraEasingType::OUTQUAD : easing.setType(QEasingCurve::OutQuad); break;
    case CameraEasingType::INOUTQUAD : easing.setType(QEasingCurve::InOutQuad); break;
    case CameraEasingType::OUTINQUAD : easing.setType(QEasingCurve::OutInQuad); break;
    case CameraEasingType::INCUBIC : easing.setType(QEasingCurve::InCubic); break;
    case CameraEasingType::OUTCUBIC : easing.setType(QEasingCurve::OutCubic); break;
    case CameraEasingType::INOUTCUBIC : easing.setType(QEasingCurve::InOutCubic); break;
    case CameraEasingType::OUTINCUBIC : easing.setType(QEasingCurve::OutInCubic); break;
    case CameraEasingType::INQUART : easing.setType(QEasingCurve::InQuart); break;
    case CameraEasingType::OUTQUART : easing.setType(QEasingCurve::OutQuart); break;
    case CameraEasingType::INOUTQUART : easing.setType(QEasingCurve::InOutQuart); break;
    case CameraEasingType::OUTINQUART : easing.setType(QEasingCurve::OutInQuart); break;
    case CameraEasingType::INQUINT : easing.setType(QEasingCurve::InQuint); break;
    case CameraEasingType::OUTQUINT : easing.setType(QEasingCurve::OutQuint); break;
    case CameraEasingType::INOUTQUINT : easing.setType(QEasingCurve::InOutQuint); break;
    case CameraEasingType::OUTINQUINT : easing.setType(QEasingCurve::OutInQuint); break;
    case CameraEasingType::INSINE : easing.setType(QEasingCurve::InSine); break;
    case CameraEasingType::OUTSINE : easing.setType(QEasingCurve::OutSine); break;
    case CameraEasingType::INOUTSINE : easing.setType(QEasingCurve::InOutSine); break;
    case CameraEasingType::OUTINSINE: easing.setType(QEasingCurve::OutInSine); break;
    case CameraEasingType::INEXPO : easing.setType(QEasingCurve::InExpo); break;
    case CameraEasingType::OUTEXPO : easing.setType(QEasingCurve::OutExpo); break;
    case CameraEasingType::INOUTEXPO : easing.setType(QEasingCurve::InOutExpo); break;
    case CameraEasingType::OUTINEXPO: easing.setType(QEasingCurve::OutInExpo); break;
    case CameraEasingType::INCIRC : easing.setType(QEasingCurve::InCirc); break;
    case CameraEasingType::OUTCIRC : easing.setType(QEasingCurve::OutCirc); break;
    case CameraEasingType::INOUTCIRC : easing.setType(QEasingCurve::InOutCirc); break;
    case CameraEasingType::OUTINCIRC: easing.setType(QEasingCurve::OutInCirc); break;
    case CameraEasingType::INELASTIC: easing.setType(QEasingCurve::OutElastic); break;
    case CameraEasingType::OUTELASTIC: easing.setType(QEasingCurve::InOutElastic); break;
    case CameraEasingType::INOUTELASTIC: easing.setType(QEasingCurve::OutInElastic); break;
    case CameraEasingType::OUTINELASTIC: easing.setType(QEasingCurve::InElastic); break;
    case CameraEasingType::INBACK: easing.setType(QEasingCurve::InBack); break;
    case CameraEasingType::OUTBACK: easing.setType(QEasingCurve::OutBack); break;
    case CameraEasingType::INOUTBACK: easing.setType(QEasingCurve::InOutBack); break;
    case CameraEasingType::OUTINBACK: easing.setType(QEasingCurve::OutInBack); break;
    case CameraEasingType::INBOUNCE: easing.setType(QEasingCurve::InBounce); break;
    case CameraEasingType::OUTBOUNCE: easing.setType(QEasingCurve::OutBounce); break;
    case CameraEasingType::INOUTBOUNCE: easing.setType(QEasingCurve::InOutBounce); break;
    case CameraEasingType::OUTINBOUNCE: easing.setType(QEasingCurve::OutInBounce); break;
    default: easing.setType(QEasingCurve::Linear); break;
    }
    return easing.valueForProgress(percent);
}

QPointF LayerCamera::getBezierPoint(QPointF first, QPointF last, QPointF midpoint, qreal percent) const
{
    QLineF line1(first, midpoint);
    QLineF line2(midpoint, last);
    return QLineF(line1.pointAt(percent), line2.pointAt(percent)).pointAt(percent);
}

void LayerCamera::updateOnDeleteFrame(int frame)
{
    int prev = getPreviousKeyFramePosition(frame);
    if (prev < frame)
        resetPath(prev);
    else if (prev == frame)
        resetPath(frame);
}

void LayerCamera::updateOnAddFrame(int frame)
{
    int next = getNextKeyFramePosition(frame);
    int prev = getPreviousKeyFramePosition(frame);

    // if frame is last keyframe
    if (next == frame)
    {
        return;
    }
    // if inbetween frames
    else if (prev < frame)
    {
        Camera* camPrev = static_cast<Camera*>(getKeyFrameAt(prev));
        Camera* camFrame = static_cast<Camera*>(getKeyFrameAt(frame));
        Camera* camNext = static_cast<Camera*>(getKeyFrameAt(next));
        Q_ASSERT(camPrev && camFrame && camNext);

        // get center point for new frame
        QPointF point = camFrame->translation();
        QPointF midPoint = camPrev->getPathMidPoint();

        // from prev to frame
        QLineF toPoint(-camPrev->translation(), -point);
        QLineF toMidpoint(-camPrev->translation(), midPoint);
        toMidpoint.setLength(toPoint.length());
        camPrev->setPathMidPoint(toMidpoint.pointAt(0.5));
        camPrev->modification();

        // from frame to next
        toPoint = QLineF(-camNext->translation(), -point);
        toMidpoint = QLineF(-camNext->translation(), midPoint);
        toMidpoint.setLength(toPoint.length());
        camFrame->setPathMidPoint(toMidpoint.pointAt(0.5));
        camFrame->modification();
    }
    else
    {
        // if first frame
        resetPath(frame);
    }
}

QRect LayerCamera::getViewRect()
{
    return viewRect;
}

QSize LayerCamera::getViewSize() const
{
    return viewRect.size();
}

void LayerCamera::setViewRect(QRect newViewRect)
{
    viewRect = newViewRect;
    emit resolutionChanged();
}

void LayerCamera::setCameraEasing(CameraEasingType type, int frame)
{
    Q_ASSERT(keyExists(frame));

    Camera* camera = getCameraAtFrame(frame);
    camera->setEasingType(type);
    camera->updateViewTransform();
    camera->modification();
}

void LayerCamera::setCameraReset(CameraFieldOption type, int frame)
{
    Q_ASSERT(keyExists(frame));

    Camera* camera = getCameraAtFrame(frame);
    Camera* copyCamera = getCameraAtFrame(frame);
    int nextFrame = getNextKeyFramePosition(frame);
    switch (type)
    {
    case CameraFieldOption::RESET_FIELD:
        camera->reset();
        break;
    case CameraFieldOption::RESET_TRANSITION:
        camera->translate(QPoint(0,0));
        break;
    case CameraFieldOption::RESET_SCALING:
        camera->scale(1.0);
        break;
    case CameraFieldOption::RESET_ROTATION:
        camera->rotate(0.0);
        break;
    case CameraFieldOption::ALIGN_HORIZONTAL:
        camera = getCameraAtFrame(nextFrame);
        camera->translate(camera->translation().x(), copyCamera->translation().y());
        break;
    case CameraFieldOption::ALIGN_VERTICAL:
        camera = getCameraAtFrame(nextFrame);
        camera->translate(copyCamera->translation().x(), camera->translation().y());
        break;
    case CameraFieldOption::HOLD_FRAME:
        camera = getCameraAtFrame(nextFrame);
        camera->translate(copyCamera->translation());
        camera->scale(copyCamera->scaling());
        camera->rotate(copyCamera->rotation());
        break;
    default:
        break;
    }
    camera->updateViewTransform();
    camera->modification();
}

void LayerCamera::setDotColor(DotColor color)
{
    switch (color)
    {
    case DotColor::RED_DOT:
        setDotColor(Qt::red);
        break;
    case DotColor::GREEN_DOT:
        setDotColor(Qt::green);
        break;
    case DotColor::BLUE_DOT:
        setDotColor(Qt::blue);
        break;
    case DotColor::BLACK_DOT:
        setDotColor(Qt::black);
        break;
    case DotColor::WHITE_DOT:
        setDotColor(Qt::white);
        break;
    }
}

QString LayerCamera::getInterpolationText(int frame)
{
    Camera* camera = getCameraAtFrame(frame);
    Q_ASSERT(camera);

    CameraEasingType type = camera->getEasingType();
    QString retString = "";

    switch (type)
    {
    case CameraEasingType::LINEAR: retString = tr("Linear"); break;
    case CameraEasingType::INSINE: retString = tr("Slow Ease-in"); break;
    case CameraEasingType::OUTSINE: retString = tr("Slow  Ease-out"); break;
    case CameraEasingType::INOUTSINE: retString = tr("Slow  Ease-in - Ease-out"); break;
    case CameraEasingType::OUTINSINE: retString = tr("Slow  Ease-out - Ease-in"); break;
    case CameraEasingType::INQUAD: retString = tr("Normal Ease-in"); break;
    case CameraEasingType::OUTQUAD: retString = tr("Normal Ease-out"); break;
    case CameraEasingType::INOUTQUAD: retString = tr("Normal Ease-in - Ease-out"); break;
    case CameraEasingType::OUTINQUAD: retString = tr("Normal Ease-out - Ease-in"); break;
    case CameraEasingType::INCUBIC: retString = tr("Quick Ease-in"); break;
    case CameraEasingType::OUTCUBIC: retString = tr("Quick Ease-out"); break;
    case CameraEasingType::INOUTCUBIC: retString = tr("Quick Ease-in - Ease-out"); break;
    case CameraEasingType::OUTINCUBIC: retString = tr("Quick Ease-out - Ease-in"); break;
    case CameraEasingType::INQUART: retString = tr("Fast Ease-in"); break;
    case CameraEasingType::OUTQUART: retString = tr("Fast Ease-out"); break;
    case CameraEasingType::INOUTQUART: retString = tr("Fast Ease-in - Ease-out"); break;
    case CameraEasingType::OUTINQUART: retString = tr("Fast Ease-out - Ease-in"); break;
    case CameraEasingType::INQUINT: retString = tr("Faster Ease-in"); break;
    case CameraEasingType::OUTQUINT: retString = tr("Faster Ease-out"); break;
    case CameraEasingType::INOUTQUINT: retString = tr("Faster Ease-in - Ease-out"); break;
    case CameraEasingType::OUTINQUINT: retString = tr("Faster Ease-out - Ease-in"); break;
    case CameraEasingType::INEXPO: retString = tr("Fastest Ease-in"); break;
    case CameraEasingType::OUTEXPO: retString = tr("Fastest Ease-out"); break;
    case CameraEasingType::INOUTEXPO: retString = tr("Fastest Ease-in - Ease-out"); break;
    case CameraEasingType::OUTINEXPO: retString = tr("Fastest Ease-out - Ease-in"); break;
    case CameraEasingType::INCIRC: retString = tr("Circle-based  Ease-in"); break;
    case CameraEasingType::OUTCIRC: retString = tr("LineCircle-based  Ease-outar"); break;
    case CameraEasingType::INOUTCIRC: retString = tr("Circle-based  Ease-in - Ease-out"); break;
    case CameraEasingType::OUTINCIRC: retString = tr("Circle-based  Ease-out - Ease-in"); break;
    case CameraEasingType::INELASTIC: retString = tr("Elastic (inElastic)"); break;
    case CameraEasingType::OUTELASTIC: retString = tr("Elastic (outElastic)"); break;
    case CameraEasingType::INOUTELASTIC: retString = tr("Elastic (inOutElastic)"); break;
    case CameraEasingType::OUTINELASTIC: retString = tr("Elastic (outInElastic)"); break;
    case CameraEasingType::INBACK: retString = tr("Overshoot (inBack)"); break;
    case CameraEasingType::OUTBACK: retString = tr("Overshoot (outBack)"); break;
    case CameraEasingType::INOUTBACK: retString = tr("Overshoot (inOutBack)"); break;
    case CameraEasingType::OUTINBACK: retString = tr("Overshoot (outInBack)"); break;
    case CameraEasingType::INBOUNCE: retString = tr("Bounce (inBounce)"); break;
    case CameraEasingType::OUTBOUNCE: retString = tr("Bounce (outBounce)"); break;
    case CameraEasingType::INOUTBOUNCE: retString = tr("Bounce (inOutBounce)"); break;
    case CameraEasingType::OUTINBOUNCE: retString = tr("Bounce (outInBounce)"); break;
    default: retString = tr("Linear"); break;
    }

    return retString;
}

QPointF LayerCamera::getPathMidPoint(int frame)
{
    Camera* camera = getCameraAtFrame(getPreviousKeyFramePosition(frame));
    Q_ASSERT(camera);

    return camera->getPathMidPoint();
}

QPointF LayerCamera::getPathStartPoint(int frame)
{
    Camera* camera = getCameraAtFrame(getPreviousKeyFramePosition(frame));
    Q_ASSERT(camera);

    return camera->translation();
}

bool LayerCamera::hasSameTranslation(int first, int last)
{
    Camera* camera1 = getCameraAtFrame(first);
    Camera* camera2 = getCameraAtFrame(last);
    Q_ASSERT(camera1 && camera2);

    return camera1->translation() == camera2->translation();
}

QList<QPointF> LayerCamera::getBezierPoints(int frame)
{
    QList<QPointF> points;
    int prevFrame = getPreviousKeyFramePosition(frame);
    int nextFrame = getNextKeyFramePosition(frame);
    if (prevFrame < nextFrame)
    {
        Camera* prevCam = getCameraAtFrame(prevFrame);
        Camera* nextCam = getCameraAtFrame(nextFrame);
        points.append(QPointF(-prevCam->translation()));
        points.append(QPointF(prevCam->getPathMidPoint()));
        points.append(QPointF(-nextCam->translation()));
    }
    return points;
}

void LayerCamera::resetPath(int frame)
{
    int nextFrame = getNextKeyFramePosition(frame);
    Camera* cam1 = getLastCameraAtFrame(frame, 0);
    Camera* cam2 = getCameraAtFrame(nextFrame);
    cam1->setPathMidPoint(QLineF(-cam1->translation(), -cam2->translation()).pointAt(0.5));
    cam1->modification();
}

void LayerCamera::dragCameraPath(MoveMode mode, QPointF point, int frame)
{
    Camera* camera = getCameraAtFrame(getPreviousKeyFramePosition(frame));
    Q_ASSERT(camera);

    switch (mode)
    {
    case MoveMode::MIDDLE:
        camera->setPathMidPoint(point);
        camera->setIsMidPointSet(true);
        camera->modification();
        break;
    default:
        break;
    }
    setOffsetPoint(point);
}

void LayerCamera::loadImageAtFrame(int frameNumber, qreal dx, qreal dy, qreal rotate, qreal scale, CameraEasingType easing, QPointF midPoint)
{
    if (keyExists(frameNumber))
    {
        removeKeyFrame(frameNumber);
    }
    Camera* camera = new Camera(QPointF(dx, dy), rotate, scale);
    camera->setPos(frameNumber);
    camera->setEasingType(easing);
    camera->setPathMidPoint(midPoint);
    loadKey(camera);
    int nextFrame = getNextKeyFramePosition(frameNumber);
    if (frameNumber < nextFrame)
    {
        Camera* nextCam = getCameraAtFrame(nextFrame);
        QPointF mid2 = QLineF(camera->translation(), nextCam->translation()).pointAt(0.5);
        if (mid2 != midPoint)
            camera->setIsMidPointSet(true);
    }
}

Status LayerCamera::saveKeyFrameFile(KeyFrame*, QString)
{
    return Status::OK;
}

KeyFrame* LayerCamera::createKeyFrame(int position, Object*)
{
    Camera* c = new Camera;
    c->setPos(position);
    c->setEasingType(CameraEasingType::LINEAR);
    linearInterpolateTransform(c);
    return c;
}

QDomElement LayerCamera::createDomElement(QDomDocument& doc) const
{
    QDomElement layerElem = createBaseDomElement(doc);
    layerElem.setAttribute("width", viewRect.width());
    layerElem.setAttribute("height", viewRect.height());

    foreachKeyFrame([&](KeyFrame* pKeyFrame)
                    {
                        Camera* camera = static_cast<Camera*>(pKeyFrame);
                        QDomElement keyTag = doc.createElement("camera");
                        keyTag.setAttribute("frame", camera->pos());

                        keyTag.setAttribute("r", camera->rotation());
                        keyTag.setAttribute("s", camera->scaling());
                        keyTag.setAttribute("dx", camera->translation().x());
                        keyTag.setAttribute("dy", camera->translation().y());
                        keyTag.setAttribute("easing", static_cast<int>(camera->getEasingType()));
                        keyTag.setAttribute("midx", camera->getPathMidPoint().x());
                        keyTag.setAttribute("midy", camera->getPathMidPoint().y());
                        layerElem.appendChild(keyTag);
                    });

    return layerElem;
}

void LayerCamera::loadDomElement(const QDomElement& element, QString dataDirPath, ProgressCallback progressStep)
{
    Q_UNUSED(dataDirPath);
    Q_UNUSED(progressStep);

    this->loadBaseDomElement(element);

    int width = element.attribute("width").toInt();
    int height = element.attribute("height").toInt();
    viewRect = QRect(-width / 2, -height / 2, width, height);

    QDomNode imageTag = element.firstChild();
    while (!imageTag.isNull())
    {
        QDomElement imageElement = imageTag.toElement();
        if (!imageElement.isNull())
        {
            if (imageElement.tagName() == "camera")
            {
                int frame = imageElement.attribute("frame").toInt();

                qreal rotate = imageElement.attribute("r", "0").toDouble();
                qreal scale = imageElement.attribute("s", "1").toDouble();
                qreal dx = imageElement.attribute("dx", "0").toDouble();
                qreal dy = imageElement.attribute("dy", "0").toDouble();
                CameraEasingType easing = static_cast<CameraEasingType>(imageElement.attribute("easing", "0").toInt());
                qreal midx = imageElement.attribute("midx", "0").toDouble();
                qreal midy = imageElement.attribute("midy", "0").toDouble();

                loadImageAtFrame(frame, dx, dy, rotate, scale, easing, QPointF(midx, midy));
            }
        }
        imageTag = imageTag.nextSibling();
    }
}
