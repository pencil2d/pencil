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

#include <QPoint>
#include <QPixmap>

class Tile
{
public:

    explicit Tile (const QPoint& pos, QSize size);
    ~Tile();

    const QPixmap& pixmap() const { return mTilePixmap; }
    QPixmap& pixmap() { return mTilePixmap; }

    const QPoint& pos() const { return mPos; }
    const QPointF& posF() const { return mPosF; }
    const QRect& bounds() const { return mBounds; }
    const QSize& size() const { return mSize; }

    /** Loads the input image into the tile */
    void load(const QImage& image, const QPoint& topLeft);
    void clear();

private:
    QPixmap mTilePixmap;
    QPointF mPosF;
    QPoint mPos;
    QRect mBounds;
    QSize mSize;
};

#endif // TILE_H
