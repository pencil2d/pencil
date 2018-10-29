/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2012-2018 Matthew Chiawen Chang

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

#include <QList>
#include <QDialog>
#include "layer.h"

class QLineEdit;
class QSpinBox;
class Camera;

namespace Ui {
class CameraPropertiesDialog;
}

class CameraPropertiesDialog : public QDialog
{
    Q_OBJECT
public:
    CameraPropertiesDialog(QString name, int width, int height);
    ~CameraPropertiesDialog();
    QString getName();
    void setName(QString);
    int getWidth();
    void setWidth(int);
    int getHeight();
    void setHeight(int);
private:
    Ui::CameraPropertiesDialog* ui = nullptr;
};

class LayerCamera : public Layer
{
    Q_OBJECT

public:
    // constructor
    LayerCamera(Object* object);
    ~LayerCamera();

    void loadImageAtFrame(int frame, qreal dx, qreal dy, qreal rotate, qreal scale);
    
    void editProperties() override;
    QDomElement createDomElement(QDomDocument& doc) override;
    void loadDomElement(QDomElement element, QString dataDirPath, ProgressCallback progressStep) override;

    Camera* getCameraAtFrame(int frameNumber);
    Camera* getLastCameraAtFrame(int frameNumber, int increment);
    QTransform getViewAtFrame(int frameNumber);

    QRect getViewRect();
    QSize getViewSize();

signals:
    void resolutionChanged();

protected:
    Status saveKeyFrameFile(KeyFrame*, QString path) override;
    KeyFrame* createKeyFrame(int position, Object*) override;

private:
    void linearInterpolateTransform(Camera*);

    QRect viewRect;
    CameraPropertiesDialog* dialog = nullptr;
};

#endif
