#include "timelinebasecell.h"

#include <QMouseEvent>

#include "editor.h"
#include "layer.h"
#include "preferencemanager.h"
#include "timeline.h"

TimeLineBaseCell::TimeLineBaseCell(TimeLine* parent, 
                                    Editor* editor, 
                                    Layer* layer,
                                    const QPalette& palette,
                                    const QPoint& origin, 
                                    int width, 
                                    int height)
{
    mTimeLine = parent;
    mEditor = editor;
    mLayer = layer;
    mPalette = palette;
    mPrefs = mEditor->preference();
    mGlobalBounds = QRect(origin, QSize(width,height));
}

TimeLineBaseCell::~TimeLineBaseCell()
{
}

bool TimeLineBaseCell::contains(const QPoint& point) const
{
    return mGlobalBounds.contains(point);
}

void TimeLineBaseCell::move(int x, int y)
{
    mGlobalBounds.setLeft(x);
    mGlobalBounds.setTop(y);
}

QPoint TimeLineBaseCell::localPosition(QMouseEvent* event) const
{
    return QPoint(mGlobalBounds.x() + event->pos().x(), mGlobalBounds.y() + event->pos().y());
}
