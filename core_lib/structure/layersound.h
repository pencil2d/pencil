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
#ifndef LAYERSOUND_H
#define LAYERSOUND_H

#include <cstdint>
#include "keyframe.h"
#include "layer.h"

class SoundClip;
class LayerSound : public Layer
{
    Q_OBJECT

public:
    LayerSound( Object* object );
    ~LayerSound();
    QDomElement createDomElement(QDomDocument& doc) override;
    void loadDomElement(QDomElement element, QString dataDirPath) override;

    Status loadSoundClipAtFrame( const QString& sSoundClipName, const QString& filePathString, int frame );

    void updateFrameLengths(int fps);

    // These functions will be removed later.
    // Don't use them!!
    int getSoundSize() { return 0; }
    bool soundIsNotNull( int ) { return true; }
    QString getSoundFilepathAt( int ) { return ""; }
    bool isEmpty() { return true; }
    // These functions will be removed.

    SoundClip* getSoundClipWhichCovers(int frameNumber);

protected:
    Status saveKeyFrame( KeyFrame*, QString path ) override;
};

#endif
