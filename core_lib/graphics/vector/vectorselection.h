/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2012-2017 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/

#ifndef VECTORSELECTION_H
#define VECTORSELECTION_H


#include <QList>
#include "vertexref.h"

class VectorSelection
{
public:
    VectorSelection();
    QList<VertexRef> vertex;
    QList<int> curve;
    //QList<int> area;
    void clear();
    void add(int curveNumber);
    void add(QList<int> curveNumbers);
    void add(VertexRef point);
    void add(QList<VertexRef> points);
};

#endif // VECTORSELECTION_H
