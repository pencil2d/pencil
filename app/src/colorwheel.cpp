/*

Pencil - Traditional Animation Software
Copyright (C) 2013-2018 Matt Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/

#include <QVBoxLayout>
#include <QtMath>
#include <QPainter>
#include <QResizeEvent>
#include <QPixmapCache>
#include <QStyleOption>
#include <QStylePainter>
#include <QRect>
#include <QDebug>
#include "pencildef.h"

#include "colorwheel.h"

ColorWheel::ColorWheel(QWidget* parent) : QWidget(parent)
{
    setWindowTitle(tr("Color Wheel", "Color Wheel's window title"));
    mCurrentColor = mCurrentColor.toHsv();
    setMinimumHeight(100);
}

QColor ColorWheel::color()
{
    return mCurrentColor;
}

void ColorWheel::setColor(QColor color)
{
    // this is a UI updating function, never emit any signals
    // and don't call any functions that will emit signals

    color = color.toHsv();

    if (color == mCurrentColor)
    {
        return;
    }

    if (color.hue() == -1) // grayscale color, keep the current hue
    {
        color.setHsv(mCurrentColor.hue(), color.saturation(), color.value(), color.alpha());
    }

    mCurrentColor = color;

    drawSquareImage(color.hue());
    update();
}

QColor ColorWheel::pickColor(const QPoint& point)
{
    if (!mWheelPixmap.rect().contains(point))
    {
        return QColor();
    }
    if (mIsInWheel)
    {
        qreal hue = 0;
        int r = qMin(width(), height()) / 2;
        QPoint center(width() / 2, height() / 2);
        QPoint diff = point - center;

        hue = qAtan2(-diff.y(), diff.x()) / M_PI * 180;
        hue = fmod((hue + 360), 360); // shift -180~180 to 0~360

        //QString strDebug = "";
        //strDebug += QString("Radius=%1").arg(r);
        //strDebug += QString(" Atan2=%1").arg(qAtan2(diff.y(), diff.x()));
        //strDebug += QString(" Hue=%1").arg(hue);
        //qDebug() << strDebug;

        hue = (hue > 359) ? 359 : hue;
        hue = (hue < 0) ? 0 : hue;

        return QColor::fromHsv(static_cast<int>(hue),
                               mCurrentColor.saturation(),
                               mCurrentColor.value());
    }
    else if (mIsInSquare)
    {
        QPointF p = point - mSquareRect.topLeft();
        //qDebug("TopRight(%d, %d) Point(%d, %d)", rect.topRight().x(), rect.topRight().y(), point.x(), point.y());

        //qDebug("p(%d, %d), Region(%.1f, %.1f)", p.x(), p.y(), regionSize.width(), regionSize.height());
        return QColor::fromHsvF(mCurrentColor.hueF(),
                                p.x() / (mSquareRect.width() - 1),
                                1.0 - (p.y() / (mSquareRect.height()-1)));
    }
    return QColor();
}

void ColorWheel::mousePressEvent(QMouseEvent *event)
{
    QPoint lastPos = event->pos();
    if (mSquareRect.contains(lastPos))
    {
        mIsInWheel = false;
        mIsInSquare = true;
        QColor color = pickColor(lastPos);
        saturationChanged(color.saturation());
        valueChanged(color.value());

    }
    else if (mWheelRect.contains(lastPos))
    {
        mIsInWheel = true;
        mIsInSquare = false;
        QColor color = pickColor(lastPos);
        hueChanged(color.hue());
    }
}

void ColorWheel::mouseMoveEvent(QMouseEvent* event)
{
    QPoint lastPos = event->pos();
    if (event->buttons() == Qt::NoButton)
    {
        return;
    }
    if (mIsInSquare)
    {
        if (lastPos.x() < mSquareRect.topLeft().x())
        {
            lastPos.setX(mSquareRect.topLeft().x());
        }
        else if (lastPos.x() > mSquareRect.bottomRight().x())
        {
            lastPos.setX(mSquareRect.bottomRight().x());
        }

        if (lastPos.y() < mSquareRect.topLeft().y())
        {
            lastPos.setY(mSquareRect.topLeft().y());
        }
        else if (lastPos.y() > mSquareRect.bottomRight().y())
        {
            lastPos.setY(mSquareRect.bottomRight().y());
        }

        QColor color = pickColor(lastPos);
        saturationChanged(color.saturation());
        valueChanged(color.value());
    }
    else if (mWheelRect.contains(lastPos) && mIsInWheel)
    {
        QColor color = pickColor(lastPos);
        hueChanged(color.hue());
    }
}

void ColorWheel::mouseReleaseEvent(QMouseEvent *)
{
    mIsInWheel = false;
    mIsInSquare = false;
    emit colorSelected(mCurrentColor);
}

void ColorWheel::resizeEvent(QResizeEvent* event)
{
    mWheelPixmap = QPixmap(event->size());
    mWheelPixmap.fill(palette().background().color());
    drawWheelImage(event->size());
    drawSquareImage(mCurrentColor.hue());

    update();
}

void ColorWheel::paintEvent(QPaintEvent*)
{
    QPainter painter;

    painter.begin(this);
    QStyleOption opt;
    opt.initFrom(this);

    composeWheel(mWheelPixmap);
    painter.translate(width() / 2, height() / 2);
    painter.translate(-mWheelPixmap.width() / 2, -mWheelPixmap.height() / 2);
    painter.drawPixmap(0, 0, mWheelPixmap);

    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);
}

void ColorWheel::drawWheelImage(const QSize &newSize)
{
    int r = qMin(newSize.width(), newSize.height());

    QStyleOption option;
    option.initFrom(this);

    QBrush backgroundBrush = option.palette.window();

    mWheelImage = QImage(newSize, QImage::Format_ARGB32_Premultiplied);

    QPainter painter(&mWheelImage);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.fillRect(mWheelImage.rect(), backgroundBrush);

    QConicalGradient conicalGradient(0, 0, 0);

    for (int hue = 0; hue < 360; hue += 1)
    {
        conicalGradient.setColorAt(hue / 360.0, QColor::fromHsv(hue, 255, 255));
    }

    qreal ir = r - mWheelThickness;

    /* outer circle */
    painter.translate(width() / 2, height() / 2);

    QBrush brush(conicalGradient);
    painter.setPen(Qt::NoPen);
    painter.setBrush(brush);
    painter.drawEllipse(QPoint(0, 0), r / 2, r / 2);

    /* inner circle */
    painter.setBrush(backgroundBrush);
    painter.drawEllipse(QPoint(0, 0), r / 2 - mWheelThickness, r / 2 - mWheelThickness);

    // Center of wheel
    qreal m1 = (mWheelPixmap.width() / 2) - (ir / qSqrt(2));
    qreal m2 = (mWheelPixmap.height() / 2) - (ir / qSqrt(2));

    // Calculate size of wheel width
    qreal wheelWidth = 2 * ir / qSqrt(2);

    // Calculate wheel region
    mWheelRect = QRectF(m1, m2, wheelWidth, wheelWidth).toAlignedRect();
}

