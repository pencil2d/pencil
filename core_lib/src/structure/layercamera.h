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
#include "layer.h"
#include "cameraeasingtype.h"

class Camera;

class LayerCamera : public Layer
{
    Q_OBJECT

public:
    explicit LayerCamera(Object* object);
    ~LayerCamera() override;

    void loadImageAtFrame(int frame, qreal dx, qreal dy, qreal rotate, qreal scale, CameraEasingType type);

    QDomElement createDomElement(QDomDocument& doc) const override;
    void loadDomElement(const QDomElement& element, QString dataDirPath, ProgressCallback progressStep) override;

    Camera* getCameraAtFrame(int frameNumber);
    Camera* getLastCameraAtFrame(int frameNumber, int increment);
    QTransform getViewAtFrame(int frameNumber) const;

    QRect getViewRect();
    QSize getViewSize() const;
    void setViewRect(QRect newViewRect);

    void showContextMenu(QPoint point);

signals:
    void resolutionChanged();

protected:
    Status saveKeyFrameFile(KeyFrame*, QString path) override;
    KeyFrame* createKeyFrame(int position, Object*) override;

private:
    void linearInterpolateTransform(Camera*);
    qreal getInterpolationPercent(CameraEasingType type, qreal percent) const;

    int mFieldW = 800;
    int mFieldH = 600;
    QRect viewRect;
};

#endif
