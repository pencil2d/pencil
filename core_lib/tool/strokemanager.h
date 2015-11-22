#ifndef STROKEMANAGER_H
#define STROKEMANAGER_H

#include <deque>
#include <QPointF>
#include <QList>
#include <QPoint>
#include <time.h>
#include <QTabletEvent>
#include <QTimer>
#include <QTime>

class StrokeManager
{
public:
    StrokeManager();

    void tabletEvent(QTabletEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void setPressure(float pressure);

    float getPressure() { return mTabletPressure; }
    bool isTabletInUse() { return mTabletInUse; }

    QList<QPointF> interpolateStroke();

    QPointF getLastPressPixel() const { return mLastPressPixel; }
    QPointF getCurrentPixel() const { return mCurrentPixel; }
    QPointF getLastPixel() const { return mLastPixel; }

private:
    static const int STROKE_QUEUE_LENGTH = 3; // 4 points for cubic bezier

    void reset();

    QPointF getEventPosition(QMouseEvent *);

    float pressure = 1.0f; // last pressure
    std::deque<QPointF> strokeQueue;

    QTime singleshotTime;

    QPointF mLastPressPixel = { 0, 0 };
    QPointF mCurrentPixel   = { 0, 0 };
    QPointF mLastPixel      = { 0, 0 };

    QPointF m_previousTangent;
    bool    hasTangent   = false;
    int     previousTime = 0;

    bool    mStrokeStarted = false;

    bool    mTabletInUse = false;
    float   mTabletPressure = 1.f;
    QPointF mTabletPosition;

    clock_t m_timeshot;
};

#endif // STROKEMANAGER_H
