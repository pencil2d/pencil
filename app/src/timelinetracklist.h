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

#ifndef TIMELINETRACKLIST_H
#define TIMELINETRACKLIST_H

#include <QString>
#include <QWidget>

#include "pencildef.h"

class Layer;
enum class LayerVisibility;
class TimeLine;
class QPaintEvent;
class QMouseEvent;
class QResizeEvent;
class Editor;
class PreferenceManager;
class QMenu;
class QAction;
enum class SETTING;

class TimeLineTrackList : public QWidget
{
    Q_OBJECT

public:
    TimeLineTrackList( TimeLine* parent, Editor* editor);
    ~TimeLineTrackList() override;

    int getLayerHeight() const { return mLayerHeight; }

    int getFrameLength() const { return mFrameLength; }
    int getFrameSize() const { return mFrameSize; }

    void setFrameLength(int n) { mFrameLength = n; }
    void setFrameSize(int size);
    void clearCache() { delete mCache; mCache = nullptr; }

    void showCameraMenu(QPoint pos);

signals:
    void mouseMovedY(int);
    void lengthChanged(int);
    void offsetChanged(int);
    void selectionChanged();
    void insertNewKeyFrame();

public slots:
    void onLayerCountChanged(int count);
    void updateContent();
    void updateFrame(int frameNumber);
    void hScrollChange(int);
    void vScrollChange(int);
    void setCellDragY(const DragEvent& event, int y);

protected:
    bool event(QEvent *event) override;
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;

private slots:
    void loadSetting(SETTING setting);

private:
    int getCellNumber(int y) const;
    int getCellY(int layerNumber) const;
    int getFrameX(int frameNumber) const;
    int getFrameNumber(int x) const;

    void onDidLeaveWidget();

    bool trackScrubber();
    void drawContent();
    void paintTrack(QPainter& painter, const Layer* layer, int x, int y, int width, int height, bool selected, int frameSize) const;
    void paintFrames(QPainter& painter, QColor trackCol, const Layer* layer, int y, int height, bool selected, int frameSize) const;
    void paintCurrentFrameBorder(QPainter& painter, int recLeft, int recTop, int recWidth, int recHeight) const;
    void paintFrameCursorOnCurrentLayer(QPainter& painter, int recTop, int recWidth, int recHeight) const;
    void paintSelectedFrames(QPainter& painter, const Layer* layer, const int layerIndex) const;
    void paintSelection(QPainter& painter, int x, int y, int width, int height) const;
    void paintHighlightedFrame(QPainter& painter, int framePos, int recTop, int recWidth, int recHeight) const;

    TimeLine* mTimeLine;
    Editor* mEditor; // the editor for which this timeLine operates
    PreferenceManager* mPrefs;

    QPixmap* mCache = nullptr;
    bool mRedrawContent = false;
    bool mDrawFrameNumber = true;
    bool mbShortScrub = false;
    int mFrameLength = 1;
    int mFrameSize = 0;
    bool mScrubbing = false;
    bool mHighlightFrameEnabled = false;
    int mHighlightedFrame = -1;
    int mLayerHeight = 20;

    DragEvent mDragEvent = DragEvent::ENDED;

    int mCurrentLayerNumber = 0;
    int mLastScrubFrame = 0;

    int mStartFrameNumber = 0;
    int mLastFrameNumber = -1;

    int mCellDragY = 0;
    int mPrevFrame = 0;
    int mFrameOffset = 0;
    Qt::MouseButton primaryButton = Qt::NoButton;

    bool mCanMoveFrame   = false;
    bool mMovingFrames   = false;

    bool mCanBoxSelect   = false;
    bool mBoxSelecting   = false;

    bool mClickSelecting = false;

    int mFramePosMoveX = 0;
    int mLayerPosMoveY = 0;

    int mMouseMoveX = 0;
    int mMousePressX = 0;
};

#endif // TIMELINETRACKLIST_H
