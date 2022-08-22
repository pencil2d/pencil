/*

Pencil2D - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2012-2020 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/
#include "bitmapimage.h"

#include <cmath>
#include <QDebug>
#include <QtMath>
#include <QFile>
#include <QFileInfo>
#include <QPainterPath>
#include "util.h"

#include "blitrect.h"

BitmapImage::BitmapImage()
{
}

BitmapImage::BitmapImage(const BitmapImage& a) : KeyFrame(a)
{
    mBounds = a.mBounds;
    mMinBound = a.mMinBound;
    mEnableAutoCrop = a.mEnableAutoCrop;
    mOpacity = a.mOpacity;
    mImage = a.mImage;
}

BitmapImage::BitmapImage(const QRect& rectangle, const QColor& color)
{
    mBounds = rectangle;
    mImage = QImage(mBounds.size(), QImage::Format_ARGB32_Premultiplied);
    mImage.fill(color.rgba());
    mMinBound = false;
}

BitmapImage::BitmapImage(const QPoint& topLeft, const QImage& image)
{
    mBounds = QRect(topLeft, image.size());
    mMinBound = true;
    mImage = image;
}

BitmapImage::BitmapImage(const QPoint& topLeft, const QString& path)
{
    setFileName(path);
    mImage = QImage();

    mBounds = QRect(topLeft, QSize(-1, 0));
    mMinBound = true;
    setModified(false);
}

BitmapImage::~BitmapImage()
{
}

void BitmapImage::setImage(QImage* img)
{
    Q_ASSERT(img);
    mImage = *img;
    mMinBound = false;

    modification();
}

BitmapImage& BitmapImage::operator=(const BitmapImage& a)
{
    if (this == &a)
    {
        return *this; // a self-assignment
    }

    KeyFrame::operator=(a);
    mBounds = a.mBounds;
    mMinBound = a.mMinBound;
    mOpacity = a.mOpacity;
    mImage = a.mImage;
    modification();
    return *this;
}

BitmapImage* BitmapImage::clone() const
{
    BitmapImage* b = new BitmapImage(*this);
    b->setFileName(""); // don't link to the file of the source bitmap image

    const bool validKeyFrame = !fileName().isEmpty();
    if (validKeyFrame && !isLoaded()) 
    {
        // This bitmapImage is temporarily unloaded.
        // since it's not in the memory, we need to copy the linked png file to prevent data loss.
        QFileInfo finfo(fileName());
        Q_ASSERT(finfo.isAbsolute());
        Q_ASSERT(QFile::exists(fileName()));

        QString newFileName = QString("%1/%2-%3.%4")
            .arg(finfo.canonicalPath())
            .arg(finfo.completeBaseName())
            .arg(uniqueString(12))
            .arg(finfo.suffix());
        b->setFileName(newFileName);

        bool ok = QFile::copy(fileName(), newFileName);
        Q_ASSERT(ok);
        qDebug() << "COPY>" << fileName();
    }
    return b;
}

void BitmapImage::loadFile()
{
    if (!fileName().isEmpty() && !isLoaded())
    {
        mImage = QImage(fileName());
        mBounds.setSize(mImage.size());
        mMinBound = false;
    }
}

void BitmapImage::unloadFile()
{
    if (isModified() == false)
    {
        mImage = QImage();
    }
}

bool BitmapImage::isLoaded() const
{
    return mImage.width() == mBounds.width();
}

quint64 BitmapImage::memoryUsage()
{
    if (!mImage.isNull())
    {
        return imageSize(mImage);
    }
    return 0;
}

void BitmapImage::paintImage(QPainter& painter)
{
    painter.drawImage(mBounds.topLeft(), *image());
}

void BitmapImage::paintImage(QPainter& painter, QImage& image, QRect sourceRect, QRect destRect)
{
    painter.drawImage(QRect(mBounds.topLeft(), destRect.size()),
                      image,
                      sourceRect);
}

QImage* BitmapImage::image()
{
    loadFile();
    return &mImage;
}

BitmapImage BitmapImage::copy()
{
    return BitmapImage(mBounds.topLeft(), *image());
}

BitmapImage BitmapImage::copy(QRect rectangle)
{
    if (rectangle.isEmpty() || mBounds.isEmpty()) return BitmapImage();

    QRect intersection2 = rectangle.translated(-mBounds.topLeft());

    // If the region goes out of bounds, make sure the image is formatted in ARGB
    // so that the area beyond the image bounds is transparent.
    if (!mBounds.contains(rectangle) && !image()->hasAlphaChannel())
    {
        mImage = mImage.convertToFormat(QImage::Format_ARGB32);
    }

    BitmapImage result(rectangle.topLeft(), image()->copy(intersection2));
    return result;
}

void BitmapImage::paste(BitmapImage* bitmapImage, QPainter::CompositionMode cm)
{
    if(bitmapImage->width() <= 0 || bitmapImage->height() <= 0)
    {
        return;
    }

    setCompositionModeBounds(bitmapImage, cm);

    QImage* image2 = bitmapImage->image();

    QPainter painter(image());
    painter.setCompositionMode(cm);
    painter.drawImage(bitmapImage->mBounds.topLeft() - mBounds.topLeft(), *image2);
    painter.end();

    modification();
}

void BitmapImage::moveTopLeft(QPoint point)
{
    mBounds.moveTopLeft(point);
    // Size is unchanged so there is no need to update mBounds
    modification();
}

void BitmapImage::transform(QRect newBoundaries, bool smoothTransform)
{
    mBounds = newBoundaries;
    newBoundaries.moveTopLeft(QPoint(0, 0));
    QImage newImage(mBounds.size(), QImage::Format_ARGB32_Premultiplied);

    QPainter painter(&newImage);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, smoothTransform);
    painter.setCompositionMode(QPainter::CompositionMode_Source);
    painter.fillRect(newImage.rect(), QColor(0, 0, 0, 0));
    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
    painter.drawImage(newBoundaries, *image());
    painter.end();
    mImage = newImage;

    modification();
}

BitmapImage BitmapImage::transformed(QRect selection, QTransform transform, bool smoothTransform)
{
    Q_ASSERT(!selection.isEmpty());

    BitmapImage selectedPart = copy(selection);

    // Get the transformed image
    QImage transformedImage;
    if (smoothTransform)
    {
        transformedImage = selectedPart.image()->transformed(transform, Qt::SmoothTransformation);
    }
    else
    {
        transformedImage = selectedPart.image()->transformed(transform);
    }
    return BitmapImage(transform.mapRect(selection).normalized().topLeft(), transformedImage);
}

BitmapImage BitmapImage::transformed(QRect newBoundaries, bool smoothTransform)
{
    BitmapImage transformedImage(newBoundaries, QColor(0, 0, 0, 0));
    QPainter painter(transformedImage.image());
    painter.setRenderHint(QPainter::SmoothPixmapTransform, smoothTransform);
    newBoundaries.moveTopLeft(QPoint(0, 0));
    painter.drawImage(newBoundaries, *image());
    painter.end();
    return transformedImage;
}

/** Update image bounds.
 *
 *  @param[in] newBoundaries the new bounds
 *
 *  Sets this image's bounds to rectangle.
 *  Modifies mBounds and crops mImage.
 */
