/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2012-2017 Matthew Chiawen Chang

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
#include "util.h"

BitmapImage::BitmapImage()
{
    mImage = std::make_shared< QImage >(); // null image
    mBounds = QRect( 0, 0, 0, 0 );
}

BitmapImage::BitmapImage( const BitmapImage& a ) : KeyFrame(a)
{
    mBounds = a.mBounds;
    mImage = std::make_shared< QImage >( *a.mImage );
}

BitmapImage::BitmapImage( const QRect& rectangle, const QColor& colour)
{
    mBounds = rectangle;
    mImage = std::make_shared< QImage >( mBounds.size(), QImage::Format_ARGB32_Premultiplied);
    mImage->fill(colour.rgba());
}

BitmapImage::BitmapImage( const QRect& rectangle, const QImage& image )
{
    mBounds = rectangle.normalized();
    mExtendable = true;
    mImage = std::make_shared< QImage >(image);
    if ( mImage->width() != rectangle.width() || mImage->height() != rectangle.height())
    {
        qDebug() << "Error instancing bitmapImage.";
    }
}

BitmapImage::BitmapImage( const QString& path, const QPoint& topLeft )
{
    mImage = std::make_shared< QImage >(path);
    if ( mImage->isNull() )
    {
        qDebug() << "ERROR: Image " << path << " not loaded";
    }
    mBounds = QRect( topLeft, mImage->size() );
}

BitmapImage::~BitmapImage()
{
}

void BitmapImage::setImage( QImage* img )
{
    Q_CHECK_PTR( img );
    mImage.reset( img );
}

BitmapImage& BitmapImage::operator=(const BitmapImage& a)
{
    mBounds = a.mBounds;
    mImage = std::make_shared< QImage >( *a.mImage );
    return *this;
}

BitmapImage* BitmapImage::clone()
{
    return new BitmapImage(*this);
}

void BitmapImage::paintImage(QPainter& painter)
{
    painter.drawImage(topLeft(), *mImage);
}

void BitmapImage::paintImage(QPainter& painter, QImage& image,QRect sourceRect, QRect destRect)
{
    painter.drawImage(QRect(QPoint(topLeft()), destRect.size()),
                      image,
                      sourceRect);
}

BitmapImage BitmapImage::copy()
{
    return BitmapImage(mBounds, QImage(*mImage));
}

BitmapImage BitmapImage::copy(QRect rectangle)
{
    //QRect intersection = boundaries.intersected( rectangle );
    QRect intersection2  = rectangle.translated( -topLeft() );
    BitmapImage result = BitmapImage(rectangle, mImage->copy(intersection2));
    return result;
}

void BitmapImage::paste(BitmapImage* bitmapImage)
{
    paste( bitmapImage, QPainter::CompositionMode_SourceOver );
}

void BitmapImage::paste(BitmapImage* bitmapImage, QPainter::CompositionMode cm)
{
    QRect newBoundaries;
    if ( mImage->width() == 0 || mImage->height() == 0 )
    {
        newBoundaries = bitmapImage->mBounds;
    }
    else
    {
        newBoundaries = mBounds.united( bitmapImage->mBounds );
    }
    extend( newBoundaries );

    QImage* image2 = bitmapImage->image();

    QPainter painter( mImage.get() );
    painter.setCompositionMode(cm);
    painter.drawImage( bitmapImage->mBounds.topLeft() - mBounds.topLeft(), *image2);
    painter.end();
}

void BitmapImage::add(BitmapImage* bitmapImage)
{
    QImage* image2 = bitmapImage->image();

    QRect newBoundaries;
    if ( mImage->width() == 0 || mImage->height() == 0 )
    {
        newBoundaries = bitmapImage->mBounds;
    }
    else
    {
        newBoundaries = mBounds.united( bitmapImage->mBounds );
    }
    extend( newBoundaries );
    QPoint offset = bitmapImage->mBounds.topLeft() - mBounds.topLeft();
    for (int y = 0; y < image2->height(); y++)
    {
        for (int x = 0; x < image2->width(); x++)
        {
            QRgb p1  = mImage->pixel(offset.x()+x,offset.y()+y);
            QRgb p2 = image2->pixel(x,y);

            int a1 = qAlpha(p1);
            int a2 = qAlpha(p2);
            int r1 = qRed(p1);
            int r2 = qRed(p2); // remember that the bitmap format is RGB32 Premultiplied
            int g1 = qGreen(p1);
            int g2 = qGreen(p2);
            int b1 = qBlue(p1);
            int b2 = qBlue(p2);

            // unite
            int a = qMax(a1, a2);
            int r = qMax(r1, r2);
            int g = qMax(g1, g2);
            int b = qMax(b1, b2);

            QRgb mix = qRgba(r, g, b, a);
            if (a2 != 0)
            {
                mImage->setPixel(offset.x()+x,offset.y()+y, mix);
            }
        }
    }
}

