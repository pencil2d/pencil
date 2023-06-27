/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2012-2018 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/
#include "tiledbuffer.h"
#include "QtGui/qpainterpath.h"

#include <QDebug>

#include <QPixmap>
#include <QPainter>
#include <QDebug>
#include <QtMath>
#include <QPainterPath>

TiledBuffer::TiledBuffer(QObject* parent) : QObject(parent)
{
}

TiledBuffer::~TiledBuffer()
{
    clear();
}

Tile* TiledBuffer::getTileFromIndex(int tileX, int tileY)
{
    TileIndex tileIndex;
    tileIndex.x = tileX;
    tileIndex.y = tileY;

    Tile* selectedTile = mTiles.value(tileIndex, nullptr);

    if (!selectedTile) {
        // Time to allocate it, update table:
        selectedTile = new Tile(QSize(UNIFORM_TILESIZE, UNIFORM_TILESIZE));
        mTiles.insert(tileIndex, selectedTile);

        const QPoint& tilePos (getTilePos(tileIndex));
        selectedTile->setPos(tilePos);

        emit this->onNewTile(this, selectedTile);
    } else {
        emit this->onUpdateTile(this, selectedTile);
    }

    return selectedTile;
}

void TiledBuffer::drawBrush(const QPointF& point, int brushWidth, QPen pen, QBrush brush, QPainter::CompositionMode cm, bool antialiasing) {
    QRectF brushRect(point.x() - 0.5 * brushWidth, point.y() - 0.5 * brushWidth, brushWidth, brushWidth);

    int radius = (brushWidth + 1);
    float tilesize = UNIFORM_TILESIZE;

    // Gather the amount of tiles that fits the size of the brush width
    int xLeft = qFloor((qFloor(point.x() - radius)) / tilesize);
    int xRight = qFloor((qFloor(point.x() + radius)) / tilesize);
    int yTop = qFloor(qFloor(point.y() - radius) / tilesize);
    int yBottom = qFloor(qFloor(point.y() + radius) / tilesize);

    for (int tileY = yTop; tileY <= yBottom; tileY++) {
        for (int tileX = xLeft; tileX <= xRight; tileX++) {

            Tile* tile = getTileFromIndex(tileX, tileY);

            if (tile)
            {
                QPainter painter(&tile->pixmap());

                painter.translate(-tile->pos());
                painter.setRenderHint(QPainter::Antialiasing, antialiasing);
                painter.setPen(pen);
                painter.setBrush(brush);
                painter.setCompositionMode(cm);
                painter.drawEllipse(brushRect);
                painter.end();

                mTileBounds.extend(tile->pos(), tile->boundingRect().size());
            }
        }
    }
}

void TiledBuffer::drawImage(const QImage& image, const QPointF& point, QPainter::CompositionMode cm, bool antialiasing) {
    float tilesize = UNIFORM_TILESIZE;

    float imageXRad = image.width();
    float imageYRad = image.height();
    // Gather the amount of tiles that fits the size of the brush width
    int xLeft = qFloor((qFloor(point.x() - imageXRad)) / tilesize);
    int xRight = qFloor((qFloor(point.x() + imageXRad)) / tilesize);
    int yTop = qFloor(qFloor(point.y() - imageYRad) / tilesize);
    int yBottom = qFloor(qFloor(point.y() + imageYRad) / tilesize);

    for (int tileY = yTop; tileY <= yBottom; tileY++) {
        for (int tileX = xLeft; tileX <= xRight; tileX++) {

            Tile* tile = getTileFromIndex(tileX, tileY);

            if (tile)
            {
                QPainter painter(&tile->pixmap());

                painter.translate(-tile->pos());
                painter.setRenderHint(QPainter::Antialiasing, antialiasing);
                painter.setCompositionMode(cm);
                painter.drawImage(point, image);
                painter.end();

                mTileBounds.extend(tile->pos(), tile->boundingRect().size());
            }
        }
    }
}


