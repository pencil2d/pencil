/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation;

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
protected:
    QLineEdit* nameBox;
    QSpinBox* widthBox, *heightBox;
};

class LayerCamera : public Layer
{
    Q_OBJECT

public:
    // constructor
    LayerCamera(Object* object);
    ~LayerCamera();

    // method from layerImage
    QImage* getImageAtIndex(int index);

    void loadImageAtFrame(int, QTransform);
    
    void editProperties() override;
    QDomElement createDomElement(QDomDocument& doc) override;
    void loadDomElement(QDomElement element, QString dataDirPath) override;

    Camera* getCameraAtFrame(int frameNumber);
    Camera* getLastCameraAtFrame(int frameNumber, int increment);
    QTransform getViewAtFrame(int frameNumber);

    QRect getViewRect();

protected:
    bool saveKeyFrame( KeyFrame*, QString path ) override;

    QRect viewRect;
    CameraPropertiesDialog* dialog;
};

#endif
