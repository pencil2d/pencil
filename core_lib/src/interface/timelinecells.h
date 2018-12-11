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
    int getMouseMoveY() { return mMouseMoveY; }
    int getOffsetX() { return mOffsetX; }
    int getOffsetY() { return mOffsetY; }
    int getLayerHeight() { return mLayerHeight; }
    
    int getFrameLength() {return mFrameLength;}
    void setFrameLength(int n) { mFrameLength = n; }
    void setFrameSize(int size);

    int getFrameSize() { return mFrameSize; }
    void clearCache() { if ( mCache ) delete mCache; mCache = new QPixmap( size() ); }

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
    TimeLine* mTimeLine;
    Editor* mEditor; // the editor for which this timeLine operates
    PreferenceManager* mPrefs;

    TIMELINE_CELL_TYPE mType;

    QPixmap* mCache = nullptr;
    bool mDrawFrameNumber = true;
    bool mbShortScrub = false;
    int mFrameLength = 1;
    int mFrameSize = 0;
    int mFontSize = 10;
    bool mScrubbing = false;
    int mLayerHeight = 20;
    int mStartY = 0;
    int mEndY   = 0;

    int mStartLayerNumber = -1;
    int mStartFrameNumber = 0;
    int mLastFrameNumber = -1;
    int mMouseMoveY = 0;
    int mMouseMoveX = 0;
    int mFrameOffset = 0;
    int mLayerOffset = 0;
    Qt::MouseButton primaryButton = Qt::NoButton;

    bool mCanMoveFrame   = false;
    bool mMovingFrames   = false;

    bool mCanBoxSelect   = false;
    bool mBoxSelecting   = false;

    bool mClickSelecting = false;

    const static int mOffsetX = 0;
    const static int mOffsetY = 20;
};

#endif // TIMELINECELLS_H
