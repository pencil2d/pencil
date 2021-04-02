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
#include "camera.h"
#include "pencildef.h"
#include "cameraeasingtype.h"
#include "mathutils.h"
#include <QEasingCurve>
#include <QPainterPath>
#include <QDebug>

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

    mFrameList = getKeyList();
    connect(this, &LayerCamera::selectedFramesChanged, this, &LayerCamera::updateAllCameraPaths);
    connect(this, &LayerCamera::keyframeDeleted, this, &LayerCamera::updateAllCameraPaths);
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
    cam->setCameraPath(camera1->getCameraPath());
    cam->setEasingType(camera1->getEasingType());
    QPainterPath path = cam->getCameraPath();
    qreal percent = getInterpolationPercent(cam->getEasingType(), (frameNumber - frame1)/ (frame2 - frame1));

    auto lerp = [](double f1, double f2, double percent) -> double
    {
        return f1 * (1.0 - percent) + f2 * percent;
    };
    double dx = path.pointAtPercent(percent).x();
    double dy = path.pointAtPercent(percent).y();
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
    if (QLineF(point, camPoly.at(1)).length() < tolerance)
    {
        return MoveMode::TOPRIGHT;
    }
    if (QLineF(point, camPoly.at(2)).length() < tolerance)
    {
        return MoveMode::BOTTOMRIGHT;
    }
    if (QLineF(point, camPoly.at(3)).length() < tolerance)
    {
        return MoveMode::BOTTOMLEFT;
    }
    else if (QLineF(point, QPoint(camPoly.at(1) + (camPoly.at(2) - camPoly.at(1)) / 2)).length() < tolerance)
    {
        return MoveMode::ROTATION;
    }
    else if (camPoly.containsPoint(point.toPoint(), Qt::FillRule::OddEvenFill))
    {
        return MoveMode::CENTER;
    }
    return MoveMode::NONE;
}

MoveMode LayerCamera::getMoveModeForCameraPath(int frameNumber, QPointF point, qreal tolerance)
{
    Camera* camera = static_cast<Camera*>(getKeyFrameAt(getPreviousKeyFramePosition(frameNumber)));
    Q_ASSERT(camera);

    QPainterPath path = camera->getCameraPath();

    if (QLineF(path.pointAtPercent(0.5), point).length() < tolerance)
    {
        return MoveMode::MIDDLE;
    }
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
    switch (mode)
    {
    case MoveMode::CENTER:
        curCam->translate(curCam->translation() - (point - mOffsetPoint));
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
    case MoveMode::ROTATION:
        degree = -qRadiansToDegrees(MathUtils::getDifferenceAngle(curCenter, point));
        curCam->translate(curCenter);
        curCam->rotate(curCam->rotation() + (degree - curCam->rotation()));
        curCam->translate(-curCenter);
        break;
    default:
        break;
    }
    updateCameraPath(frameNumber);
    setOffsetPoint(point);
    curCam->updateViewTransform();
    curCam->modification();
}

void LayerCamera::dragCameraPath(MoveMode mode, QPointF point, int frameNumber)
{
    Camera* camera1 = getCameraAtFrame(getPreviousKeyFramePosition(frameNumber));
    Q_ASSERT(camera1);

    QPointF start = camera1->getCameraPath().pointAtPercent(0);
    QPointF middle = -mOffsetPoint;
    QPointF end = camera1->getCameraPath().pointAtPercent(1);
    QPainterPath path(start);
     switch (mode)
    {
    case MoveMode::MIDDLE:
        path.cubicTo(middle - (point - mOffsetPoint), middle - (point - mOffsetPoint), end);
        camera1->setCameraPath(path);
        camera1->updateViewTransform();
        camera1->modification();
        break;
    default:
        break;
    }
    setOffsetPoint(point);
}