void TiledBuffer::drawPath(QPainterPath path, QPen pen, QBrush brush,
                           QPainter::CompositionMode cm, bool antialiasing)
{
    int width = pen.width();



//    QPainter painter(image());
//    painter.setCompositionMode(cm);
//    painter.setRenderHint(QPainter::Antialiasing, antialiasing);
//    painter.setPen(pen);
//    painter.setBrush(brush);
//    painter.setWorldMatrixEnabled(true);
//    if (path.length() > 0)
//    {
//        painter.drawPath( path );
//    }
//    else
//    {
//        // forces drawing when points are coincident (mousedown)
//        painter.drawPoint(static_cast<int>(path.elementAt(0).x), static_cast<int>(path.elementAt(0).y));
//    }
//    painter.end();

    int radius = (width + 1);
    float tilesize = UNIFORM_TILESIZE;
    QRectF pathRect = path.boundingRect();

    // Gather the amount of tiles that fits the size of the brush width
    int xLeft = qFloor((qFloor(pathRect.left() - radius)) / tilesize);
    int xRight = qFloor((qFloor(pathRect.right() + radius)) / tilesize);
    int yTop = qFloor(qFloor(pathRect.top() - radius) / tilesize);
    int yBottom = qFloor(qFloor(pathRect.bottom() + radius) / tilesize);

    for (int tileY = yTop; tileY <= yBottom; tileY++) {
        for (int tileX = xLeft; tileX <= xRight; tileX++) {

            Tile* tile = getTileFromIndex(tileX, tileY);

            if (tile)
            {
                QPainter painter(&tile->pixmap());

                painter.translate(-tile->pos());
                painter.setRenderHint(QPainter::Antialiasing, antialiasing);
                painter.setPen(pen);
                painter.setBrush(brush);
                painter.setCompositionMode(cm);
                painter.drawPath(path);
                painter.end();

                mTileBounds.extend(tile->pos(), tile->boundingRect().size());
            }
        }
    }
}

void TiledBuffer::loadTile(const QImage& image, const QPoint& topLeft, Tile* tile)
{
    QPainter painter(&tile->pixmap());

    painter.translate(-tile->pos());
    painter.drawImage(topLeft, image);
    painter.end();
}

void TiledBuffer::clear()
{
    QHashIterator<TileIndex, Tile*> i(mTiles);

    while (i.hasNext()) {
        i.next();
        Tile* tile = i.value();
        if (tile)
        {
            // Clear the content of the tile
            //
            tile->clear();
            mTiles.remove(i.key());
            delete tile;
        }
    }

    mTileBounds = BlitRect();

    emit this->onClearedSurface(this);
}

//Status TiledBuffer::writeFile(const QString& filename)
//{
//    if (mTiles.values().first()) {
//        const QImage& image = surfaceAsImage();
//        bool b = image.save(filename);
//        return (b) ? Status::OK : Status::FAIL;
//    }
//    return Status::FAIL;
//}

QPoint TiledBuffer::getTilePos(const TileIndex& index) const
{
    return QPoint { qRound(UNIFORM_TILESIZE*static_cast<qreal>(index.x)),
                    qRound(UNIFORM_TILESIZE*static_cast<qreal>(index.y)) };
}

TileIndex TiledBuffer::getTileIndex(const TileIndex& pos) const
{
    return { qRound(static_cast<qreal>(pos.x)/UNIFORM_TILESIZE),
             qRound(static_cast<qreal>(pos.y)/UNIFORM_TILESIZE) };
}

QRect TiledBuffer::getRectForPoint(const QPoint& point, const QSize size) const
{
    return QRect(point.x(), point.y(), size.width(), size.height());
}

QRect TiledBuffer::getRectForPoint(const QPoint& point) const
{
    return QRect(point.x(), point.y(), UNIFORM_TILESIZE, UNIFORM_TILESIZE);
}
