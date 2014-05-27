#ifndef CAMERA_H
#define CAMERA_H

#include <QMatrix>
#include "keyframe.h"


class Camera : public KeyFrame
{
public:
    Camera();
    Camera( QMatrix );
    ~Camera();


    QMatrix view;
};

#endif // CAMERA_H
