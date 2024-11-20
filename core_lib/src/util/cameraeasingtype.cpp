/*

Pencil2D - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2012-2020 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/

#include <QCoreApplication>

#include "cameraeasingtype.h"

// Array structure must match CameraEasingType
static const char* interpolationNames[] {
    QT_TRANSLATE_NOOP("CameraEasingType", "Linear"),
    QT_TRANSLATE_NOOP("CameraEasingType", "Moderate Ease-in"),
    QT_TRANSLATE_NOOP("CameraEasingType", "Moderate Ease-out"),
    QT_TRANSLATE_NOOP("CameraEasingType", "Moderate Ease-in - Ease-out"),
    QT_TRANSLATE_NOOP("CameraEasingType", "Moderate Ease-out - Ease-in"),
    QT_TRANSLATE_NOOP("CameraEasingType", "Quick Ease-in"),
    QT_TRANSLATE_NOOP("CameraEasingType", "Quick Ease-out"),
    QT_TRANSLATE_NOOP("CameraEasingType", "Quick Ease-in - Ease-out"),
    QT_TRANSLATE_NOOP("CameraEasingType", "Quick Ease-out - Ease-in"),
    QT_TRANSLATE_NOOP("CameraEasingType", "Fast Ease-in"),
    QT_TRANSLATE_NOOP("CameraEasingType", "Fast Ease-out"),
    QT_TRANSLATE_NOOP("CameraEasingType", "Fast Ease-in - Ease-out"),
    QT_TRANSLATE_NOOP("CameraEasingType", "Fast Ease-out - Ease-in"),
    QT_TRANSLATE_NOOP("CameraEasingType", "Faster Ease-in"),
    QT_TRANSLATE_NOOP("CameraEasingType", "Faster Ease-out"),
    QT_TRANSLATE_NOOP("CameraEasingType", "Faster Ease-in - Ease-out"),
    QT_TRANSLATE_NOOP("CameraEasingType", "Faster Ease-out - Ease-in"),
    QT_TRANSLATE_NOOP("CameraEasingType", "Slow Ease-in"),
    QT_TRANSLATE_NOOP("CameraEasingType", "Slow Ease-out"),
    QT_TRANSLATE_NOOP("CameraEasingType", "Slow Ease-in - Ease-out"),
    QT_TRANSLATE_NOOP("CameraEasingType", "Slow Ease-out - Ease-in"),
    QT_TRANSLATE_NOOP("CameraEasingType", "Fastest Ease-in"),
    QT_TRANSLATE_NOOP("CameraEasingType", "Fastest Ease-out"),
    QT_TRANSLATE_NOOP("CameraEasingType", "Fastest Ease-in - Ease-out"),
    QT_TRANSLATE_NOOP("CameraEasingType", "Fastest Ease-out - Ease-in"),
    QT_TRANSLATE_NOOP("CameraEasingType", "Circle-based Ease-in"),
    QT_TRANSLATE_NOOP("CameraEasingType", "Circle-based Ease-out"),
    QT_TRANSLATE_NOOP("CameraEasingType", "Circle-based Ease-in - Ease-out"),
    QT_TRANSLATE_NOOP("CameraEasingType", "Circle-based Ease-out - Ease-in"),
    QT_TRANSLATE_NOOP("CameraEasingType", "Elastic Ease-in"),
    QT_TRANSLATE_NOOP("CameraEasingType", "Elastic Ease-out"),
    QT_TRANSLATE_NOOP("CameraEasingType", "Elastic Ease-in - Ease-out"),
    QT_TRANSLATE_NOOP("CameraEasingType", "Elastic Ease-out - Ease-in"),
    QT_TRANSLATE_NOOP("CameraEasingType", "Overshoot Ease-in"),
    QT_TRANSLATE_NOOP("CameraEasingType", "Overshoot Ease-out"),
    QT_TRANSLATE_NOOP("CameraEasingType", "Overshoot Ease-in - Ease-out"),
    QT_TRANSLATE_NOOP("CameraEasingType", "Overshoot Ease-out - Ease-in"),
    QT_TRANSLATE_NOOP("CameraEasingType", "Bounce Ease-in"),
    QT_TRANSLATE_NOOP("CameraEasingType", "Bounce Ease-out"),
    QT_TRANSLATE_NOOP("CameraEasingType", "Bounce Ease-in - Ease-out"),
    QT_TRANSLATE_NOOP("CameraEasingType", "Bounce Ease-out - Ease-in"),
};

QString getInterpolationText(CameraEasingType type)
{
    return QCoreApplication::translate("CameraEasingType", interpolationNames[static_cast<int>(type)]);
}
