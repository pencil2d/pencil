#ifndef TIMELINEBASECELL_H
#define TIMELINEBASECELL_H


#include <QPainter>
#include <QPalette>
#include <QObject>

#include "pencildef.h"
#include "timelinedef.h"

class TimeLine;
class Editor;
class Layer;
class PreferenceManager;
class QMouseEvent;

class TimeLineBaseCell: public QObject {
    Q_OBJECT
public:
    TimeLineBaseCell(TimeLine* timeline,
                     QWidget* parent,
                    Editor* editor);
    virtual ~TimeLineBaseCell();
    
    virtual TimeLineCellType type() const { return TimeLineCellType::INVALID; }

    virtual void setSize(const QSize& size) = 0;

    Editor* mEditor = nullptr;
    TimeLine* mTimeLine = nullptr;
    PreferenceManager* mPrefs = nullptr;

private:
};

#endif // TIMELINEBASECELL_H
