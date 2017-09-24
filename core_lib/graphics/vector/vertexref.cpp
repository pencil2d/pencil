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

#include "vertexref.h"

VertexRef::VertexRef()
{
}

VertexRef::VertexRef(int curveN, int vertexN)
{
    curveNumber = curveN;
    vertexNumber = vertexN;
}

VertexRef VertexRef::nextVertex()
{
    return VertexRef(curveNumber, vertexNumber+1);
}

VertexRef VertexRef::prevVertex()
{
    return VertexRef(curveNumber, vertexNumber-1);
}

bool VertexRef::operator==(VertexRef vertexRef1)
{
    if ( (curveNumber == vertexRef1.curveNumber) && (vertexNumber == vertexRef1.vertexNumber))
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool VertexRef::operator!=(VertexRef vertexRef1)
{
    if ( (curveNumber != vertexRef1.curveNumber) || (vertexNumber != vertexRef1.vertexNumber))
    {
        return true;
    }
    else
    {
        return false;
    }
}


