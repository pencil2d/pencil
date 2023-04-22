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
