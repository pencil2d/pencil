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

#include <QColor>
#include <QImage>
#include <QPoint>
#include <QWidget>
#include <QDockWidget>
#include <QScrollBar>
#include <QSound>
#include "toolset.h"
#include "timecontrols.h"

class Editor;

class TimeLine;

class TimeLineCells : public QWidget
{
    Q_OBJECT

public:
    TimeLineCells(TimeLine* parent = 0, Editor* editor = 0, QString type = "");
    //int currentFrame;
    //int currentLayer;
    //int getFps() { return fps; }
    int getLayerNumber(int y);
    int getLayerY(int layerNumber);
    int getFrameNumber(int x);
    int getFrameX(int frameNumber);
    int getMouseMoveY() { return mouseMoveY; }
    int getOffsetY() { return offsetY; }
    int getLayerHeight() { return layerHeight; }
    int getFrameLength() {return frameLength;}

signals:
    void mouseMovedY(int);

public slots:
    void updateContent();
    void updateFrame(int frameNumber);
    void lengthChange(QString); //try to remove content and call TimeLine::forceUpdateLength instead
    void frameSizeChange(int);
    void fontSizeChange(int);
    void scrubChange(int);
    void labelChange(int);
    void hScrollChange(int);
    void vScrollChange(int);
    void setMouseMoveY(int x) { mouseMoveY = x;}

protected:
    void drawContent();
    void paintEvent(QPaintEvent* event);
    void resizeEvent(QResizeEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void mouseDoubleClickEvent(QMouseEvent* event);

private:
    TimeLine* timeLine;
    Editor* editor; // the editor for which this timeLine operates

    QString type;
    QPixmap* cache;
    bool drawFrameNumber;
    bool shortScrub;
    int frameLength;
    int frameSize;
    int fontSize;
    //int fps;
    bool scrubbing;
    int layerHeight;
    int offsetX, offsetY;
    int startY, endY, startLayerNumber;
    int mouseMoveY;
    int frameOffset, layerOffset;
};

class TimeLine : public QDockWidget //DockPalette
{
    Q_OBJECT

signals:
    void modification();
    void lengthChange(QString);
    void frameSizeChange(int);
    void fontSizeChange(int);
    void labelChange(int);
    void scrubChange(int);
    void addKeyClick();
    void removeKeyClick();
    void duplicateKeyClick();
    void newBitmapLayer();
    void newVectorLayer();
    void newSoundLayer();
    void newCameraLayer();
    void deleteCurrentLayer();

    void playClick();
    void loopClick(bool);
    void toggleLoop(bool);
    void loopToggled(bool);
    void soundClick();
    void endplayClick();
    void startplayClick();
    void fpsClick(int);
    void onionPrevClick();
    void onionNextClick();


public slots:
    void updateFrame(int frameNumber);
    void updateLayerNumber(int number);
    void updateLayerView();
    void updateLength(int frameLength);
    void updateContent();

public:
    TimeLine(QWidget* parent = 0, Editor* editor = 0);
    QScrollBar* hScrollBar, *vScrollBar;
    //int currentFrame;
    //int currentLayer;
    bool scrubbing;
    void forceUpdateLength(QString newLength); //when Animation -> Add Frame is clicked, this will auto update timeline
    void setFps ( int );
    int getFrameLength() {return cells->getFrameLength();}

protected:
    void resizeEvent(QResizeEvent* event);
    //void mousePressEvent(QMouseEvent *event);
    //void mouseMoveEvent(QMouseEvent *event);
    //void mouseReleaseEvent(QMouseEvent *event);
    //void mouseDoubleClickEvent(QMouseEvent *event);

private:
    Editor* editor; // the editor for which this timeLine operates
    //TimeLineCells* list;
    TimeLineCells* cells;
    TimeLineCells* list;
    int numberOfLayers;
    TimeControls* timeControls;

};

#endif
