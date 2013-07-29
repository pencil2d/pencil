#ifndef CAMERA_H
#define CAMERA_H

#include <QMatrix>

class Camera
{
public:
    Camera();
    ~Camera();
    QMatrix view;
};

#endif // CAMERA_H