void BitmapImage::updateBounds(QRect newBoundaries)
{
    // Check to make sure changes actually need to be made
    if (mBounds == newBoundaries) return;

    QImage newImage(newBoundaries.size(), QImage::Format_ARGB32_Premultiplied);
    newImage.fill(Qt::transparent);
    if (!newImage.isNull())
    {
        QPainter painter(&newImage);
        painter.drawImage(mBounds.topLeft() - newBoundaries.topLeft(), mImage);
        painter.end();
    }
    mImage = newImage;
    mBounds = newBoundaries;
    mMinBound = false;

    modification();
}

void BitmapImage::extend(const QPoint &p)
{
    if (!mBounds.contains(p))
    {
        extend(QRect(p, QSize(1, 1)));
    }
}

void BitmapImage::extend(QRect rectangle)
{
    if (rectangle.width() <= 0) rectangle.setWidth(1);
    if (rectangle.height() <= 0) rectangle.setHeight(1);
    if (mBounds.contains(rectangle))
    {
        // Do nothing
    }
    else
    {
        QRect newBoundaries = mBounds.united(rectangle).normalized();
        QImage newImage(newBoundaries.size(), QImage::Format_ARGB32_Premultiplied);
        newImage.fill(Qt::transparent);
        if (!newImage.isNull())
        {
            QPainter painter(&newImage);
            painter.drawImage(mBounds.topLeft() - newBoundaries.topLeft(), *image());
            painter.end();
        }
        mImage = newImage;
        mBounds = newBoundaries;

        modification();
    }
}

