/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2012-2018 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/
#ifndef TIMELINE_H
#define TIMELINE_H

#include "basedockwidget.h"

class QScrollBar;
class Editor;
class TimeLineCells;
class TimeControls;


class TimeLine : public BaseDockWidget
{
    Q_OBJECT

public:
    TimeLine( QWidget* parent );

    void initUI() override;
    void updateUI() override;

    void updateFrame( int frameNumber );
    void updateLayerNumber( int number );
    void updateLayerView();
    void updateLength();
    void updateContent();
    void setLoop( bool loop );
    void setRangeState( bool range );
    void setPlaying( bool isPlaying );
    
    int  getLength();
    void setLength(int frame);
    void extendLength(int frame);

    int getRangeLower();
    int getRangeUpper();

    void onObjectLoaded();

Q_SIGNALS:
    void modification();

    void addKeyClick();
    void removeKeyClick();
    void duplicateKeyClick();
    void toogleAbsoluteOnionClick();

    void newBitmapLayer();
    void newVectorLayer();
    void newSoundLayer();
    void newCameraLayer();

    void soundClick( bool );
    void fpsClick( int );
    void onionPrevClick();
    void onionNextClick();

public:
    bool scrubbing = false;

protected:
    void resizeEvent( QResizeEvent* event ) override;
    void wheelEvent( QWheelEvent* ) override;

private:
    void deleteCurrentLayer();

    QScrollBar* mHScrollbar = nullptr;
    QScrollBar* mVScrollbar = nullptr;
    TimeLineCells* mTracks = nullptr;
    TimeLineCells* mLayerList = nullptr;
    TimeControls* mTimeControls = nullptr;

    int mNumLayers = 0;
    int mLastUpdatedFrame = 0;
};

#endif
