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

#ifndef COLORWHEEL_H
#define COLORWHEEL_H

#include <QWidget>


class ColorWheel : public QWidget
{
    Q_OBJECT
public:
    explicit ColorWheel(QWidget *parent = 0);

    virtual QSize sizeHint () const;
    virtual QSize minimumSizeHint () const;
    QColor color();

    
signals:
    void colorSelected(const QColor &color);
    void colorChanged(const QColor &color);
    
public slots:
    void setColor(const QColor &color);

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *);
    void resizeEvent(QResizeEvent *event);
    void paintEvent(QPaintEvent *);

private:
    void changeColor(const QColor &color);
    void hueChanged(const int &hue);
    void svChanged(const QColor &newcolor);
    void alphaChanged(const int &alpha);

    QColor pickColor(const QPoint &point);
    
    void drawHueIndicator(const int &hue);
    void drawPicker(const QColor &color);

    void drawWheelImage(const QSize &newSize);
    void drawSquareImage(const int &hue);
    void composeWheel(QPixmap& pixmap);

    QSize m_initSize;
    QImage m_wheelImage;
    QImage m_squareImage;
    QPixmap m_wheelPixmap;
   
    int m_wheelThickness;
    QRegion m_wheelRegion;
    QRegion m_squareRegion;
    QColor m_currentColor;
    bool m_isInWheel;
    bool m_isInSquare;
    
};

#endif // COLORWHEEL_H