/** Updates the bounds after a drawImage operation with the composition mode cm.
 *
 *  @param[in] source The source image used for the drawImage call.
 *  @param[in] cm The composition mode that will be used for the draw image
 *
 *  @see BitmapImage::setCompositionModeBounds(BitmapImage, QPainter::CompositionMode)
 */
void BitmapImage::setCompositionModeBounds(BitmapImage *source, QPainter::CompositionMode cm)
{
    if (source)
    {
        setCompositionModeBounds(source->mBounds, source->mMinBound, cm);
    }
}

/** Updates the bounds after a draw operation with the composition mode cm.
 *
 * @param[in] sourceBounds The bounds of the source used for drawcall.
 * @param[in] isSourceMinBounds Is sourceBounds the minimal bounds for the source image
 * @param[in] cm The composition mode that will be used for the draw image
 *
 * For a call to draw image of a QPainter (initialized with mImage) with an argument
 * of source, this function intelligently calculates the bounds. It will attempt to
 * preserve minimum bounds based on the composition mode.
 *
 * This works baed on the principle that some minimal bounds can be determined
 * solely by the minimal bounds of this and source, depending on the value of cm.
 * Some composition modes only expand, or have no affect on the bounds.
 *
 * @warning The draw operation described by the arguments of this
 *          function needs to be called after this function is run,
 *          or the bounds will be out of sync. If mBounds is null,
 *          no draw operation needs to be performed.
 */
void BitmapImage::setCompositionModeBounds(QRect sourceBounds, bool isSourceMinBounds, QPainter::CompositionMode cm)
{
    QRect newBoundaries;
    switch(cm)
    {
    case QPainter::CompositionMode_Destination:
    case QPainter::CompositionMode_SourceAtop:
        // The Destination and SourceAtop modes
        // do not change the bounds from destination.
        newBoundaries = mBounds;
        // mMinBound remains the same
        break;
    case QPainter::CompositionMode_SourceIn:
    case QPainter::CompositionMode_DestinationIn:
    case QPainter::CompositionMode_Clear:
    case QPainter::CompositionMode_DestinationOut:
        // The bounds of the result of SourceIn, DestinationIn, Clear, and DestinationOut
        // modes are no larger than the destination bounds
        newBoundaries = mBounds;
        mMinBound = false;
        break;
    default:
        // If it's not one of the above cases, create a union of the two bounds.
        // This contains the minimum bounds, if both the destination and source
        // use their respective minimum bounds.
        newBoundaries = mBounds.united(sourceBounds);
        mMinBound = mMinBound && isSourceMinBounds;
    }

    updateBounds(newBoundaries);
}

/** Removes any transparent borders by reducing the boundaries.
 *
 *  This function reduces the bounds of an image until the top and
 *  bottom rows, and the left and right columns of pixels each
 *  contain at least one pixel with a non-zero alpha value
 *  (i.e. non-transparent pixel). Both mBounds and
 *  the size of #mImage are updated.
 *
 *  @pre mBounds.size() == mImage->size()
 *  @post Either the first and last rows and columns all contain a
 *        pixel with alpha > 0 or mBounds.isEmpty() == true
 *  @post isMinimallyBounded() == true
 */
