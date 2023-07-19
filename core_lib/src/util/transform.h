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
#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <QPolygonF>

class QTransform;

/**
 * The Transform class is meant to be used for mapping between the local and world coordinate spaces.
 * @li mapFrom(...) will take the input and and return the inverted point in the mapped space.
 * @li mapToWorld(...) functions will take the input and first map it from the local space and then to the world space.
 *
 * @note It is assumed that the rect, polygon, point etc... hasn't been transformed when inputted.
 */
class Transform
{
public:
    static QRectF mapFromLocalRect(const QTransform& transform, const QRect& rect);
    static QRectF mapToWorldRect(const QTransform& transform, const QTransform& worldT, const QRect rect);

    static QPointF mapFromLocalPoint(const QTransform& transform, const QPoint& point);
    static QPointF mapToWorldPoint(const QTransform& transform, const QTransform& worldT, const QPoint& point);

    static QPolygonF mapFromLocalPolygon(const QTransform& transform, const QRect& rect);
    static QPolygonF mapToWorldPolygon(const QTransform& transform, const QTransform& worldT, const QRect& rect);
};

#endif // TRANSFORM_H
