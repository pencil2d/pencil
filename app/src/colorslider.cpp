#include "colorslider.h"

#include <QWidget>
#include <QDebug>
#include <QResizeEvent>
#include <QStyleOption>
#include <QPixmapCache>
#include <QPainter>
#include <QSlider>


ColorSlider::ColorSlider(QWidget* parent) : QWidget(parent)
{

}

ColorSlider::~ColorSlider()
{

}

void ColorSlider::init(ColorType type, QColor color, qreal min, qreal max)
{
    init(type, color, min, max, QSize(this->size()));
}

void ColorSlider::init(ColorType type, QColor color, qreal min, qreal max, QSize size)
{
    mMin = min;
    mMax = max;
    mColor = color;
    mColorType = type;

    drawColorBox(color, size);
}

void ColorSlider::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    drawColorBox(mColor, mSize);

    painter.drawPixmap(0, 0, mBoxPixmapSource);
    drawPicker(mColor);
    painter.end();

}

void ColorSlider::resizeEvent(QResizeEvent *event)
{
    mSize = event->size();
    drawColorBox(mColor, event->size());
}

QLinearGradient ColorSlider::setColorSpec(QColor color)
{
    if (mSpecType == ColorSpecType::HSV)
    {
       return hsvGradient(color);
    }
    else if (mSpecType == ColorSpecType::RGB)
    {
        return rgbGradient(color);
    }
    Q_ASSERT(false);
    return QLinearGradient();
}

QLinearGradient ColorSlider::rgbGradient(QColor color)
{
    int val = 0;
    if (mColorType == ColorType::RED)
    {

        for (; val < mMax; val += 1)
        {
            mGradient.setColorAt(val / mMax, QColor::fromRgb(val,
                                                    255,
                                                    255,
                                                    color.alpha()));
        }
    }
    else if (mColorType == ColorType::GREEN)
    {

        for (; val < mMax; val += 1)
        {
            mGradient.setColorAt(val / mMax, QColor::fromRgb(color.red(),
                                                    val,
                                                    color.blue(),
                                                    color.alpha()));
        }
    }
    else if (mColorType == ColorType::BLUE)
    {
        for (; val < mMax; val += 1)
        {
            mGradient.setColorAt(val / mMax, QColor::fromRgb(color.red(),
                                                    color.green(),
                                                    val,
                                                    color.alpha()));
        }
    }
    else if (mColorType == ColorType::ALPHA)
    {
        for (; val < mMax; val += 1)
        {
            mGradient.setColorAt(val / mMax, QColor::fromRgb(0,
                                                    0,
                                                    0,
                                                    val));
        }
    }
    return mGradient;
}

QLinearGradient ColorSlider::hsvGradient(QColor color)
{
    int val = 0;
    if (mColorType == ColorType::HUE) {

        for (; val < mMax; val += 1)
        {
            mGradient.setColorAt(val / mMax, QColor::fromHsv(val,
                                                             255,
                                                             255,
                                                             color.alpha()));
        }
    }
    else if (mColorType == ColorType::SAT)
    {

        for (; val < mMax; val += 1)
        {
            mGradient.setColorAt(val / mMax, QColor::fromHsv(color.hsvHue(),
                                                             val,
                                                             color.value(),
                                                             color.alpha()));
        }
    }
    else if (mColorType == ColorType::VAL)
    {
        for (; val < mMax; val += 1)
        {
            mGradient.setColorAt(val / mMax, QColor::fromHsv(color.hsvHue(),
                                                             color.hsvSaturation(),
                                                             val,
                                                             color.alpha()));
        }
    }
    else if (mColorType == ColorType::ALPHA)
    {
        for (; val < mMax; val += 1)
        {
            mGradient.setColorAt(val / mMax, QColor::fromHsv(0,
                                                             0,
                                                             0,
                                                             val));
        }
    }
    return mGradient;
}

void ColorSlider::drawColorBox(QColor color, QSize size)
{
    QStyleOption option;
    option.initFrom(this);

    QBrush backgroundBrush = option.palette.window();

    mBoxPixmapSource = QPixmap(size);

    QPainter painter(&mBoxPixmapSource);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.fillRect(mBoxPixmapSource.rect(), backgroundBrush);

    mGradient = QLinearGradient(0,0,mBoxPixmapSource.width(),0);
    mGradient = setColorSpec(color);

    painter.end();

    // draw checkerboard background
    painter.begin(&mBoxPixmapSource);
    QBrush brush2(QBrush(QPixmap("://icons/new/checkerboard_smaller")));

    painter.setBrush(brush2);
    QPen pen2;
    pen2.setWidthF(0);
    pen2.setColor(Qt::gray);
    pen2.setCosmetic(false);
    painter.setPen(pen2);
    painter.drawRoundedRect(0,
                            0,
                            mBoxPixmapSource.width(),
                            mBoxPixmapSource.height(),
                            4,
                            mBoxPixmapSource.width(),
                            Qt::SizeMode::AbsoluteSize);

    painter.end();

    painter.begin(&mBoxPixmapSource);
    painter.setRenderHint(QPainter::Antialiasing);

    QBrush brush(mGradient);
    QPen pen;
    pen.setWidthF(0);
    pen.setColor(Qt::gray);
    pen.setCosmetic(false);
    painter.setPen(pen);


    painter.setBrush(brush);

    painter.drawRoundedRect(0,
                            0,
                            mBoxPixmapSource.width(),
                            mBoxPixmapSource.height(),
                            4,
                            mBoxPixmapSource.width(),
                            Qt::SizeMode::AbsoluteSize);
    painter.end();
}