void BitmapImage::autoCrop()
{
    if (!mEnableAutoCrop) return;
    if (mBounds.isEmpty()) return; // Exit if current bounds are null
    if (mImage.isNull()) return;

    Q_ASSERT(mBounds.size() == mImage.size());

    // Exit if already min bounded
    if (mMinBound) return;

    // Get image properties
    const int width = mImage.width();

    // Relative top and bottom row indices (inclusive)
    int relTop = 0;
    int relBottom = mBounds.height() - 1;

    // Check top row
    bool isEmpty = true; // Used to track if a non-transparent pixel has been found
    while (isEmpty && relTop <= relBottom) // Loop through rows
    {
        // Point cursor to the first pixel in the current top row
        const QRgb* cursor = reinterpret_cast<const QRgb*>(mImage.constScanLine(relTop));
        for (int col = 0; col < width; col++) // Loop through pixels in row
        {
            // If the pixel is not transparent
            // (i.e. alpha channel > 0)
            if (qAlpha(*cursor) != 0)
            {
                // We've found a non-transparent pixel in row relTop,
                // so we can stop looking for one
                isEmpty = false;
                break;
            }
            // Move cursor to point to the next pixel in the row
            cursor++;
        }
        if (isEmpty)
        {
            // If the row we just checked was empty, increase relTop
            // to remove the empty row from the top of the bounding box
            ++relTop;
        }
    }

    // Check bottom row
    isEmpty = true; // Reset isEmpty
    while (isEmpty && relBottom >= relTop) // Loop through rows
    {
        // Point cursor to the first pixel in the current bottom row
        const QRgb* cursor = reinterpret_cast<const QRgb*>(mImage.constScanLine(relBottom));
        for (int col = 0; col < width; col++) // Loop through pixels in row
        {
            // If the pixel is not transparent
            // (i.e. alpha channel > 0)
            if(qAlpha(*cursor) != 0)
            {
                // We've found a non-transparent pixel in row relBottom,
                // so we can stop looking for one
                isEmpty = false;
                break;
            }
            // Move cursor to point to the next pixel in the row
            ++cursor;
        }
        if (isEmpty)
        {
            // If the row we just checked was empty, decrease relBottom
            // to remove the empty row from the bottom of the bounding box
            --relBottom;
        }
    }

    // Relative left and right column indices (inclusive)
    int relLeft = 0;
    int relRight = mBounds.width()-1;

    // Check left row
    isEmpty = (relBottom >= relTop); // Check left only when
    while (isEmpty && relBottom >= relTop && relLeft <= relRight) // Loop through columns
    {
        // Point cursor to the pixel at row relTop and column relLeft
        const QRgb* cursor = reinterpret_cast<const QRgb*>(mImage.constScanLine(relTop)) + relLeft;
        // Loop through pixels in column
        // Note: we only need to loop from relTop to relBottom (inclusive)
        //       not the full image height, because rows 0 to relTop-1 and
        //       relBottom+1 to mBounds.height() have already been
        //       confirmed to contain only transparent pixels
        for (int row = relTop; row <= relBottom; row++)
        {
            // If the pixel is not transparent
            // (i.e. alpha channel > 0)
            if(qAlpha(*cursor) != 0)
            {
                // We've found a non-transparent pixel in column relLeft,
                // so we can stop looking for one
                isEmpty = false;
                break;
            }
            // Move cursor to point to next pixel in the column
            // Increment by width because the data is in row-major order
            cursor += width;
        }
        if (isEmpty)
        {
            // If the column we just checked was empty, increase relLeft
            // to remove the empty column from the left of the bounding box
            ++relLeft;
        }
    }

    // Check right row
    isEmpty = (relBottom >= relTop); // Reset isEmpty
    while (isEmpty && relRight >= relLeft) // Loop through columns
    {
        // Point cursor to the pixel at row relTop and column relRight
        const QRgb* cursor = reinterpret_cast<const QRgb*>(mImage.constScanLine(relTop)) + relRight;
        // Loop through pixels in column
        // Note: we only need to loop from relTop to relBottom (inclusive)
        //       not the full image height, because rows 0 to relTop-1 and
        //       relBottom+1 to mBounds.height()-1 have already been
        //       confirmed to contain only transparent pixels
        for (int row = relTop; row <= relBottom; row++)
        {
            // If the pixel is not transparent
            // (i.e. alpha channel > 0)
            if(qAlpha(*cursor) != 0)
            {
                // We've found a non-transparent pixel in column relRight,
                // so we can stop looking for one
                isEmpty = false;
                break;
            }
            // Move cursor to point to next pixel in the column
            // Increment by width because the data is in row-major order
            cursor += width;
        }
        if (isEmpty)
        {
            // If the column we just checked was empty, increase relRight
            // to remove the empty column from the left of the bounding box
            --relRight;
        }
    }

    //qDebug() << "Original" << mBounds;
    //qDebug() << "Autocrop" << relLeft << relTop << relRight - mBounds.width() + 1 << relBottom - mBounds.height() + 1;
    // Update mBounds and mImage if necessary
    updateBounds(mBounds.adjusted(relLeft, relTop, relRight - mBounds.width() + 1, relBottom - mBounds.height() + 1));

    //qDebug() << "New bounds" << mBounds;

    mMinBound = true;
}

QRgb BitmapImage::pixel(int x, int y)
{
    return pixel(QPoint(x, y));
}

QRgb BitmapImage::pixel(QPoint p)
{
    QRgb result = qRgba(0, 0, 0, 0); // black
    if (mBounds.contains(p))
        result = image()->pixel(p - mBounds.topLeft());
    return result;
}

void BitmapImage::setPixel(int x, int y, QRgb color)
{
    setPixel(QPoint(x, y), color);
}

void BitmapImage::setPixel(QPoint p, QRgb color)
{
    setCompositionModeBounds(QRect(p, QSize(1,1)), true, QPainter::CompositionMode_SourceOver);
    if (mBounds.contains(p))
    {
        image()->setPixel(p - mBounds.topLeft(), color);
    }
    modification();
}

