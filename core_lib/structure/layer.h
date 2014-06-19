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
#ifndef LAYER_H
#define LAYER_H

#include <map>
#include <functional>
#include <QString>
#include <QPainter>
#include <QtXml>

class QMouseEvent;
class KeyFrame;
class Object;
class TimeLineCells;

class Layer : public QObject
{
    Q_OBJECT

public:
    enum LAYER_TYPE
    {
        UNDEFINED = 0,
        BITMAP = 1,
        VECTOR = 2,
        MOVIE = 3,
        SOUND = 4,
        CAMERA = 5
    };

    Layer(Object*, LAYER_TYPE);
    virtual ~Layer();

    QString mName;
    bool visible;
    int mId;

    static const int NO_KeyFrame = -1;

    LAYER_TYPE type() { return meType; }
    Object* object() { return mObject; }

    void setName( QString name ) { mName = name; }

    void switchVisibility() { visible = !visible; }

    // KeyFrame interface
    bool hasKeyFrameAtPosition(int position);
    int getPreviousKeyFramePosition(int position);
    int getNextKeyFramePosition(int position);

    int getMaxKeyFramePosition();
    int getFirstKeyFramePosition();

    int keyFrameCount() { return mKeyFrames.size(); }

    virtual bool addNewKeyFrameAt( int frameNumber ) = 0;
    virtual bool saveKeyFrame( KeyFrame*, QString path ) = 0;
    virtual void loadDomElement( QDomElement element, QString dataDirPath ) = 0;
    virtual QDomElement createDomElement( QDomDocument& doc ) = 0;

    bool addKeyFrame( int position, KeyFrame* );
    bool removeKeyFrame( int position );
    bool loadKey( KeyFrame* );
    KeyFrame* getKeyFrameAtPosition( int position );
    KeyFrame* getLastKeyFrameAtPosition( int position );

    void foreachKeyFrame( std::function<void( KeyFrame* )> );

    void setModified( int position, bool isModified );
    
    bool save( QString dataFolder );

    // graphic representation -- could be put in another class
    void paintTrack(QPainter& painter, TimeLineCells* cells, int x, int y, int width, int height, bool selected, int frameSize);
    void paintFrames( QPainter& painter, TimeLineCells* cells, int x, int y, int width, int height, bool selected, int frameSize );
    void paintLabel(QPainter& painter, TimeLineCells* cells, int x, int y, int height, int width, bool selected, int allLayers);
    virtual void paintSelection(QPainter& painter, int x, int y, int height, int width);

    void mousePress(QMouseEvent*, int frameNumber);
    void mouseMove(QMouseEvent*, int frameNumber);
    void mouseRelease(QMouseEvent*, int frameNumber);
    void mouseDoubleClick(QMouseEvent*, int frameNumber);

    virtual void editProperties();

private:
    LAYER_TYPE meType;
    Object* mObject;

    std::map<int, KeyFrame*, std::greater<int>> mKeyFrames;
};

#endif
