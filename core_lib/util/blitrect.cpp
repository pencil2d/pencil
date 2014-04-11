#include <QDebug>

#include "blitrect.h"

BlitRect::BlitRect() :
    QRect()
{
    points = 0;
}

void BlitRect::extend(QPoint p)
{
    if (points == 0) {
        setBottomLeft(p);
        setTopRight(p);
        points = 1;
    } else {
        if (left() > p.x()) {
            setLeft(p.x());
        }
        if (right() < p.x()) {
            setRight(p.x());
        }
        if (top() > p.y()) {
            setTop(p.y());
        }
        if (bottom() < p.y()) {
            setBottom(p.y());
        }
    }
}
