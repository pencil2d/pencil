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

LayerCamera::LayerCamera(int id) : Layer(id, Layer::CAMERA)
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
}

LayerCamera::~LayerCamera()
{
}

bool LayerCamera::addKeyFrame(int position, KeyFrame *pKeyFrame)
{
    bool keyAdded = Layer::addKeyFrame(position, pKeyFrame);
    if (keyAdded) {
        splitControlPointIfNeeded(position);
    }
    return keyAdded;
}

bool LayerCamera::removeKeyFrame(int position)
{
    mergeControlPointIfNeeded(position);
    return Layer::removeKeyFrame(position);
}

Camera* LayerCamera::getCameraAtFrame(int frameNumber) const
{
    return static_cast<Camera*>(getKeyFrameAt(frameNumber));
}

Camera* LayerCamera::getLastCameraAtFrame(int frameNumber, int increment) const
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

    int nextFrame = getNextKeyFramePosition(frameNumber);
    Camera* camera2 = static_cast<Camera*>(getLastKeyFrameAtPosition(nextFrame));

    if (camera1 == nullptr && camera2 == nullptr)
    {
        return QTransform();
    }
    else if (camera1 == nullptr && camera2 != nullptr)
    {
        return camera2->getView();
    }
    else if (camera2 == nullptr && camera1 != nullptr)
    {
        return camera1->getView();
    }

    if (camera1->compare(*camera2))
    {
        return camera1->getView();
    }

    double frame1 = camera1->pos();
    double frame2 = camera2->pos();

    // interpolation
    qreal percent = getInterpolationPercent(camera1->getEasingType(), (frameNumber - frame1) / (frame2 - frame1));
    auto lerp = [](double f1, double f2, double percent) -> double
    {
        return f1 * (1.0 - percent) + f2 * percent;
    };

    QPointF controlPoint = camera1->getPathControlPoint();
    if (!camera1->pathControlPointMoved()) {
        controlPoint = getCenteredPathPoint(frame1);
    }

    const QPointF& point = getBezierPoint(camera1->translation(), camera2->translation(),
                                   -controlPoint, percent);

    double dx = point.x();
    double dy = point.y();
    double r = lerp(camera1->rotation(), camera2->rotation(), percent);
    double s = lerp(camera1->scaling(), camera2->scaling(), percent);

    QTransform camTransform;
    camTransform.scale(s, s);
    camTransform.rotate(r);
    camTransform.translate(dx, dy);

    return camTransform;
}

void LayerCamera::linearInterpolateTransform(Camera* cam)
{
    if (keyFrameCount() == 0)
        return;

    int frameNumber = cam->pos();
    Camera* camera1 = static_cast<Camera*>(getLastKeyFrameAtPosition(frameNumber - 1));

    int nextFrame = getNextKeyFramePosition(frameNumber);
    Camera* camera2 = static_cast<Camera*>(getLastKeyFrameAtPosition(nextFrame));

    if (camera1 == nullptr && camera2 == nullptr)
    {
        return; // do nothing
    }

    else if (camera1 == nullptr && camera2 != nullptr)
    {
        return cam->assign(*camera2);
    }

    else if (camera2 == nullptr && camera1 != nullptr)
    {
        return cam->assign(*camera1);
    }

    if (camera1->compare(*camera2))
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

    QPointF controlPoint = camera1->getPathControlPoint();
    if (!camera1->pathControlPointMoved()) {
        controlPoint = getCenteredPathPoint(frame1);
    }

    QPointF point = getBezierPoint(camera1->translation(), camera2->translation(),
                                   -controlPoint, percent);
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

    switch (type)
    {
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
    case CameraEasingType::INELASTIC: easing.setType(QEasingCurve::InElastic); break;
    case CameraEasingType::OUTELASTIC: easing.setType(QEasingCurve::OutElastic); break;
    case CameraEasingType::INOUTELASTIC: easing.setType(QEasingCurve::InOutElastic); break;
    case CameraEasingType::OUTINELASTIC: easing.setType(QEasingCurve::OutInElastic); break;
    case CameraEasingType::INBACK: easing.setType(QEasingCurve::InBack); break;
    case CameraEasingType::OUTBACK: easing.setType(QEasingCurve::OutBack); break;
    case CameraEasingType::INOUTBACK: easing.setType(QEasingCurve::InOutBack); break;
    case CameraEasingType::OUTINBACK: easing.setType(QEasingCurve::OutInBack); break;
    case CameraEasingType::INBOUNCE: easing.setType(QEasingCurve::InBounce); break;
    case CameraEasingType::OUTBOUNCE: easing.setType(QEasingCurve::OutBounce); break;
    case CameraEasingType::INOUTBOUNCE: easing.setType(QEasingCurve::InOutBounce); break;
    case CameraEasingType::OUTINBOUNCE: easing.setType(QEasingCurve::OutInBounce); break;
    default: Q_UNREACHABLE(); break;
    }
    return easing.valueForProgress(percent);
}

