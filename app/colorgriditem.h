/*

Pencil - Traditional Animation Software
Copyright (C) 2012-2017 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/
#ifndef COLORGRIDITEM_H
#define COLORGRIDITEM_H

#include <QWidget>
#include <QMenu>

class ColorGridItem : public QWidget
{
    Q_OBJECT
public:
    explicit ColorGridItem(int id, QWidget *parent = 0);
    QColor color();
    virtual QSize sizeHint () const;
    virtual QSize minimumSizeHint () const;

signals:
    void colorDroped(int);
    void colorPicked(int, const QColor &);

public slots:
//    void setSource(ColorWheel * wheel);
    void setColor(const QColor& color);
    void onColorPicked();
    void onColorDroped();
    void onColorCleared();
protected:
    void drawTransparent();
    void paintEvent(QPaintEvent *);
    void enterEvent(QEvent* event);
    void leaveEvent(QEvent* event);
    void mousePressEvent(QMouseEvent *event);
    void initMenu();

private:
    int mGridId;
    QMenu menu;
    QColor mColor;
    QPixmap mBackgroundImg;
    bool mHovered;
    QCursor picker;
};

#endif // COLORGRIDITEM_H
