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

#ifndef CAMERATOOL_H
#define CAMERATOOL_H

#include <QCursor>

#include "basetool.h"
#include "camerafieldoption.h"
#include "preferencemanager.h"

enum class CameraMoveType {
    PATH,
    TOPLEFT,
    TOPRIGHT,
    BOTTOMLEFT,
    BOTTOMRIGHT,
    ROTATION,
    CENTER,
    NONE
};

class PointerEvent;
class LayerCamera;

class CameraTool : public BaseTool
{
    Q_OBJECT
public:
    explicit CameraTool(QObject* object);
    ~CameraTool() override;

    QCursor cursor() override;
    ToolType type() override { return ToolType::CAMERA; }

    void loadSettings() override;

    void pointerPressEvent(PointerEvent*) override;
    void pointerReleaseEvent(PointerEvent*) override;
    void pointerMoveEvent(PointerEvent*) override;

    void setShowCameraPath(const bool showCameraPath) override;
    void resetCameraPath() override;
    void setPathDotColorType(const DotColorType pathDotColor) override;
    void resetTransform(CameraFieldOption option);

    void transformView(LayerCamera* layerCamera, CameraMoveType mode, const QPointF& point, const QPointF& offset, qreal angle, int frameNumber) const;

private:
    CameraMoveType moveMode();
    void transformCamera(Qt::KeyboardModifiers keyMod);
    void transformCameraPath();
    void updateSettings(const SETTING setting);
    int constrainedRotation(const qreal rotatedAngle, const int rotationIncrement) const;

    void updateProperties();

    qreal getAngleBetween(const QPointF& pos1, const QPointF& pos2) const;

    CameraMoveType getCameraMoveMode(const LayerCamera* layerCamera, int frameNumber, const QPointF& point, qreal tolerance) const;
    CameraMoveType getPathMoveMode(const LayerCamera* layerCamera, int frameNumber, const QPointF& point, qreal tolerance) const;

    QPointF mTransformOffset;
    CameraMoveType mCamMoveMode = CameraMoveType::NONE;
    CameraMoveType mCamPathMoveMode = CameraMoveType::NONE;
    int mDragPathFrame = 1;
    int mRotationIncrement = 0;

    qreal mStartAngle = 0;
    qreal mCurrentAngle = 0;
};

#endif // CAMERATOOL_H
