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
#ifndef BEZIERAREA_H
#define BEZIERAREA_H


#include <QtXml>
#include <QPainterPath>

#include "vertexref.h"

class Status;

class BezierArea
{
public:
    BezierArea();
    BezierArea(QList<VertexRef> vertexList, int colour);

    Status createDomElement(QXmlStreamWriter& xmlStream);
    void loadDomElement(QDomElement element);

    VertexRef getVertexRef(int i);
    int getColourNumber() { return mColourNumber; }
    void decreaseColourNumber() { mColourNumber--; }
    void setSelected(bool YesOrNo);
    bool isSelected() const { return mSelected; }
    void setColourNumber(int cn) { mColourNumber = cn; }

    QList<VertexRef> mVertex;
    QPainterPath mPath;
    int mColourNumber = 0;

private:
    bool mSelected = false;
};

#endif
