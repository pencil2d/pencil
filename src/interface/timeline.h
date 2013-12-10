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
#ifndef TIMELINE_H
#define TIMELINE_H

#include <QDockWidget>
#include "toolset.h"
#include "timecontrols.h"

class Editor;
class TimeLineCells;
class ScrollBar;

class TimeLine : public QDockWidget //DockPalette
{
	Q_OBJECT

signals :
	void modification();
	void lengthChange( QString );
	void frameSizeChange( int );
	void fontSizeChange( int );
	void labelChange( int );
	void scrubChange( int );
	void addKeyClick();
	void removeKeyClick();
	void duplicateKeyClick();
	void newBitmapLayer();
	void newVectorLayer();
	void newSoundLayer();
	void newCameraLayer();
	void deleteCurrentLayer();

	void playClick();
	void loopClick( bool );

    void loopControlClick( bool );//adding loopControl
    void loopStartClick(int);
    void loopEndClick(int);

    void toggleLoop( bool );
	void loopToggled( bool );
	void soundClick();
	void endplayClick();
	void startplayClick();
	void fpsClick( int );
	void onionPrevClick();
	void onionNextClick();

	public slots:
	void updateFrame( int frameNumber );
	void updateLayerNumber( int number );
	void updateLayerView();
	void updateLength( int frameLength );
	void updateContent();

public:
	TimeLine( QWidget* parent = 0, Editor* editor = 0 );
	QScrollBar* hScrollBar;
	QScrollBar* vScrollBar;
	bool scrubbing;
	void forceUpdateLength( QString newLength ); //when Animation -> Add Frame is clicked, this will auto update timeline
	void setFps( int );
	int getFrameLength();

protected:
	void resizeEvent( QResizeEvent* event );

private:
	Editor* editor; // the editor for which this timeLine operates
	TimeLineCells* cells;
	TimeLineCells* list;
	int numberOfLayers;
	TimeControls* timeControls;
};

#endif
