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
#include "colorpreviewwidget.h"

#include <QPainter>
#include <QPaintEvent>

ColorPreviewWidget::ColorPreviewWidget(QWidget* parent) : QWidget(parent)
{

}

void ColorPreviewWidget::setColor(const QColor& color)
{
    if (color == mColor) { return; }

    mColor = color;
    update();
}

void ColorPreviewWidget::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);

    painter.setPen(Qt::NoPen);
    painter.setBrush(QBrush(mCheckerboard));
    painter.drawRect(event->rect());

    painter.fillRect(event->rect(), mColor);
}
