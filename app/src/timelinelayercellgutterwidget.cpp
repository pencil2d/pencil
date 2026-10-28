/*

Pencil2D - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2008-2009 Mj Mendoza IV
Copyright (C) 2012-2020 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/

#include "timelinelayercellgutterwidget.h"

#include <QPainter>
#include <QApplication>

TimeLineLayerCellGutterWidget::TimeLineLayerCellGutterWidget(int width, QWidget* parent)
    : QWidget(parent)
{

    setGeometry(0, 0, width, 8);
}

void TimeLineLayerCellGutterWidget::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    QPalette palette = QApplication::palette();

    QPen pen = palette.color(QPalette::HighlightedText);
    pen.setWidth(1);
    painter.setPen(pen);
    QColor brushColor = palette.color(QPalette::Highlight);
    brushColor.setAlphaF(0.5);
    painter.setBrush(brushColor);
    painter.drawRect(rect().x()+1, rect().top()+1, rect().width()-1, rect().bottom()-1);
}

void TimeLineLayerCellGutterWidget::updateWidth(int width)
{
    resize(width, height());
}
