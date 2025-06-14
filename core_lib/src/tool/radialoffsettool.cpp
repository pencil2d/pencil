/*

Pencil2D - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2012-2020 Matthew Chiawen Chang
Copyright (C) 2025-2099 Oliver S. Larsen

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/

#include "radialoffsettool.h"

#include "pointerevent.h"

#include <QLineF>
#include <QDebug>
#include <QLineF>

RadialOffsetTool::RadialOffsetTool(QObject* parent) : QObject(parent)
{

}

RadialOffsetTool::~RadialOffsetTool()
{
}

void RadialOffsetTool::pointerEvent(PointerEvent* event)
{
    if (event->eventType() == PointerEvent::Press) {
        startAdjusting(event);
    } else if (event->eventType() == PointerEvent::Move) {
        if (event->buttons() & Qt::LeftButton && mIsAdjusting) {
            adjust(event);
        }
    } else if (event->eventType() == PointerEvent::Release && mIsAdjusting) {
        stopAdjusting();
    }
}

void RadialOffsetTool::adjust(PointerEvent* event)
{
    const qreal newValue = QLineF(mAdjustPoint, event->canvasPos()).length();

    emit offsetChanged(newValue);
}

bool RadialOffsetTool::startAdjusting(PointerEvent* event)
{
    const qreal rad = mOffset;

    QPointF direction(-1, -1); // 45 deg back
    QLineF line(event->canvasPos(), event->canvasPos() + direction);
    line.setLength(rad);

    mAdjustPoint = line.p2(); // Adjusted point on circle boundary

    mIsAdjusting = true;
    return true;
}

void RadialOffsetTool::stopAdjusting()
{
    Q_UNUSED(event)
    mIsAdjusting = false;
    mAdjustPoint = QPointF();
}
