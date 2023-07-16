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

#ifndef TILE_H
#define TILE_H

#include <QPixmap>

class Tile
{
public:

    explicit Tile (QSize size);
    explicit Tile (QPixmap& pixmap);
    ~Tile();

    const QPixmap& pixmap() const { return mTilePixmap; }
    QPixmap& pixmap() { return mTilePixmap; }

    QRect boundingRect() const;

    void clear();
    void replaceTile(const QPixmap& pixmap);

    bool isDirty() { return mDirty; }
    void setDirty(bool dirty) { mDirty = dirty; }
    void setPos(const QPoint& pos) { m_pos = pos; }
    QPoint pos() const { return m_pos; }

private:
    QPixmap    mTilePixmap;
    bool      mCacheValid;
    bool mDirty = false;
    QPoint m_pos;
};

#endif // TILE_H
