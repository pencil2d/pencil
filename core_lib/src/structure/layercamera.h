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
#include "cameraeasingtype.h"

class Camera;

class LayerCamera : public Layer
{
    Q_OBJECT

public:
    explicit LayerCamera(Object* object);
    ~LayerCamera() override;

    void loadImageAtFrame(int frame, qreal dx, qreal dy, qreal rotate, qreal scale, int easing, QPointF midPoint);

    QDomElement createDomElement(QDomDocument& doc) const override;
    void loadDomElement(const QDomElement& element, QString dataDirPath, ProgressCallback progressStep) override;

    Camera* getCameraAtFrame(int frameNumber);
    Camera* getLastCameraAtFrame(int frameNumber, int increment);
    QTransform getViewAtFrame(int frameNumber) const;
    MoveMode getMoveModeForCamera(int frameNumber, QPointF point, qreal tolerance);
    MoveMode getMoveModeForCameraPath(int frameNumber, QPointF point, qreal tolerance);

    void transformCameraView(MoveMode mode, QPointF point, int frameNumber);
    void dragCameraPath(MoveMode mode, QPointF point, int frameNumber);
    void setOffsetPoint(QPointF offset) { mOffsetPoint = offset; }

    QRect getViewRect();
    QSize getViewSize() const;
    void setViewRect(QRect newViewRect);

    void showContextMenu(QPoint point);
    void setShowPath(bool showPath) { mShowPath = showPath; }
    bool getShowPath() { return mShowPath; }
    QPointF getPathMidPont(int frame);
    void setDotColor(QColor color) { dotColor = color ; }
    QColor const getDotColor() { return dotColor; }
    QString getInterpolationText(int frame);

    void updateCameraPath(int frame);
    void updateExistingCameraPath(int frame);
    void updateAllCameraPaths();

signals:
    void resolutionChanged();

protected:
    Status saveKeyFrameFile(KeyFrame*, QString path) override;
    KeyFrame* createKeyFrame(int position, Object*) override;

private:
    void linearInterpolateTransform(Camera*);
    qreal getInterpolationPercent(CameraEasingType type, qreal percent) const;
    CameraEasingType getCameraEasingType(int type);
    void initCameraPath(int frame);
    void setMidPoint(int frame);
    QPointF mOffsetPoint = QPointF();

    int mFieldW = 800;
    int mFieldH = 600;
    QRect viewRect;

    bool mShowPath = false;
    QColor dotColor = Qt::red;

    QList<int> mFrameList;
};

#endif
