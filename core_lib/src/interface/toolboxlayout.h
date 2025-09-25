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
#ifndef TOOLBOXLAYOUT_H
#define TOOLBOXLAYOUT_H

#include "flowlayout.h"

class ToolBoxLayout : public FlowLayout
{
public:
    ToolBoxLayout(QWidget* parent, int margin, int hSpacing, int vSpacing);

protected:
    void lastLineAlignment(int startIndex, int count, RowLayoutInfo rowInfo, const QRect& effectiveRect) const override;

private:
    void alignRowFromRowInfo(int startIndex, int count, RowLayoutInfo rowInfo) const;
};

#endif // TOOLBOXLAYOUT_H
