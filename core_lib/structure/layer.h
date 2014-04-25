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

    QString name;
    bool visible;
    int id;

    static const int NO_KeyFrame = -1;

    LAYER_TYPE type() { return m_eType; }
    Object* object() { return m_pObject; }

    void switchVisibility() { visible = !visible; }

    // KeyFrame interface
    bool hasKeyFrameAtPosition(int position);
    int getPreviousKeyFramePosition(int position);
    int getNextKeyFramePosition(int position);

    int getMaxKeyFramePosition();
    int getFirstKeyFramePosition();

    virtual bool addNewKeyFrameAt( int frameNumber ) = 0;
    bool addKeyFrame( int position, KeyFrame* );
    bool removeKeyFrame( int position );
    KeyFrame* getKeyFrameAtPosition( int position );
    KeyFrame* getLastKeyFrameAtPosition( int position );

    void foreachKeyFrame( std::function<void( KeyFrame* )> );

    void setModified( int position, bool isModified );

    // export element
    virtual QDomElement createDomElement(QDomDocument& doc); // constructs an dom/xml representation of the layer for the document doc
    virtual void loadDomElement(QDomElement element); // construct a layer from a dom/xml representation
    virtual void loadDomElement(QDomElement element, QString dataDirPath) = 0;

    // graphic representation -- could be put in another class
    virtual void paintTrack(QPainter& painter, TimeLineCells* cells, int x, int y, int height, int width, bool selected, int frameSize);
    void paintLabel(QPainter& painter, TimeLineCells* cells, int x, int y, int height, int width, bool selected, int allLayers);
    virtual void paintSelection(QPainter& painter, int x, int y, int height, int width);

    virtual void mousePress(QMouseEvent*, int frameNumber) = 0;
    virtual void mouseMove(QMouseEvent*, int frameNumber) = 0;
    virtual void mouseRelease(QMouseEvent*, int frameNumber) = 0;
    virtual void mouseDoubleClick(QMouseEvent*, int frameNumber) = 0;

    virtual void editProperties();

private:
    LAYER_TYPE m_eType;
    Object* m_pObject;

    std::map<int, KeyFrame*, std::greater<int>> m_KeyFrames;
};

#endif
