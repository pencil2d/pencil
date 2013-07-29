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
#ifndef LAYERIMAGE_H
#define LAYERIMAGE_H

#include <QImage>
#include <QSize>
#include <QList>
#include <QString>
#include <QPainter>

#include "layer.h"
#include "keyframe.h"

class TimeLineCells;

class LayerImage : public Layer
{
    Q_OBJECT

public:
    LayerImage(Object* object);
    ~LayerImage();

    // keyframe interface
    virtual bool hasKeyframeAtPosition(int position);
    virtual int getPreviousKeyframePosition(int position);
    virtual int getNextKeyframePosition(int position);
    virtual int getMaxFramePosition();
    virtual int getMaxFrameIndex();

    // frame <-> image API
    int getFramePositionAt(int index);
    int getIndexAtFrame(int frameNumber);
    int getLastIndexAtFrame(int frameNumber);

    virtual QImage* getImageAtIndex(int index);
    virtual QImage* getImageAtIndex(int, QSize, bool, bool, qreal, bool, int) {
        return NULL;
    }

    QImage* getImageAtFrame(int frameNumber);
    QImage* getLastImageAtFrame(int frameNumber, int increment);
    virtual bool addImageAtFrame(int frameNumber);
    virtual void removeImageAtFrame(int frameNumber);
    virtual void setModified(int frameNumber, bool trueOrFalse);
    void deselectAllFrames();

    bool saveImages(QString path, int layerNumber);
    virtual bool saveImage(int index, QString path, int layerNumber);
    virtual QString fileName(int index, int layerNumber);

    // graphic representation -- could be put in another class
    void paintTrack(QPainter& painter, TimeLineCells* cells, int x, int y, int width, int height, bool selected, int frameSize);
    virtual void paintImages(QPainter& painter, TimeLineCells* cells, int x, int y, int width, int height, bool selected, int frameSize);
    void mousePress(QMouseEvent* event, int frameNumber);
    void mouseMove(QMouseEvent* event, int frameNumber);
    void mouseRelease(QMouseEvent* event, int frameNumber);
    void mouseDoubleClick(QMouseEvent* event, int frameNumber);

signals:
    void imageAdded(int);
    void imageAdded(int,int);
    void imageRemoved(int);

protected:
    QList<Keyframe> keyframes;

    // list of frame positions, sorted from lowest to largest
    QList<int> framesPosition;
    QList<int> framesOriginalPosition;
    QList<QString> framesFilename;
    QList<bool> framesModified;

    // graphic representation -- could be put in another class
    QList<bool> framesSelected;
    int frameClicked;
    int frameOffset;

    // sorts all QList according to frame position
    void bubbleSort();
    virtual void swap(int i, int j);
};

#endif