void BitmapImage::fillNonAlphaPixels(const QRgb color)
{
    if (mBounds.isEmpty()) { return; }

    BitmapImage fill(bounds(), color);
    paste(&fill, QPainter::CompositionMode_SourceIn);
}

void BitmapImage::drawLine(QPointF P1, QPointF P2, QPen pen, QPainter::CompositionMode cm, bool antialiasing)
{
    int width = 2 + pen.width();
    setCompositionModeBounds(QRect(P1.toPoint(), P2.toPoint()).normalized().adjusted(-width, -width, width, width), true, cm);
    if (!image()->isNull())
    {
        QPainter painter(image());
        painter.setCompositionMode(cm);
        painter.setRenderHint(QPainter::Antialiasing, antialiasing);
        painter.setPen(pen);
        painter.drawLine(P1 - mBounds.topLeft(), P2 - mBounds.topLeft());
        painter.end();
    }
    modification();
}

void BitmapImage::drawRect(QRectF rectangle, QPen pen, QBrush brush, QPainter::CompositionMode cm, bool antialiasing)
{
    int width = pen.width();
    setCompositionModeBounds(rectangle.adjusted(-width, -width, width, width).toRect(), true, cm);
    if (brush.style() == Qt::RadialGradientPattern)
    {
        QRadialGradient* gradient = (QRadialGradient*)brush.gradient();
        gradient->setCenter(gradient->center() - mBounds.topLeft());
        gradient->setFocalPoint(gradient->focalPoint() - mBounds.topLeft());
    }
    if (!image()->isNull())
    {
        QPainter painter(image());
        painter.setCompositionMode(cm);
        painter.setRenderHint(QPainter::Antialiasing, antialiasing);
        painter.setPen(pen);
        painter.setBrush(brush);
        painter.drawRect(rectangle.translated(-mBounds.topLeft()));
        painter.end();
    }
    modification();
}

void BitmapImage::drawEllipse(QRectF rectangle, QPen pen, QBrush brush, QPainter::CompositionMode cm, bool antialiasing)
{
    int width = pen.width();
    setCompositionModeBounds(rectangle.adjusted(-width, -width, width, width).toRect(), true, cm);
    if (brush.style() == Qt::RadialGradientPattern)
    {
        QRadialGradient* gradient = (QRadialGradient*)brush.gradient();
        gradient->setCenter(gradient->center() - mBounds.topLeft());
        gradient->setFocalPoint(gradient->focalPoint() - mBounds.topLeft());
    }
    if (!image()->isNull())
    {
        QPainter painter(image());

        painter.setRenderHint(QPainter::Antialiasing, antialiasing);
        painter.setPen(pen);
        painter.setBrush(brush);
        painter.setCompositionMode(cm);
        painter.drawEllipse(rectangle.translated(-mBounds.topLeft()));
        painter.end();
    }
    modification();
}

void BitmapImage::drawPath(QPainterPath path, QPen pen, QBrush brush,
                           QPainter::CompositionMode cm, bool antialiasing)
{
    int width = pen.width();
    // qreal inc = 1.0 + width / 20.0;

    setCompositionModeBounds(path.controlPointRect().adjusted(-width, -width, width, width).toRect(), true, cm);

    if (!image()->isNull())
    {
        QPainter painter(image());
        painter.setCompositionMode(cm);
        painter.setRenderHint(QPainter::Antialiasing, antialiasing);
        painter.setPen(pen);
        painter.setBrush(brush);
        painter.setTransform(QTransform().translate(-mBounds.left(), -mBounds.top()));
        painter.setWorldMatrixEnabled(true);
        if (path.length() > 0)
        {
            /*
            for (int pt = 0; pt < path.elementCount() - 1; pt++)
            {
                qreal dx = path.elementAt(pt + 1).x - path.elementAt(pt).x;
                qreal dy = path.elementAt(pt + 1).y - path.elementAt(pt).y;
                qreal m = sqrt(dx*dx + dy*dy);
                qreal factorx = dx / m;
                qreal factory = dy / m;
                for (float h = 0.f; h < m; h += inc)
                {
                    qreal x = path.elementAt(pt).x + factorx * h;
                    qreal y = path.elementAt(pt).y + factory * h;
                    painter.drawPoint(QPointF(x, y));
                }
            }
            */
            painter.drawPath( path );
        }
        else
        {
            // forces drawing when points are coincident (mousedown)
            painter.drawPoint(static_cast<int>(path.elementAt(0).x), static_cast<int>(path.elementAt(0).y));
        }
        painter.end();
    }
    modification();
}

