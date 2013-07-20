#ifndef STROKEMANAGER_H
#define STROKEMANAGER_H

#include <QPointF>
#include <QList>
#include <QPoint>
#include <time.h>
#include <QTabletEvent>

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
    bool isTabletInUse() { return m_tabletInUse; }

    QList<QPoint> interpolateStroke(int radius);
    QList<QPoint> interpolateStrokeInSteps(int steps);

    bool isUsingHighResPosition() { return m_useHighResPosition; }
    void useHighResPosition(bool val) { m_useHighResPosition = val; }

    QPointF getLastPressPixel() const { return m_lastPressPixel; }
    QPointF getCurrentPixel() const { return m_currentPixel; }
    QPointF getLastPixel() const { return m_lastPixel; }

protected:
    static const int STROKE_QUEUE_LENGTH = 3;
    static const int STROKE_PRESSURE_QUEUE_LENGTH = 16;

    void reset();
    void interpolate(float t, int &x, int &y);

    QPointF getEventPosition(QMouseEvent *);

    float pressure; // last pressure
    int xQueue[STROKE_QUEUE_LENGTH + 1];
    int yQueue[STROKE_QUEUE_LENGTH + 1];
    int nQueued;

    float pressQueue[STROKE_PRESSURE_QUEUE_LENGTH + 1];
    int nQueued_p;

    long meter;
    float velocity[2];


    QPointF m_lastPressPixel;
    QPointF m_lastReleasePosition;
    QPointF m_currentPixel;
    QPointF m_lastPixel;

    bool m_strokeStarted;

    bool m_tabletInUse;
    float m_tabletPressure;
    QPointF m_tabletPosition;
    bool m_useHighResPosition;

    clock_t m_timeshot;
};

#endif // STROKEMANAGER_H
