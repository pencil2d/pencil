/***************************************************************************
 * This code is heavily influenced by the instrument proxy from QAquarelle *
 * QAquarelle -   Copyright (C) 2009 by Anton R. <commanderkyle@gmail.com> *
 *                                                                         *
 *   QAquarelle is free software; you can redistribute it and/or modify    *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <QDebug>
#include <QLineF>

#include <math.h>

#include "bspline.h"
#include "strokemanager.h"

template<class T> inline bool IS_SIGNIFICANT (T x) {
    return x > 0.001f;
}

namespace MATH {
    static inline float CLAMP (float x, float a, float b) {
        if (x > b) return b;
        else if (x < a) return a;
        else return x;
    }

    static inline float CLAMP_01 (float x) {
        if (x > 1.0f) return 1.0f;
        else if (x < 0.0f) return 0.0f;
        else return x;
    }

    static inline float CLAMP_m11 (float x) {
        if (x > 1.0f) return 1.0f;
        else if (x < -1.0f) return -1.0f;
        else return x;
    }

    static inline float lerp (float a, float w0, float b, float w1) {
        return (a * w0 + b * w1) / (w0 + w1);
    }

    static inline float lerp (float a, float b, float w) {
        return (a * (1.0f - w) + b * w);
    }

    static inline float round (float x) {
        double i;
        return (modf (x, &i) < 0.5) ? (int)i : (int)i + 1;
    }
}


StrokeManager::StrokeManager()
{
    m_timeshot = 0;

    m_tabletInUse = false;
    m_tabletPressure = 0;
    m_useHighResPosition = false;

    reset();
}

void StrokeManager::reset()
{
    m_strokeStarted = false;
    meter = 0;
    strokeQueue.clear();
    nQueued_p = 0;
    pressure = 0.0f;
    velocity = QPointF(0,0);
}

void StrokeManager::setPressure(float pressure) {
    while (nQueued_p >= STROKE_PRESSURE_QUEUE_LENGTH) {
        for (int i = 0; i < nQueued_p - 1; i++) {
            pressQueue[i] = pressQueue[i+1];
        }
        nQueued_p--;
    }

    pressQueue[nQueued_p] = pressure;
    nQueued_p++;

    m_tabletPressure = pressure;

}

QPointF cubicSpline(const QPointF &p1, const QPointF &p2, const QPointF &p3, float t)
{
    return p1;
}

QPointF StrokeManager::interpolate(float t)
{
    if (strokeQueue.size() < 3)
    {
        return strokeQueue[0];
    } else {
        return cubicSpline(strokeQueue[0], strokeQueue[1], strokeQueue[2], t);
    }

}

void StrokeManager::mousePressEvent(QMouseEvent *event)
{
    reset();
    if (!(event->button() == Qt::NoButton))    // if the user is pressing the left or right button
    {
        m_lastPressPixel = getEventPosition(event);
    }
    m_lastPixel = getEventPosition(event);

    m_strokeStarted = true;
}

void StrokeManager::mouseReleaseEvent(QMouseEvent *event)
{
    // flush out stroke
    if (m_strokeStarted) {
        mouseMoveEvent(event);
        mouseMoveEvent(event);
    }
    m_lastReleasePosition = getEventPosition(event);

    m_strokeStarted = false;
}

void StrokeManager::tabletEvent(QTabletEvent *event)
{
    if (event->type() == QEvent::TabletPress) { m_tabletInUse = true; }
    if (event->type() == QEvent::TabletRelease) { m_tabletInUse = false; }

    m_tabletPosition = event->hiResGlobalPos();
    setPressure(event->pressure());
}

QPointF StrokeManager::getEventPosition(QMouseEvent *event)
{
    QPointF pos;

    if (m_tabletInUse && m_useHighResPosition) {
        pos = event->pos() + m_tabletPosition - event->globalPos();
    } else {
        pos = event->pos();
    }

    return pos;
}

void StrokeManager::mouseMoveEvent(QMouseEvent *event)
{
    QPointF pos = getEventPosition(event);
    m_lastPixel = m_currentPixel;
    m_currentPixel = pos;

    if (!m_strokeStarted)
        return;


    if (!m_tabletInUse)   // a mouse is used instead of a tablet
    {
        setPressure(1.0);
    }

    // shift queue

    while (strokeQueue.size()  >= STROKE_QUEUE_LENGTH)
    {
        strokeQueue.removeFirst();
    }

    strokeQueue.append(pos);

    clock_t t = clock();
    if (m_timeshot && strokeQueue.size() > 2)
    {
        float dt = (t - m_timeshot) / (float)CLOCKS_PER_SEC;
        if (IS_SIGNIFICANT(dt))
        {
            QPointF f = pos - strokeQueue[strokeQueue.size() - 2];
            f /= (100.0f * dt);

            f.setX(MATH::CLAMP(f.x(), -10.0f, +10.0f));
            f.setY(MATH::CLAMP(f.y(), -10.0f, +10.0f));
            velocity = 0.9f * f + 0.1f * velocity;
        }
    }
    m_timeshot = t;
}

//QList<QPoint> StrokeManager::interpolateStrokeInSteps(int steps)
//{
//    int sx0, sx1;
//    int sy0, sy1;

//    QList<QPoint> result;

//    interpolate(0, sx0, sy0);
//    interpolate(1, sx1, sy1);

//    int strokeLen = 0;

//    for (int j = 0; j < steps && strokeLen < 1024; j++)
//    {
//        interpolate((float)j/(float)steps, sx0, sy0);

//        if (abs (sx0 - sx1) > 1 ||
//                abs (sy0 - sy1) > 1 ||
//                j == 0) {
//            result.append(QPoint(sx0, sy0));
//            sx1 = sx0;
//            sy1 = sy0;
//        }
//    }

//    return result;
//}

QList<QPoint> StrokeManager::interpolateStroke(int radius)
{
    int sx0, sx1;
    int sy0, sy1;

    QList<QPoint> result;

    QPointF p0 = interpolate(0);
    QPointF p1 = interpolate(1);

    QLineF line(p0, p1);

    const int span   = 1 + line.length();
    static const int strokeQuality = 1;
    const int step  = qMax (1, radius / strokeQuality);

//    qDebug() << "span " << span << "step " << step;

    int strokeLen = 0;

    result << p0.toPoint();
    p1 = p0;

    for (int j = step; j <= span && strokeLen < 1024; j += step)
    {
//        qDebug() << "interpolate at " << (float)j/span;
        p0 = interpolate((float)j/span);
        QLineF line(p0, p1);
        if (line.length() > 1) {
            result << p0.toPoint();
            p1 = p0;
        }
    }

    return result;
}