QPointF LayerCamera::getBezierPoint(const QPointF& first, const QPointF& last, const QPointF& pathPoint, qreal percent) const
{
    QLineF line1(first, pathPoint);
    QLineF line2(pathPoint, last);
    return QLineF(line1.pointAt(percent), line2.pointAt(percent)).pointAt(percent);
}

void LayerCamera::splitControlPointIfNeeded(int frame) const
{
    int next = getNextKeyFramePosition(frame);
    int prev = getPreviousKeyFramePosition(frame);

    // if inbetween frames
    if (frame > prev && (frame > 1) && frame < next)
    {
        Camera* camFrame = getLastCameraAtFrame(frame, 0);
        Camera* camPrev = getCameraAtFrame(prev);
        Camera* camNext = getCameraAtFrame(next);
        Q_ASSERT(camPrev && camFrame && camNext);

        if (camPrev->pathControlPointMoved()) {
            qreal t = static_cast<qreal>(frame - prev) / (next - prev);
            QPointF previousControlPoint = camPrev->getPathControlPoint();

            // Line from the current control point to the next frame
            QLineF interpolatedLineCN = QLineF(previousControlPoint, -camNext->translation());

            // Line from the previous frame to the current control point
            QLineF interpolatedLinePC = QLineF(-camPrev->translation(), previousControlPoint);
            // Place the new control points, so the curve is kept
            camPrev->setPathControlPoint(interpolatedLinePC.pointAt(t));
            camFrame->setPathControlPoint(interpolatedLineCN.pointAt(t));
            camFrame->setPathControlPointMoved(true);
        }
    } else {
        Camera* camPrev = getCameraAtFrame(prev);
        camPrev->setPathControlPointMoved(false);
    }
}

void LayerCamera::mergeControlPointIfNeeded(int frame) const
{
    int next = getNextKeyFramePosition(frame);
    int prev = getPreviousKeyFramePosition(frame);

    // if inbetween frames
    if (frame > prev && (frame > 1) && frame < next)
    {
        Camera* camPrev = getCameraAtFrame(prev);
        Camera* camFrame = getLastCameraAtFrame(frame, 0);
        Camera* camNext = getCameraAtFrame(next);
        Q_ASSERT(camPrev && camFrame && camNext);

        if (camPrev->pathControlPointMoved()) {

            // Line from the current control point to the next frame
            const QLineF& interpolatedLineCN = QLineF(camFrame->getPathControlPoint(), -camNext->translation());

            // Line from the previous frame translation to previous control point
            const QLineF& interpolatedLinePC = QLineF(-camPrev->translation(), camPrev->getPathControlPoint());

            QPointF mergedCPoint;
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
            auto intersection = interpolatedLinePC.intersects(interpolatedLineCN, &mergedCPoint);
#else
            auto intersection = interpolatedLinePC.intersect(interpolatedLineCN, &mergedCPoint);
#endif
            // Try to recover the control point if the distance is within the threshold, otherwise do nothing
            if (intersection == QLineF::UnboundedIntersection &&
                QLineF(camFrame->getPathControlPoint(), mergedCPoint).length() < mControlPointMergeThreshold)
            {
                camPrev->setPathControlPoint(mergedCPoint);
                camPrev->setPathControlPointMoved(true);
            } else if (intersection == QLineF::NoIntersection) {
                camPrev->setPathControlPointMoved(false);
            }
        }
    }
}

QRect LayerCamera::getViewRect() const
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
}

void LayerCamera::setCameraEasingAtFrame(CameraEasingType type, int frame) const
{
    Camera* camera = getLastCameraAtFrame(frame, 0);
    camera->setEasingType(type);
    camera->updateViewTransform();
}