void LayerCamera::linearInterpolateTransform(Camera* cam)
{
    if (keyFrameCount() == 0)
        return;

    int frameNumber = cam->pos();
    Camera* camera1 = static_cast<Camera*>(getLastKeyFrameAtPosition(frameNumber - 1));
    if (camera1)
        camera1->setEasingType(camera1->getEasingType());

    int nextFrame = getNextKeyFramePosition(frameNumber);
    Camera* camera2 = static_cast<Camera*>(getLastKeyFrameAtPosition(nextFrame));
    if (camera2)
        camera2->setEasingType(camera2->getEasingType());


    if (camera1 == nullptr && camera2 == nullptr)
    {
        return; // do nothing
    }
    else if (camera1 == nullptr && camera2 != nullptr)
    {
        cam->assign(*camera2);
        QPainterPath path(camera2->translation());
        QPointF midPoint = QLineF(cam->translation(), camera2->translation()).pointAt(0.5);
        path.cubicTo(midPoint, midPoint, camera2->translation());
        cam->setCameraPath(path);
        cam->setEasingType(CameraEasingType::LINEAR);
        return;
    }
    else if (camera2 == nullptr && camera1 != nullptr)
    {
        return cam->assign(*camera1);
    }

    if (camera1 == camera2)
    {
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

    double dx = camera1->getCameraPath().pointAtPercent(percent).toPoint().x();
    double dy = camera1->getCameraPath().pointAtPercent(percent).toPoint().y();
    double r = lerp(camera1->rotation(), camera2->rotation(), percent);
    double s = lerp(camera1->scaling(), camera2->scaling(), percent);

    cam->translate(dx, dy);
    cam->rotate(r);
    cam->scale(s);

    // set path for new camera
    QPainterPath path(cam->translation());
    QPointF midPoint = QLineF(cam->translation(), camera2->translation()).pointAt(0.5);
    path.cubicTo(midPoint, midPoint, camera2->translation());
    cam->setCameraPath(path);
    cam->setEasingType(CameraEasingType::LINEAR);

    // recalculate path for camera1
    QPainterPath path1(camera1->translation());
    QPointF midPoint1 = QLineF(camera1->translation(), cam->translation()).pointAt(0.5);
    path1.cubicTo(midPoint1, midPoint1, cam->translation());
    camera1->setCameraPath(path1);
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
    case CameraEasingType::OUTBOUNCE: easing.setType(QEasingCurve::OutBounce); break;
    default: easing.setType(QEasingCurve::Linear); break;
    }
    return easing.valueForProgress(percent);
}

CameraEasingType LayerCamera::getCameraEasingType(int type)
{
    CameraEasingType retType = CameraEasingType::LINEAR;

    switch (type) {
    case 0 : retType = CameraEasingType::LINEAR; break;
    case 1 : retType = CameraEasingType::INQUAD; break;
    case 2 : retType = CameraEasingType::OUTQUAD; break;
    case 3 : retType = CameraEasingType::INOUTQUAD; break;
    case 4 : retType = CameraEasingType::OUTINQUAD; break;
    case 5 : retType = CameraEasingType::INCUBIC; break;
    case 6 : retType = CameraEasingType::OUTCUBIC; break;
    case 7 : retType = CameraEasingType::INOUTCUBIC; break;
    case 8 : retType = CameraEasingType::OUTINCUBIC; break;
    case 9 : retType = CameraEasingType::INQUART; break;
    case 10: retType = CameraEasingType::OUTQUART; break;
    case 11: retType = CameraEasingType::INOUTQUART; break;
    case 12: retType = CameraEasingType::OUTINQUART; break;
    case 13: retType = CameraEasingType::INQUINT; break;
    case 14: retType = CameraEasingType::OUTQUINT; break;
    case 15: retType = CameraEasingType::INOUTQUINT; break;
    case 16: retType = CameraEasingType::OUTINQUINT; break;
    case 17: retType = CameraEasingType::INSINE; break;
    case 18: retType = CameraEasingType::OUTSINE; break;
    case 19: retType = CameraEasingType::INOUTSINE; break;
    case 20: retType = CameraEasingType::OUTINSINE; break;
    case 21: retType = CameraEasingType::INEXPO; break;
    case 22: retType = CameraEasingType::OUTEXPO; break;
    case 23: retType = CameraEasingType::INOUTEXPO; break;
    case 24: retType = CameraEasingType::OUTINEXPO; break;
    case 25: retType = CameraEasingType::INCIRC; break;
    case 26: retType = CameraEasingType::OUTCIRC; break;
    case 27: retType = CameraEasingType::INOUTCIRC; break;
    case 28: retType = CameraEasingType::OUTINCIRC; break;
    case 38: retType = CameraEasingType::OUTBOUNCE; break;
    default: retType = CameraEasingType::LINEAR; break;
    }
    return retType;
}

void LayerCamera::initCameraPath(int frame)
{
    Q_ASSERT(keyExists(frame));

    Camera* initCamera = static_cast<Camera*>(getKeyFrameAt(frame));
    int next = getNextKeyFramePosition(frame);
    Camera* nextCamera = static_cast<Camera*>(getKeyFrameAt(next));
    QPainterPath path(initCamera->translation());
    path.cubicTo(initCamera->getPathMidPoint(), initCamera->getPathMidPoint(), nextCamera->translation());
    initCamera->setCameraPath(path);
}

