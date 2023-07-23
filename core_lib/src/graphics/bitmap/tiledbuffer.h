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

#include <QRect>
#include <QImage>
#include <QPainter>

#include <QHash>

#include "blitrect.h"
#include "pencilerror.h"
#include "tile.h"

struct TileIndex {
    int x;
    int y;
};

inline uint qHash(const TileIndex &key, uint seed)
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

    /** Loads the input tile into the tiledBuffer */
    void loadTile(const QImage& image, const QPoint& topLeft, Tile* tile);

    /** Clears the content of the tiled buffer */
    void clear();

    /** Returns true if there are any tiles, otherwise false */
    bool isValid() { return !mTiles.isEmpty(); }

    /** Draws a brush with the specified parameters to the tiled buffer */
    void drawBrush(const QPointF& point, int brushWidth, int brushCursorWidth, QPen pen, QBrush brush, QPainter::CompositionMode cm, bool antialiasing);
    /** Draws a path with the specified parameters to the tiled buffer */
    void drawPath(QPainterPath path, int cursorWidth, QPen pen, QBrush brush,
                  QPainter::CompositionMode cm, bool antialiasing);
    /** Draws a image with the specified parameters to the tiled buffer */
    void drawImage(const QImage& image, const QRect& imageBounds, QPainter::CompositionMode cm, bool antialiasing);

    QHash<TileIndex, Tile*> tiles() const { return mTiles; }

    const QRect& bounds() const { return mTileBounds; }


signals:
    void onUpdateTile(TiledBuffer* tiledBuffer, Tile* tile);
    void onNewTile(TiledBuffer* tiledBuffer, Tile* tile);
    void onClearTile(TiledBuffer* tiledBuffer, Tile* tile);
    void onClearedSurface(TiledBuffer* tiledBuffer);

private:

    Tile* getTileFromIndex(int tileX, int tileY);

    inline QPoint getTilePos(const TileIndex& index) const;
    inline TileIndex getTileIndex(const TileIndex& pos) const;

    /**
     * @brief getRectForPoint
     * Returns a rectangle with a specified size for the given point
     * @param point
     * @param size
     * @return QRect
     */
    QRect getRectForPoint(const QPoint& point, const QSize size) const;

    /**
     * @brief getRectForPoint
     * Returns a rectnagle with the size of TILESIZE (64,64)
     * @param point
     * @return QRect
     */
    QRect getRectForPoint(const QPoint& point) const;

    const int UNIFORM_TILESIZE = 64;

    BlitRect mTileBounds;

    QHash<TileIndex, Tile*> mTiles;

    bool mImageCacheValid = false;
};

#endif // TILEDBUFFER_H
