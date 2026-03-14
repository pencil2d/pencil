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
#include "slidergeometry.h"

QRectF SliderGeometry::contentsRect(const QRectF& contentRect, qreal devicePixelRatio, qreal borderWidth)
{
    // For non high DPI scaling,
    // we have to move the coordinate 0.5 pixel to account for anti-aliasing
    // Otherwise certain lines will look blurry

    return SliderGeometry::subPixelAdjustedRectF(contentRect, devicePixelRatio, borderWidth);
}

qreal SliderGeometry::innerCornerRadius(qreal outerRadius, qreal borderWidth)
{
    return outerRadius - borderWidth;
}

QRectF SliderGeometry::subPixelAdjustedRectF(const QRectF& rect, qreal devicePixelRatio, qreal borderWidth)
{
    qreal topLeftRatio = devicePixelRatio > 1 ? 0.0 : 0.5;
    qreal bottomRightRatio = devicePixelRatio > 1 ? 0.5 : 0.0;

    return QRectF(rect.left() + borderWidth + topLeftRatio,
                      rect.top() + borderWidth + topLeftRatio,
                      rect.right() - borderWidth - bottomRightRatio,
                      rect.bottom() - borderWidth - bottomRightRatio
                );
}
