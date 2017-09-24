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
class Status;

class Layer : public QObject
{
    Q_OBJECT

public:
    enum LAYER_TYPE
    {
        UNDEFINED = 0,
        BITMAP    = 1,
        VECTOR    = 2,
        MOVIE     = 3,
        SOUND     = 4,
        CAMERA    = 5,

        //PAINTABLE = 
    };

    explicit Layer(Object*, LAYER_TYPE);
    virtual ~Layer();

    QString mName;
    bool mVisible = true;

    int id() { return mId; }

    LAYER_TYPE type() { return meType; }
    Object* object() { return mObject; }

    void setName( QString name ) { mName = name; }
    QString name() { return mName; }

    void switchVisibility() { mVisible = !mVisible; }

    bool visible() { return mVisible; }

    // KeyFrame interface
    int getMaxKeyFramePosition();
    int firstKeyFramePosition();

    virtual Status saveKeyFrame( KeyFrame*, QString path ) = 0;
    virtual void loadDomElement( QDomElement element, QString dataDirPath ) = 0;
    virtual QDomElement createDomElement( QDomDocument& doc ) = 0;
    
    bool keyExists( int position );
    int  getPreviousKeyFramePosition( int position );
    int  getNextKeyFramePosition( int position );
    int  getPreviousFrameNumber( int position, bool isAbsolute );
    int  getNextFrameNumber( int position, bool isAbsolute );

    int keyFrameCount() { return static_cast< int >( mKeyFrames.size() ); }

    bool addNewEmptyKeyAt( int position );
    bool addKeyFrame( int position, KeyFrame* );
    bool removeKeyFrame(int position);
    bool swapKeyFrames( int position1, int position2 );
    bool moveKeyFrameForward( int position );
    bool moveKeyFrameBackward( int position );
    bool loadKey( KeyFrame* );
    KeyFrame* getKeyFrameAt( int position );
    KeyFrame* getLastKeyFrameAtPosition( int position );
    bool keyExistsWhichCovers(int frameNumber);
    KeyFrame *getKeyFrameWhichCovers(int frameNumber);

    void foreachKeyFrame( std::function<void( KeyFrame* )> );

    void setModified( int position, bool isModified );

    // Handle selection
    bool isFrameSelected( int position );
    void setFrameSelected( int position, bool isSelected);
    void toggleFrameSelected( int position, bool allowMultiple = false );
    void extendSelectionTo( int position );
    void selectAllFramesAfter( int position );
    void deselectAll();

    bool moveSelectedFrames( int offset );
    
    Status save( QString dataFolder );

    // graphic representation -- could be put in another class
    void paintTrack(QPainter& painter, TimeLineCells* cells, int x, int y, int width, int height, bool selected, int frameSize);
    void paintFrames( QPainter& painter, TimeLineCells* cells, int y, int height, bool selected, int frameSize );
    void paintLabel(QPainter& painter, TimeLineCells* cells, int x, int y, int height, int width, bool selected, int allLayers);
    virtual void paintSelection(QPainter& painter, int x, int y, int height, int width);

    void mousePress(QMouseEvent*, int frameNumber);
    void mouseMove(QMouseEvent*, int frameNumber);
    void mouseRelease(QMouseEvent*, int frameNumber);
    void mouseDoubleClick(QMouseEvent*, int frameNumber);

    virtual void editProperties();

    void setUpdated();
    bool isPaintable();

protected:
    void setId( int LayerId ) { mId = LayerId; }

private:
    LAYER_TYPE meType = UNDEFINED;
    Object* mObject   = nullptr;
    int mId           = 0;

    std::map<int, KeyFrame*, std::greater<int>> mKeyFrames;

    // We need to keep track of selected frames ordered by last selected
    // and by position.
    // Both should be pre-sorted on each selection for optimization purpose when moving frames.
    //
    QList<int> mSelectedFrames_byLast; // Used to hadle selection range (based on last selected
    QList<int> mSelectedFrames_byPosition; // Used to handle frames movements on the timeline
};

#endif
