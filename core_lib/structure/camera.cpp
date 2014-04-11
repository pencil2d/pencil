#include "camera.h"

Camera::Camera()
{
    view = QMatrix();
}

Camera::Camera( QMatrix viewMatrix )
{
    view = viewMatrix;
}

Camera::~Camera()
{
}

