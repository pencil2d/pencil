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
#ifndef BEZIERAREA_H
#define BEZIERAREA_H


#include <QPainterPath>

#include "vertexref.h"

class Status;
class QXmlStreamWriter;
class QDomElement;


class BezierArea
{
public:
    BezierArea();
    BezierArea(QList<VertexRef> vertexList, int color);

    Status createDomElement(QXmlStreamWriter& xmlStream);
    void loadDomElement(const QDomElement& element);

    VertexRef getVertexRef(int i);
    int getColorNumber() { return mColorNumber; }
    void decreaseColorNumber() { mColorNumber--; }
    void setSelected(bool YesOrNo);
    bool isSelected() const { return mSelected; }
    void setColorNumber(int cn) { mColorNumber = cn; }

    QList<VertexRef> mVertex;
    QPainterPath mPath;
    int mColorNumber = 0;

private:
    bool mSelected = false;
    bool mIsFilled = false;
};

#endif