Status BitmapImage::writeFile(const QString& filename)
{
    if (!mImage.isNull())
    {
        bool b = mImage.save(filename);
        return (b) ? Status::OK : Status::FAIL;
    }

    if (bounds().isEmpty())
    {
        QFile f(filename);
        if(f.exists())
        {
            bool b = f.remove();
            return (b) ? Status::OK : Status::FAIL;
        }
        return Status::SAFE;
    }
    return Status::SAFE;
}

void BitmapImage::clear()
{
    mImage = QImage(); // null image
    mBounds = QRect(0, 0, 0, 0);
    mMinBound = true;
    modification();
}

QRgb BitmapImage::constScanLine(int x, int y) const
{
    QRgb result = QRgb();
    if (mBounds.contains(x, y)) {
        result = *(reinterpret_cast<const QRgb*>(mImage.constScanLine(y - mBounds.top())) + x - mBounds.left());
    }
    return result;
}

void BitmapImage::scanLine(int x, int y, QRgb color)
{
    if (!mBounds.contains(x, y)) {
        return;
    }
    // Make sure color is premultiplied before calling
    *(reinterpret_cast<QRgb*>(image()->scanLine(y - mBounds.top())) + x - mBounds.left()) = color;
}

void BitmapImage::clear(QRect rectangle)
{
    QRect clearRectangle = mBounds.intersected(rectangle);
    clearRectangle.moveTopLeft(clearRectangle.topLeft() - mBounds.topLeft());

    setCompositionModeBounds(clearRectangle, true, QPainter::CompositionMode_Clear);

    QPainter painter(image());
    painter.setCompositionMode(QPainter::CompositionMode_Clear);
    painter.fillRect(clearRectangle, QColor(0, 0, 0, 0));
    painter.end();

    modification();
}

/** Compare colors for the purposes of flood filling
 *
 *  Calculates the Eulcidian difference of the RGB channels
 *  of the image and compares it to the tolerance
 *
 *  @param[in] newColor The first color to compare
 *  @param[in] oldColor The second color to compare
 *  @param[in] tolerance The threshold limit between a matching and non-matching color
 *  @param[in,out] cache Contains a mapping of previous results of compareColor with rule that
 *                 cache[someColor] = compareColor(someColor, oldColor, tolerance)
 *
 *  @return Returns true if the colors have a similarity below the tolerance level
 *          (i.e. if Eulcidian distance squared is <= tolerance)
 */
bool BitmapImage::compareColor(QRgb newColor, QRgb oldColor, int tolerance, QHash<QRgb, bool> *cache)
{
    // Handle trivial case
    if (newColor == oldColor) return true;

    if(cache && cache->contains(newColor)) return cache->value(newColor);

    // Get Eulcidian distance between colors
    // Not an accurate representation of human perception,
    // but it's the best any image editing program ever does
    int diffRed = static_cast<int>(qPow(qRed(oldColor) - qRed(newColor), 2));
    int diffGreen = static_cast<int>(qPow(qGreen(oldColor) - qGreen(newColor), 2));
    int diffBlue = static_cast<int>(qPow(qBlue(oldColor) - qBlue(newColor), 2));
    // This may not be the best way to handle alpha since the other channels become less relevant as
    // the alpha is reduces (ex. QColor(0,0,0,0) is the same as QColor(255,255,255,0))
    int diffAlpha = static_cast<int>(qPow(qAlpha(oldColor) - qAlpha(newColor), 2));

    bool isSimilar = (diffRed + diffGreen + diffBlue + diffAlpha) <= tolerance;

    if(cache)
    {
        Q_ASSERT(cache->contains(isSimilar) ? isSimilar == (*cache)[newColor] : true);
        (*cache)[newColor] = isSimilar;
    }

    return isSimilar;
}

