/*

Pencil2D - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2012-2020 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/

#ifndef PAINTERUTILS_H
#define PAINTERUTILS_H

#include <QtMath>

/** Calculate layer opacity based on current layer offset */
inline qreal calculateRelativeOpacityForLayer(int currentLayerIndex, int layerIndexNext, float threshold)
{
    int layerOffset = currentLayerIndex - layerIndexNext;
    int absoluteOffset = qAbs(layerOffset);
    qreal newOpacity = 1.0;
    if (absoluteOffset != 0)
    {
        newOpacity = qPow(static_cast<qreal>(threshold), absoluteOffset);
    }
    return newOpacity;
};

#endif // PAINTERUTILS_H
