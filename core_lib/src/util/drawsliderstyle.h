#ifndef DRAWSLIDERSTYLE_H
#define DRAWSLIDERSTYLE_H

#include <QPainter>
#include <QPalette>

struct SliderPainterStyle {

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

void drawSliderStyle(QPainter& painter, const QRectF& rect, SliderPainterStyle& style, const QPalette& palette);
void updateSliderStyleCache(SliderPainterStyle& style, const QSizeF& newSize);
QRectF calculatedContentsRect(const QRectF& contentRect, qreal devicePixelRatio, qreal borderWidth);
qreal innerCornerRadius(qreal outerRadius, qreal borderWidth);

#endif // DRAWSLIDERSTYLE_H
