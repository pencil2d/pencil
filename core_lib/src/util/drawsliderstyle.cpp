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
#include "drawsliderstyle.h"

QColor resolveColorRole(const QPalette& palette, QPalette::ColorRole role)
{
    if (role == QPalette::NoRole) {
        return Qt::transparent;
    }
    return palette.color(role);
}

QBrush resolveFill(const SliderPainterStyle& style, const QPalette& palette)
{
    if (style.hasCustomFill) {
        return style.customFill;
    } else {
        return resolveColorRole(palette, style.fillRole);
    }
}

void SliderPainter::drawSliderStyle(QPainter& painter, const QRectF& rect, SliderPainterStyle& style, const QPalette& palette)
{
    updateSliderStyleCache(style, rect.size());

    painter.save();
    painter.setRenderHint(QPainter::Antialiasing);

    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);

    QPen pen = resolveColorRole(palette, style.strokeRole);
    pen.setWidth(style.borderWidth);
    painter.setPen(pen);

    painter.setBrush(resolveFill(style, palette));
    painter.drawRoundedRect(rect,
                            style.cachedCornerRadiusX,
                            style.cachedCornerRadiusY,
                            Qt::AbsoluteSize);

    painter.restore();
}

void SliderPainter::updateSliderStyleCache(SliderPainterStyle& style, const QSizeF& newSize)
{
    if (style.cachedSize == newSize) {
        return;
    }

    const qreal minRad = qMin(newSize.width(), newSize.height());
    const qreal maxRad = qMax(newSize.width(), newSize.height());

    qreal radiusRatio = style.cornerRadiusRatio;
    qreal absolutePercentage = maxRad * radiusRatio;

    if (minRad * radiusRatio < absolutePercentage) {
        style.cachedCornerRadiusX = minRad * radiusRatio;
        style.cachedCornerRadiusY = style.cachedCornerRadiusX;
    } else {
        style.cachedCornerRadiusX = absolutePercentage;
        style.cachedCornerRadiusY = style.cachedCornerRadiusX;
    }
    style.cachedSize = newSize;
}
