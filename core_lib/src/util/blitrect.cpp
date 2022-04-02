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

#include "blitrect.h"

BlitRect::BlitRect()
{
}

BlitRect::BlitRect(const QRect rect)
{
    setTopLeft(rect.topLeft());
    setBottomRight(rect.bottomRight());
    mInitialized = true;
}

BlitRect::BlitRect(const QPoint p)
{
    extend(p);
}

void BlitRect::extend(const QPoint p)
{
    if (mInitialized == false)
    {
        setBottomLeft(p);
        setTopRight(p);
        mInitialized = true;
    }
    else
    {
        if (left() > p.x()) { setLeft(p.x()); }
        if (right() < p.x()) { setRight(p.x()); }
        if (top() > p.y()) { setTop(p.y()); }
        if (bottom() < p.y()) { setBottom(p.y()); }
    }
}
