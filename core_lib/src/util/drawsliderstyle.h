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
#ifndef DRAWSLIDERSTYLE_H
#define DRAWSLIDERSTYLE_H

#include <QPainter>
#include <QPalette>

struct SliderPainterStyle {

    SliderPainterStyle(QPalette::ColorRole fillRole,
                       QPalette::ColorRole strokeRole,
                       bool hasCustomFill,
                       QBrush customFill,
                       float borderWidth,
                       float cornerRadiusRatio)
    {
        this->fillRole = fillRole;
        this->strokeRole = strokeRole;
        this->hasCustomFill = hasCustomFill;
        this->customFill = customFill;
        this->borderWidth = borderWidth;
        this->cornerRadiusRatio = cornerRadiusRatio;
    }

    SliderPainterStyle(QPalette::ColorRole strokeRole,
                       bool hasCustomFill,
                       QBrush customFill)
    {
        this->strokeRole = strokeRole;
        this->hasCustomFill = hasCustomFill;
        this->customFill = customFill;
    }


    // The filled part of the slider
    QPalette::ColorRole fillRole     = QPalette::Window;

    // The border of the slider, by default there is none
    QPalette::ColorRole strokeRole   = QPalette::NoRole;

    bool hasCustomFill = false;
    QBrush customFill = QBrush();

    float  borderWidth   = 1.0f;
    float cornerRadiusRatio = 0.2;

    QSizeF cachedSize = {};

    float cachedCornerRadiusX = 0;
    float cachedCornerRadiusY = 0;
};

namespace SliderPainter {
    void drawSliderStyle(QPainter& painter, const QRectF& rect, SliderPainterStyle& style, const QPalette& palette);
    void updateSliderStyleCache(SliderPainterStyle& style, const QSizeF& newSize);
}

#endif // DRAWSLIDERSTYLE_H
