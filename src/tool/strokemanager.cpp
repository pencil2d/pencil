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


#include <cmath>
#include <limits>

#include <QDebug>
#include <QLineF>
#include <QPainterPath>

#include "strokemanager.h"

#ifndef NAN
    // VS2010 did not implement NAN yet.
    #define NAN (std::numeric_limits<float>::quiet_NaN())
#endif



StrokeManager::StrokeManager()
{
    m_timeshot = 0;

    m_tabletInUse = false;
    m_tabletPressure = 0;
    m_useHighResPosition = true;

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

    if (m_tabletInUse && m_useHighResPosition)
    {
        // QT BUG (Wacom Tablets): updates are not synchronised in Windows giving different coordinates.
        // Clue: Not a Microsoft nor Wacom problem because other windows apps are working fine in the same tablet mode.
        // Solved: Qt bug in Wacom coding -> a lot of patches but no real solutions.
        //pos = event->pos() + m_tabletPosition - event->globalPos();
        // Patch: next line skips the coordinate problem and it seems safe .
        pos = event->pos() + m_tabletPosition - m_tabletPosition.toPoint();
        //pos = event->pos();
        qDebug() << pos;
    }
    else
    {
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

QList<QPointF> StrokeManager::interpolateStroke(int radius)
{
    QList<QPointF> result;

    int time = singleshotTime.elapsed();
    static const qreal smoothness = 0.5f;
    QLineF line(m_lastPixel, m_currentPixel);
    if (line.length() == 0) {
        return result;
    }

    qreal scaleFactor = line.length();

    if (!hasTangent && scaleFactor > 0.01f)
    {
        hasTangent = true;
//        qDebug() << "scaleFactor" << scaleFactor << "current pixel " << m_currentPixel << "last pixel" << m_lastPixel;
        m_previousTangent = (m_currentPixel - m_lastPixel) * smoothness / (3.0 * scaleFactor);
//        qDebug() << "previous tangent" << m_previousTangent;
        QLineF _line(QPointF(0,0), m_previousTangent);
        // don't bother for small tangents, as they can induce single pixel wobbliness
        if (_line.length() < 2) {
            m_previousTangent = QPointF(0,0);
        }
    } else {
        QPointF c1 = m_lastPixel + m_previousTangent * scaleFactor;
        QPointF newTangent = (m_currentPixel - c1) * smoothness / (3.0 * scaleFactor);
//        qDebug() << "scalefactor1" << scaleFactor << m_previousTangent << newTangent;
        if (scaleFactor == 0) {
            newTangent = QPointF(0,0);
        } else {
        QLineF _line(QPointF(0,0), newTangent);
        if (_line.length() < 2) {
            newTangent = QPointF(0,0);
        }
        }
        QPointF c2 = m_currentPixel - newTangent * scaleFactor;
//        qDebug() << "scalefactor2" << scaleFactor << m_previousTangent << newTangent;

        result << m_lastPixel << c1 << c2 << m_currentPixel;

        m_previousTangent = newTangent;
    }

    previousTime = time;

    return result;
}

