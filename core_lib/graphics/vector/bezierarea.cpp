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


#include "bezierarea.h"

#include "pencilerror.h"

BezierArea::BezierArea()
{
}

BezierArea::BezierArea(QList<VertexRef> vertexList, int colour)
{
    mVertex = vertexList;
    mColourNumber = colour;
    mSelected = false;
}

VertexRef BezierArea::getVertexRef(int i)
{
    while (i >= mVertex.size() )
    {
        i = i - mVertex.size();
    }
    while (i < 0 )
    {
        i = i + mVertex.size();
    }
    return mVertex[i];
}

void BezierArea::setSelected(bool YesOrNo)
{
    mSelected = YesOrNo;
}

Status BezierArea::createDomElement( QXmlStreamWriter& xmlStream )
{
    xmlStream.writeStartElement( "area" );
    xmlStream.writeAttribute( "colourNumber", QString::number( mColourNumber ) );
    xmlStream.writeAttribute("filled", QString::number( mIsFilled ) );

    int errorLocation = -1;
    for ( int i = 0; i < mVertex.size(); i++ )
    {
        xmlStream.writeEmptyElement( "vertex" );
        xmlStream.writeAttribute( "curve", QString::number( mVertex.at( i ).curveNumber ) );
        xmlStream.writeAttribute( "vertex", QString::number( mVertex.at( i ).vertexNumber ) );

        if ( errorLocation < 0 && xmlStream.hasError() )
        {
            errorLocation = i;
        }
    }

    xmlStream.writeEndElement(); // Close area element

    if ( xmlStream.hasError() && errorLocation >= 0 )
    {
        QStringList debugInfo = QStringList() << "BezierArea::createDomElement"
                                              << QString( "colourNumber = %1" ).arg( mColourNumber )
                                              << QString( "- mVertex[%1] has failed to write" ).arg( errorLocation )
                                              << QString( "&nbsp;&nbsp;curve = %1" ).arg( mVertex.at( errorLocation ).curveNumber )
                                              << QString( "&nbsp;&nbsp;vertex = %1 " ).arg( mVertex.at( errorLocation ).vertexNumber );

        return Status( Status::FAIL, debugInfo );
    }

    return Status::OK;
}

void BezierArea::loadDomElement(QDomElement element)
{
    mColourNumber = element.attribute("colourNumber").toInt();

    QDomNode vertexTag = element.firstChild();
    while (!vertexTag.isNull())
    {
        QDomElement vertexElement = vertexTag.toElement();
        if (!vertexElement.isNull())
        {
            if (vertexElement.tagName() == "vertex")
            {
                mVertex.append( VertexRef(vertexElement.attribute("curve").toInt() , vertexElement.attribute("vertex").toInt() )  );
            }
        }
        vertexTag = vertexTag.nextSibling();
    }
}