void ColorWheel::drawSquareImage(const int &hue)
{
    // region of the widget
    int w = qMin(width(), height());
    // radius of outer circle
    qreal r = w / 2.0;
    // radius of inner circle
    qreal ir = r - mWheelThickness;

    // top left of square
    qreal m1 = (width() / 2) - (ir / qSqrt(2.1));
    qreal m2 = (height() / 2) - (ir / qSqrt(2.1));

    QImage square(255, 255, QImage::Format_ARGB32);

    QLinearGradient colorGradient = QLinearGradient(0, 0, square.width(), 0);
    colorGradient.setColorAt(0, QColor(255,255,255));

    // color square should always use full value and saturation
    colorGradient.setColorAt(1, QColor::fromHsv(hue, 255, 255));

    QLinearGradient blackGradient = QLinearGradient(0, 0, 0, square.height());
    blackGradient.setColorAt(0, QColor(0,0,0,0));
    blackGradient.setColorAt(1, QColor(0,0,0,255));

    QBrush colorGradiantBrush = QBrush(colorGradient);
    QBrush blackGradiantBrush = QBrush(blackGradient);

    QPainter painter(&square);

    painter.fillRect(square.rect(), colorGradiantBrush);
    painter.fillRect(square.rect(), blackGradiantBrush);

    qreal SquareWidth = 2 * ir / qSqrt(2.1);
    mSquareRect = QRectF(m1, m2, SquareWidth, SquareWidth).toAlignedRect();
    mSquareImage = square.scaled(mSquareRect.size());

}

