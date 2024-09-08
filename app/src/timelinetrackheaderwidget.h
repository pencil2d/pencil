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
