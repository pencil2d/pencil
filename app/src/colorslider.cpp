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
#include "colorslider.h"

#include <QWidget>
#include <QMouseEvent>
#include <QStyleOption>
#include <QPainter>

ColorSlider::ColorSlider(QWidget* parent) : QWidget(parent)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
}

ColorSlider::~ColorSlider()
{

}

void ColorSlider::init(ColorSpecType specType, ColorType type, const QColor &color)
{
    mColor = color;
    mColorType = type;
    mSpecType = specType;

    update();
}

void ColorSlider::setupPicker()
{
    QRectF sliderRect = SliderGeometry::contentsRect(contentsRect(), mSliderStyle.borderWidth);
    mPickerSize = QSizeF(10, sliderRect.bottom() - sliderRect.top() - mSliderStyle.borderWidth);
}

void ColorSlider::paintEvent(QPaintEvent* event)
{
    drawColorBox(mColor, event->rect().size());

    QPainter painter(this);
    painter.drawPixmap(0, 0, mBoxPixmapSource);
    painter.end();

    drawPicker(mColor);
}

int ColorSlider::colorTypeMax() const
{
    switch (mColorType)
    {
        case HUE: return 359;
        case RED:
        case GREEN:
        case BLUE:
        case SAT:
        case VAL:
        case ALPHA: return 255;
        default: Q_UNREACHABLE();
    }
}

int ColorSlider::colorSteps() const
{
    return colorTypeMax() + 1;
}

QLinearGradient ColorSlider::setColorSpec(const QColor &color)
{
    switch (mSpecType)
    {
    case HSV:
        return hsvGradient(color);
    case RGB:
        return rgbGradient(color);
    default:
        Q_UNREACHABLE();
    }
}

QLinearGradient ColorSlider::rgbGradient(const QColor &color)
{
    int val = 0;
    int max = colorTypeMax();
    switch (mColorType)
    {
    case RED:
        for (; val <= max; val += 1)
        {
            mGradient.setColorAt(static_cast<qreal>(val) / max, QColor::fromRgb(val,
                                                             255,
                                                             255,
                                                             color.alpha()));
        }
        break;
    case GREEN:
        for (; val <= max; val += 1)
        {
            mGradient.setColorAt(static_cast<qreal>(val) / max, QColor::fromRgb(color.red(),
                                                             val,
                                                             color.blue(),
                                                             color.alpha()));
        }
        break;
    case BLUE:
        for (; val <= max; val += 1)
        {
            mGradient.setColorAt(static_cast<qreal>(val) / max, QColor::fromRgb(color.red(),
                                                             color.green(),
                                                             val,
                                                             color.alpha()));
        }
        break;
    case ALPHA:
        for (; val <= max; val += 1)
        {
            mGradient.setColorAt(static_cast<qreal>(val) / max, QColor::fromRgb(0,
                                                             0,
                                                             0,
                                                             val));
        }
        break;
    default:
        Q_UNREACHABLE();
    }
    return mGradient;
}

QLinearGradient ColorSlider::hsvGradient(const QColor &color)
{
    int val = 0;
    int max = colorTypeMax();
    switch (mColorType)
    {
    case HUE:
        for (; val <= max; val += 1)
        {
            mGradient.setColorAt(static_cast<qreal>(val) / max, QColor::fromHsv(val,
                                                             255,
                                                             255,
                                                             color.alpha()));
        }
        break;
    case SAT:
        for (; val <= max; val += 1)
        {
            mGradient.setColorAt(static_cast<qreal>(val) / max, QColor::fromHsv(color.hsvHue(),
                                                             val,
                                                             color.value(),
                                                             color.alpha()));
        }
        break;
    case VAL:
        for (; val <= max; val += 1)
        {
            mGradient.setColorAt(static_cast<qreal>(val) / max, QColor::fromHsv(color.hsvHue(),
                                                             color.hsvSaturation(),
                                                             val,
                                                             color.alpha()));
        }
        break;
    case ALPHA:
        for (; val <= max; val += 1)
        {
            mGradient.setColorAt(static_cast<qreal>(val) / max, QColor::fromHsv(0,
                                                             0,
                                                             0,
                                                             val));
        }
        break;
    default:
        Q_UNREACHABLE();
    }
    return mGradient;
}

