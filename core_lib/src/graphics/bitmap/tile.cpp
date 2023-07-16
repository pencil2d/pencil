/*

Pencil2D - Traditional Animation Software
Copyright (C) 2012-2020 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/

#include "tile.h"

#include <QDebug>

Tile::Tile(QSize size):
    mTilePixmap(size)
{
    clear(); //Default tiles are transparent
}

Tile::Tile(QPixmap& pixmap)
{
    mTilePixmap = pixmap;
}

Tile::~Tile()
{
}

QRect Tile::boundingRect() const
{
    return mTilePixmap.rect();
}

void Tile::replaceTile(const QPixmap& pixmap)
{
    if (pixmap.isNull()) { return; }

    mTilePixmap = pixmap;
    mCacheValid = true;

}

void Tile::clear()
{
    mTilePixmap.fill(Qt::transparent); // image cache is transparent too, and aligned to the pixel table:
    mCacheValid = true;
    mDirty = false;
}
