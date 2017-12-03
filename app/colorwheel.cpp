/*

Pencil - Traditional Animation Software
Copyright (C) 2013-2017 Matt Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/

#include <cmath>
#include <QPainter>
#include <QResizeEvent>
#include <QStyleOption>
#include <QtCore/qmath.h>
#include <QRect>
#include <QDebug>

#include "colorwheel.h"

ColorWheel::ColorWheel(QWidget *parent) : QWidget(parent),
    m_initSize(200, 200),
    m_wheelThickness(20),
    m_currentColor(Qt::red),
    m_isInWheel(false),
    m_isInSquare(false)

{
    m_currentColor = m_currentColor.toHsv();
}

QColor ColorWheel::color()
{
    return m_currentColor;
}

void ColorWheel::changeColor(const QColor &color)
{
    if (color.toHsv() == m_currentColor)
    {
        return;
    }

    if (color.spec() == QColor::Spec::Rgb) {
        changeRgbColors(color);
    } else {
        changeHsvColors(color);
    }

    if (color.alpha() != m_currentColor.alpha())
    {
        alphaChanged(color.alpha());
    }

//    emit colorChanged(color);
    update();
}

void ColorWheel::setColor(const QColor &color)
{
    if (color.toHsv() == m_currentColor)
    {
        return;
    }

    if (color.spec() == QColor::Spec::Rgb) {
        drawSquareImage(color.hue());
        changeRgbColors(color);
    } else {
        changeHsvColors(color);
    }

    if ( color.alpha() != m_currentColor.alpha() )
    {
        alphaChanged(color.alpha());
    }

    update();
    emit colorSelected(color);
}

void ColorWheel::changeRgbColors(const QColor &color) {
    if (color.red() != m_currentColor.red())
    {
        redChanged(color.red());
    }

    if (color.green() != m_currentColor.green())
    {
        greenChanged(color.green());
    }

    if (color.blue() != m_currentColor.blue())
    {
        blueChanged(color.blue());
    }
}

void ColorWheel::changeHsvColors(const QColor &color) {
    if (color.hue() != m_currentColor.hue())
    {
        hueChanged(color.hue());
    }

    if (color.saturation() != m_currentColor.saturation())
    {
        saturationChanged(color.saturation());
    }

    if (color.value() != m_currentColor.value())
    {
        valueChanged(color.value());
    }
}

QColor ColorWheel::pickColor(const QPoint& point)
{
    if ( ! m_wheelPixmap.rect().contains(point) )
    {
        return QColor();
    }
    if (m_isInWheel)
    {
        qreal hue = 0;
        int r = qMin(width(), height()) / 2;
        QString strDebug = "";
        strDebug += QString("Radius=%1").arg(r);

        QPoint center(width() / 2, height() / 2);

        QPoint diff = point - center;
        strDebug += QString(" Atan2=%1").arg(qAtan2(diff.y(), diff.x()));

        hue = qAtan2( -diff.y(), diff.x() ) / M_PI * 180;

        hue = fmod((hue + 360), 360); // shift -180~180 to 0~360

        strDebug += QString(" Hue=%1").arg(hue);
        //qDebug() << strDebug;

        hue = (hue > 359) ? 359 : hue;
        hue = (hue < 0) ? 0 : hue;

        return QColor::fromHsv(hue,
            m_currentColor.saturation(),
            m_currentColor.value());
    }
    else if (m_isInSquare)
    {
        QRect rect = m_squareRegion.boundingRect();
        QPoint p = point - rect.topLeft();
        //qDebug("TopRight(%d, %d) Point(%d, %d)", rect.topRight().x(), rect.topRight().y(), point.x(), point.y());
        QSizeF regionSize = rect.size() - QSizeF(1, 1);

        //qDebug("p(%d, %d), Region(%.1f, %.1f)", p.x(), p.y(), regionSize.width(), regionSize.height());
        return QColor::fromHsvF( m_currentColor.hueF(),
            p.x() / regionSize.width(),
            1.0 - (p.y() / regionSize.height()));
    }
    return QColor();
}

QSize ColorWheel::sizeHint () const
{
    return m_initSize;
}

QSize ColorWheel::minimumSizeHint () const
{
    return m_initSize;
}

void ColorWheel::mousePressEvent(QMouseEvent *event)
{
    QPoint lastPos = event->pos();
    if (m_squareRegion.contains(lastPos))
    {
        m_isInWheel = false;
        m_isInSquare = true;
        QColor color = pickColor(lastPos);
        saturationChanged(color.saturation());
        valueChanged(color.value());

    }
    else if (m_wheelRegion.contains(lastPos))
    {
        m_isInWheel = true;
        m_isInSquare = false;
        QColor color = pickColor(lastPos);
        hueChanged(color.hue());
    }
}

void ColorWheel::mouseMoveEvent(QMouseEvent* event)
{
    QPoint lastPos = event->pos();
    if ( event->buttons() == Qt::NoButton )
    {
        return;
    }
    if (m_isInSquare)
    {
        QRect rect = m_squareRegion.boundingRect();

        if ( lastPos.x() < rect.topLeft().x() )
        {
            lastPos.setX( rect.topLeft().x() );
        }
        else if ( lastPos.x() > rect.bottomRight().x() )
        {
            lastPos.setX( rect.bottomRight().x() );
        }

        if ( lastPos.y() < rect.topLeft().y() )
        {
            lastPos.setY( rect.topLeft().y() );
        }
        else if ( lastPos.y() > rect.bottomRight().y() )
        {
            lastPos.setY( rect.bottomRight().y() );
        }

        QColor color = pickColor(lastPos);
        saturationChanged(color.saturation());
        valueChanged(color.value());
    }
    else if (m_wheelRegion.contains(lastPos) && m_isInWheel)
    {
        QColor color = pickColor(lastPos);
        hueChanged(color.hue());
    }
}

void ColorWheel::mouseReleaseEvent(QMouseEvent *)
{
    m_isInWheel = false;
    m_isInSquare = false;
    emit colorSelected(m_currentColor);
}

void ColorWheel::resizeEvent(QResizeEvent *event)
{
    m_wheelPixmap = QPixmap(event->size());
    m_wheelPixmap.fill(palette().background().color());
    drawWheelImage(event->size());
    drawSquareImage(m_currentColor.hue());
    update();
}

void ColorWheel::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    QStyleOption opt;
    opt.initFrom(this);
    composeWheel(m_wheelPixmap);
    painter.translate(width() / 2, height() / 2);
    painter.translate(-m_wheelPixmap.width() / 2,-m_wheelPixmap.height() / 2);
    painter.drawPixmap(0, 0, m_wheelPixmap);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);
}

void ColorWheel::drawWheelImage(const QSize &newSize)
{
    int r = qMin(newSize.width(), newSize.height());

    QStyleOption option;
    option.initFrom(this);

    QBrush backgroundBrush = option.palette.window();

    m_wheelImage = QImage(newSize, QImage::Format_ARGB32_Premultiplied);

    //m_wheelImage.fill(background.color());  // Only in 4.8

    QPainter painter(&m_wheelImage);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.fillRect(m_wheelImage.rect(), backgroundBrush);

    QConicalGradient conicalGradient(0, 0, 0);

    for (int hue = 0; hue < 360; hue +=1)
    {
        conicalGradient.setColorAt( hue / 360.0,  QColor::fromHsv(hue, 255, 255));
    }

    qreal ir = r - m_wheelThickness;

    /* outer circle */
    painter.translate(width() / 2, height() / 2);

    QBrush brush(conicalGradient);
    painter.setPen(Qt::NoPen);
    painter.setBrush(brush);
    painter.drawEllipse(QPoint(0, 0), r/2, r/2);

    /* inner circle */
    painter.setBrush(backgroundBrush);
    painter.drawEllipse(QPoint(0, 0), r/2 - m_wheelThickness, r/2 - m_wheelThickness);

    // Center of wheel
    qreal m1 = (m_wheelPixmap.width() / 2) - (ir / qSqrt(2));
    qreal m2 = (m_wheelPixmap.height() / 2) - (ir / qSqrt(2));

    // Calculate size of wheel width
    qreal wheelWidth = 2 * ir / qSqrt(2);

    // Caculate wheel region
    m_wheelRegion = QRegion(m1, m2, wheelWidth, wheelWidth);
}

