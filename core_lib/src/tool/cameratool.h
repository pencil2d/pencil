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

#include <QPen>

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
class Camera;

class CameraTool : public BaseTool
{
    Q_OBJECT
public:
    explicit CameraTool(QObject* object);
    ~CameraTool() override;

    QCursor cursor() override;
    ToolType type() override { return ToolType::CAMERA; }

    void paint(QPainter& painter, const QRect& blitRect) override;

    void loadSettings() override;

    void pointerPressEvent(PointerEvent* event) override;
    void pointerReleaseEvent(PointerEvent* event) override;
    void pointerMoveEvent(PointerEvent* event) override;

    void setShowCameraPath(const bool showCameraPath) override;
    void resetCameraPath() override;
    void setPathDotColorType(const DotColorType pathDotColor) override;
    void resetTransform(CameraFieldOption option);

    void transformView(LayerCamera* layerCamera, CameraMoveType mode, const QPointF& point, const QPointF& offset, qreal angle, int frameNumber) const;

private:

    QPointF localRotationHandlePoint(const QPoint& origin, const QTransform& localT, const qreal objectScale, float worldScale) const;
    QPointF worldRotationHandlePoint(const QPoint& origin, const QTransform& localT, const qreal objectScale, const QTransform& worldT, float worldScale) const;

    void paintHandles(QPainter& painter, const QTransform& worldTransform, const QTransform& camTransform, const QRect& cameraRect, const QPointF translation, const qreal scale, const qreal rotation, bool hollowHandles) const;
    void paintInterpolations(QPainter& painter, const QTransform& worldTransform, int currentFrame, const LayerCamera* cameraLayer, const Camera* keyframe, bool isPlaying) const;
    void paintControlPoint(QPainter& painter, const QTransform& worldTransform, const LayerCamera* cameraLayer, const int frameIndex, const QPointF& pathPoint, bool hollowHandle) const;

    void updateMoveMode(const QPointF& pos);
    void transformCamera(const QPointF& pos, Qt::KeyboardModifiers keyMod);
    void transformCameraPath(const QPointF& pos);
    void updateSettings(const SETTING setting);
    int constrainedRotation(const qreal rotatedAngle, const int rotationIncrement) const;

    void updateProperties();
    void updateUIAssists(const Layer* layer);

    qreal getAngleBetween(const QPointF& pos1, const QPointF& pos2) const;

    CameraMoveType getCameraMoveMode(const QPointF& point, qreal tolerance) const;
    CameraMoveType getPathMoveMode(const LayerCamera* layerCamera, int frameNumber, const QPointF& point, qreal tolerance) const;

    QPointF mTransformOffset;
    CameraMoveType mCamMoveMode = CameraMoveType::NONE;
    int mDragPathFrame = 1;
    int mRotationIncrement = 0;

    QRectF mCameraRect;
    QPolygonF mCameraPolygon;
    QPointF mRotationHandlePoint;

    qreal mStartAngle = 0;
    qreal mCurrentAngle = 0;

    const int mDotWidth = 6;
    const int mHandleWidth = 12;
    const qreal mRotationHandleOffsetPercentage = 0.05;

    QPen mHandlePen;
    QColor mHandleColor;
    QColor mHandleDisabledColor;
    QColor mHandleTextColor;
};

#endif // CAMERATOOL_H
