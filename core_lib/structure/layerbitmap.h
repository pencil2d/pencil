/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2012-2017 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/
#ifndef LAYERBITMAP_H
#define LAYERBITMAP_H

#include "layer.h"

class BitmapImage;

class LayerBitmap : public Layer
{
    Q_OBJECT

public:
    LayerBitmap( Object* object );
    ~LayerBitmap();

    // method from layerImage
    void loadImageAtFrame( QString strFilePath, QPoint topLeft, int frameNumber );

    QDomElement createDomElement( QDomDocument& doc ) override;
    void loadDomElement( QDomElement element, QString dataDirPath ) override;

    BitmapImage* getBitmapImageAtFrame( int frameNumber );
    BitmapImage* getLastBitmapImageAtFrame( int frameNumber, int increment );

protected:
    Status saveKeyFrame( KeyFrame*, QString strPath ) override;

private:
    QString fileName( int index );
};

#endif
