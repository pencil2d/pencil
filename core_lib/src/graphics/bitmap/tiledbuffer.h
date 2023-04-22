#ifndef TILEDBUFFER_H
#define TILEDBUFFER_H

#include <QRect>
#include <QImage>
#include <QPainter>

#include <QHash>

#include "blitrect.h"
#include "pencilerror.h"
#include "tile.h"

class TiledBuffer: public QObject
{
    Q_OBJECT
public:
    TiledBuffer(QObject* parent = nullptr);
    TiledBuffer(const TiledBuffer& pieces);
    TiledBuffer(const QRect bounds);
    ~TiledBuffer();

//    void refreshSurface();

//    QImage surfaceAsImage();

    Status writeFile(const QString& filename);

    void addTileToSurface(const QPixmap& pixmap, const QPoint& pos);
//    void appendBitmapSurface(const Surface& surface);

    /**
     * @brief paintSurfaceUsing
     * Paint the surface using a given pixmap and position as input
     * Currently useful for selections but might have other uses.
     * @param inPixmap
     * @param newPos
     */
    void paintSurfaceUsing(const QPixmap& inPixmap, const QPoint& newPos);

    /**
     * @brief drawRectOnSurface
     * For test and debug currently
     * @param rect
     * @param color
     */
    void drawRect(QRect rect, QColor color);

    /**
     * @brief createNewSurfaceFromImage
     * Creates a new surface from an input image or path
     * Useful if you need to import a new image or load a project
     * @param image
     * @param topLeft
     */
    void createNewSurfaceFromImage(const QImage& image, const QPoint& topLeft);
    void createNewSurfaceFromImage(const QString& path, const QPoint& topLeft);

    bool isTransparent(QImage image);

    /**
     * @brief extendBoundaries
     * Extend boundaries of the surface if the input rectangle lies outside.
     * @param rect
     */
    void extendBoundaries(const QRect &rect);

    /**
     * @brief cutSurfaceAsPixmap
     * Retrieves a pixmap from the surface you've selected and erases what's underneath
     * @param selection
     * @return a pixmap with the given paint within the selection
     */
    QPixmap cutSurfaceAsPixmap(const QRect selection);

    /**
     * @brief copySurfaceAsPixmap
     * Retrives a pixmap from the surface you've selected and keeps the surface intact
     * @param selection
     * @return a pixmap with the given paint within the selection
     */
    QPixmap copySurfaceAsPixmap(const QRect selection);

    /**
     * @brief eraseSelection
     * Removes paint within a given selection
     * Useful for selections.
     * @param selection
     */
    void eraseSelection(const QRect selection);
    void eraseSelection(const QPoint pos, QPixmap& pixmap, const QRect selection);
    void fillSelection(const QPoint &pos, QPixmap &pixmap, QColor color, const QRect selection);
    void clear();

    QList<QPixmap> pixmaps();
    QList<QPoint> tilePositions();

//    inline const QRect bounds() { return mSurface.bounds; }

    QImage cachedSurfaceImage() { return mCachedSurface; }

    void renderSurfaceImage();

//    Surface surface() { return mSurface; }
//    const Surface readOnlySurface() const {return mSurface; }

    bool isValid() { return !mTiles.isEmpty(); }

    void drawBrush(const QPointF& point, int brushWidth, QPen pen, QBrush brush, QPainter::CompositionMode cm, bool antialiasing);
    void drawPath(QPainterPath path, QPen pen, QBrush brush,
                  QPainter::CompositionMode cm, bool antialiasing);

    QHash<QString, Tile*> tiles() const { return mTiles; }

    const QRect& bounds() const { return mTileBounds; }


signals:
    void onUpdateTile(TiledBuffer *tiledBuffer, Tile* tile);
    void onNewTile(TiledBuffer *tiledBuffer, Tile* tile);
    void onClearTile(TiledBuffer *tiledBuffer, Tile* tile);
    void onClearedSurface(TiledBuffer *tiledBuffer);

private:

    Tile* getTileFromIndex(const QPoint& index);

    inline QPoint getTilePos(const QPoint& idx) const;
    inline QPoint getTileIndex(const QPoint& pos) const;
    inline QPointF getTileFIndex(const QPoint& pos) const;

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

    QHash<QString, Tile*> mTiles;

//    Surface mSurface;
    QImage mCachedSurface;

    bool mImageCacheValid = false;
};

#endif // TILEDBUFFER_H
