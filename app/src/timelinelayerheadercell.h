#ifndef TIMELINELAYERHEADERCELL_H
#define TIMELINELAYERHEADERCELL_H

#include "timeline.h"
#include "editor.h"

#include <QPainter>

class TimeLineLayerHeaderCell
{
public:
    TimeLineLayerHeaderCell(TimeLine* timeLine,
                            Editor* editor,
                            QPoint origin,
                            QSize size);
    TimeLineLayerHeaderCell();

    void paintGlobalDotVisibility(QPainter& painter, const QPalette& palette);

private:
    TimeLine* mTimeLine = nullptr;
    Editor* mEditor = nullptr;

    QPoint mOrigin = QPoint();
    QSize mSize = QSize();
};

#endif // TIMELINELAYERHEADERCELL_H
