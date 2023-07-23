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

#include <QDebug>

#include <QPixmap>
#include <QPainter>
#include <QPainterPath>
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

void TiledBuffer::drawBrush(const QPointF& point, int brushWidth, int brushCursorWidth, QPen pen, QBrush brush, QPainter::CompositionMode cm, bool antialiasing) {
    QRectF brushRect(point.x() - 0.5 * brushWidth, point.y() - 0.5 * brushWidth, brushWidth, brushWidth);

    float tilesize = UNIFORM_TILESIZE;

    int width = qMax(brushCursorWidth,brushWidth);

    // Gather the amount of tiles that fits the size of the brush width
    int xLeft = qFloor((qFloor(point.x() - width)) / tilesize);
    int xRight = qFloor((qFloor(point.x() + width)) / tilesize);
    int yTop = qFloor(qFloor(point.y() - width) / tilesize);
    int yBottom = qFloor(qFloor(point.y() + width) / tilesize);

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

void TiledBuffer::drawImage(const QImage& image, const QRect& imageBounds, QPainter::CompositionMode cm, bool antialiasing) {
    float tilesize = UNIFORM_TILESIZE;

    float imageXRad = image.width();
    float imageYRad = image.height();
    // Gather the amount of tiles that fits the size of the brush width
    int xLeft = qFloor((qFloor(imageBounds.left() - imageXRad)) / tilesize);
    int xRight = qFloor((qFloor(imageBounds.right() + imageXRad)) / tilesize);
    int yTop = qFloor(qFloor(imageBounds.top() - imageYRad) / tilesize);
    int yBottom = qFloor(qFloor(imageBounds.bottom() + imageYRad) / tilesize);

    for (int tileY = yTop; tileY <= yBottom; tileY++) {
        for (int tileX = xLeft; tileX <= xRight; tileX++) {

            Tile* tile = getTileFromIndex(tileX, tileY);

            if (tile)
            {
                QPainter painter(&tile->pixmap());

                painter.translate(-tile->pos());
                painter.setRenderHint(QPainter::Antialiasing, antialiasing);
                painter.setCompositionMode(cm);
                painter.drawImage(imageBounds.topLeft(), image);
                painter.end();

                mTileBounds.extend(tile->pos(), tile->boundingRect().size());
            }
        }
    }
}


void TiledBuffer::drawPath(QPainterPath path, int cursorWidth, QPen pen, QBrush brush,
                           QPainter::CompositionMode cm, bool antialiasing)
{
    int pathWidth = pen.width();

    int width = (qMax(pathWidth,cursorWidth) + 1);
    float tilesize = UNIFORM_TILESIZE;
    QRectF pathRect = path.boundingRect();

    // Gather the amount of tiles that fits the size of the brush width
    int xLeft = qFloor((qFloor(pathRect.left() - width)) / tilesize);
    int xRight = qFloor((qFloor(pathRect.right() + width)) / tilesize);
    int yTop = qFloor(qFloor(pathRect.top() - width) / tilesize);
    int yBottom = qFloor(qFloor(pathRect.bottom() + width) / tilesize);

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
