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
                    Editor* editor,
                    const QPoint& origin,
                    int width,
                    int height);
    virtual ~TimeLineBaseCell();
    
    virtual TimeLineCellType type() const { return TimeLineCellType::INVALID; }

    bool contains(const QPoint& point) const;
    void move(int x, int y);
    virtual void setSize(const QSize& size) { mGlobalBounds.setSize(size); }
    const QSize size() const { return mGlobalBounds.size(); }
    const QPoint topLeft() const { return mGlobalBounds.topLeft(); }

    Editor* mEditor = nullptr;
    TimeLine* mTimeLine = nullptr;
    PreferenceManager* mPrefs = nullptr;

    QRect mGlobalBounds = QRect();

private:
};

#endif // TIMELINEBASECELL_H
