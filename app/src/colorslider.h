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

    void init(ColorType type, QColor color, qreal min, qreal max);
    void init(ColorType type, QColor color, qreal min, qreal max, QSize size);

    QLinearGradient setColorSpec(QColor color);

    QColor color() { return mColor; }

    void setHsv(QColor hsv) { mColor.setHsv(hsv.hsvHue(),
                                           hsv.hsvSaturation(),
                                           hsv.value(),
                                           hsv.alpha());
                            }

    void setRgb(QColor rgb) { mColor.setRgb(rgb.red(),
                                           rgb.green(),
                                           rgb.blue(),
                                           rgb.alpha());
                            }

    void setColorSpecType(ColorSpecType newType) { this->mSpecType = newType; }
    void setColorType(ColorType newType) { this->mColorType = newType; }

    void setMin(qreal min) { mMin = min; }
    void setMax(qreal max) { mMax = max; }

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;

//public slots:

signals:
    void valueChanged(QColor color);
//    void valueChanged(QColor color);

private:

    void drawColorBox(QColor color, QSize size);
    void drawPicker(QColor color);
    QLinearGradient hsvGradient(QColor color);
    QLinearGradient rgbGradient(QColor color);

    void colorPicked(QPoint point);

    QPixmap mBoxPixmapTarget;
    QPixmap mBoxPixmapSource;

    QColor mColor;
    qreal mMin = 0.0;
    qreal mMax = 0.0;

    ColorType mColorType;
    ColorSpecType mSpecType;

    QSize mSize = QSize(0,0);

    QLinearGradient mGradient;
};

#endif // COLORSLIDER_H
