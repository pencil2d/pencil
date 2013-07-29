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


#include "bezierarea.h"

BezierArea::BezierArea()
{
    selected = false;
    // nothing;
}

BezierArea::BezierArea(QList<VertexRef> vertexList, int colour)
{
    vertex = vertexList;
    colourNumber = colour;
    selected = false;
}

VertexRef BezierArea::getVertexRef(int i)
{
    while (i >= vertex.size() )
    {
        i = i - vertex.size();
    }
    while (i < 0 )
    {
        i = i + vertex.size();
    }
    return vertex[i];
}

void BezierArea::setSelected(bool YesOrNo)
{
    selected = YesOrNo;
}

QDomElement BezierArea::createDomElement(QDomDocument& doc)
{
    QDomElement areaTag = doc.createElement("area");
    areaTag.setAttribute("colourNumber", colourNumber);

    for(int i=0; i < vertex.size() ; i++)
    {
        QDomElement vertexTag = doc.createElement("vertex");
        vertexTag.setAttribute("curve", vertex.at(i).curveNumber);
        vertexTag.setAttribute("vertex", vertex.at(i).vertexNumber);
        areaTag.appendChild(vertexTag);
    }
    return areaTag;
}

void BezierArea::loadDomElement(QDomElement element)
{
    colourNumber = element.attribute("colourNumber").toInt();

    QDomNode vertexTag = element.firstChild();
    while (!vertexTag.isNull())
    {
        QDomElement vertexElement = vertexTag.toElement();
        if (!vertexElement.isNull())
        {
            if (vertexElement.tagName() == "vertex")
            {
                vertex.append( VertexRef(vertexElement.attribute("curve").toInt() , vertexElement.attribute("vertex").toInt() )  );
            }
        }
        vertexTag = vertexTag.nextSibling();
    }
}
