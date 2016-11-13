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



StrokeManager::StrokeManager()
{
    m_timeshot = 0;

    mTabletInUse = false;
    mTabletPressure = 0;

    reset();
}

void StrokeManager::reset()
{
    mStrokeStarted = false;
    strokeQueue.clear();
    pressure = 0.0f;
    hasTangent = false;
}

void StrokeManager::setPressure(float pressure)
{
    mTabletPressure = pressure;
}

QPointF StrokeManager::getEventPosition(QMouseEvent* event)
{
    QPointF pos;

    if ( mTabletInUse )
    {
        // QT BUG (Wacom Tablets): updates are not synchronised in Windows giving different coordinates.
        // Clue: Not a Microsoft nor Wacom problem because other windows apps are working fine in the same tablet mode.
        // Solved: Qt bug in Wacom coding -> a lot of patches but no real solutions.
        QPointF pos2 = event->pos() + mTabletPosition - event->globalPos();
        // Patch: next line skips the coordinate problem and it seems safe .
        pos = event->pos() + mTabletPosition - mTabletPosition.toPoint();
        //pos = event->pos();
        //qDebug() << "New pos" << pos << ", Old pos" << pos2;
    }
    else
    {
        pos = event->localPos();
    }

    return pos;
}

void StrokeManager::mousePressEvent(QMouseEvent* event)
{
    reset();
    if ( !(event->button() == Qt::NoButton) ) // if the user is pressing the left/right button
    {
        mLastPressPixel = getEventPosition(event);
        qDebug() << mLastPressPixel;
    }
    mLastPixel = getEventPosition( event );
    mCurrentPixel = getEventPosition( event );

    mStrokeStarted = true;
    mSingleshotTime.start();
    previousTime = mSingleshotTime.elapsed();

}

void StrokeManager::mouseReleaseEvent(QMouseEvent* event)
{
    // flush out stroke
    if ( mStrokeStarted )
    {
        mouseMoveEvent(event);
    }

    mStrokeStarted = false;
}

void StrokeManager::tabletEvent(QTabletEvent* event)
{
    if (event->type() == QEvent::TabletPress) { mTabletInUse = true; }
    if (event->type() == QEvent::TabletRelease) { mTabletInUse = false; }

    mTabletPosition = event->posF();
    setPressure(event->pressure());
}

void StrokeManager::mouseMoveEvent(QMouseEvent* event)
{
    QPointF pos = getEventPosition(event);
    QPointF smoothPos = QPointF( ( pos.x() + mLastPixel.x() ) / 2.0, ( pos.y() + mLastPixel.y() ) / 2.0 );

    mLastPixel = mCurrentPixel;
    mCurrentPixel = smoothPos;

	if ( !mStrokeStarted )
	{
		return;
	}

    if (!mTabletInUse)   // a mouse is used instead of a tablet
    {
        setPressure(1.0);
    }

    // shift queue
    while ( strokeQueue.size()  >= STROKE_QUEUE_LENGTH )
    {
        strokeQueue.pop_front();
    }

    strokeQueue.push_back( smoothPos );

}

QList<QPointF> StrokeManager::interpolateStroke()
{
    QList<QPointF> result;

    int time = mSingleshotTime.elapsed();
    static const qreal smoothness = 1.f;
    QLineF line( mLastPixel, mCurrentPixel);


    qreal scaleFactor = line.length() * 3.f;

    if ( !hasTangent && scaleFactor > 0.01f)
    {
        hasTangent = true;
        /*
        qDebug() << "scaleFactor" << scaleFactor
                 << "current pixel " << mCurrentPixel
                 << "last pixel" << mLastPixel;
         */
        m_previousTangent = (mCurrentPixel - mLastPixel) * smoothness / (3.0 * scaleFactor);
        //qDebug() << "previous tangent" << m_previousTangent;
        QLineF _line(QPointF(0,0), m_previousTangent);
        // don't bother for small tangents, as they can induce single pixel wobbliness
        if (_line.length() < 2) 
		{
            m_previousTangent = QPointF(0,0);
        }
    }
    else
    {
        QPointF c1 = mLastPixel + m_previousTangent * scaleFactor;
        QPointF newTangent = (mCurrentPixel - c1) * smoothness / (3.0 * scaleFactor);
        //qDebug() << "scalefactor1=" << scaleFactor << m_previousTangent << newTangent;
        if (scaleFactor == 0)
        {
            newTangent = QPointF(0,0);
        }
        else
        {
            //QLineF _line(QPointF(0,0), newTangent);
            //if (_line.length() < 2)
            //{
            //    newTangent = QPointF(0,0);
            //}
        }
        QPointF c2 = mCurrentPixel - newTangent * scaleFactor;
        //c1 = mLastPixel;
        //c2 = mCurrentPixel;
        result << mLastPixel << c1 << c2 << mCurrentPixel;
        /*
        qDebug() << mLastPixel
                 << c1
                 << c2
                 << mCurrentPixel;
         */
        m_previousTangent = newTangent;
    }

    previousTime = time;

    return result;
}

