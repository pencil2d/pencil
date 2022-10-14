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
#include "cameraeasingtype.h"


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

    Camera* camera1 = static_cast<Camera*>(getLastKeyFrameAtPosition(frameNumber));
    camera1->setEasingType(camera1->getEasingType());

    int nextFrame = getNextKeyFramePosition(frameNumber);
    Camera* camera2 = static_cast<Camera*>(getLastKeyFrameAtPosition(nextFrame));
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
    qreal percent = getInterpolationPercent(camera1->getEasingType(), (frameNumber - frame1)/ (frame2 - frame1));

    auto interpolation = [=](double f1, double f2) -> double
    {
        return f1 * (1.0 - percent) + f2 * percent;
    };

    return QTransform(interpolation(camera1->view.m11(), camera2->view.m11()),
                      interpolation(camera1->view.m12(), camera2->view.m12()),
                      interpolation(camera1->view.m21(), camera2->view.m21()),
                      interpolation(camera1->view.m22(), camera2->view.m22()),
                      interpolation(camera1->view.m31(), camera2->view.m31()),
                      interpolation(camera1->view.m32(), camera2->view.m32()));
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

    double dx = lerp(camera1->translation().x(), camera2->translation().x(), percent);
    double dy = lerp(camera1->translation().y(), camera2->translation().y(), percent);
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
    default: easing.setType(QEasingCurve::Linear); break;
    }
    return easing.valueForProgress(percent);
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
}

void LayerCamera::loadImageAtFrame(int frameNumber, qreal dx, qreal dy, qreal rotate, qreal scale, CameraEasingType type)
{
    if (keyExists(frameNumber))
    {
        removeKeyFrame(frameNumber);
    }
    Camera* camera = new Camera(QPointF(dx, dy), rotate, scale, type);
    camera->setPos(frameNumber);
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
                CameraEasingType type = static_cast<CameraEasingType>(imageElement.attribute("easing", "0").toInt());

                loadImageAtFrame(frame, dx, dy, rotate, scale, type);
            }
        }
        imageTag = imageTag.nextSibling();
    }
}
