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
    bool addImageAtFrame(int frameNumber) override;
    void removeImageAtFrame(int frameNumber) override;

    void loadImageAtFrame(QString, int);
    QImage* getImageAtIndex(int, QSize, bool, bool, qreal, bool );
    QImage* getLastImageAtFrame(int, int, QSize, bool, bool, qreal, bool );

    bool saveImage(int, QString, int) override;
    void setView(QMatrix view);
    QString fileName(int index, int layerNumber) override;
    void setModified(bool trueOrFalse);
    void setModified(int frameNumber, bool trueOrFalse) override;

    QDomElement createDomElement(QDomDocument& doc) override;
    void loadDomElement(QDomElement element,  QString dataDirPath) override;

    // graphic representation -- could be put in another class
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
