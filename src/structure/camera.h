#ifndef CAMERA_H
#define CAMERA_H

#include <QMatrix>
#include "keyframe.h"


class Camera : public Keyframe
{
public:
    Camera( QMatrix );
    ~Camera();


    QMatrix view;
};

#endif // CAMERA_H
