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
#ifndef LAYERCAMERA_H
#define LAYERCAMERA_H

#include <QRect>
#include <QColor>
#include "layer.h"
#include "movemode.h"
#include "camerafieldoption.h"
#include "cameraeasingtype.h"

class Camera;

class LayerCamera : public Layer
{
public:
    explicit LayerCamera(Object* object);
    ~LayerCamera() override;

    void loadImageAtFrame(int frame, qreal dx, qreal dy, qreal rotate, qreal scale, CameraEasingType easing, QPointF pathPoint, bool pathMoved);

    QDomElement createDomElement(QDomDocument& doc) const override;
    void loadDomElement(const QDomElement& element, QString dataDirPath, ProgressCallback progressStep) override;

    Camera* getCameraAtFrame(int frameNumber) const;
    Camera* getLastCameraAtFrame(int frameNumber, int increment) const;
    QTransform getViewAtFrame(int frameNumber) const;
    MoveMode getMoveModeForCamera(int frameNumber, QPointF point, qreal tolerance);
    MoveMode getMoveModeForCameraPath(int frameNumber, QPointF point, qreal tolerance);

    void transformCameraView(MoveMode mode, QPointF point, QPointF offset, qreal angle, int frameNumber);

    QRect getViewRect() const;
    QSize getViewSize() const;
    void setViewRect(QRect newViewRect);

    // Functions for camera path
    void showContextMenu(QPoint point);
    void setShowCameraPath(bool show) { mShowPath = show; }
    bool getShowCameraPath() { return mShowPath; }
    void setCameraEasing(CameraEasingType type, int frame);
    void setCameraReset(CameraFieldOption type, int frame);
    void setDotColorType(DotColorType color);
    QColor getDotColor() const { return mDotColor; }
    DotColorType getDotColorType() const { return mDotColorType; }

    QString getInterpolationText(int frame) const;
    QPointF getPathHandle(int frame) const;
    bool hasSameTranslation(int first, int last) const;
    QList<QPointF> getBezierPoints(int frame) const;
    void centerPathControlPoint(int frame);
    QPointF getNewPathControlPoint(int frame);
    void updatePathControlPointAtFrame(QPointF point, int frame);
    void setPathMovedAtFrame(int frame, bool moved);

    void updateControlPointOnDeleteFrame(int frame);
    void updateControlPointsOnAddFrame(int frame);

protected:
    Status saveKeyFrameFile(KeyFrame*, QString path) override;
    KeyFrame* createKeyFrame(int position, Object*) override;

private:
    void linearInterpolateTransform(Camera*);
    qreal getInterpolationPercent(CameraEasingType type, qreal percent) const;
    QPointF getBezierPoint(QPointF first, QPointF last, QPointF pathPoint, qreal percent) const;
    qreal getRealLineAngle(QLineF line);

    int mFieldW = 800;
    int mFieldH = 600;
    QRect viewRect;

    bool mShowPath = false;
    QColor mDotColor = Qt::red;
    DotColorType mDotColorType = DotColorType::RED;
};

#endif
