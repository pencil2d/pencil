#ifndef TIMELINETRACKHEADERCELL_H
#define TIMELINETRACKHEADERCELL_H

#include "timeline.h"
#include "editor.h"

#include <QPainter>

class TimeLineTrackHeaderCell
{
public:
    TimeLineTrackHeaderCell(TimeLine* timeLine,
                            Editor* editor,
                            QPoint origin,
                            QSize size);

    void paintTicks(QPainter& painter,
                    const QPalette& palette,
                    int frameOffset,
                    int frameWidth,
                    int trackWidth,
                    int offsetX) const;

private:
    TimeLine* mTimeLine = nullptr;
    Editor* mEditor = nullptr;

    QPoint mOrigin = QPoint();
    QSize mSize = QSize();
};

#endif // TIMELINETRACKHEADERCELL_H
