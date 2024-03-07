/*

Pencil2D - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2012-2020 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/

#ifndef STROKEINTERPOLATOR_H
#define STROKEINTERPOLATOR_H

#include <QQueue>
#include <QPointF>
#include <QList>
#include <QTimer>


class PointerEvent;

class StrokeInterpolator : public QObject
{
public:
    StrokeInterpolator();

    void pointerPressEvent(PointerEvent* event);
    void pointerMoveEvent(PointerEvent* event);
    void pointerReleaseEvent(PointerEvent* event);
    void setPressure(float pressure);
    void setStabilizerLevel(int level);

    float getPressure() { return mTabletPressure; }
    int getStabilizerLevel() { return mStabilizerLevel; }
    bool isActive() const { return mStrokeStarted; }

    QList<QPointF> interpolateStroke();
    void interpolatePoll();
    QPointF interpolateStart(QPointF firstPoint);
    void interpolatePollAndPaint();
    void interpolateEnd();
    void smoothMousePos(QPointF pos);
    QList<QPointF> meanInpolOp(QList<QPointF> points, qreal x, qreal y, qreal pressure);
    QList<QPointF> noInpolOp(QList<QPointF> points);
    QList<QPointF> tangentInpolOp(QList<QPointF> points);

    QPointF getCurrentPixel() const { return mCurrentPixel; }
    QPointF getLastPixel() const { return mLastPixel; }
    QPointF getLastMeanPixel() const { return mLastInterpolated; }
    QPointF getCurrentPressPixel() const { return mCurrentPressPixel; }

private:
    static const int STROKE_QUEUE_LENGTH = 3; // 4 points for cubic bezier

    void reset();

    float pressure = 1.0f; // last pressure
    QQueue<QPointF> strokeQueue;
    QQueue<qreal> pressureQueue;

    QTimer timer;

    QPointF mCurrentPressPixel = { 0, 0 };
    QPointF mCurrentPixel = { 0, 0 };
    QPointF mLastPixel = { 0, 0 };
    QPointF mLastInterpolated = { 0, 0 };

    QPointF m_previousTangent;
    bool    mHasTangent = false;
    bool    mStrokeStarted = false;
    bool    mTabletInUse = false;
    float   mTabletPressure = 1.f;
    int     mStabilizerLevel = 0;
};

#endif // STROKEINTERPOLATOR_H