void BitmapImage::compareAlpha(BitmapImage* bitmapImage) // this function picks the greater alpha value
{
    QImage* image2 = bitmapImage->image();

    QRect newBoundaries;
    if ( mImage->width() == 0 || mImage->height() == 0 )
    {
        newBoundaries = bitmapImage->mBounds;
    }
    else
    {
        newBoundaries = mBounds.united( bitmapImage->mBounds );
    }
    extend( newBoundaries );
    QPoint offset = bitmapImage->mBounds.topLeft() - mBounds.topLeft();
    for (int y = 0; y < image2->height(); y++)
    {
        for (int x = 0; x < image2->width(); x++)
        {
            QRgb p1  = mImage->pixel(offset.x()+x,offset.y()+y);
            QRgb p2 = image2->pixel(x,y);

            int a1 = qAlpha(p1);
            int a2 = qAlpha(p2);

            if (a1 <= a2)
            {
            QRgb mix = qRgba(qRed(p2), qGreen(p2), qBlue(p2), a2);
            mImage->setPixel(offset.x()+x,offset.y()+y, mix);
            }
        }
    }
}

void BitmapImage::moveTopLeft(QPoint point)
{
    mBounds.moveTopLeft(point);
}

void BitmapImage::transform(QRect newBoundaries, bool smoothTransform)
{
    mBounds = newBoundaries;
    newBoundaries.moveTopLeft( QPoint(0,0) );
    QImage* newImage = new QImage( mBounds.size(), QImage::Format_ARGB32_Premultiplied);
    //newImage->fill(QColor(255,255,255).rgb());
    QPainter painter(newImage);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, smoothTransform);
    painter.setCompositionMode(QPainter::CompositionMode_Source);
    painter.fillRect( newImage->rect(), QColor(0,0,0,0) );
    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
    painter.drawImage(newBoundaries, *mImage );
    painter.end();
    mImage.reset( newImage );
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
    return BitmapImage(transform.mapRect(selection), transformedImage);
}

BitmapImage BitmapImage::transformed(QRect newBoundaries, bool smoothTransform)
{
    BitmapImage transformedImage(newBoundaries, QColor(0,0,0,0));
    QPainter painter(transformedImage.image());
    painter.setRenderHint(QPainter::SmoothPixmapTransform, smoothTransform);
    newBoundaries.moveTopLeft( QPoint(0,0) );
    painter.drawImage(newBoundaries, *mImage );
    painter.end();
    return transformedImage;
}


void BitmapImage::extend(QPoint P)
{
    if (!mBounds.contains(P))
    {
        extend( QRect(P, QSize(1,1)) );
    }
}

void BitmapImage::extend(QRect rectangle)
{
    if (!mExtendable) return;
    if (rectangle.width() <= 0) rectangle.setWidth(1);
    if (rectangle.height() <= 0) rectangle.setHeight(1);
    if (mBounds.contains( rectangle ))
    {
        // nothing
    }
    else
    {
        QRect newBoundaries = mBounds.united(rectangle).normalized();
        QImage* newImage = new QImage( newBoundaries.size(), QImage::Format_ARGB32_Premultiplied);
        newImage->fill(Qt::transparent);
        if (!newImage->isNull())
        {
            QPainter painter(newImage);
            painter.drawImage(mBounds.topLeft() - newBoundaries.topLeft(), *mImage);
            painter.end();
        }
        mImage.reset( newImage );
        mBounds = newBoundaries;
    }
}

QRgb BitmapImage::pixel(int x, int y)
{
    return pixel( QPoint(x,y) );
}

QRgb BitmapImage::pixel(QPoint P)
{
    QRgb result = qRgba(0,0,0,0); // black
    if ( mBounds.contains( P ) ) result = mImage->pixel(P - topLeft());
    return result;
}

void BitmapImage::setPixel(int x, int y, QRgb colour)
{
    setPixel( QPoint(x,y), colour);
}

void BitmapImage::setPixel(QPoint P, QRgb colour)
{
    extend( P );
    if ( mBounds.contains(P) )
        mImage->setPixel(P-topLeft(), colour);
    //drawLine( QPointF(P), QPointF(P), QPen(QColor(colour)), QPainter::CompositionMode_SourceOver, false);
}


