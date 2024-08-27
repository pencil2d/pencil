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
    void paintSplitter(QPainter& painter, const QPalette& palette);

    void mousePressEvent(QMouseEvent* event, int layerNumber);

private:
    TimeLine* mTimeLine = nullptr;
    Editor* mEditor = nullptr;

    QPoint mOrigin = QPoint();
    QSize mSize = QSize();
};

#endif // TIMELINELAYERHEADERCELL_H
