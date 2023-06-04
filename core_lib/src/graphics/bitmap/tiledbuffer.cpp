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

TiledBuffer::TiledBuffer(const TiledBuffer& pieces)
{
}

TiledBuffer::TiledBuffer(const QRect bounds)
{
//    Tiles tiles;
//    mSurface = Surface(tiles, bounds);
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


//void TiledBuffer::createNewSurfaceFromImage(const QString& path, const QPoint& topLeft)
//{
//    QImage image(path);
//    createNewSurfaceFromImage(image, topLeft);
//}

//void TiledBuffer::createNewSurfaceFromImage(const QImage& image, const QPoint& topLeft)
//{
//    QPixmap paintTo(TILESIZE);
//    mSurface = Surface();
//    mSurface.bounds = QRect(topLeft, image.size());

//    QList<QPoint> touchedPoints = touchedTiles(bounds());
//    for (int point = 0; point < touchedPoints.count(); point++) {

//        const QPoint& touchedPoint = touchedPoints.at(point);

//        paintTo.fill(Qt::transparent);
//        QPainter painter(&paintTo);

//        painter.save();
//        painter.translate(-touchedPoint);
//        painter.drawImage(topLeft, image);
//        painter.restore();
//        if (!isTransparent(paintTo.toImage())) {
//            mSurface.appendTile(paintTo, touchedPoint);
//        }
//    }
//}

void TiledBuffer::addTileToSurface(const QPixmap& pixmap, const QPoint& pos)
{
//    mSurface.appendTile(pixmap, pos);


//    extendBoundaries(QRect(pos, pixmap.size()));
    mImageCacheValid = false;
}

//void TiledBuffer::appendBitmapSurface(const Surface &surface)
//{
//    mSurface += surface;
//    mImageCacheValid = false;
//}

//void TiledBuffer::renderSurfaceImage()
//{
//    if (!mImageCacheValid) {
//        mCachedSurface = surfaceAsImage();
//        mImageCacheValid = true;
//    }
//}

//void TiledBuffer::extendBoundaries(const QRect& rect)
//{
//    mSurface.extendBoundaries(rect);
//}

//void TiledBuffer::paintSurfaceUsing(const QPixmap& inPixmap, const QPoint& newPos)
//{
//    Surface outSurface;

//    QPixmap outPix = QPixmap(TILESIZE);
//    outPix.fill(Qt::transparent);

//    const QRect& adjustedPixRect = QRect(newPos, inPixmap.size());
//    const QList<QPoint>& touchedPoints = touchedTiles(adjustedPixRect);

//    // paint input pixmap on tiles
//    for (int point = 0; point < touchedPoints.count(); point++) {

//        const QPoint& touchedPoint = touchedPoints.at(point);
//        QPainter painter(&outPix);
//        outPix.fill(Qt::transparent);

//        painter.save();
//        painter.translate(-touchedPoint);
//        painter.drawPixmap(newPos, inPixmap);
//        painter.restore();
//        painter.end();

//        QImage testImage = outPix.toImage();
//        if (isTransparent(testImage)) {
//            continue;
//        }

//        outSurface.appendTile(outPix, touchedPoint);
//        outSurface.bounds = adjustedPixRect;
//    }

//    Surface extraSurface;

//    // paint new tiles on previous tiles if possible, otherwise
//    // prepare to be added to bitmapsurface
//    QHashIterator<QPoint, std::shared_ptr<QPixmap>> newTiles(outSurface.tiles);
//    while (newTiles.hasNext()) {
//        newTiles.next();

//        QPixmap newPix = outSurface.pixmapAtPos(newTiles.key());
//        const QPoint& newPos = outSurface.posFromPixmap(newTiles.value());
//        QPixmap paintToPix = QPixmap(newPix.size());
//        paintToPix.fill(Qt::transparent);

//        bool noMatch = false;
//        QHashIterator<QPoint, std::shared_ptr<QPixmap>> cuTiles(mSurface.tiles);
//        while (cuTiles.hasNext())
//        {
//            cuTiles.next();

//            QPixmap& existingPix = mSurface.pixmapAtPos(cuTiles.key());
//            const QPoint existingPos = mSurface.posFromPixmap(cuTiles.value());

//            if (existingPos == newPos) {
//                QPainter painter(&existingPix);
//                painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
//                painter.drawPixmap(QPoint(), newPix);
//                painter.end();
//            } else {

//                if (mSurface.contains(newPos)) {
//                    continue;
//                }
//                noMatch = true;
//            }
//        }

//        if (noMatch)
//        {
//            extraSurface.appendTile(newPix, newPos);
//            extraSurface.bounds = outSurface.bounds;
//            noMatch = false;
//        }
//    }

//    appendBitmapSurface(extraSurface);
//}

//QList<QPoint> TiledBuffer::scanForSurroundingTiles(const QRect& rect)
//{
//    QPoint kernel[] = {QPoint(-64,-64),QPoint(0,-64), QPoint(64,-64),
//                       QPoint(-64,0), QPoint(0,0), QPoint(64,0),
//                       QPoint(-64,64), QPoint(0,64), QPoint(64,64)};

//    QList<QPoint> points;
//    QList<QPoint> corners;

//    corners.append({rect.topLeft(), rect.topRight(), rect.bottomLeft(), rect.bottomRight()});
//    for (unsigned int i = 0; i < sizeof(kernel)/sizeof(kernel[0]); i++) {

//        for (int p = 0; p < corners.count(); p++) {
//            const QPoint& corner = corners[p];
//            const QPoint& idx = getTileIndex(corner+kernel[i]);
//            const QPoint& pos = getTilePos(idx);
//            const QRect& rectToIntersect = getRectForPoint(pos);

//            if (rectToIntersect.intersects(rect)) {
//                if (points.contains(pos)) {
//                    continue;
//                }

//                points.append(pos);
//            }
//        }
//    }

//    return points;
//}

//QList<QPoint> TiledBuffer::scanForTilesAtRect(const QRect& rect)
//{
//    const float& imageWidth = static_cast<float>(rect.width());
//    const float& imageHeight = static_cast<float>(rect.height());
//    const float& tileWidth = static_cast<float>(TILESIZE.width());
//    const float& tileHeight = static_cast<float>(TILESIZE.height());
//    const int& nbTilesOnWidth = static_cast<int>(ceil(imageWidth / tileWidth));
//    const int& nbTilesOnHeight = static_cast<int>(ceil(imageHeight / tileHeight));

//    QList<QPoint> points;

//    QList<QPoint> corners;
//    const QPoint& cornerOffset = QPoint(TILESIZE.width(), TILESIZE.height());

//    corners.append({rect.topLeft(), rect.topRight(), rect.bottomLeft(), rect.bottomRight()});
//    for (int h=0; h < nbTilesOnHeight; h++) {
//        for (int w=0; w < nbTilesOnWidth; w++) {

//            const QPoint& tilePos = getTilePos(QPoint(w,h));
//            for (int i = 0; i < corners.count(); i++) {
//                QPoint movedPos = getTileIndex(corners[i]-cornerOffset);
//                movedPos = getTilePos(movedPos)+tilePos;

//                if (points.contains(movedPos)) {
//                    continue;
//                }

//                if (getRectForPoint(movedPos).intersects(rect)) {
//                    points.append(movedPos);
//                }
//            }
//        }
//    }
//    return points;
//}

//QList<QPoint> TiledBuffer::touchedTiles(const QRect& rect)
//{
//    return scanForTilesAtRect(rect);
//}

//void TiledBuffer::drawRect(QRect rect, QColor color)
//{
//    float rectWidth = static_cast<float>(rect.width());
//    float rectHeight = static_cast<float>(rect.height());
//    float tileWidth = static_cast<float>(TILESIZE.width());
//    float tileHeight = static_cast<float>(TILESIZE.height());
//    int nbTilesOnWidth = static_cast<int>(ceil(rectWidth / tileWidth));
//    int nbTilesOnHeight = static_cast<int>(ceil(rectHeight / tileHeight));

//    QPixmap paintTo(UNIFORM_TILESIZE, UNIFORM_TILESIZE);
//    mSurface = Surface();
//    mSurface.bounds = QRect(rect.topLeft(), rect.size());

//    for (int h=0; h < nbTilesOnHeight; h++) {
//        for (int w=0; w < nbTilesOnWidth; w++) {
//            paintTo.fill(Qt::transparent);
//            const QPoint& idx = QPoint(w, h);
//            const QPoint& tilePos = getTilePos(idx);

//            const QRect& tileRect = QRect(tilePos, TILESIZE);
//            QImage colorImage = QImage(rect.size(), QImage::Format_ARGB32_Premultiplied);
//            colorImage.fill(color);
//            const QImage& tileImage = colorImage.copy(tileRect);

//            QPainter painter(&paintTo);
//            painter.drawImage(QPoint(), tileImage);
//            painter.end();

//            mSurface.appendTile(paintTo, tilePos);
//        }
//    }
//}

//QImage TiledBuffer::surfaceAsImage()
//{
//    QImage paintedImage(bounds.size(), QImage::Format_ARGB32_Premultiplied);
//    paintedImage.fill(Qt::transparent);

//    QPainter painter(&paintedImage);
//    painter.translate(-mSurface.topLeft());

//    QHashIterator<QPoint, std::shared_ptr<QPixmap>> cuTiles(mSurface.tiles);
//    while (cuTiles.hasNext()) {
//        cuTiles.next();
//        const QPixmap& pix = mSurface.pixmapAtPos(cuTiles.key());
//        const QPoint& pos = mSurface.posFromPixmap(cuTiles.value());
//        painter.drawPixmap(pos, pix);
//    }
//    painter.end();

//    return paintedImage;
//}

//void TiledBuffer::eraseSelection(const QPoint pos, QPixmap& pixmap, const QRect selection)
//{
//    fillSelection(pos, pixmap, Qt::transparent, selection);
//}

//void TiledBuffer::eraseSelection(const QRect selection)
//{
//    QHashIterator<QPoint, std::shared_ptr<QPixmap>> cuTiles(mSurface.tiles);
//    while (cuTiles.hasNext()) {
//        cuTiles.next();

//        QPixmap& pixmap = mSurface.pixmapAtPos(cuTiles.key());
//        const QPoint& pos = mSurface.posFromPixmap(cuTiles.value());
//        eraseSelection(pos, pixmap, selection);
//    }
//}

//void TiledBuffer::fillSelection(const QPoint& pos, QPixmap& pixmap, QColor color, const QRect selection)
//{
//    QPainter painter(&pixmap);
//    painter.translate(-pos);
//    painter.setCompositionMode(QPainter::CompositionMode_Source);

//    const QRect& intersection = selection.intersected(getRectForPoint(pos, pixmap.size()));
//    painter.fillRect(intersection, color);
//    painter.end();

//}

//Surface TiledBuffer::intersectedSurface(const QRect rect)
//{
//    Surface outSurface;
//    QHashIterator<QPoint, std::shared_ptr<QPixmap>> cuTiles(mSurface.tiles);
//    while (cuTiles.hasNext())
//    {
//        cuTiles.next();
//        const QPixmap& pix = mSurface.pixmapAtPos(cuTiles.key());
//        const QPoint& pos = mSurface.posFromPixmap(cuTiles.value());

//        if (getRectForPoint(pos, pix.size()).intersects(rect)) {
//            outSurface.appendTile(pix, pos);
//            outSurface.bounds = rect;
//        }
//    }

//    return outSurface;
//}

//QPixmap TiledBuffer::cutSurfaceAsPixmap(const QRect selection)
//{
//    Q_ASSERT(!selection.isEmpty());

//    Surface intersectSurface = intersectedSurface(selection);

//    eraseSelection(selection);

//    QPixmap paintedImage(selection.size());
//    paintedImage.fill(Qt::transparent);

//    QPainter painter(&paintedImage);
//    painter.translate(-selection.topLeft());

//    QHashIterator<QPoint, std::shared_ptr<QPixmap>> interTiles(intersectSurface.tiles);
//    while(interTiles.hasNext())
//    {
//        interTiles.next();
//        const QPixmap& pix = intersectSurface.pixmapAtPos(interTiles.key());
//        const QPoint& pos = intersectSurface.posFromPixmap(interTiles.value());
//        painter.drawPixmap(pos, pix);
//    }
//    return paintedImage;
//}

//QPixmap TiledBuffer::copySurfaceAsPixmap(const QRect selection)
//{
//    Q_ASSERT(!selection.isEmpty());

//    const Surface& intersectSurface = intersectedSurface(selection);

//    QPixmap paintedImage(selection.size());
//    paintedImage.fill(Qt::transparent);

//    QPainter painter(&paintedImage);
//    painter.translate(-selection.topLeft());
//    QHashIterator<QPoint, std::shared_ptr<QPixmap>> interTiles(intersectSurface.tiles);
//    while(interTiles.hasNext())
//    {
//        interTiles.next();
//        const QPixmap& pix = intersectSurface.pixmapAtPos(interTiles.key());
//        const QPoint& pos = intersectSurface.posFromPixmap(interTiles.value());
//        painter.drawPixmap(pos, pix);
//    }
//    return paintedImage;
//}

//QList<QPoint> TiledBuffer::tilePositions()
//{
//    return mTiles.keys();
//}

//QList<QPixmap> TiledBuffer::pixmaps()
//{
//    return mTiles.values();
//}

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
