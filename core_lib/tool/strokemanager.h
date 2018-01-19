/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2012-2018 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/

#ifndef STROKEMANAGER_H
#define STROKEMANAGER_H

#include <QQueue>
#include <QPointF>
#include <QList>
#include <QPoint>
#include <time.h>
#include <QTabletEvent>
#include <QTimer>
#include <QTime>
#include "object.h"
#include "assert.h"

class StrokeManager : public QObject
{
public:
    StrokeManager();

    void tabletEvent(QTabletEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void setPressure(float pressure);
    void setInpolLevel(int level);

    float getPressure() { return mTabletPressure; }
    int getInpolLevel() { return mInpolLevel; }
    bool isTabletInUse() { return mTabletInUse; }

    QList<QPointF> interpolateStroke();
    void interpolatePoll();
    QPointF interpolateStart(QPointF firstPoint);
    void interpolatePollAndPaint();
    void interpolateEnd();
    void smoothMousePos(QPointF pos);
    QList<QPointF> meanInpolOp( QList<QPointF> points, qreal x, qreal y, qreal pressure );
    QList<QPointF> noInpolOp(QList<QPointF> points);
    QList<QPointF> tangentInpolOp(QList<QPointF> points);

    QPointF getLastPressPixel() const { return mLastPressPixel; }
    QPointF getCurrentPixel() const { return mCurrentPixel; }
    QPointF getLastPixel() const { return mLastPixel; }
    QPointF getLastMeanPixel() const { return mLastInterpolated; }
    QPointF getMousePos() const { return mousePos; }

private:

    static const int STROKE_QUEUE_LENGTH = 3; // 4 points for cubic bezier

    void reset();

    QPointF getEventPosition(QMouseEvent *);

    float pressure = 1.0f; // last pressure
    QQueue<QPointF> strokeQueue;
    QQueue<qreal> pressureQueue;

    QTimer timer;

    QTime mSingleshotTime;
    QPointF mLastPressPixel2 = { 0, 0 };
    QPointF mLastPressPixel = { 0, 0 };
    QPointF mCurrentPixel   = { 0, 0 };
    QPointF mLastPixel      = { 0, 0 };
    QPointF mLastInterpolated = { 0, 0 };
    QPointF mousePos = { 0, 0 };

    QPointF m_previousTangent;
    bool    hasTangent   = false;
    int     previousTime = 0;

    bool    mStrokeStarted = false;

    bool    mTabletInUse = false;
    float   mTabletPressure = 1.f;
    int     mInpolLevel = 0;
    QPointF mTabletPosition;
    qreal mMeanPressure;

    clock_t m_timeshot;

};

#endif // STROKEMANAGER_H
