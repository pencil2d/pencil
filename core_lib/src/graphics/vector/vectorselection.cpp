/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2012-2018 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/

#include "vectorselection.h"

VectorSelection::VectorSelection()
{
}

void VectorSelection::clear()
{
    vertices.clear();
    curves.clear();
}

void VectorSelection::add(int curveNumber)
{
    curves.append(curveNumber);
}

void VectorSelection::add(QList<int> list)
{
    if (list.size() > 0)

    for (int num : list) {
        add(num);
    }
}

void VectorSelection::add(VertexRef point)
{
    vertices << point;
    add(point.curveNumber);
}

void VectorSelection::add(QList<VertexRef> list)
{
    if (list.size() > 0)
    for (VertexRef ref : list) {
        add(ref);
    }
}
