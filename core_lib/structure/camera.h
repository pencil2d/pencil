/*

Pencil - Traditional Animation Software
Copyright (C) 2012-2017 Matthew Chiawen Chang

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


class Camera : public KeyFrame
{
public:
    Camera();
    Camera(QPointF translation, float rotation, float scaling);
    ~Camera();

    void setViewTransform(QPointF translation, float rotation, float scaling);

    QPointF translation() { return mTranslate; }
    float rotation() { return mRotate; }
    float scaling() { return mScale; }

    QTransform view;

private:
    void updateViewTransform();

    QPointF mTranslate;
    float mRotate = 0.f;
    float mScale = 1.f;
};

#endif // CAMERA_H
