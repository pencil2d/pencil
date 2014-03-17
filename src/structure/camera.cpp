#include "camera.h"

Camera::Camera( QMatrix viewMatrix )
{
    view = viewMatrix;
}

Camera::~Camera()
{
}

