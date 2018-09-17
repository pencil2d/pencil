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

#ifndef COLORWHEEL_H
#define COLORWHEEL_H

#include <QWidget>


class ColorWheel : public QWidget
{
    Q_OBJECT
public:
    explicit ColorWheel(QWidget* parent);

    QColor color();

signals:
    void colorSelected(const QColor& color);
    void colorChanged(const QColor& color);
    
public slots:
    void setColor(QColor color);

protected:
    void mousePressEvent(QMouseEvent*) override;
    void mouseMoveEvent(QMouseEvent*) override;
    void mouseReleaseEvent(QMouseEvent*) override;
    void resizeEvent(QResizeEvent*) override;
    void paintEvent(QPaintEvent*) override;

private:
    void hueChanged(const int& hue);
    void saturationChanged(const int& sat);
    void valueChanged(const int& value);

    QColor pickColor(const QPoint& point);
    
    void drawHueIndicator(const int& hue);
    void drawPicker(const QColor& color);

    void drawWheelImage(const QSize& newSize);
    void drawSquareImage(const int& hue);
    void composeWheel(QPixmap& pixmap);

private:
    QSize mInitSize{ 20, 20 };
    QImage mWheelImage;
    QImage mSquareImage;
    QPixmap mWheelPixmap;
   
    int mWheelThickness = 20;
    QRegion mWheelRegion;
    QRegion mSquareRegion;
    QColor mCurrentColor = Qt::red;
    bool mIsInWheel = false;
    bool mIsInSquare = false;
    
};

#endif // COLORWHEEL_H
