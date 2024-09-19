#include "timelinebasecell.h"

#include <QMouseEvent>
#include <QDebug>

#include "editor.h"
#include "layer.h"
#include "preferencemanager.h"
#include "timeline.h"

TimeLineBaseCell::TimeLineBaseCell(TimeLine* timeline,
                                   QWidget* parent,
                                   Editor* editor) : QObject(parent)
{
    mTimeLine = timeline;
    mEditor = editor;
    mPrefs = mEditor->preference();
}

TimeLineBaseCell::~TimeLineBaseCell()
{
}
