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

    void tabletEvent(QTabletEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void setPressure(float pressure);

    float getPressure() { return m_tabletPressure; }
    bool isTabletInUse() { return mTabletInUse; }

    QList<QPointF> interpolateStroke();

    QPointF getLastPressPixel() const { return m_lastPressPixel; }
    QPointF getCurrentPixel() const { return m_currentPixel; }
    QPointF getLastPixel() const { return m_lastPixel; }

private:
    static const int STROKE_QUEUE_LENGTH = 3; // 4 points for cubic bezier

    void reset();

    QPointF getEventPosition(QMouseEvent *);

    float pressure = 1.0f; // last pressure
    std::deque<QPointF> strokeQueue;

    QTime singleshotTime;

    QPointF m_lastPressPixel = { 0, 0 };
    QPointF m_currentPixel   = { 0, 0 };
    QPointF m_lastPixel      = { 0, 0 };

    QPointF m_previousTangent;
    bool hasTangent;
    int previousTime;

    bool m_strokeStarted;

    bool mTabletInUse;
    float m_tabletPressure;
    QPointF m_tabletPosition;

    clock_t m_timeshot;
};

#endif // STROKEMANAGER_H
