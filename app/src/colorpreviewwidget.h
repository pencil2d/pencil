/*

Pencil2D - Traditional Animation Software
Copyright (C) 2026 Oliver Stevns Larsen

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/
#ifndef COLORPREVIEWWIDGET_H
#define COLORPREVIEWWIDGET_H

#include <QObject>
#include <QWidget>

class ColorPreviewWidget: public QWidget
{
    Q_OBJECT
public:
    explicit ColorPreviewWidget(QWidget* parent = nullptr);

    void setColor(const QColor& color);

protected:
     void paintEvent(QPaintEvent* event) override;

private:
     QPixmap mCheckerboard = QPixmap(":/background/checkerboard.png");

     QColor mColor;
};

#endif // COLORPREVIEWWIDGET_H
