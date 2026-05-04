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

#include "drawsliderstyle.h"
#include "slidergeometry.h"

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

    void init(ColorSpecType specType, ColorType type, const QColor &color);

    QLinearGradient setColorSpec(const QColor &color);

    QColor color() { return mColor; }

    void setHsv(const QColor& hsv);

    void setRgb(const QColor& rgb);

    void setColorSpecType(ColorSpecType newType) { this->mSpecType = newType; }
    void setColorType(ColorType newType) { this->mColorType = newType; }

    QSize sizeHint() const override;

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent *event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;

signals:
    void valueChanged(QColor color);

private:

    int colorSteps() const;
    int colorTypeMax() const;

    void setupPicker();
    void drawColorBox(const QColor &color, QSize size);
    void drawPicker(const QColor &color);
    QLinearGradient hsvGradient(const QColor &color);
    QLinearGradient rgbGradient(const QColor &color);

    void colorPicked(QPoint point);

    bool mPixmapCacheInvalid = true;
    QPixmap mBoxPixmapSource;

    QColor mColor;
    int mMin = 0;

    QSizeF mPickerSize = QSize(-1, -1);

    ColorType mColorType = ColorType::HUE;
    ColorSpecType mSpecType = ColorSpecType::RGB;

    QLinearGradient mGradient;

    SliderPainterStyle mSliderStyle = SliderPainterStyle(
        QPalette::Dark,
        true,
        QBrush(QPixmap(":icons/general/checkerboard_smaller.png"))
    );

    QPixmap mCheckerboardPixmap = QPixmap(":icons/general/checkerboard_smaller.png");
};

#endif // COLORSLIDER_H
