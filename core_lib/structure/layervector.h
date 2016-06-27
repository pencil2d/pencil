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
#include "layer.h"

class VectorImage;

class LayerVector : public Layer
{
    Q_OBJECT

public:
    LayerVector(Object* object);
    ~LayerVector();

    // method from layerImage
    void loadImageAtFrame(QString strFileName, int);

    QDomElement createDomElement(QDomDocument& doc) override;
    void loadDomElement(QDomElement element,  QString dataDirPath) override;

    VectorImage* getVectorImageAtFrame(int frameNumber);
    VectorImage* getLastVectorImageAtFrame(int frameNumber, int increment);

    bool usesColour(int index);
    void removeColour(int index);

protected:
    bool saveKeyFrame( KeyFrame*, QString path ) override;
    QString fileName( int index );
};

#endif