void ColorSlider::setRgb(const QColor &rgb)
{
    mColor.setRgb(rgb.red(),
                  rgb.green(),
                  rgb.blue(),
                  rgb.alpha());

    mPixmapCacheInvalid = true;
}

void ColorSlider::setHsv(const QColor &hsv)
{
    mColor.setHsv(hsv.hsvHue(),
                  hsv.hsvSaturation(),
                  hsv.value(),
                  hsv.alpha());

    mPixmapCacheInvalid = true;
}

void ColorSlider::resizeEvent(QResizeEvent*)
{
    mPixmapCacheInvalid = true;
    update();
}

void ColorSlider::drawColorBox(const QColor &color, QSize size)
{
    QStyleOption option;
    option.initFrom(this);

    if (mPixmapCacheInvalid) {
        setupPicker();

        QRectF sliderRect = SliderGeometry::contentsRect(contentsRect(), mSliderStyle.borderWidth);
        mBoxPixmapSource = QPixmap(size * devicePixelRatio());
        mBoxPixmapSource.setDevicePixelRatio(devicePixelRatioF());
        mBoxPixmapSource.fill(Qt::transparent);
        mPixmapCacheInvalid = false;

        mGradient = QLinearGradient(0,0,sliderRect.width(),0);
        mGradient = setColorSpec(color);

        QPainter painter(&mBoxPixmapSource);

        mSliderStyle.customFill = QBrush(mCheckerboardPixmap);
        SliderPainter::drawSliderStyle(painter, sliderRect, mSliderStyle, option.palette);

        QBrush brush(mGradient);
        mSliderStyle.customFill = brush;
        SliderPainter::drawSliderStyle(painter, sliderRect, mSliderStyle, option.palette);
    }
}

QSize ColorSlider::sizeHint() const
{
    return {-1, 10};
}

void ColorSlider::mouseMoveEvent(QMouseEvent* event)
{
    colorPicked(event->pos());
}

void ColorSlider::mousePressEvent(QMouseEvent *event)
{
    colorPicked(event->pos());
}

void ColorSlider::drawPicker(const QColor &color)
{
    QPainter painter(this);
    qreal val = 0;

    const qreal borderWidth = mSliderStyle.borderWidth;
    const qreal inset = SliderGeometry::penStrokeInset(borderWidth);
    const QRectF innerSliderRect = SliderGeometry::contentsRect(contentsRect(), borderWidth)
                             .adjusted(borderWidth, borderWidth, -borderWidth, -borderWidth);

    switch (mSpecType)
    {
    case HSV:
        switch (mColorType)
        {
        case HUE:
            val = color.hsvHueF();
            break;
        case SAT:
            val = color.hsvSaturationF();
            break;
        case VAL:
            val = color.valueF();
            break;
        case ALPHA:
            break;
        default:
            Q_UNREACHABLE();
        }
        break;
    case RGB:
        switch (mColorType)
        {
        case RED:
            val = color.redF();
            break;
        case GREEN:
            val = color.greenF();
            break;
        case BLUE:
            val = color.blueF();
            break;
        case ALPHA:
            break;
        default:
            Q_UNREACHABLE();
        }
        break;
    default:
        Q_UNREACHABLE();
    }
    if (mColorType == ALPHA)
    {
        val = color.alphaF();
    }

    const qreal maxDistance = SliderGeometry::pickerUpperBound(innerSliderRect.width(), mPickerSize.width());
    val = static_cast<int>(innerSliderRect.left() + qMax(static_cast<qreal>(mMin), (val * maxDistance))) + inset;

    QPen outerPen;
    outerPen.setJoinStyle(Qt::MiterJoin);
    outerPen.setWidthF(mSliderStyle.borderWidth);
    outerPen.setColor(QColor(0, 0, 0, 255));

    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(outerPen);

    QRectF pickerOuterRect = QRectF(val,
                              innerSliderRect.top(),
                              mPickerSize.width(),
                              innerSliderRect.height());

    painter.drawRoundedRect(pickerOuterRect,
                            mSliderStyle.cachedCornerRadiusX,
                            mSliderStyle.cachedCornerRadiusY,
                            Qt::AbsoluteSize);

    QPen innerPen;
    innerPen.setJoinStyle(Qt::MiterJoin);
    innerPen.setWidthF(mSliderStyle.borderWidth);
    innerPen.setColor(palette().dark().color());
    painter.setPen(innerPen);

    // Draw inner picker
    painter.drawRoundedRect(pickerOuterRect.adjusted(
                                mSliderStyle.borderWidth,
                                mSliderStyle.borderWidth,
                                -mSliderStyle.borderWidth,
                                -mSliderStyle.borderWidth),
                            SliderGeometry::innerCornerRadius(mSliderStyle.cachedCornerRadiusX, mSliderStyle.borderWidth),
                            SliderGeometry::innerCornerRadius(mSliderStyle.cachedCornerRadiusY, mSliderStyle.borderWidth),
                            Qt::AbsoluteSize);
}

