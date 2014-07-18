#include "camera.h"

Camera::Camera()
{
}

Camera::Camera( const QTransform& viewMatrix )
{
    view = viewMatrix;
}

Camera::~Camera()
{
}

