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

    Status writeFile(const QString& filename);

    void loadTile(const QImage& image, const QPoint& topLeft, Tile* tile);

    /**
     * @brief drawRectOnSurface
     * For test and debug currently
     * @param rect
     * @param color
     */
    void drawRect(QRect rect, QColor color);

    bool isTransparent(QImage image);

    /**
     * @brief extendBoundaries
     * Extend boundaries of the surface if the input rectangle lies outside.
     * @param rect
     */
    void extendBoundaries(const QRect &rect);

    void clear();

    bool isValid() { return !mTiles.isEmpty(); }

    void drawBrush(const QPointF& point, int brushWidth, QPen pen, QBrush brush, QPainter::CompositionMode cm, bool antialiasing);
    void drawPath(QPainterPath path, QPen pen, QBrush brush,
                  QPainter::CompositionMode cm, bool antialiasing);
    void drawImage(const QImage& image, const QPointF& point, QPainter::CompositionMode cm, bool antialiasing);

    QHash<TileIndex, Tile*> tiles() const { return mTiles; }

    const QRect& bounds() const { return mTileBounds; }


signals:
    void onUpdateTile(TiledBuffer *tiledBuffer, Tile* tile);
    void onNewTile(TiledBuffer *tiledBuffer, Tile* tile);
    void onClearTile(TiledBuffer *tiledBuffer, Tile* tile);
    void onClearedSurface(TiledBuffer *tiledBuffer);

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

    /**
     * @brief touchedTiles
     * Will find and return points within and surrounding the selection
     * @param QRect rect
     * @return list of touched points
     */
    QList<QPoint> touchedTiles(const QRect& rect);

//    /**
//     * @brief BitmapSurface::intersectedSurface
//     * Returns a Surface containing the tiles that intersected the region
//     * @param rect
//     * @return Surface
//     */
//    Surface intersectedSurface(const QRect rect);

    QList<QPoint> scanForSurroundingTiles(const QRect& rect);
    QList<QPoint> scanForTilesAtRect(const QRect& rect);

    const int UNIFORM_TILESIZE = 64;

    BlitRect mTileBounds;

    QHash<TileIndex, Tile*> mTiles;

//    Surface mSurface;
    QImage mCachedSurface;

    bool mImageCacheValid = false;
};

#endif // TILEDBUFFER_H