void ColorSlider::colorPicked(QPoint point)
{
    qreal borderWidth = mSliderStyle.borderWidth;
    QRectF innerSliderRect = SliderGeometry::contentsRect(contentsRect(), borderWidth)
                        .adjusted(borderWidth,
                                  borderWidth,
                                  -borderWidth,
                                  -borderWidth);
    QColor colorPicked = mColor;
    int colorMax = colorTypeMax();

    qreal pickerCenter = mPickerSize.width() * 0.5;
    int colorVal = (point.x() - pickerCenter) * colorSteps() / SliderGeometry::pickerUpperBound(innerSliderRect.width(), mPickerSize.width());
    colorVal = qBound(mMin, colorVal, colorMax);

    switch (mSpecType)
    {
    case HSV:
        switch (mColorType)
        {
            case HUE:
            {
                colorPicked = QColor::fromHsv(colorVal,
                                              mColor.hsvSaturation(),
                                              mColor.value(),
                                              mColor.alpha());

                break;
            }
            case SAT:
            {
                colorPicked = QColor::fromHsv(mColor.hsvHue(),
                                              colorVal,
                                              mColor.value(),
                                              mColor.alpha());
                break;
            }
            case VAL:
            {
                colorPicked = QColor::fromHsv(mColor.hsvHue(),
                                              mColor.hsvSaturation(),
                                              colorVal,
                                              mColor.alpha());
                break;
            }
            case ALPHA:
            {

                colorPicked = QColor::fromHsv(mColor.hsvHue(),
                                              mColor.hsvSaturation(),
                                              mColor.value(),
                                              colorVal);
                break;
            }
            default:
                Q_UNREACHABLE();
        }
        break;
    case RGB:
        switch(mColorType)
        {
            case RED:
            {
                colorPicked = QColor::fromRgb(colorVal,
                                              mColor.green(),
                                              mColor.blue(),
                                              mColor.alpha());

                break;
            }
            case GREEN:
            {
                colorPicked = QColor::fromRgb(mColor.red(),
                                              colorVal,
                                              mColor.blue(),
                                              mColor.alpha());
                break;
            }
            case BLUE:
            {
                colorPicked = QColor::fromRgb(mColor.red(),
                                              mColor.green(),
                                              colorVal,
                                              mColor.alpha());
                break;
            }
            case ALPHA:
            {

                colorPicked = QColor::fromRgb(mColor.red(),
                                              mColor.green(),
                                              mColor.blue(),
                                              colorVal);
                break;
            }
            default:
                Q_UNREACHABLE();
        }
        break;
    default:
        Q_UNREACHABLE();
    }

    mColor = colorPicked;
    mPixmapCacheInvalid = true;

    emit valueChanged(mColor);
}
