#ifndef CAMERA_H
#define CAMERA_H

#include <QTransform>
#include "keyframe.h"


class Camera : public KeyFrame
{
public:
    Camera();
    Camera( const QTransform& );
    ~Camera();

    QTransform view;
};

#endif // CAMERA_H
