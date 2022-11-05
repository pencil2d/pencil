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
#include "transform.h"

/* The Transform class is meant to be used for mapping between the local and world coordinate spaces
 * mapFrom(...) will take the input and and return the inverted point in the mapped space.
 * mapToWorld(...) functions will take the input and first map it from the local space and then to the world space.
 *
 * Note: It is assumed that the rect, polygon, point etc... hasn't been transformed when inputted.
*/

QRectF Transform::mapFromLocalRect(const QTransform& transform, const QRect& rect)
{
    return QRectF(transform.inverted().mapRect(QRectF(rect)));
}

QRectF Transform::mapToWorldRect(const QTransform& transform, const QTransform& worldT, const QRect rect)
{
    return worldT.mapRect(mapFromLocalRect(transform, rect));
}

QPointF Transform::mapFromLocalPoint(const QTransform& transform, const QPoint& point)
{
    return QPointF(transform.inverted().map(QPointF(point)));
}

QPointF Transform::mapToWorldPoint(const QTransform& transform, const QTransform& worldT, const QPoint& point)
{
    return worldT.map(mapFromLocalPoint(transform, point));
}

QPolygonF Transform::mapFromLocalPolygon(const QTransform& transform, const QRect& rect)
{
    return QPolygonF(transform.inverted().map(QPolygonF(QRectF(rect))));
}

QPolygonF Transform::mapToWorldPolygon(const QTransform& transform, const QTransform& worldT, const QRect& rect)
{
    return worldT.map(mapFromLocalPolygon(transform, rect));
}