bool BitmapImage::floodFill(BitmapImage** replaceImage,
                            const BitmapImage* targetImage,
                            const QRect& cameraRect,
                            const QPoint& point,
                            const QRgb& fillColor,
                            int tolerance,
                            const int expandValue)
{
    // Fill region must be 1 pixel larger than the target image to fill regions on the edge connected only by transparent pixels
    const QRect& fillBounds = targetImage->mBounds;
    QRect maxBounds = cameraRect.united(fillBounds).adjusted(-expandValue, -expandValue, expandValue, expandValue);
    const int maxWidth = maxBounds.width(), left = maxBounds.left(), top = maxBounds.top();

    // If the point we are supposed to fill is outside the max bounds, do nothing
    if(!maxBounds.contains(point))
    {
        return false;
    }

    // Square tolerance for use with compareColor
    tolerance = static_cast<int>(qPow(tolerance, 2));

    QRect newBounds;
    bool shouldFillBorder = false;
    bool *filledPixels = floodFillPoints(targetImage, fillBounds, maxBounds, point, tolerance, newBounds, shouldFillBorder);

    QRect translatedSearchBounds = newBounds.translated(-maxBounds.topLeft());

    if (shouldFillBorder)
    {
        for (int y = 0; y < maxBounds.height(); y++)
        {
            for (int x = 0; x < maxBounds.width(); x++)
            {
                if(!translatedSearchBounds.contains(x, y))
                {
                    filledPixels[y*maxWidth+x] = true;
                }
            }
        }
        newBounds = maxBounds;
    }

    // The scanned bounds should take the expansion into account
    const QRect& expandRect = newBounds.adjusted(-expandValue, -expandValue, expandValue, expandValue);
    if (expandValue > 0) {
        newBounds = expandRect;
    }
    if (!maxBounds.contains(newBounds)) {
        newBounds = maxBounds;
    }
    translatedSearchBounds = newBounds.translated(-maxBounds.topLeft());

    if (expandValue > 0) {
        expandFill(filledPixels, translatedSearchBounds, maxBounds, expandValue);
    }

    *replaceImage = new BitmapImage(newBounds, Qt::transparent);

    // Fill all the found pixels
    for (int y = translatedSearchBounds.top(); y <= translatedSearchBounds.bottom(); y++)
    {
        for (int x = translatedSearchBounds.left(); x <= translatedSearchBounds.right(); x++)
        {
            const int index = y * maxWidth + x;
            if (!filledPixels[index])
            {
                continue;
            }
            (*replaceImage)->scanLine(x + left, y + top, fillColor);
        }
    }

    delete[] filledPixels;

    return true;
}

// Flood filling based on this scanline algorithm
// ----- http://lodev.org/cgtutor/floodfill.html
bool* BitmapImage::floodFillPoints(const BitmapImage* targetImage,
                                   QRect searchBounds,
                                   const QRect& maxBounds,
                                   QPoint point,
                                   const int tolerance,
                                   QRect& newBounds,
                                   bool& fillBorder)
{
    QRgb oldColor = targetImage->constScanLine(point.x(), point.y());
    oldColor = qRgba(qRed(oldColor), qGreen(oldColor), qBlue(oldColor), qAlpha(oldColor));
    QRect borderBounds = searchBounds.intersected(maxBounds);
    searchBounds = searchBounds.adjusted(-1, -1, 1, 1).intersected(maxBounds);

    // Preparations
    QList<QPoint> queue; // queue all the pixels of the filled area (as they are found)

    QPoint tempPoint;
    QScopedPointer< QHash<QRgb, bool> > cache(new QHash<QRgb, bool>());

    int xTemp = 0;
    bool spanLeft = false;
    bool spanRight = false;

    if (!searchBounds.contains(point))
    {
        // If point is outside the search area, move it anywhere in the 1px transparent border
        point = searchBounds.topLeft();
    }

    queue.append(point);
    // Preparations END

    bool *filledPixels = new bool[maxBounds.height()*maxBounds.width()]{};

    // True if the algorithm has attempted to fill a pixel outside the search bounds
    bool checkOutside = false;

    BlitRect blitBounds(point);
    while (!queue.empty())
    {
        tempPoint = queue.takeFirst();

        point.setX(tempPoint.x());
        point.setY(tempPoint.y());

        xTemp = point.x();

        int xCoord = xTemp - maxBounds.left();
        int yCoord = point.y() - maxBounds.top();

        // In case we fill outside the searchBounds, expand the search area to the max.
        if (!borderBounds.contains(point)) {
            checkOutside = true;
        }

        if (filledPixels[yCoord*maxBounds.width()+xCoord]) continue;

        while (xTemp >= searchBounds.left() &&
               compareColor(targetImage->constScanLine(xTemp, point.y()), oldColor, tolerance, cache.data())) xTemp--;
        xTemp++;

        spanLeft = spanRight = false;
        while (xTemp <= searchBounds.right() &&
               compareColor(targetImage->constScanLine(xTemp, point.y()), oldColor, tolerance, cache.data()))
        {

            QPoint floodPoint = QPoint(xTemp, point.y());
            if (!blitBounds.contains(floodPoint)) {
                blitBounds.extend(floodPoint);
            }

            xCoord = xTemp - maxBounds.left();
            // This pixel is what we're going to fill later
            filledPixels[yCoord*maxBounds.width()+xCoord] = true;

            if (!spanLeft && (point.y() > searchBounds.top()) &&
                compareColor(targetImage->constScanLine(xTemp, point.y() - 1), oldColor, tolerance, cache.data())) {
                queue.append(QPoint(xTemp, point.y() - 1));
                spanLeft = true;
            }
            else if (spanLeft && (point.y() > searchBounds.top()) &&
                     !compareColor(targetImage->constScanLine(xTemp, point.y() - 1), oldColor, tolerance, cache.data())) {
                spanLeft = false;
            }

            if (!spanRight && point.y() < searchBounds.bottom() &&
                compareColor(targetImage->constScanLine(xTemp, point.y() + 1), oldColor, tolerance, cache.data())) {
                queue.append(QPoint(xTemp, point.y() + 1));
                spanRight = true;
            }
            else if (spanRight && point.y() < searchBounds.bottom() &&
                     !compareColor(targetImage->constScanLine(xTemp, point.y() + 1), oldColor, tolerance, cache.data())) {
                spanRight = false;
            }

            Q_ASSERT(queue.count() < (maxBounds.width() * maxBounds.height()));
            xTemp++;
        }
    }

    fillBorder = checkOutside && compareColor(qRgba(0,0,0,0), oldColor, tolerance, cache.data());

    newBounds = blitBounds;

    return filledPixels;
}

