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

#include <QImage>
#include <QSize>
#include <QList>
#include <QString>
#include <QPainter>
#include <phonon/MediaObject>
#include <phonon/AudioOutput>
#include "layerimage.h"


class LayerSound : public LayerImage
{
    Q_OBJECT

public:
    LayerSound(Object* object);
    ~LayerSound();
    QDomElement createDomElement(QDomDocument& doc);
    void loadDomElement(QDomElement element, QString dataDirPath);

    bool addImageAtFrame(int frameNumber);
    void removeImageAtFrame(int frameNumber);

    //void setModified(bool trueOrFalse) { modified = trueOrFalse; }

    void loadSoundAtFrame( QString filePathString, int frame );

    bool saveImage(int index, QString path, int layerNumber);
    void playSound(int frame,int fps);
    void stopSound();

    bool isEmpty() const { return sound.count() == 0; }
    // graphic representation -- could be put in another class
    void paintImages(QPainter& painter, TimeLineCells* cells, int x, int y, int width, int height, bool selected, int frameSize);

    //void mousePress(QMouseEvent *event, int frameNumber);
    //void mouseMove(QMouseEvent *event, int frameNumber);
    //void mouseRelease(QMouseEvent *event, int frameNumber);

    QString getSoundFilepathAt(int index) { return soundFilepath.at(index); }
    int getSoundSize() { return sound.size(); }
    bool soundIsNotNull(int index) { return (sound[index] != NULL); }

protected:

    QList<QString> soundFilepath;
    QList<qint64> soundSize;
//#	QList<QSound*> sound;
    // graphic representation -- could be put in another class
    void swap(int i, int j);


    QList<Phonon::MediaObject*> sound;
    QList<Phonon::AudioOutput*> outputDevices;


public slots:
    void addTimelineKey(qint64 newTotalTime);
};

#endif

