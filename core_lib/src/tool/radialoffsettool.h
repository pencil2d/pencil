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
#ifndef RADIALOFFSETTOOL_H
#define RADIALOFFSETTOOL_H

#include <QObject>
#include <QPointF>

class PointerEvent;

/*
 * A tool that can be used to make quick adjustments
 * based on an offset vector from current pointer position
 *
 *         Drag origin                Cursor position
             ●                           ●
             |                         /
             |                        /
             |  offset vector       /
             |                    /
             ▼                  ▼
        [Adjusted Point] —————————————>  (Direction of drag)
*/
class RadialOffsetTool : public QObject
{
    Q_OBJECT
public:
    RadialOffsetTool(QObject* parent = nullptr);
    ~RadialOffsetTool();

    void setOffset(qreal offset) { mOffset = offset; }
    void pointerEvent(PointerEvent* event);

    void stopAdjusting();
    bool isAdjusting() const { return mIsAdjusting; }

    const QPointF& offsetPoint() const { return mAdjustPoint; }

signals:
    void offsetChanged(qreal distance);

private:

    bool startAdjusting(PointerEvent* event);
    void adjust(PointerEvent* event);

    bool mIsAdjusting = false;
    qreal mOffset = 0.;
    QPointF mAdjustPoint = QPointF();
};

#endif // RADIALOFFSETTOOL_H
