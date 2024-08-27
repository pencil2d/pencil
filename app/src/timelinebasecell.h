#ifndef TIMELINEBASECELL_H
#define TIMELINEBASECELL_H


#include <QPainter>
#include <QPalette>

#include "pencildef.h"

class TimeLine;
class Editor;
class Layer;
class PreferenceManager;
class QMouseEvent;

enum class TimeLineBaseCellType
{
    HEADER,
    LAYER,
    TRACK,
};

class TimeLineBaseCell {
public:
    TimeLineBaseCell(TimeLine* parent,
                    Editor* editor,
                    Layer* layer,
                    const QPalette& palette,
                    const QPoint& origin,
                    int width,
                    int height);
    virtual ~TimeLineBaseCell();

    bool contains(const QPoint& point) const;
    void move(int x, int y);
    const QSize size() const { return mGlobalBounds.size(); }
    const QPoint topLeft() const { return mGlobalBounds.topLeft(); }

    QPoint localPosition(QMouseEvent* event) const;

    Editor* mEditor = nullptr;
    TimeLine* mTimeLine = nullptr;
    PreferenceManager* mPrefs = nullptr;
    Layer* mLayer = nullptr;
    QPalette mPalette;

protected:
    virtual void mousePressEvent(QMouseEvent* event) { }
    virtual void mouseMoveEvent(QMouseEvent* event) { }
    virtual void mouseReleaseEvent(QMouseEvent* event) { }
    virtual void paint(QPainter& painter, bool isSelected, const LayerVisibility &LayerVisibility) const {}

private:

    QRect mGlobalBounds = QRect();
};

#endif // TIMELINEBASECELL_H
