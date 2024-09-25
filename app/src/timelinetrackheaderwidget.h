/*

Pencil2D - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2008-2009 Mj Mendoza IV
Copyright (C) 2012-2020 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/

#ifndef TIMELINETRACKHEADERWIDGET_H
#define TIMELINETRACKHEADERWIDGET_H

#include <QWidget>
#include <QPainter>

#include "preferencesdef.h"

class Editor;
class TimeLine;

class TimeLineTrackHeaderWidget : public QWidget
{
public:
    TimeLineTrackHeaderWidget(TimeLine* timeLine, Editor* editor);

    void onSettingChanged(SETTING setting);
    void onHScrollChange(int x);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;

private:
    void paintOnionSkin(QPainter& painter) const;
    void paintBackground(QPainter& painter, const QPalette& palette) const;
    void paintTicks(QPainter& painter, const QPalette& palette) const;
    void paintScrubber(QPainter& painter, const QPalette& palette) const;
    void paintSplitter(QPainter& painter, const QPalette& palette) const;

    int getFrameNumber(int x) const;
    int getFrameX(int frameNumber) const;

    int mFrameSize = 10;
    Editor* mEditor = nullptr;
    TimeLine* mTimeLine = nullptr;

    bool mShortScrub = false;

    int mScrollOffsetX = 0;
};

#endif // TIMELINETRACKHEADERWIDGET_H
