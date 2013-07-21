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
#include <QPainterPath>

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
    hasTangent = false;
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


QPointF StrokeManager::getEventPosition(QMouseEvent *event)
{
    QPointF pos;

    if (m_tabletInUse && m_useHighResPosition) {
//        pos = event->pos() + m_tabletPosition - event->globalPos();
        pos = event->pos();
    } else {
        pos = event->pos();
    }

    return pos;
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
    singleshotTime.start();
    previousTime = singleshotTime.elapsed();
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

    strokeQueue << pos;

}


QVector<qreal> spline(QVector<qreal> x, QVector<qreal> y) {
  float p, qn, sig, un;

  int n = y.size();

  QVector<qreal> y2(n);
  QVector<qreal> u(n);

  // the lower boundary condition is set either to be natural
  y2[0] = u[0] = 0.0;

  for (int i = 1; i < n - 1; i++) {
    // decomposition loop of the tridiagonal algorithm
    // y2 and u are used for temporary storage of the decomposed factors
    sig = (x[i] -x[i-1]) / (x[i+1] -x[i-1]);
    p = sig * y2[i-1] + 2.0;
    y2[i] = (sig - 1.0) / p;
    u[i] = (y[i+1] - y[i]) / (x[i+1] - x[i]) -
      (y[i] - y[i-1]) / (x[i] - x[i-1]);
    u[i] = (6.0* u[i] / (x[i+1] - x[i-1]) - sig * u[i-1]) / p;
  }

  // the upper boundary condition is set either to be natural
  qn = un = 0.0;

  y2[n-1] = (un - qn * u[n-2]) / (qn * y2[n-2] + 1.0);

  for (int k = n-2; k >= 0; k--) {
    y2[k] = y2[k] * y2[k+1] + u[k];
  }

  return y2;
}

qreal splint(QVector<qreal> xa, QVector<qreal> ya, QVector<qreal> y2a, float x) {
  int klo, khi, k;
  float h, b, a;

  int n = xa.size();

  klo = 0;
  khi = n-1;

  while (khi - klo > 1) {
    k = (khi + klo) >> 1;
    if (xa[k] > x) {
      khi = k;
    } else {
      klo = k;
    }
  }

  h = xa[khi] - xa[klo];
  if (h == 0.0) {
      qDebug() << "Bad xa input to routine splint" << xa;
      return NAN;
  }
  a = (xa[khi] - x) / h;
  b = (x - xa[klo]) / h;
  qreal y = a * ya[klo] + b * ya[khi]
    + ((a * a * a - a) * y2a[klo]
       + (b * b * b - b) * y2a[khi]) * (h * h) / 6.0;
  return y;
}


QList<QPointF> StrokeManager::interpolateStroke(int radius)
{
    QList<QPointF> result;

    int time = singleshotTime.elapsed();
    static const qreal smoothness = 1.0;
    QLineF line(m_lastPixel, m_currentPixel);
    if (!hasTangent)
    {
        if (line.length() > 10) {
            hasTangent = true;
            m_previousTangent = (m_currentPixel - m_lastPixel) * smoothness / (3.0 * (line.length()));
        }
        result << m_lastPixel << m_currentPixel;
    } else {
        qreal scaleFactor = line.length();
        if (line.length() < 10) {
            result << m_lastPixel << m_currentPixel;
            hasTangent = false;
        } else {
            QPointF c1 = m_lastPixel + m_previousTangent * scaleFactor;
            QPointF newTangent = (m_currentPixel - c1) * smoothness / (3.0 * line.length());
            qDebug() << "scale factor" << scaleFactor << m_previousTangent << newTangent;
            QPointF c2 = m_currentPixel - newTangent * scaleFactor;

            QPainterPath path(m_lastPixel);
            path.cubicTo(c1, c2, m_currentPixel);

            result << m_lastPixel << c1 << c2 << m_currentPixel;

            m_previousTangent = newTangent;
        }


    }

    previousTime = time;

/*
    QPointF p0 = strokeQueue[0];
    result << p0;

    if (strokeQueue.size() < 3 || false) {
        return result;
    }

    int n = 3;
    QVector<qreal> x(n), y(n), t(n);
    for (int i = 0; i < n; i++) {
        x[i] = strokeQueue[i].x();
        y[i] = strokeQueue[i].y();
        t[i] = i;
    }

    QVector<qreal> x2a = spline(t, x);
    QVector<qreal> y2a = spline(t, y);

    QPointF p1 = strokeQueue[1];

    QLineF line(p0, p1);

    const int span   = 1 + line.length();
    static const float strokeQuality = 1;
    const int step  = qMax (1.0f, radius / strokeQuality);

    int strokeLen = 0;

    p1 = p0;

    for (int j = step; j <= span && strokeLen < 1024; j += step)
    {
        float _t = (float)j/span;
        qreal x0 = splint(t, x, x2a, _t);
        qreal y0 = splint(t, y, y2a, _t);
        p0 = QPointF(x0, y0);

        QLineF line(p0, p1);
        if (line.length() > (radius / 2.0)) {
            result << p0;
            p1 = p0;
        }
    }
    */

//    qDebug() << "interpolated " << strokeQueue << "to" << result;

    return result;
}