void ColorWheel::drawHueIndicator(const int &hue)
{
    QPainter painter(&mWheelPixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    if (hue > 20 && hue < 200)
    {
        painter.setPen(Qt::black);
    }
    else
    {
        painter.setPen(Qt::white);
    }
    painter.setBrush(Qt::NoBrush);

    QPen pen = painter.pen();
    pen.setWidth(3);
    painter.setPen(pen);
    qreal r = qMin(height(), width());
    painter.translate(width() / 2, height() / 2);
    painter.rotate(-hue);

    r = r / 2.0 - mWheelThickness / 2;
    painter.drawEllipse(QPointF(r, 0), 7, 7);
}

void ColorWheel::drawPicker(const QColor& color)
{
    QPainter painter(&mWheelPixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    int ellipseSize = 9;

    QPoint squareTopLeft = mSquareRect.topLeft();
    QSize squareSize = mSquareRect.size();

    qreal S = color.hsvSaturationF() * (squareSize.width()-1);
    qreal V = (squareSize.height() - (color.valueF() * squareSize.height()-1));

    QPen pen;
    pen.setWidth(1);
    if (color.hsvSaturation() > 30 || color.value() < 50)
    {
        pen.setColor(Qt::white);
    }
    painter.setPen(pen);

    QTransform transform;
    transform.translate(-ellipseSize/2,-ellipseSize/2);
    transform.translate(squareTopLeft.x(),squareTopLeft.y()-1);
    painter.setTransform(transform);
    painter.drawEllipse(static_cast<int>(S), static_cast<int>(V), ellipseSize, ellipseSize);
}

void ColorWheel::composeWheel(QPixmap& pixmap)
{
    QPainter composePainter(&pixmap);
    composePainter.drawImage(0, 0, mWheelImage);
    composePainter.drawImage(mSquareRect, mSquareImage);
    composePainter.end();
    drawHueIndicator(mCurrentColor.hsvHue());
    drawPicker(mCurrentColor);
}

void ColorWheel::hueChanged(const int &hue)
{
    if (hue < 0 || hue > 359)
    {
        return;
    }
    int s = mCurrentColor.hsvSaturation();
    int v = mCurrentColor.value();
    int a = mCurrentColor.alpha();

    mCurrentColor.setHsv(hue, s, v, a);

    if (!isVisible())
    {
        return;
    }

    drawSquareImage(hue);

    update();
    emit colorChanged(mCurrentColor);
}

void ColorWheel::saturationChanged(const int &sat)
{
    int hue = mCurrentColor.hsvHue();
    int value = mCurrentColor.value();
    int alpha = mCurrentColor.alpha();

    mCurrentColor.setHsv(hue, sat, value, alpha);

    update();
    emit colorChanged(mCurrentColor);
}

void ColorWheel::valueChanged(const int &value)
{
    int hue = mCurrentColor.hsvHue();
    int sat = mCurrentColor.hsvSaturation();
    int alpha = mCurrentColor.alpha();
    mCurrentColor.setHsv(hue, sat, value, alpha);

    update();
    emit colorChanged(mCurrentColor);
}
