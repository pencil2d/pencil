#include "timelinetrackheadercell.h"

#include <QPalette>

TimeLineTrackHeaderCell::TimeLineTrackHeaderCell(TimeLine* timeLine, Editor* editor, QPoint origin, QSize size)
{
    mEditor = editor;
    mTimeLine = timeLine;

    mOrigin = origin;
    mSize = size;
}

void TimeLineTrackHeaderCell::paintTicks(QPainter& painter, const QPalette& palette, int frameOffset, int frameWidth, int trackWidth, int offsetX) const
{
    // painter.setPen(palette.color(QPalette::Text));
    // painter.setBrush(palette.brush(QPalette::Text));
    // int fps = mEditor->playback()->fps();
    // for (int i = frameOffset; i < frameOffset + (trackWidth - offsetX) / frameWidth; i++)
    // {
    //     // line x pos + some offset
    //     const int lineX = getFrameX(i) + 1;
    //     if (i + 1 >= mTimeLine->getRangeLower() && i < mTimeLine->getRangeUpper())
    //     {
    //         painter.setPen(Qt::NoPen);
    //         painter.setBrush(palette.color(QPalette::Highlight));

    //         painter.drawRect(lineX, 1, mFrameSize + 1, 2);

    //         painter.setPen(palette.color(QPalette::Text));
    //         painter.setBrush(palette.brush(QPalette::Text));
    //     }

    //     // Draw large tick at fps mark
    //     if (i % fps == 0 || i % fps == fps / 2)
    //     {
    //         painter.drawLine(lineX, 1, lineX, 5);
    //     }
    //     else // draw small tick
    //     {
    //         painter.drawLine(lineX, 1, lineX, 3);
    //     }
    //     if (i == 0 || i % fps == fps - 1)
    //     {
    //         int incr = (i < 9) ? 4 : 0; // poor man’s text centering
    //         painter.drawText(QPoint(lineX + incr, 15), QString::number(i + 1));
    //     }
    // }
}