void LayerCamera::resetCameraAtFrame(CameraFieldOption type, int frame) const
{
    int frameToModify = frame;
    if (!keyExists(frame)) {
        frameToModify = getPreviousKeyFramePosition(frame);
    }
    Camera* camera = getLastCameraAtFrame(frameToModify, 0);

    switch (type)
    {
    case CameraFieldOption::RESET_FIELD:
        camera->resetTransform();
        break;
    case CameraFieldOption::RESET_TRANSLATION:
        camera->translate(QPoint(0,0));
        break;
    case CameraFieldOption::RESET_SCALING:
        camera->scale(1.0);
        break;
    case CameraFieldOption::RESET_ROTATION:
        camera->rotate(0.0);
        break;
    case CameraFieldOption::ALIGN_HORIZONTAL: {
        qreal otherYCoord = camera->translation().y();
        Camera* nextCam = getCameraAtFrame(getNextKeyFramePosition(frameToModify));
        nextCam->translate(nextCam->translation().x(), otherYCoord);
        camera->setPathControlPointMoved(false);
        break;
    }
    case CameraFieldOption::ALIGN_VERTICAL: {
        qreal otherXCoord = camera->translation().x();
        Camera* nextCam = getCameraAtFrame(getNextKeyFramePosition(frameToModify));
        nextCam->translate(otherXCoord, nextCam->translation().y());
        camera->setPathControlPointMoved(false);
        break;
    }
    case CameraFieldOption::HOLD_FRAME: {

        if (getMaxKeyFramePosition() == camera->pos()) { return; }

        QPointF translation = camera->translation();
        qreal rotation = camera->rotation();
        qreal scaling = camera->scaling();
        camera->setPathControlPointMoved(false);
        Camera* nextCamera = getLastCameraAtFrame(getNextKeyFramePosition(frame), 0);
        nextCamera->translate(translation);
        nextCamera->scale(scaling);
        nextCamera->rotate(rotation);
        nextCamera->setPathControlPointMoved(false);
        break;
    }
    default:
        Q_UNREACHABLE();
        break;
    }

    if (type != CameraFieldOption::RESET_SCALING && type != CameraFieldOption::RESET_ROTATION) {
        setPathMovedAtFrame(frame - 1, false);
    }

    camera->updateViewTransform();
}

void LayerCamera::updateDotColor(DotColorType color)
{
    switch (color)
    {
    case DotColorType::RED:
        mDotColor = Qt::red;
        break;
    case DotColorType::GREEN:
        mDotColor = Qt::green;
        break;
    case DotColorType::BLUE:
        mDotColor = Qt::blue;
        break;
    case DotColorType::BLACK:
        mDotColor = Qt::black;
        break;
    case DotColorType::WHITE:
        mDotColor = Qt::white;
        break;
    }
    mDotColorType = color;
}

QString LayerCamera::getInterpolationTextAtFrame(int frame) const
{
    Camera* camera = getLastCameraAtFrame(frame, 0);
    return getInterpolationText(camera->getEasingType());
}

QPointF LayerCamera::getPathControlPointAtFrame(int frame) const
{
    Camera* camera = getCameraAtFrame(getPreviousKeyFramePosition(frame));
    Q_ASSERT(camera);

    return camera->getPathControlPoint();
}

bool LayerCamera::hasSameTranslation(int frame1, int frame2) const
{
    Camera* camera1 = getCameraAtFrame(frame1);
    Camera* camera2 = getCameraAtFrame(frame2);

    if (camera1 == nullptr)
    {
        return false;
    }
    else if (camera2 == nullptr)
    {
        return false;
    }

    return camera1->translation() == camera2->translation();
}

QList<QPointF> LayerCamera::getBezierPointsAtFrame(int frame) const
{
    QList<QPointF> points;
    int prevFrame = getPreviousKeyFramePosition(frame);
    int nextFrame = getNextKeyFramePosition(frame);
    if (prevFrame < nextFrame)
    {
        Camera* prevCam = getCameraAtFrame(prevFrame);
        Camera* nextCam = getCameraAtFrame(nextFrame);
        points.append(QPointF(-prevCam->translation()));
        if (prevCam->pathControlPointMoved()) {
            points.append(QPointF(prevCam->getPathControlPoint()));
        } else {
            points.append(QPointF(getCenteredPathPoint(prevFrame)));
        }
        points.append(QPointF(-nextCam->translation()));
    }
    return points;
}

