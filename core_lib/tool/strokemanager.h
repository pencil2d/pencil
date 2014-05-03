#ifndef STROKEMANAGER_H
#define STROKEMANAGER_H

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
    bool isTabletInUse() { return m_tabletInUse; }

    QList<QPointF> interpolateStroke(int radius);

    bool isUsingHighResPosition() { return m_useHighResPosition; }
    void useHighResPosition(bool val) { m_useHighResPosition = val; }

    QPointF getLastPressPixel() const { return m_lastPressPixel; }
    QPointF getCurrentPixel() const { return m_currentPixel; }
    QPointF getLastPixel() const { return m_lastPixel; }

private:
    static const int STROKE_QUEUE_LENGTH = 3; // 4 points for cubic bezier

    void reset();

    QPointF getEventPosition(QMouseEvent *);

    float pressure; // last pressure
    QList<QPointF> strokeQueue;

    QTime singleshotTime;

    QPointF m_lastPressPixel;
    QPointF m_lastReleasePosition;
    QPointF m_currentPixel;
    QPointF m_lastPixel;

    QPointF m_previousTangent;
    bool hasTangent;
    int previousTime;

    bool m_strokeStarted;

    bool m_tabletInUse;
    float m_tabletPressure;
    QPointF m_tabletPosition;
    bool m_useHighResPosition;

    clock_t m_timeshot;
};

#endif // STROKEMANAGER_H
