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

#ifndef COLOURREF_H
#define COLOURREF_H

#include <QColor>
#include <QString>

class ColourRef
{
public:
    ColourRef();
    ColourRef(QColor theColour, QString theName = "");
    bool operator==(ColourRef colourRef1);
    bool operator!=(ColourRef colourRef1);

    QColor colour;
    QString name;
};

QDebug& operator<<(QDebug debug, const ColourRef &colourRef);

#endif