QPointF LayerCamera::getCenteredPathPoint(int frame) const
{
    if (!keyExists(frame) || frame == getMaxKeyFramePosition())
        frame = getPreviousKeyFramePosition(frame);
    int nextFrame = getNextKeyFramePosition(frame);
    Camera* cam1 = getCameraAtFrame(frame);
    Camera* cam2 = getCameraAtFrame(nextFrame);

    if (cam1 && cam2 == nullptr) {
        return -cam1->translation();
    } else if (cam2 && cam1 == nullptr) {
        return -cam2->translation();
    } else if (cam1 == nullptr && cam2 == nullptr) {
        return QPointF();
    }
    return QLineF(-cam1->translation(), -cam2->translation()).pointAt(0.5);
}

void LayerCamera::setPathMovedAtFrame(int frame, bool moved) const
{
    Camera* cam = getLastCameraAtFrame(frame, 0);
    Q_ASSERT(cam);

    cam->setPathControlPointMoved(moved);
}

void LayerCamera::updatePathControlPointAtFrame(const QPointF& point, int frame) const
{
    Camera* camera = getLastCameraAtFrame(frame, 0);
    Q_ASSERT(camera);

    camera->setPathControlPoint(point);
    camera->setPathControlPointMoved(true);
}

void LayerCamera::loadImageAtFrame(int frameNumber, qreal dx, qreal dy, qreal rotate, qreal scale, CameraEasingType easing, const QPointF& pathPoint, bool pathMoved)
{
    if (keyExists(frameNumber))
    {
        removeKeyFrame(frameNumber);
    }
    Camera* camera = new Camera(QPointF(dx, dy), rotate, scale);
    camera->setPos(frameNumber);
    camera->setEasingType(easing);
    if (pathMoved) {
        camera->setPathControlPoint(pathPoint);
        camera->setPathControlPointMoved(pathMoved);
    }
    loadKey(camera);
}

Status LayerCamera::saveKeyFrameFile(KeyFrame*, QString)
{
    return Status::OK;
}

KeyFrame* LayerCamera::createKeyFrame(int position)
{
    Camera* c = new Camera;
    c->setPos(position);
    c->setEasingType(CameraEasingType::LINEAR);
    linearInterpolateTransform(c);
    c->setPathControlPoint(c->translation());
    return c;
}

QDomElement LayerCamera::createDomElement(QDomDocument& doc) const
{
    QDomElement layerElem = createBaseDomElement(doc);
    layerElem.setAttribute("width", viewRect.width());
    layerElem.setAttribute("height", viewRect.height());

    if (mShowPath) {
        layerElem.setAttribute("showPath", mShowPath);
    }

    if (mDotColorType != DotColorType::RED) {
        layerElem.setAttribute("pathColorType", static_cast<int>(mDotColorType));
    }

    foreachKeyFrame([&](KeyFrame* pKeyFrame)
                    {
                        Camera* camera = static_cast<Camera*>(pKeyFrame);
                        QDomElement keyTag = doc.createElement("camera");
                        keyTag.setAttribute("frame", camera->pos());

                        keyTag.setAttribute("r", camera->rotation());
                        keyTag.setAttribute("s", camera->scaling());
                        keyTag.setAttribute("dx", camera->translation().x());
                        keyTag.setAttribute("dy", camera->translation().y());

                        if (camera->getEasingType() != CameraEasingType::LINEAR) {
                            keyTag.setAttribute("easing", static_cast<int>(camera->getEasingType()));
                        }
                        if (camera->pathControlPointMoved()) {
                            keyTag.setAttribute("pathCPX", camera->getPathControlPoint().x());
                            keyTag.setAttribute("pathCPY", camera->getPathControlPoint().y());
                        }
                        layerElem.appendChild(keyTag);
                    });

    return layerElem;
}

void LayerCamera::loadDomElement(const QDomElement& element, QString dataDirPath, ProgressCallback progressStep)
{
    Q_UNUSED(dataDirPath)
    Q_UNUSED(progressStep)

    this->loadBaseDomElement(element);

    int width = element.attribute("width").toInt();
    int height = element.attribute("height").toInt();
    mShowPath = element.attribute("showPath").toInt();
    updateDotColor(static_cast<DotColorType>(element.attribute("pathColorType").toInt()));
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
                qreal pathX = imageElement.attribute("pathCPX", "0").toDouble();
                qreal pathY = imageElement.attribute("pathCPY", "0").toDouble();

                bool pathMoved = pathX != 0 || pathY != 0;

                loadImageAtFrame(frame, dx, dy, rotate, scale, easing, QPointF(pathX, pathY), pathMoved);
            }
        }
        imageTag = imageTag.nextSibling();
    }
}
