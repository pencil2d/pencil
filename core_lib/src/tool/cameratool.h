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

#include "movemode.h"
#include "basetool.h"
#include "camerafieldoption.h"
#include "preferencemanager.h"

class PointerEvent;
class LayerCamera;

class CameraTool : public BaseTool
{
    Q_OBJECT
public:
    explicit CameraTool(QObject* object);
    ~CameraTool() override;

    QCursor cursor() override;
    MoveMode moveMode();
    ToolType type() override { return ToolType::CAMERA; }
    void loadSettings() override;
    void onDidChangeLayer(int index);
    void onDidLoadObject();

    void pointerPressEvent(PointerEvent*) override;
    void pointerReleaseEvent(PointerEvent*) override;
    void pointerMoveEvent(PointerEvent*) override;

    void setShowCameraPath(const bool showCameraPath) override;
    void resetCameraPath() override;
    void setPathDotColorType(const DotColorType pathDotColor) override;
    void resetTransform(CameraFieldOption option);

    void transformView(LayerCamera* layerCamera, MoveMode mode, const QPointF& point, const QPointF& offset, qreal angle, int frameNumber) const;

private:
    void transformCamera(Qt::KeyboardModifiers keyMod);
    void transformCameraPath();
    void updateSettings(const SETTING setting);
    int constrainedRotation(const qreal rotatedAngle, const int rotationIncrement) const;

    MoveMode getCameraMoveMode(const LayerCamera* layerCamera, int frameNumber, const QPointF& point, qreal tolerance) const;
    MoveMode getPathMoveMode(const LayerCamera* layerCamera, int frameNumber, const QPointF& point, qreal tolerance) const;

    QPointF mTransformOffset;
    MoveMode mCamMoveMode = MoveMode::NONE;
    MoveMode mCamPathMoveMode = MoveMode::NONE;
    int mDragPathFrame = 1;
    int mRotationIncrement = 0;
};

#endif // CAMERATOOL_H
