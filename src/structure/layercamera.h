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

#include <QImage>
#include <QSize>
#include <QList>
#include <QString>
#include <QPainter>
#include <QDialog>
#include "layerimage.h"
#include "bitmapimage.h"

class QLineEdit;
class QSpinBox;


class Camera
{
public:
    Camera();
    ~Camera();
    QMatrix view;
};

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

class LayerCamera : public LayerImage
{
    Q_OBJECT

public:
    // constructor
    LayerCamera(Object* object);
    ~LayerCamera();

    // method from layerImage
    QImage* getImageAtIndex(int index);
    bool addImageAtFrame(int frameNumber);
    void removeImageAtFrame(int frameNumber);

    void loadImageAtFrame(int, QMatrix);
    bool saveImage(int, QString, int);

    void editProperties();

    QDomElement createDomElement(QDomDocument& doc);
    void loadDomElement(QDomElement element, QString filePath);

    Camera* getCameraAtIndex(int index);
    Camera* getCameraAtFrame(int frameNumber);
    Camera* getLastCameraAtFrame(int frameNumber, int increment);
    QMatrix getViewAtFrame(int frameNumber);

    QRect getViewRect();

protected:
    QRect viewRect;
    CameraPropertiesDialog* dialog;

    QList<Camera*> framesCamera;
    void swap(int i, int j);
};

#endif
