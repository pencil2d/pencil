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
