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
#ifndef LAYERSOUND_H
#define LAYERSOUND_H

#include <QSize>
#include <QList>

#include <cstdint>
#include <QString>
#include <QPainter>
#include "layerimage.h"
#include "keyframe.h"


class QMediaPlayer;


class SoundClip : public KeyFrame
{
public:
    QString m_strFilePath;
    QMediaPlayer* m_pPlayer;
    uint64_t m_soundSize;
};


class LayerSound : public Layer
{
    Q_OBJECT

public:
    LayerSound(Object* object);
    ~LayerSound();
    QDomElement createDomElement(QDomDocument& doc);
    void loadDomElement(QDomElement element, QString dataDirPath);

    bool addNewKeyAt( int frameNumber ) override;

    void loadSoundAtFrame( QString filePathString, int frame );

    bool saveImage(int index, QString path, int layerNumber);
    void playSound(int frame,int fps);
    void stopSound();

    // graphic representation -- could be put in another class
    void paintImages(QPainter& painter, TimeLineCells* cells, int x, int y, int width, int height, bool selected, int frameSize);

protected:
    bool saveKeyFrame( KeyFrame*, QString path ) override;

private:
};

#endif

