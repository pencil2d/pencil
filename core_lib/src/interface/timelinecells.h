/*

Pencil2D - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2012-2020 Matthew Chiawen Chang

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

#include <QString>
#include <QWidget>
#include "layercamera.h"

class Layer;
enum class LayerVisibility;
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
    ~TimeLineCells() override;

    int getLayerNumber(int y) const;
    int getInbetweenLayerNumber(int y) const;
    int getLayerY(int layerNumber) const;
    int getFrameNumber(int x) const;
    int getFrameX(int frameNumber) const;
    int getMouseMoveY() const { return mMouseMoveY; }
    static int getOffsetX() { return mOffsetX; }
    static int getOffsetY() { return mOffsetY; }
    int getLayerHeight() const { return mLayerHeight; }

    int getFrameNumberAtMouseX() const { return mFramePosMouseX; }
    int getLayerNumberAtMouseY() const { return mLayerPosMouseY; }

    int getFrameLength() const {return mFrameLength;}
    int getFrameSize() const { return mFrameSize; }

    void setFrameLength(int n) { mFrameLength = n; }
    void setFrameSize(int size);
    void clearCache() { delete mCache; mCache = new QPixmap( size() ); }

    bool didDetachLayer() const;

signals:
    void mouseMovedY(int);
    void lengthChanged(int);
    void offsetChanged(int);
    void selectionChanged();
    void insertNewKeyFrame();

public slots:
    void updateContent();
    void updateFrame(int frameNumber);
    void hScrollChange(int);
    void vScrollChange(int);
    void setMouseMoveY(int x);

protected:
    void trackScrubber();
    void drawContent();
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;

private slots:
    void loadSetting(SETTING setting);

private:
    void paintTrack(QPainter& painter, const Layer* layer,
                    int x, int y, int width, int height,
                    bool selected, int frameSize) const;

    void paintTicks(QPainter& painter, const QPalette& palette) const;
    void paintFrames(QPainter& painter, QColor trackCol, const Layer* layer, int y, int height, bool selected, int frameSize) const;
    void paintGhostOfFrameAtPosition(QPainter& painter, int recTop, int recHeight, int recWidth, bool selected) const;
    void paintLabel(QPainter& painter, const Layer* layer,
                    int x, int y, int width, int height,
                    bool selected, LayerVisibility layerVisibility) const;
    void paintSelection(QPainter& painter, int x, int y, int width, int height) const;
    void paintLayerGutter(QPainter& painter) const;
    void paintOnionSkin(QPainter& painter) const;

    void editLayerProperties(Layer* layer) const;
    void editLayerProperties(LayerCamera *layer) const;
    void editLayerName(Layer* layer) const;

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

    int mLastScrubFrame = 0;

    int mFramePosMouseX = 0;
    int mLayerPosMouseY = 0;

    int mFromLayer = 0;
    int mToLayer   = 1;
    int mStartLayerNumber = -1;
    int mStartFrameNumber = 0;
    int mLastFrameNumber = -1;

    // is used to move layers, don't use this to get mousePos;
    int mMouseMoveY = 0;
    int mPrevFrame = 0;
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
    const static int mLayerDetachThreshold = 5;
};

#endif // TIMELINECELLS_H
