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

#include <functional>
#include <QSize>
#include <QList>
#include <QString>
#include "layer.h"


class QImage;
class QPainter;
class TimeLineCells;
class KeyFrame;


class LayerImage : public Layer
{
    Q_OBJECT

public:
    LayerImage(Object* object, LAYER_TYPE );
    ~LayerImage();

    // KeyFrame interface
    bool hasKeyFrameAtPosition( int position );
    int getPreviousKeyFramePosition( int position );
    int getNextKeyFramePosition( int position );
    int getMaxFramePosition();
    bool addKeyFrame( int position, KeyFrame* );
    bool removeKeyFrame( int position );
    KeyFrame* getKeyFrameAtPosition( int position );
    KeyFrame* getLastKeyFrameAtPosition( int position );

    // frame <-> image API
    int getFramePositionAt(int index);
    int getIndexAtFrame(int frameNumber);
    int getLastIndexAtFrame(int frameNumber);

    // FIXME: this API only used in vector layer
    virtual QImage* getImageAtIndex( int, QSize, bool, bool, qreal, bool, int ) { return NULL; }

    virtual bool addNewKeyFrameAt(int frameNumber);
    virtual void removeImageAtFrame(int frameNumber);
    virtual void setModified(int frameNumber, bool trueOrFalse);
    void deselectAllFrames();

    bool saveImages(QString path, int layerNumber);
    
    // graphic representation -- could be put in another class
    void paintTrack(QPainter& painter, TimeLineCells* cells, int x, int y, int width, int height, bool selected, int frameSize) override;
    virtual void paintImages(QPainter& painter, TimeLineCells* cells, int x, int y, int width, int height, bool selected, int frameSize);
    void mousePress(QMouseEvent* event, int frameNumber) override;
    void mouseMove(QMouseEvent* event, int frameNumber) override;
    void mouseRelease(QMouseEvent* event, int frameNumber) override;
    void mouseDoubleClick(QMouseEvent* event, int frameNumber) override;

    void foreachKeyFrame( std::function<void( KeyFrame* )> );

protected:
    virtual bool saveKeyFrame( KeyFrame*, QString path ) = 0;

private:
    std::map<int, KeyFrame*, std::greater<int>> m_KeyFrames;
    
    // graphic representation -- could be put in another class
    QList<bool> framesSelected;
    int frameClicked;
    int frameOffset;
};

#endif
