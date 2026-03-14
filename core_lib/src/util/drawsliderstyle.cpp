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

void drawSliderStyle(QPainter& painter, const QRectF& rect, SliderPainterStyle& style, const QPalette& palette)
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
                            Qt::SizeMode::AbsoluteSize);

    painter.restore();
}

void updateSliderStyleCache(SliderPainterStyle& style, const QSizeF& newSize)
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

QRectF subPixelAdjustRectF(const QRectF rect, qreal devicePixelRatio, qreal borderWidth)
{
    qreal topLeftRatio = devicePixelRatio > 1 ? 0.0 : 0.5;
    qreal bottomRightRatio = devicePixelRatio > 1 ? 0.5 : 0.0;

    return QRectF(rect.left() + borderWidth + topLeftRatio,
                      rect.top() + borderWidth + topLeftRatio,
                      rect.right() - borderWidth - bottomRightRatio,
                      rect.bottom() - borderWidth - bottomRightRatio
                );
}

QRectF calculatedContentsRect(const QRectF& contentRect, qreal devicePixelRatio, qreal borderWidth)
{
    // For non high DPI scaling,
    // we have to move the coordinate 0.5 pixel to account for anti-aliasing
    // Otherwise certain lines will look blurry

    return subPixelAdjustRectF(contentRect, devicePixelRatio, borderWidth);
}

qreal innerCornerRadius(qreal outerRadius, qreal borderWidth)
{
    return outerRadius - borderWidth;
}
