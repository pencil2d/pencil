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

#ifndef CAMERA_H
#define CAMERA_H

#include <QTransform>
#include "keyframe.h"
#include "cameraeasingtype.h"

class Camera : public KeyFrame
{
public:
    explicit Camera();
    explicit Camera(QPointF translation, qreal rotation, qreal scaling, CameraEasingType type);
    explicit Camera(const Camera&);
    ~Camera() override;

    Camera* clone() const override;

    QTransform getView();
    void reset();
    void updateViewTransform();
    void assign(const Camera& rhs);

    void translate(qreal dx, qreal dy);
    void translate(const QPointF);
    QPointF translation() { return mTranslate; }

    void rotate(qreal degree);
    qreal rotation() { return mRotate; }

    void scale(qreal scaleValue);
    void scaleWithOffset(qreal scaleValue, QPointF offset); // for zooming at the mouse position
    qreal scaling() { return mScale; }

    QTransform view;

    bool operator==(const Camera& rhs) const;

    void setEasingType(CameraEasingType type) { mEasingType = type; }
    CameraEasingType getEasingType() const { return mEasingType; }

private:
    QPointF mTranslate;
    qreal mRotate = 0.;
    qreal mScale = 1.;
    bool mNeedUpdateView = true;

    CameraEasingType mEasingType = CameraEasingType::LINEAR;
};

#endif // CAMERA_H
