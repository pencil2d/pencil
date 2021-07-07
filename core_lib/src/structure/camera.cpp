/*

Pencil2D - Traditional Animation Software
Copyright (C) 2012-2020 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/

#include "camera.h"

Camera::Camera()
{
}

Camera::Camera(QPointF translation, qreal rotation, qreal scaling, CameraEasingType type)
{
    Q_ASSERT(scaling > 0);
    mTranslate = translation;
    mRotate = rotation;
    mScale = scaling;
    mEasingType = static_cast<CameraEasingType>(type);
    updateViewTransform();
}

Camera::Camera(const Camera& c2) : KeyFrame(c2)
{
    mTranslate = c2.mTranslate;
    mRotate = c2.mRotate;
    mScale = c2.mScale;
    mNeedUpdateView = true;
}

Camera::~Camera()
{
}

Camera* Camera::clone() const
{
    return new Camera(*this);
}

void Camera::assign(const Camera& rhs)
{
    mTranslate = rhs.mTranslate;
    mRotate = rhs.mRotate;
    mScale = rhs.mScale;
    mNeedUpdateView = true;
    updateViewTransform();
    modification();
}

QTransform Camera::getView()
{
    if (mNeedUpdateView)
        updateViewTransform();
    return view;
}

void Camera::reset()
{
    mTranslate = QPointF(0, 0);
    mRotate = 0.;
    mScale = 1.;
    mNeedUpdateView = true;
    modification();
}

void Camera::updateViewTransform()
{
    if (mNeedUpdateView)
    {
        QTransform t;
        t.translate(mTranslate.x(), mTranslate.y());

        QTransform r;
        r.rotate(mRotate);

        QTransform s;
        s.scale(mScale, mScale);

        view = t * r * s;
    }
    mNeedUpdateView = false;
}

void Camera::translate(qreal dx, qreal dy)
{
    mTranslate.setX(dx);
    mTranslate.setY(dy);

    mNeedUpdateView = true;
    modification();
}

void Camera::translate(const QPointF pt)
{
    translate(pt.x(), pt.y());
}

void Camera::rotate(qreal degree)
{
    mRotate = degree;
    if (mRotate > 360)
    {
        mRotate = mRotate - 360;
    }
    else if (mRotate < 0)
    {
        mRotate = mRotate + 360;
    }
    mRotate = degree;

    mNeedUpdateView = true;
    modification();
}

void Camera::scale(qreal scaleValue)
{
    mScale = scaleValue;

    mNeedUpdateView = true;
    modification();
}

void Camera::scaleWithOffset(qreal scaleValue, QPointF offset)
{
    mTranslate = (mTranslate + offset) * mScale / scaleValue - offset;
    scale(scaleValue);
}

bool Camera::operator==(const Camera& rhs) const
{
    bool b = (mTranslate == rhs.mTranslate)
        && qFuzzyCompare(mRotate, rhs.mRotate)
        && qFuzzyCompare(mScale, rhs.mScale);

    return b;
}
