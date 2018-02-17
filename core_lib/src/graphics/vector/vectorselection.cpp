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
    vertex.clear();
    curve.clear();
}

void VectorSelection::add(int curveNumber)
{
    curve << curveNumber;
}

void VectorSelection::add(QList<int> list)
{
    if (list.size() > 0) add(list[0]);
}

void VectorSelection::add(VertexRef point)
{
    vertex << point;
    add(point.curveNumber);
}

void VectorSelection::add(QList<VertexRef> list)
{
    if (list.size() > 0) add(list[0]);
}
