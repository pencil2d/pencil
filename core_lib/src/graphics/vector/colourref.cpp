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
#include "colourref.h"

ColourRef::ColourRef()
{
    colour = Qt::green;
    name = QString("Green");
}

ColourRef::ColourRef(QColor theColour, QString theName)
{
    colour = theColour;
    name = theName;
}

bool ColourRef::operator==(ColourRef colourRef1)
{
    if ( (colour == colourRef1.colour) && (name == colourRef1.name) )
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool ColourRef::operator!=(ColourRef colourRef1)
{
    if ( (colour != colourRef1.colour) || (name != colourRef1.name) )
    {
        return true;
    }
    else
    {
        return false;
    }
}
