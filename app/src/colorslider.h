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
#ifndef COLORSLIDER_H
#define COLORSLIDER_H

#include <QWidget>


class ColorSlider : public QWidget
{
    Q_OBJECT
public:

    enum ColorType {
        HUE,
        SAT,
        VAL,
        RED,
        GREEN,
        BLUE,
        ALPHA
    };
    enum ColorSpecType {
        RGB,
        HSV,
        HSL,
        CMYK
    };

    explicit ColorSlider(QWidget* parent);
    ~ColorSlider() override;

    void init(ColorSpecType specType, ColorType type, const QColor &color, qreal min, qreal max);

    QLinearGradient setColorSpec(const QColor &color);

    QColor color() { return mColor; }

    void setHsv(const QColor& hsv) { mColor.setHsv(hsv.hsvHue(),
                                                  hsv.hsvSaturation(),
                                                  hsv.value(),
                                                  hsv.alpha());
                                   }

    void setRgb(const QColor& rgb) { mColor.setRgb(rgb.red(),
                                                  rgb.green(),
                                                  rgb.blue(),
                                                  rgb.alpha());
                                   }

    void setColorSpecType(ColorSpecType newType) { this->mSpecType = newType; }
    void setColorType(ColorType newType) { this->mColorType = newType; }

    void setMin(qreal min) { mMin = min; }
    void setMax(qreal max) { mMax = max; }

    QSize sizeHint() const override;

protected:
    void paintEvent(QPaintEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;

signals:
    void valueChanged(QColor color);

private:

    void drawColorBox(const QColor &color, QSize size);
    void drawPicker(const QColor &color);
    QLinearGradient hsvGradient(const QColor &color);
    QLinearGradient rgbGradient(const QColor &color);

    void colorPicked(QPoint point);

    QPixmap mBoxPixmapSource;

    QColor mColor;
    qreal mMin = 0.0;
    qreal mMax = 0.0;

    ColorType mColorType = ColorType::HUE;
    ColorSpecType mSpecType = ColorSpecType::RGB;

    QLinearGradient mGradient;
};

#endif // COLORSLIDER_H
