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
#ifndef TILEDBUFFER_H
#define TILEDBUFFER_H

#include <QObject>
#include <QPainter>
#include <QHash>

#include "blitrect.h"

class QImage;
class QRect;
class Tile;

struct TileIndex {
    int x;
    int y;
};

#if QT_VERSION < QT_VERSION_CHECK(6,0,0)
inline uint qHash(const TileIndex &key, uint seed)
#else
inline size_t qHash(const TileIndex &key, size_t seed)
#endif
{
    return qHash(key.x, seed) ^ key.y;
}

inline bool operator==(const TileIndex &e1, const TileIndex &e2)
{
    return e1.x == e2.x
           && e1.y == e2.y;
}

class TiledBuffer: public QObject
{
    Q_OBJECT
public:
    TiledBuffer(QObject* parent = nullptr);
    ~TiledBuffer();

    /** Clears the content of the tiled buffer */
    void clear();

    /** Returns true if there are any tiles, otherwise false */
    bool isValid() const { return !mTiles.isEmpty(); }

    /** Draws a brush with the specified parameters to the tiled buffer */
    void drawBrush(QPointF point, qreal brushWidth, QPen pen, QBrush brush, QPainter::CompositionMode cm, bool antialiasing);
    /** Draws a path with the specified parameters to the tiled buffer */
    void drawPath(QPainterPath path, QPen pen, QBrush brush,
                  QPainter::CompositionMode cm, bool antialiasing);
    /** Draws a image with the specified parameters to the tiled buffer */
    void drawImage(const QImage& image, const QRect& imageBounds, QPainter::CompositionMode cm, bool antialiasing);

    QHash<TileIndex, Tile*> tiles() const { return mTiles; }

    const QRect& bounds() const { return mTileBounds; }

signals:
    void tileUpdated(TiledBuffer* tiledBuffer, Tile* tile);
    void tileCreated(TiledBuffer* tiledBuffer, Tile* tile);

private:

    Tile* getTileFromIndex(const TileIndex& tileIndex);

    inline QPoint getTilePos(const TileIndex& index) const;

    const int UNIFORM_TILE_SIZE = 64;

    BlitRect mTileBounds;

    QHash<TileIndex, Tile*> mTiles;
};

#endif // TILEDBUFFER_H
