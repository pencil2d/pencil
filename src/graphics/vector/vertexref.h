/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation;

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/
#ifndef VERTEXREF_H
#define VERTEXREF_H

class VertexRef
{
public:
    VertexRef();
    VertexRef(int curveN, int vertexN);
    VertexRef nextVertex();
    VertexRef prevVertex();
    bool operator==(VertexRef vertexRef1);
    bool operator!=(VertexRef vertexRef1);
    int curveNumber;
    int vertexNumber;
};

#endif