void ColorSlider::mouseMoveEvent(QMouseEvent* event)
{
    colorPicked(event->pos());
}

void ColorSlider::mousePressEvent(QMouseEvent *event)
{
    colorPicked(event->pos());

}

void ColorSlider::drawPicker(QColor color)
{
    QPainter painter(this);
    qreal val = 0;
    QSize mPickerSize = QSize(10, this->height()-1);

    QPen pen;
    pen.setWidth(0);
    pen.setColor(QColor(0,0,0,255));

    if (mSpecType == ColorSpecType::HSV) {
        if (mColorType == ColorType::HUE)
        {
            val = color.hsvHueF() * (mBoxPixmapSource.width()-mPickerSize.width());
        }
        else if (mColorType == ColorType::SAT)
        {
            if ( (color.hsvSaturation() > 127 || color.value() < 127) && color.alpha() > 127)
            {
                pen.setColor(Qt::white);
            }
            val = color.hsvSaturationF() * (mBoxPixmapSource.width()-mPickerSize.width());
        }
        else if (mColorType == ColorType::VAL)
        {
            if ( color.value() < 127 && color.alpha() > 127)
            {
                pen.setColor(Qt::white);
            }
            val = color.valueF() * (mBoxPixmapSource.width()-mPickerSize.width());
        }
    } else if (mSpecType == ColorSpecType::RGB) {
        if (mColorType == ColorType::RED)
        {
            val = color.redF() * (mBoxPixmapSource.width()-mPickerSize.width());
        }
        else if (mColorType == ColorType::GREEN)
        {
            if ( color.alpha() > 127)
            {
                pen.setColor(Qt::white);
            }
            val = color.greenF() * (mBoxPixmapSource.width()-mPickerSize.width());
        }
        else if (mColorType == ColorType::BLUE)
        {
            if (color.alpha() > 127)
            {
                pen.setColor(Qt::white);
            }
            val = color.blueF() * (mBoxPixmapSource.width()-mPickerSize.width());
        }
    }
    if (mColorType == ColorType::ALPHA)
    {
        if ( color.alpha() > 127)
        {
            pen.setColor(Qt::white);
        }
        val = color.alphaF() * (mBoxPixmapSource.width()-mPickerSize.width());
    }


    painter.setPen(pen);
    painter.drawRect(static_cast<int>(val), 0, mPickerSize.width(), mPickerSize.height());
    painter.end();
}

void ColorSlider::colorPicked(QPoint point)
{
    QColor colorPicked = mColor;
    int colorMax = static_cast<int>(mMax);
    int colorVal = 0;

    colorVal = point.x()*colorMax/mBoxPixmapSource.width();

    colorVal = (colorVal > colorMax) ? colorMax : colorVal;
    colorVal = (colorVal < 0) ? 0 : colorVal;

    if (mSpecType == ColorSpecType::HSV) {
        switch(mColorType)
        {
            case ColorType::HUE:
            {
                colorPicked = QColor::fromHsv(colorVal,
                                              mColor.hsvSaturation(),
                                              mColor.value(),
                                              mColor.alpha());

                break;
            }
            case ColorType::SAT:
            {
                colorPicked = QColor::fromHsv(mColor.hsvHue(),
                                                  colorVal,
                                                  mColor.value(),
                                                  mColor.alpha());
                break;
            }
            case ColorType::VAL:
            {
                colorPicked = QColor::fromHsv(mColor.hsvHue(),
                                                  mColor.hsvSaturation(),
                                                  colorVal,
                                                  mColor.alpha());
                break;
            }
            case ColorType::ALPHA:
            {

                colorPicked = QColor::fromHsv(mColor.hsvHue(),
                                        mColor.hsvSaturation(),
                                        mColor.value(),
                                        colorVal);
                break;
            }
            default:
                break;
        }
    } else if (mSpecType == ColorSpecType::RGB)
    {
        switch(mColorType)
        {
            case ColorType::RED:
            {
                colorPicked = QColor::fromRgb(colorVal,
                                              mColor.green(),
                                              mColor.blue(),
                                              mColor.alpha());

                break;
            }
            case ColorType::GREEN:
            {
                colorPicked = QColor::fromRgb(mColor.red(),
                                                  colorVal,
                                                  mColor.blue(),
                                                  mColor.alpha());
                break;
            }
            case ColorType::BLUE:
            {
                colorPicked = QColor::fromRgb(mColor.red(),
                                                  mColor.green(),
                                                  colorVal,
                                                  mColor.alpha());
                break;
            }
            case ColorType::ALPHA:
            {

                colorPicked = QColor::fromRgb(mColor.red(),
                                        mColor.green(),
                                        mColor.blue(),
                                        colorVal);
                break;
            }
            default:
                break;
        }
    }
    mColor = colorPicked;
    emit valueChanged(mColor);
}
