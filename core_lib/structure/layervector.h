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

class KeyFrame;


class LayerVector : public Layer
{
    Q_OBJECT

public:
    LayerVector(Object* object);
    ~LayerVector();

    // method from layerImage
    bool addNewKeyAt( int frameNumber ) override;
    void loadImageAtFrame(QString strFileName, int);

    QDomElement createDomElement(QDomDocument& doc) override;
    void loadDomElement(QDomElement element,  QString dataDirPath) override;

    VectorImage* getVectorImageAtFrame(int frameNumber);
    VectorImage* getLastVectorImageAtFrame(int frameNumber, int increment);

    bool usesColour(int index);
    void removeColour(int index);

protected:
    bool saveKeyFrame( KeyFrame*, QString path );
    QString fileName( int index );
};

#endif
