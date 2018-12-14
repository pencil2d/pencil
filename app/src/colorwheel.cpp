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
        QString strDebug = "";
        strDebug += QString("Radius=%1").arg(r);

        QPoint center(width() / 2, height() / 2);

        QPoint diff = point - center;
        strDebug += QString(" Atan2=%1").arg(qAtan2(diff.y(), diff.x()));

        hue = qAtan2(-diff.y(), diff.x()) / M_PI * 180;

        hue = fmod((hue + 360), 360); // shift -180~180 to 0~360

        strDebug += QString(" Hue=%1").arg(hue);
        //qDebug() << strDebug;

        hue = (hue > 359) ? 359 : hue;
        hue = (hue < 0) ? 0 : hue;

        return QColor::fromHsv(static_cast<int>(hue),
                               mCurrentColor.saturation(),
                               mCurrentColor.value());
    }
    else if (mIsInSquare)
    {
        QRect rect = mSquareRegion.boundingRect();
        QPoint p = point - rect.topLeft();
        //qDebug("TopRight(%d, %d) Point(%d, %d)", rect.topRight().x(), rect.topRight().y(), point.x(), point.y());
        QSizeF regionSize = rect.size() - QSizeF(1, 1);

        //qDebug("p(%d, %d), Region(%.1f, %.1f)", p.x(), p.y(), regionSize.width(), regionSize.height());
        return QColor::fromHsvF(mCurrentColor.hueF(),
                                p.x() / regionSize.width(),
                                1.0 - (p.y() / regionSize.height()));
    }
    return QColor();
}

void ColorWheel::mousePressEvent(QMouseEvent *event)
{
    QPoint lastPos = event->pos();
    if (mSquareRegion.contains(lastPos))
    {
        mIsInWheel = false;
        mIsInSquare = true;
        QColor color = pickColor(lastPos);
        saturationChanged(color.saturation());
        valueChanged(color.value());

    }
    else if (mWheelRegion.contains(lastPos))
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
        QRect rect = mSquareRegion.boundingRect();

        if (lastPos.x() < rect.topLeft().x())
        {
            lastPos.setX(rect.topLeft().x());
        }
        else if (lastPos.x() > rect.bottomRight().x())
        {
            lastPos.setX(rect.bottomRight().x());
        }

        if (lastPos.y() < rect.topLeft().y())
        {
            lastPos.setY(rect.topLeft().y());
        }
        else if (lastPos.y() > rect.bottomRight().y())
        {
            lastPos.setY(rect.bottomRight().y());
        }

        QColor color = pickColor(lastPos);
        saturationChanged(color.saturation());
        valueChanged(color.value());
    }
    else if (mWheelRegion.contains(lastPos) && mIsInWheel)
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
    mWheelRegion = QRegion(static_cast<int>(m1), static_cast<int>(m2),
                           static_cast<int>(wheelWidth), static_cast<int>(wheelWidth));
}

void ColorWheel::drawSquareImage(const int &hue)
{
    // region of the widget
    int w = qMin(width(), height());
    // radius of outer circle
    qreal r = w / 2.0;
    // radius of inner circle
    qreal ir = r - mWheelThickness;

    // center of square
    qreal m1 = (width() / 2) - (ir / qSqrt(2));
    qreal m2 = (height() / 2) - (ir / qSqrt(2));

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
    mSquareImage = square.scaled(static_cast<int>(SquareWidth), static_cast<int>(SquareWidth));
    mSquareRegion = QRegion(static_cast<int>(m1), static_cast<int>(m2),
                            static_cast<int>(SquareWidth), static_cast<int>(SquareWidth));
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
    int ellipseSize = 10;

    QPoint squareTopLeft = mSquareRegion.boundingRect().topLeft()-QPoint(1,1);

    QSize squareSize = mSquareRegion.boundingRect().size() * 1.01;

    qreal S = color.hsvSaturationF() * (squareSize.width());
    qreal V = (squareSize.height() - (color.valueF() * squareSize.height()));

    QPen pen;
    pen.setWidth(1);
    if (color.hsvSaturation() > 30 || color.value() < 50)
    {
        pen.setColor(Qt::white);
    }
    painter.setPen(pen);

    QTransform transform;
    transform.translate(-ellipseSize/2,-ellipseSize/2);
    transform.translate(squareTopLeft.x()+2,squareTopLeft.y()+2);
    painter.setTransform(transform);
    painter.drawEllipse(static_cast<int>(S), static_cast<int>(V), ellipseSize, ellipseSize);
}

void ColorWheel::composeWheel(QPixmap& pixmap)
{
    QPainter composePainter(&pixmap);
    composePainter.drawImage(0, 0, mWheelImage);
    composePainter.translate(width() / 2, height() / 2); //Move to center of widget
    composePainter.translate(-mSquareImage.width() / 2, -mSquareImage.height() / 2); //move to center of image
    composePainter.drawImage(0, 0, mSquareImage);
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