/** Finds all pixels closest to the input color and applies the input color to the image
 *
 * An example:
 *
 * 0 is where the color was found
 * 1 is the distance from the nearest pixel of that color
 *
 * 211112
 * 100001
 * 100001
 * 211112
 *
 * @param bitmapImage: Image to search
 * @param searchColor: Color to find
 */
void BitmapImage::expandFill(bool* fillPixels, const QRect& searchBounds, const QRect& maxBounds, int expand) {

    const int maxWidth = maxBounds.width();
    const int length = maxBounds.height() * maxBounds.width();

    int* manhattanPoints = new int[length]{};

    // Fill points with max length, this is important because otherwise the filled pixels will include a border of the expanded area
    std::fill_n(manhattanPoints, length, searchBounds.width()+searchBounds.height());

    for (int y = searchBounds.top(); y <= searchBounds.bottom(); y++)
    {
        for (int x = searchBounds.left(); x <= searchBounds.right(); x++)
        {
            const int index = y*maxWidth+x;

            if (fillPixels[index]) {
                manhattanPoints[index] = 0;
                continue;
            }

            if (y > searchBounds.top()) {
                // the value will be the num of pixels away from y - 1 of the next position
                manhattanPoints[index] = qMin(manhattanPoints[index],
                                             manhattanPoints[(y - 1) * maxWidth+x] + 1);

                int distance = manhattanPoints[index];
                if (distance <= expand) {
                    fillPixels[index] = true;
                }
            }
            if (x > searchBounds.left()) {
                // the value will be the num of pixels away from x - 1 of the next position
                manhattanPoints[index] = qMin(manhattanPoints[index],
                                             manhattanPoints[y*maxWidth+(x - 1)] + 1);

                int distance = manhattanPoints[index];
                if (distance <= expand) {
                    fillPixels[index] = true;
                }
            }
        }
    }

    // traverse from bottom right to top left
    for (int y = searchBounds.bottom(); y >= searchBounds.top(); y--)
    {
        for (int x = searchBounds.right(); x >= searchBounds.left(); x--)
        {
            const int index = y*maxWidth+x;

            if (y + 1 < searchBounds.bottom()) {
                manhattanPoints[index] = qMin(manhattanPoints[index], manhattanPoints[(y + 1)*maxWidth+x] + 1);

                int distance = manhattanPoints[index];
                if (distance <= expand) {
                    fillPixels[index] = true;
                }
            }
            if (x + 1 < searchBounds.right()) {
                manhattanPoints[index] = qMin(manhattanPoints[index], manhattanPoints[y*maxWidth+(x + 1)] + 1);

                int distance = manhattanPoints[index];
                if (distance <= expand) {
                    fillPixels[index] = true;
                }
            }
        }
    }

    delete[] manhattanPoints;
}

