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
    void strokeStart(QPointF pos, float pressure);
    void strokeMove(QPointF pos, float pressure);
    void strokeEnd(QPointF pos, float pressure);
    QList<QPoint> applyStroke(int radius);

protected:
    static const int STROKE_QUEUE_LENGTH = 3;

    void reset();
    void interpolate(float t, int &x, int &y);

    float pressure; // last pressure
    int xQueue[STROKE_QUEUE_LENGTH + 1];
    int yQueue[STROKE_QUEUE_LENGTH + 1];
    int nQueued;

    long meter;

    float velocity[2];

    bool m_strokeStarted;

    bool m_tabletInUse;
    float m_tabletPressure;
    QPointF m_tabletPosition;

    clock_t m_timeshot;
};

#endif // STROKEMANAGER_H
