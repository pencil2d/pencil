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

#include <QPainterPath>
#include <QtMath>

#include "tile.h"

TiledBuffer::TiledBuffer(QObject* parent) : QObject(parent)
{
}

TiledBuffer::~TiledBuffer()
{
    clear();
}

Tile* TiledBuffer::getTileFromIndex(const TileIndex& tileIndex)
{
    Tile* selectedTile = mTiles.value(tileIndex, nullptr);

    if (!selectedTile) {
        // Time to allocate it, update table:
        const QPoint& tilePos (getTilePos(tileIndex));
        selectedTile = new Tile(tilePos, QSize(UNIFORM_TILE_SIZE, UNIFORM_TILE_SIZE));
        mTiles.insert(tileIndex, selectedTile);

        emit this->tileCreated(this, selectedTile);
    } else {
        emit this->tileUpdated(this, selectedTile);
    }

    return selectedTile;
}

void TiledBuffer::drawBrush(QPointF point, qreal brushWidth, QPen pen, QBrush brush, QPainter::CompositionMode cm, bool antialiasing) {
    const float tileSize = UNIFORM_TILE_SIZE;

    // Gather the number of tiles that fits the size of the brush width
    const int xLeft = qFloor((qFloor(point.x() - brushWidth)) / tileSize);
    const int xRight = qFloor((qFloor(point.x() + brushWidth)) / tileSize);
    const int yTop = qFloor(qFloor(point.y() - brushWidth) / tileSize);
    const int yBottom = qFloor(qFloor(point.y() + brushWidth) / tileSize);

    // If we are not using antialiasing, make sure at least one pixel is within the brush's circle
    bool drawPoint = false;
    if (!antialiasing && brushWidth < 1.42) { // Overestimated approximation of 2*sqrt(2), which is the maximum distance a point can be from the center of a pixel
        // Measure the actual distance to the center of the nearest pixel
        const QPointF nearestPixelCenter(qRound(point.x()+0.5)-0.5, qRound(point.y()+0.5)-0.5);
        const qreal distanceToNearest = QLineF(point, nearestPixelCenter).length();
        if (distanceToNearest >= brushWidth/2) {
            // Nothing will be drawn with drawEllipse, so prepare to draw the nearest pixel with drawPoint
            drawPoint = true;
            point = QPointF(nearestPixelCenter.x() - 0.5, nearestPixelCenter.y() - 0.5);
            pen = QPen(brush, 1);
        }
    }
    const QRectF brushRect(point.x() - 0.5 * brushWidth, point.y() - 0.5 * brushWidth, brushWidth, brushWidth);

    for (int tileY = yTop; tileY <= yBottom; tileY++) {
        for (int tileX = xLeft; tileX <= xRight; tileX++) {

            Tile* tile = getTileFromIndex({tileX, tileY});

            QPainter painter(&tile->pixmap());

            painter.translate(-tile->pos());
            painter.setRenderHint(QPainter::Antialiasing, antialiasing);
            painter.setPen(pen);
            painter.setBrush(brush);
            painter.setCompositionMode(cm);
            if (drawPoint) {
                painter.drawPoint(point);
            } else {
                painter.drawEllipse(brushRect);
            }
            painter.end();

            mTileBounds.extend(tile->bounds());
        }
    }
}

void TiledBuffer::drawImage(const QImage& image, const QRect& imageBounds, QPainter::CompositionMode cm, bool antialiasing) {
    const float tileSize = UNIFORM_TILE_SIZE;
    const float imageXRad = image.width();
    const float imageYRad = image.height();
    // Gather the number of tiles that fits the size of the brush width
    const int xLeft = qFloor((qFloor(imageBounds.left() - imageXRad)) / tileSize);
    const int xRight = qFloor((qFloor(imageBounds.right() + imageXRad)) / tileSize);
    const int yTop = qFloor(qFloor(imageBounds.top() - imageYRad) / tileSize);
    const int yBottom = qFloor(qFloor(imageBounds.bottom() + imageYRad) / tileSize);

    for (int tileY = yTop; tileY <= yBottom; tileY++) {
        for (int tileX = xLeft; tileX <= xRight; tileX++) {

            Tile* tile = getTileFromIndex({tileX, tileY});

            QPainter painter(&tile->pixmap());

            painter.translate(-tile->pos());
            painter.setRenderHint(QPainter::Antialiasing, antialiasing);
            painter.setCompositionMode(cm);
            painter.drawImage(imageBounds.topLeft(), image);
            painter.end();

            mTileBounds.extend(tile->bounds());
        }
    }
}


void TiledBuffer::drawPath(QPainterPath path, QPen pen, QBrush brush,
                           QPainter::CompositionMode cm, bool antialiasing)
{
    const qreal width = pen.widthF();
    const float tileSize = UNIFORM_TILE_SIZE;
    const QRectF pathRect = path.boundingRect();

    // Gather the number of tiles that fits the size of the brush width
    const int xLeft = qFloor((qFloor(pathRect.left() - width)) / tileSize);
    const int xRight = qFloor((qFloor(pathRect.right() + width)) / tileSize);
    const int yTop = qFloor(qFloor(pathRect.top() - width) / tileSize);
    const int yBottom = qFloor(qFloor(pathRect.bottom() + width) / tileSize);

    for (int tileY = yTop; tileY <= yBottom; tileY++) {
        for (int tileX = xLeft; tileX <= xRight; tileX++) {

            Tile* tile = getTileFromIndex({tileX, tileY});

            QPainter painter(&tile->pixmap());

            painter.translate(-tile->pos());
            painter.setRenderHint(QPainter::Antialiasing, antialiasing);
            painter.setPen(pen);
            painter.setBrush(brush);
            painter.setCompositionMode(cm);
            painter.drawPath(path);
            painter.end();

            mTileBounds.extend(tile->bounds());
        }
    }
}

void TiledBuffer::clear()
{
    QHashIterator<TileIndex, Tile*> i(mTiles);

    while (i.hasNext()) {
        i.next();
        Tile* tile = i.value();
        if (tile)
        {
            mTiles.remove(i.key());
            delete tile;
        }
    }

    mTileBounds = BlitRect();
}

QPoint TiledBuffer::getTilePos(const TileIndex& index) const
{
    return QPoint { qRound(UNIFORM_TILE_SIZE*static_cast<qreal>(index.x)),
                    qRound(UNIFORM_TILE_SIZE*static_cast<qreal>(index.y)) };
}
