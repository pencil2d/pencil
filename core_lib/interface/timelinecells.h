/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2012-2017 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation;

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/

#ifndef TIMELINECELLS_H
#define TIMELINECELLS_H

#include <QWidget>
#include <QString>


class TimeLine;
class QPaintEvent;
class QMouseEvent;
class QResizeEvent;
class Editor;
class PreferenceManager;
enum class SETTING;

enum class TIMELINE_CELL_TYPE
{
    Layers,
    Tracks
};

class TimeLineCells : public QWidget
{
    Q_OBJECT

public:
    TimeLineCells( TimeLine* parent, Editor* editor, TIMELINE_CELL_TYPE );
    ~TimeLineCells();
    int getLayerNumber(int y);
    int getLayerY(int layerNumber);
    int getFrameNumber(int x);
    int getFrameX(int frameNumber);
    int getMouseMoveY() { return mouseMoveY; }
    int getOffsetX() { return m_offsetX; }
    int getOffsetY() { return m_offsetY; }
    int getLayerHeight() { return layerHeight; }
    int getFrameLength() {return frameLength;}
    int getFrameSize() { return frameSize; }
    void clearCache() { if ( m_pCache ) delete m_pCache; m_pCache = new QPixmap( size() ); }

Q_SIGNALS:
    void mouseMovedY(int);
    void lengthChanged(int);
    void offsetChanged(int);

public slots:
    void updateContent();
    void updateFrame(int frameNumber);
    void hScrollChange(int);
    void vScrollChange(int);
    void setMouseMoveY(int x);

protected:
    void drawContent();
    void paintOnionSkin(QPainter& painter);
    void paintEvent(QPaintEvent* event);
    void resizeEvent(QResizeEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void mouseDoubleClickEvent(QMouseEvent* event);

private slots:
    void loadSetting(SETTING setting);

private:
    TimeLine* timeLine;
    Editor* mEditor; // the editor for which this timeLine operates
    PreferenceManager* mPrefs;

    TIMELINE_CELL_TYPE m_eType;

    QPixmap* m_pCache;
    bool drawFrameNumber;
    bool shortScrub;
    int frameLength;
    int frameSize;
    int fontSize;
    bool scrubbing;
    int layerHeight;
    const static int m_offsetX = 0;
    const static int m_offsetY = 20;
    int startY, endY, startLayerNumber;
    int startFrameNumber;
    int lastFrameNumber = -1;
    int mouseMoveY, mouseMoveX;
    int frameOffset, layerOffset;
    Qt::MouseButton primaryButton = Qt::NoButton;

    bool canMoveFrame   = false;
    bool movingFrames   = false;

    bool canBoxSelect   = false;
    bool boxSelecting   = false;

    bool clickSelecting = false;

};

#endif // TIMELINECELLS_H
