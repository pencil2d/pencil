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

#include <cmath>
#include <QtMath>
#include <QDebug>

#include "util/colordictionary.h"

ColourRef::ColourRef()
{
    colour = Qt::green;
    name = QString("Green");
}

ColourRef::ColourRef(QColor theColour, QString theName)
{
    colour = theColour;
    name = theName.isNull() ? ColourRef::getDefaultColorName(theColour) : theName;
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

QDebug& operator<<(QDebug debug, const ColourRef& colourRef)
{
    debug.nospace() << "ColourRef(" << colourRef.colour << " " << colourRef.name <<")";
    return debug.maybeSpace();
}

QString ColourRef::getDefaultColorName(const QColor c)
{
    // Separate rgb values for convenience
    const int r = c.red();
    const int g = c.green();
    const int b = c.blue();

    // Convert RGB to XYZ with D65 white point
    // (algorithm source: https://www.cs.rit.edu/%7Encs/color/t_convert.html#RGB%20to%20XYZ%20&%20XYZ%20to%20RGB)
    const qreal x = 0.412453*r + 0.357580*g + 0.180423*b;
    const qreal y = 0.212671*r + 0.715160*g + 0.072169*b;
    const qreal z = 0.019334*r + 0.119193*g + 0.950227*b;

    // Convert XYZ to CEI L*u*v
    // (algorithm source: https://www.cs.rit.edu/~ncs/color/t_convert.html#XYZ%20to%20CIE%20L*a*b*%20(CIELAB)%20&%20CIELAB%20to%20XYZ)
    // Helper function for the conversion
    auto f = [](const double a) { return a > 0.008856 ? std::cbrt(a) : 7.787 * a + 16 / 116; };
    // XYZ tristimulus values for D65 (taken from: https://en.wikipedia.org/wiki/Illuminant_D65#Definition)
    const qreal xn = 95.047,
        yn = 100,
        zn = 108.883;
    const qreal l = y / yn > 0.008856 ? 116 * cbrt(y / yn) - 16 : 903.3*y / yn,
        u = 500 * (f(x / xn) - f(y / yn)),
        v = 200 * (f(y / yn) - f(z / zn));

    // Find closest color match in colorDict to the luv values
    int minLoc = 0;
    if (u < 0.01 && u > -0.01 && v < 0.01 && v > -0.01)
    {
        // The color is grayscale so only compare to gray centroids so there is no 'false hue'
        qreal minDist = qPow(colorDict[dictSize - 5][0] - l, 2) + qPow(colorDict[dictSize - 5][1] - u, 2) + qPow(colorDict[dictSize - 5][2] - v, 2);
        for (int i = dictSize - 4; i < dictSize; i++)
        {
            qreal curDist = qPow(colorDict[i][0] - l, 2) + qPow(colorDict[i][1] - u, 2) + qPow(colorDict[i][2] - v, 2);
            if (curDist < minDist)
            {
                minDist = curDist;
                minLoc = i;
            }
        }
    }
    else
    {
        qreal minDist = qPow(colorDict[0][0] - l, 2) + qPow(colorDict[0][1] - u, 2) + qPow(colorDict[0][2] - v, 2);
        for (int i = 1; i < dictSize; i++)
        {
            qreal curDist = qPow(colorDict[i][0] - l, 2) + qPow(colorDict[i][1] - u, 2) + qPow(colorDict[i][2] - v, 2);
            if (curDist < minDist)
            {
                minDist = curDist;
                minLoc = i;
            }
        }
    }
    return nameDict[minLoc];
}

