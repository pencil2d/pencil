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
    nQueued = 0;
    nQueued_p = 0;
    pressure = 0.0f;
    velocity[0] = 0;
    velocity[1] = 0;
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

void StrokeManager::interpolate(float t, int &x, int &y)
{
    if (nQueued < 3)
    {
        x = xQueue[0]; // XXX jitter
        y = yQueue[0];
    } else {
        BSpline::interpolate_quad(xQueue + nQueued - 3,
                                  yQueue + nQueued - 3,
                                  t);

        x = xQueue[nQueued];
        y = yQueue[nQueued];
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
    while (nQueued >= STROKE_QUEUE_LENGTH)
    {
        for (int i = 0; i < nQueued - 1; i++) {
            xQueue[i] = xQueue[i+1];
            yQueue[i] = yQueue[i+1];
        }
        nQueued--;
    }

    // update meter
    if (nQueued)
    {
        meter += abs(pos.x() - xQueue[nQueued - 1]) +
                abs(pos.y() - yQueue[nQueued - 1]);
    }

    xQueue[nQueued] = pos.x();
    yQueue[nQueued] = pos.y();
    nQueued++;

    clock_t t = clock();
    if (m_timeshot && nQueued > 2)
    {
        float dt = (t - m_timeshot) / (float)CLOCKS_PER_SEC;
        if (IS_SIGNIFICANT(dt))
        {
            float u = (pos.x() - xQueue[nQueued - 2]) / (100.0f * dt);
            float v = (pos.y() - yQueue[nQueued - 2]) / (100.0f * dt);

            u = MATH::CLAMP(u, -10.0f, +10.0f);
            v = MATH::CLAMP(v, -10.0f, +10.0f);
            velocity[0] = 0.9f * u + 0.1f * velocity[0];
            velocity[1] = 0.9f * v + 0.1f * velocity[1];
        }
    }
    m_timeshot = t;
}

QList<QPoint> StrokeManager::interpolateStrokeInSteps(int steps)
{
    int sx0, sx1;
    int sy0, sy1;

    QList<QPoint> result;

    interpolate(0, sx0, sy0);
    interpolate(1, sx1, sy1);

    int strokeLen = 0;

    for (int j = 0; j < steps && strokeLen < 1024; j++)
    {
        interpolate((float)j/(float)steps, sx0, sy0);

        if (abs (sx0 - sx1) > 1 ||
                abs (sy0 - sy1) > 1 ||
                j == 0) {
            result.append(QPoint(sx0, sy0));
            sx1 = sx0;
            sy1 = sy0;
        }
    }

    return result;
}

QList<QPoint> StrokeManager::interpolateStroke(int radius)
{
    int sx0, sx1;
    int sy0, sy1;

    QList<QPoint> result;

    interpolate(0, sx0, sy0);
    interpolate(1, sx1, sy1);

    const int span   = 1 + qMax(abs (sx1 - sx0), abs (sy1 - sy0));
    static const int strokeQuality = 1;
    const int step  = qMax (1, radius / strokeQuality);

    int strokeLen = 0;

    for (int j = 0; j < span && strokeLen < 1024; j += step)
    {
        interpolate((float)j/span, sx0, sy0);

        if (abs (sx0 - sx1) > 1 ||
                abs (sy0 - sy1) > 1 ||
                j == 0) {
            result.append(QPoint(sx0, sy0));
            sx1 = sx0;
            sy1 = sy0;
        }
    }

    return result;
}

