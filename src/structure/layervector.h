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
#ifndef LAYERVECTOR_H
#define LAYERVECTOR_H

#include <QImage>
#include <QSize>
#include <QList>
#include <QString>
#include <QPainter>
#include "layerimage.h"
#include "vectorimage.h"

class LayerVector : public LayerImage
{
    Q_OBJECT

public:
    LayerVector(Object* object);
    ~LayerVector();

    // method from layerImage
    //QImage* getImageAtFrame(int frameNumber);
    //QImage* getLastImageAtFrame(int frameNumber);
    bool addImageAtFrame(int frameNumber);
    void removeImageAtFrame(int frameNumber);

    void loadImageAtFrame(QString, int);
    //void loadImageAtFrame(VectorImage, int);
    virtual QImage* getImageAtIndex(int, QSize, bool, bool, qreal, bool, int);
    QImage* getImageAtFrame(int, QSize, bool, bool, qreal, bool, int);
    QImage* getLastImageAtFrame(int, int, QSize, bool, bool, qreal, bool, int);

    bool saveImage(int, QString, int);
    void setView(QMatrix view);
    QString fileName(int index, int layerNumber);
    void setModified(bool trueOrFalse);
    void setModified(int frameNumber, bool trueOrFalse);

    QDomElement createDomElement(QDomDocument& doc);
    virtual void loadDomElement(QDomElement element,  QString dataDirPath);

    // graphic representation -- could be put in another class
    //void paint(QPainter &painter, int verticalPosition, int layerHeight, int frameSize, int timeLineWidth);
    VectorImage* getVectorImageAtIndex(int index);
    VectorImage* getVectorImageAtFrame(int frameNumber);
    VectorImage* getLastVectorImageAtFrame(int frameNumber, int increment);

    bool usesColour(int index);
    void removeColour(int index);

protected:
    QList<VectorImage*> framesVector;
    QList<QImage*> framesImage; // bitmap output of the vector pictures
    void swap(int i, int j);
    QMatrix myView;
};

#endif