void BitmapImage::drawLine( QPointF P1, QPointF P2, QPen pen, QPainter::CompositionMode cm, bool antialiasing)
{
    int width = 2+pen.width();
    extend( QRect(P1.toPoint(), P2.toPoint()).normalized().adjusted(-width,-width,width,width) );
    if (mImage != NULL && !mImage->isNull() )
    {
        QPainter painter( mImage.get() );
        painter.setCompositionMode(cm);
        painter.setRenderHint(QPainter::Antialiasing, antialiasing);
        painter.setPen(pen);
        painter.drawLine( P1-topLeft(), P2-topLeft());
        painter.end();
    }
}

void BitmapImage::drawRect( QRectF rectangle, QPen pen, QBrush brush, QPainter::CompositionMode cm, bool antialiasing)
{
    int width = pen.width();
    extend(rectangle.adjusted(-width, -width, width, width).toRect());
    if (brush.style() == Qt::RadialGradientPattern)
    {
        QRadialGradient* gradient = (QRadialGradient*)brush.gradient();
        gradient->setCenter( gradient->center() - topLeft() );
        gradient->setFocalPoint( gradient->focalPoint() - topLeft() );
    }
    if ( mImage && !mImage->isNull() )
    {
        QPainter painter( mImage.get() );
        painter.setCompositionMode(cm);
        painter.setRenderHint(QPainter::Antialiasing, antialiasing);
        painter.setPen(pen);
        painter.setBrush(brush);
        painter.drawRect( rectangle.translated(-topLeft()) );
        painter.end();
    }
}

void BitmapImage::drawEllipse( QRectF rectangle, QPen pen, QBrush brush, QPainter::CompositionMode cm, bool antialiasing)
{
    int width = pen.width();
    extend( rectangle.adjusted(-width,-width,width,width).toRect() );
    if (brush.style() == Qt::RadialGradientPattern)
    {
        QRadialGradient* gradient = (QRadialGradient*)brush.gradient();
        gradient->setCenter( gradient->center() - topLeft() );
        gradient->setFocalPoint( gradient->focalPoint() - topLeft() );
    }
    if ( mImage && !mImage->isNull() )
    {
        QPainter painter( mImage.get() );

        painter.setRenderHint(QPainter::Antialiasing, antialiasing);
        painter.setPen(pen);
        painter.setBrush(brush);
        //if (brush == Qt::NoBrush)

        painter.setCompositionMode(cm);
        painter.drawEllipse( rectangle.translated(-topLeft()) );

        painter.end();
    }
}

void BitmapImage::drawPath( QPainterPath path, QPen pen, QBrush brush,
                            QPainter::CompositionMode cm, bool antialiasing)
{
    int width = pen.width();
    qreal inc = 1.0 + width / 20.0; // qreal?
    //if (inc<1) { inc=1.0; }
    extend( path.controlPointRect().adjusted(-width,-width,width,width).toRect() );

    if ( mImage != NULL && !mImage->isNull() )
    {
        QPainter painter( mImage.get() );
        painter.setCompositionMode(cm);
        painter.setRenderHint( QPainter::Antialiasing, antialiasing );
        painter.setPen(pen);
        painter.setBrush(brush);
        painter.setTransform(QTransform().translate(-topLeft().x(), -topLeft().y()));
        painter.setMatrixEnabled(true);
        if (path.length() > 0)
        {
            for ( int pt = 0; pt < path.elementCount() - 1; pt++ )
            {
                qreal dx = path.elementAt(pt+1).x - path.elementAt(pt).x;
                qreal dy = path.elementAt(pt+1).y - path.elementAt(pt).y;
                qreal m = sqrt(dx*dx+dy*dy);
                qreal factorx = dx / m;
                qreal factory = dy / m;
                for ( float h = 0.f; h < m; h += inc )
                {
                    qreal x = path.elementAt(pt).x + factorx * h;
                    qreal y = path.elementAt(pt).y + factory * h;
                    painter.drawPoint( QPointF( x, y ) );
                }
            }

            //painter.drawPath( path );
        }
        else
        {
            // forces drawing when points are coincident (mousedown)
            painter.drawPoint( path.elementAt(0).x, path.elementAt(0).y );
        }
        painter.end();
    }
}

void BitmapImage::clear()
{
    mImage = std::make_shared< QImage >(); // null image
    mBounds = QRect(0,0,0,0);
}

QRgb BitmapImage::constScanLine(int x, int y) {
    QRgb result = qRgba( 0, 0, 0, 0 );
    if ( mBounds.contains( QPoint( x, y ) ) ) {
        result = *( reinterpret_cast< const QRgb* >( mImage->constScanLine( y - topLeft().y() ) ) + x - topLeft().x() );
    }

    return result;
}

