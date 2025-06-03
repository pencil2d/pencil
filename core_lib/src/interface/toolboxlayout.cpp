/*

Pencil2D - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2012-2020 Matthew Chiawen Chang
Copyright (C) 2024-2099 Oliver S. Larsen

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/
#include "toolboxlayout.h"

ToolBoxLayout::ToolBoxLayout(QWidget* parent, int margin, int hSpacing, int vSpacing)
    : FlowLayout(parent, margin, hSpacing, vSpacing)
{

}

void ToolBoxLayout::lastLineAlignment(int startIndex, int count, RowLayoutInfo rowInfo, const QRect &effectiveRect) const
{
    alignRowFromRowInfo(startIndex, count, rowInfo);
}

void ToolBoxLayout::alignRowFromRowInfo(int startIndex, int count, RowLayoutInfo rowInfo) const
{
    int x = rowInfo.startX;

    for (int i = startIndex; i < startIndex + count; i += 1) {

        if (i > itemList.length() - 1) {
            break;
        }
        QLayoutItem *item = itemList.at(i);
        QSize size = item->geometry().size();
        item->setGeometry(QRect(QPoint(x, item->geometry().y()), size));
        x += size.width() + rowInfo.spacing;
    }
}