void ColorWheel::drawSquareImage(const int &hue)
{
    // region of the widget
    int w = qMin(width(), height());
    // radius of outer circle
    qreal r = w / 2.0;
    // radius of inner circle
    qreal ir = r - m_wheelThickness;

    // center of square
    qreal m1 = (width() / 2) - (ir / qSqrt(2));
    qreal m2 = (height() / 2) - (ir / qSqrt(2));

    QImage square(255, 255, QImage::Format_ARGB32_Premultiplied);
    QColor color;

    for (int i = 0; i < 255; ++i)
    {
        for (int j = 0; j < 255; ++j)
        {
            color = QColor::fromHsv(hue, i, 255 - j);
            QRgb rgb = qRgb(color.red(), color.green(), color.blue());
            square.setPixel(i, j, rgb);
        }
    }

    qreal SquareWidth =  2 * ir / qSqrt(2.1);
    m_squareImage = square.scaled(SquareWidth, SquareWidth);
    m_squareRegion = QRegion(m1, m2, SquareWidth, SquareWidth);
}

void ColorWheel::drawHueIndicator(const int &hue)
{
    QPainter painter(&m_wheelPixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    if(hue > 20 && hue < 200)
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
    painter.rotate( -hue );

    r = r / 2.0 - m_wheelThickness/2;
    painter.drawEllipse(QPointF(r, 0), 7, 7);
}

void ColorWheel::drawPicker(const QColor &color)
{
    QPainter painter(&m_wheelPixmap);
    painter.setRenderHint(QPainter::Antialiasing);

    QPoint squareTopLeft = m_squareRegion.boundingRect().topLeft();

    painter.translate(squareTopLeft.x(), squareTopLeft.y());

    QSize squareSize = m_squareRegion.boundingRect().size();

    qreal S = color.saturationF() * squareSize.width();
    qreal V = squareSize.height() - (color.valueF() * squareSize.height());

    QPen pen;
    pen.setWidth(3);
    if (color.saturation() > 30 || color.value() < 50)
    {
        pen.setColor(Qt::white);
    }
    painter.setPen(pen);

    painter.drawEllipse(S - 2, V - 2, 10, 10);
}

void ColorWheel::composeWheel( QPixmap& pixmap )
{
    QPainter composePainter(&pixmap);
    composePainter.drawImage(0, 0, m_wheelImage);
    composePainter.translate(width() / 2, height() / 2); //Move to center of widget
    composePainter.translate(-m_squareImage.width() / 2, -m_squareImage.height() / 2); //move to center of image
    composePainter.drawImage(0, 0, m_squareImage);
    composePainter.end();
    drawHueIndicator(m_currentColor.hue());
    drawPicker(m_currentColor);
}

void ColorWheel::redChanged(const int &red)
{
    int g = m_currentColor.green();
    int b = m_currentColor.blue();
    int a = m_currentColor.alpha();

    m_currentColor.setRgb(red, g, b, a);

    if(!isVisible())
    {
        return;
    }

    update();
    emit colorChanged(m_currentColor);
}

void ColorWheel::greenChanged(const int &green)
{
    int r = m_currentColor.red();
    int b = m_currentColor.blue();
    int a = m_currentColor.alpha();

    m_currentColor.setRgb(r, green, b, a);

    if(!isVisible())
    {
        return;
    }

    update();
    emit colorChanged(m_currentColor);
}

void ColorWheel::blueChanged(const int &blue)
{
    int r = m_currentColor.red();
    int g = m_currentColor.green();
    int a = m_currentColor.alpha();

    m_currentColor.setRgb(r, g, blue, a);

    if(!isVisible())
    {
        return;
    }

    update();
    emit colorChanged(m_currentColor);
}


void ColorWheel::hueChanged(const int &hue)
{
    if ( hue < 0 || hue > 359)
    {
        return;
    }
    int s = m_currentColor.saturation();
    int v = m_currentColor.value();
    int a = m_currentColor.alpha();

    m_currentColor.setHsv(hue, s, v, a);

    if(!isVisible())
    {
        return;
    }

    drawSquareImage(hue);

    update();
    emit colorChanged(m_currentColor);
}

void ColorWheel::saturationChanged(const int &sat)
{
    int hue = m_currentColor.hue();
    int value = m_currentColor.value();
    int alpha = m_currentColor.alpha();

    m_currentColor.setHsv(hue, sat, value, alpha);

    update();
    emit colorChanged(m_currentColor);
}

void ColorWheel::valueChanged(const int &value)
{
    int hue = m_currentColor.hue();
    int sat = m_currentColor.saturation();
    int alpha = m_currentColor.alpha();
    m_currentColor.setHsv(hue, sat, value, alpha);

    update();
    emit colorChanged(m_currentColor);
}

void ColorWheel::alphaChanged(const int &alpha)
{
    m_currentColor.setAlpha(alpha);

    if(!isVisible())
    {
        return;
    }

    emit colorChanged(m_currentColor);
}