void LayerCamera::updateCameraPath(int frame)
{
    Camera* camera = getCameraAtFrame(frame);
    if (keyExists(frame))
    {
        updateExistingCameraPath(frame);
    }
    else
    {
        camera = getCameraAtFrame(getNextKeyFramePosition(frame));
    }

    // update camera path from previous frame?
    if (getPreviousKeyFramePosition(frame) != frame)
    {
        Camera* camPrevious = static_cast<Camera*>(getCameraAtFrame(getPreviousKeyFramePosition(frame)));
        QPainterPath path(camPrevious->translation());
        QPointF midPoint = QLineF(camPrevious->translation(), camera->translation()).pointAt(0.5);
        path.cubicTo(midPoint, midPoint, camera->translation());
        camPrevious->setCameraPath(path);
    }
}

void LayerCamera::updateExistingCameraPath(int frame)
{
    Q_ASSERT(keyExists(frame));

    Camera* camera;
    camera = getCameraAtFrame(frame);

    Camera* nextCamera;
    nextCamera = getCameraAtFrame(getNextKeyFramePosition(frame));

    QPainterPath pathNew(camera->translation());
    QPointF midPoint = QLineF(camera->translation(), nextCamera->translation()).pointAt(0.5);
    pathNew.cubicTo(midPoint, midPoint, nextCamera->translation());
    camera->setCameraPath(pathNew);
}

void LayerCamera::updateAllCameraPaths()
{
    QList<int> newList = getKeyList();
    if (mFrameList != newList)
    {
        for (int i = 0; i < newList.size(); i++)
        {
            updateExistingCameraPath(newList.at(i));
        }
    }
    mFrameList = newList;
}

// Only to be used for old files, missing midPoint information
void LayerCamera::setMidPoint(int frame)
{
    Q_ASSERT(keyExists(frame));

    Camera* initCamera = static_cast<Camera*>(getKeyFrameAt(frame));
    int next = getNextKeyFramePosition(frame);
    if (frame == next || next == 0)
    {
        initCamera->setPathMidPoint(initCamera->translation());
    }
    else
    {
        Camera* nextCamera = static_cast<Camera*>(getKeyFrameAt(next));
        initCamera->setPathMidPoint(QLineF(initCamera->translation(), nextCamera->translation()).pointAt(0.5));
    }
}

void LayerCamera::initEasingData(int frame, CameraEasingType type)
{
    Q_ASSERT(keyExists(frame));

    Camera* initCamera = static_cast<Camera*>(getKeyFrameAt(frame));
    initCamera->setEasingType(type);
    int next = getNextKeyFramePosition(frame);

    if (frame == next || next == 0)
    {
        initCamera->setPathMidPoint(initCamera->translation());
    }
    else
    {
        Camera* nextCamera = static_cast<Camera*>(getKeyFrameAt(next));
        initCamera->setPathMidPoint(QLineF(initCamera->translation(), nextCamera->translation()).pointAt(0.5));
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

QPointF LayerCamera::getPathMidPont(int frame)
{
    Q_ASSERT(keyExists(frame));

    Camera* camera = getCameraAtFrame(frame);
    QPainterPath path = camera->getCameraPath();

    return path.pointAtPercent(0.5);
}

QString LayerCamera::getInterpolationText(int frame)
{
    Camera* camera = getCameraAtFrame(frame);
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
    case CameraEasingType::OUTELASTIC: retString = tr("Elastic (outElastic)"); break;
    case CameraEasingType::OUTBACK: retString = tr("Overshoot (outBack)"); break;
    case CameraEasingType::OUTBOUNCE: retString = tr("Bounce (outBounce)"); break;
    default: retString = tr("Linear"); break;
    }

    return retString;
}

void LayerCamera::loadImageAtFrame(int frameNumber, qreal dx, qreal dy, qreal rotate, qreal scale, int easing, QPointF midPoint)
{
    if (keyExists(frameNumber))
    {
        removeKeyFrame(frameNumber);
    }
    Camera* camera = new Camera(QPointF(dx, dy), rotate, scale);
    camera->setPos(frameNumber);
    camera->setEasingType(getCameraEasingType(easing));
    camera->setPathMidPoint(midPoint);
    loadKey(camera);
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
                        QPointF point = camera->getCameraPath().pointAtPercent(0.5);
                        keyTag.setAttribute("midx", point.x());
                        keyTag.setAttribute("midy", point.y());
                        keyTag.setAttribute("easing", static_cast<int>(camera->getEasingType()));
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
                qreal midx = imageElement.attribute("midx", "1234567890").toDouble();
                qreal midy = imageElement.attribute("midy", "1234567890").toDouble();
                int easing = imageElement.attribute("easing", "0").toInt();

                loadImageAtFrame(frame, dx, dy, rotate, scale, easing, QPointF(midx, midy));
                if (midx == 1234567890)
                {
                    initEasingData(frame, getCameraEasingType(easing));
                }
                initCameraPath(frame);
            }
        }
        imageTag = imageTag.nextSibling();
    }
}
