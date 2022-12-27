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
    explicit Camera(QPointF translation, qreal rotation, qreal scaling);
    explicit Camera(QPointF translation, qreal rotation, qreal scaling, CameraEasingType type);
    explicit Camera(const Camera&);
    ~Camera() override;

    Camera* clone() const override;

    QTransform getView();
    void resetTransform();
    void updateViewTransform();
    void assign(const Camera& rhs);

    void translate(qreal dx, qreal dy);
    void translate(const QPointF);
    QPointF translation() const { return mTranslate; }

    void rotate(qreal degree);
    qreal rotation() const { return mRotate; }

    void scale(qreal scaleValue);
    qreal scaling() const { return mScale; }

    bool compare(const Camera& rhs) const;

    void setDistance(int dist) { mDistance = dist; }
    int getDistance() { return mDistance; }

    void setEasingType(CameraEasingType type);
    CameraEasingType getEasingType() const { return mEasingType; }

    void setPathControlPoint(QPointF point);
    QPointF getPathControlPoint() const { return mPathControlPoint; }

    void setPathControlPointMoved(bool pathMoved);
    bool pathControlPointMoved() const { return mPathControlPointMoved; }

private:
    QTransform mView;
    QPointF mTranslate;
    qreal mRotate = 0.;
    qreal mScale = 1.;
    bool mNeedUpdateView = true;
    int mDistance = 10000; // 10000 mm = 10 m.

    CameraEasingType mEasingType = CameraEasingType::LINEAR;
    QPointF mPathControlPoint = QPointF();
    bool mPathControlPointMoved = false;
};

#endif // CAMERA_H
