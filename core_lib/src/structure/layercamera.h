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
#include "camerafieldoption.h"
#include "cameraeasingtype.h"
#include "pencildef.h"

class Camera;

class LayerCamera : public Layer
{
    Q_DECLARE_TR_FUNCTIONS(LayerCamera)
public:
    explicit LayerCamera(int id);
    ~LayerCamera() override;

    void loadImageAtFrame(int frame, qreal dx, qreal dy, qreal rotate, qreal scale, CameraEasingType easing, const QPointF& pathPoint, bool pathMoved);

    QDomElement createDomElement(QDomDocument& doc) const override;
    void loadDomElement(const QDomElement& element, QString dataDirPath, ProgressCallback progressStep) override;

    bool addKeyFrame(int position, KeyFrame* pKeyFrame) override;
    bool removeKeyFrame(int position) override;

    Camera* getCameraAtFrame(int frameNumber) const;
    Camera* getLastCameraAtFrame(int frameNumber, int increment) const;
    QTransform getViewAtFrame(int frameNumber) const;

    QRect getViewRect() const;
    QSize getViewSize() const;
    void setViewRect(QRect newViewRect);

    // Functions for camera path
    void setShowCameraPath(bool show) { mShowPath = show; }
    bool getShowCameraPath() const { return mShowPath; }
    void setCameraEasingAtFrame(CameraEasingType type, int frame) const;
    void resetCameraAtFrame(CameraFieldOption type, int frame) const;
    void updateDotColor(DotColorType color);
    QColor getDotColor() const { return mDotColor; }
    DotColorType getDotColorType() const { return mDotColorType; }

    QString getInterpolationTextAtFrame(int frame) const;
    QPointF getPathControlPointAtFrame(int frame) const;
    bool hasSameTranslation(int frame1, int frame2) const;
    QList<QPointF> getBezierPointsAtFrame(int frame) const;
    QPointF getCenteredPathPoint(int frame) const;
    void updatePathControlPointAtFrame(const QPointF& point, int frame) const;
    void setPathMovedAtFrame(int frame, bool moved) const;

    void splitControlPointIfNeeded(int frame) const;
    void mergeControlPointIfNeeded(int frame) const;

protected:
    Status saveKeyFrameFile(KeyFrame*, QString path) override;
    KeyFrame* createKeyFrame(int position) override;

private:
    void linearInterpolateTransform(Camera*);
    qreal getInterpolationPercent(CameraEasingType type, qreal percent) const;
    QPointF getBezierPoint(const QPointF& first, const QPointF& last, const QPointF& pathPoint, qreal percent) const;

    int mFieldW = 800;
    int mFieldH = 600;
    QRect viewRect;

    bool mShowPath = false;
    QColor mDotColor = Qt::red;
    DotColorType mDotColorType = DotColorType::RED;

    const int mControlPointMergeThreshold = 2000;
};

#endif
