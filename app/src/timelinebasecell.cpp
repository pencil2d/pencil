#include "timelinebasecell.h"

#include <QMouseEvent>
#include <QDebug>

#include "editor.h"
#include "layer.h"
#include "preferencemanager.h"
#include "timeline.h"

TimeLineBaseCell::TimeLineBaseCell(TimeLine* timeline,
                                   QWidget* parent,
                                    Editor* editor,
                                    const QPoint& origin,
                                    int width,
                                    int height) : QObject(parent)
{
    mTimeLine = timeline;
    mEditor = editor;
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
    mGlobalBounds.translate(x, y);
}
