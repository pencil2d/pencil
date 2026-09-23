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
#ifndef SLIDERGEOMETRY_H
#define SLIDERGEOMETRY_H

#include <QRectF>

namespace SliderGeometry
{
    QRectF contentsRect(const QRectF& rect, qreal borderWidth);
    qreal penStrokeInset(qreal borderWidth);
    qreal innerCornerRadius(qreal outerRadius, qreal borderWidth);
    qreal pickerUpperBound(qreal sliderWidth, qreal pickerWidth);
}

#endif // SLIDERGEOMETRY_H