void BitmapImage::scanLine(int x, int y, QRgb colour)
{
    extend( QPoint( x, y ) );
    if( mBounds.contains( QPoint( x, y ) ) ) {

        // Make sure color is premultiplied before calling
        *( reinterpret_cast< QRgb* >( mImage->scanLine( y - topLeft().y() ) ) + x - topLeft().x() ) =
                 qRgba(
                       qRed( colour ),
                       qGreen( colour ),
                       qBlue( colour ),
                       qAlpha( colour ) );
    }
}

void BitmapImage::clear(QRect rectangle)
{
    QRect clearRectangle = mBounds.intersected( rectangle );
    clearRectangle.moveTopLeft( clearRectangle.topLeft() - topLeft() );

    QPainter painter( mImage.get() );
    painter.setCompositionMode(QPainter::CompositionMode_Clear);
    painter.fillRect( clearRectangle, QColor(0,0,0,0) );
    painter.end();
}

int BitmapImage::pow(int n)   // pow of a number
{
    return n*n;
}

bool BitmapImage::compareColor(QRgb color1, QRgb color2, int tolerance)
{
    if ( color1 == color2 ) return true;

    int red1 = qRed( color1 );
    int green1 = qGreen( color1 );
    int blue1 = qBlue( color1 );
    int alpha1 = qAlpha( color1 );

    int red2 = qRed( color2 );
    int green2 = qGreen( color2 );
    int blue2 = qBlue( color2 );
    int alpha2 = qAlpha( color2 );

    int diffRed = abs( red2 - red1 );
    int diffGreen = abs( green2 - green1 );
    int diffBlue = abs( blue2 - blue1 );
    int diffAlpha = abs( alpha2 - alpha1 );

    if ( diffRed > tolerance ||
         diffGreen > tolerance ||
         diffBlue > tolerance ||
         diffAlpha > tolerance )
    {
        return false;
    }

    return true;
}

// Flood fill
// ----- http://lodev.org/cgtutor/floodfill.html
void BitmapImage::floodFill(BitmapImage* targetImage, QRect cameraRect, QPoint point, QRgb oldColor, QRgb newColor, int tolerance)
{
    if ( oldColor == newColor ){
        return;
    }

    oldColor = targetImage->pixel( point );
    oldColor = qRgba( qRed( oldColor ), qGreen( oldColor ), qBlue( oldColor ), qAlpha( oldColor ) );

    // Preparations
    QList<QPoint> queue; // queue all the pixels of the filled area (as they are found)

    BitmapImage* replaceImage = nullptr;
    QPoint tempPoint;
    QRgb newPlacedColor;

    int xTemp;
    bool spanLeft, spanRight;

    // Extend to size of Camera
    targetImage->extend( cameraRect );
    replaceImage = new BitmapImage( cameraRect, Qt::transparent );

    queue.append( point );
    // Preparations END

    while( !queue.empty() ) {
        tempPoint = queue.takeFirst();

        point.setX( tempPoint.x() );
        point.setY( tempPoint.y() );

        xTemp = point.x();

        newPlacedColor = replaceImage->constScanLine( xTemp, point.y() );
        while( xTemp >= targetImage->topLeft().x() &&
               compareColor( targetImage->constScanLine( xTemp, point.y() ), oldColor, tolerance) ) xTemp--;
        xTemp++;

        spanLeft = spanRight = false;
        while( xTemp <= targetImage->right() &&
               compareColor( targetImage->constScanLine( xTemp, point.y() ), oldColor, tolerance ) &&
               newPlacedColor != newColor ) {

            // Set pixel color
            replaceImage->scanLine( xTemp, point.y(), newColor );

            if( !spanLeft && (point.y() > targetImage->top() ) &&
                    compareColor( targetImage->constScanLine( xTemp, point.y() - 1 ), oldColor, tolerance ) ) {
                queue.append( QPoint( xTemp, point.y() - 1) );
                spanLeft = true;
            } else if( spanLeft && ( point.y() > targetImage->top() ) &&
                      !compareColor( targetImage->constScanLine( xTemp, point.y() - 1 ), oldColor, tolerance ) ) {
                spanLeft = false;
            }

            if( !spanRight && point.y() < targetImage->bottom() &&
                    compareColor( targetImage->constScanLine( xTemp, point.y() + 1 ), oldColor, tolerance ) ) {
                queue.append( QPoint( xTemp, point.y() + 1 ) );
                spanRight = true;

            } else if( spanRight && point.y() < targetImage->bottom() &&
                      !compareColor( targetImage->constScanLine( xTemp, point.y() + 1 ), oldColor, tolerance ) ) {
                spanRight = false;
            }

            Q_ASSERT( queue.count() < ( targetImage->width() * targetImage->height() ) );
            xTemp++;
        }
    }

    targetImage->paste( replaceImage );
    delete replaceImage;
}
