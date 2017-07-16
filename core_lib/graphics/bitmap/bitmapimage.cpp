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
#include <cmath>
#include "bitmapimage.h"
#include "util.h"

BitmapImage::BitmapImage()
{
    mImage = std::make_shared< QImage >(); // null image
    mBounds = QRect( 0, 0, 0, 0 );
}

BitmapImage::BitmapImage( const BitmapImage& a )
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

void BitmapImage::paintImage(QPainter& painter)
{
    painter.drawImage(topLeft(), *mImage);
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
    BitmapImage selectedPart = copy(selection);

    // Get the transformed image
    //
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
    if (mBounds.contains( P ))
    {
        // nothing
    }
    else
    {
        extend( QRect(P, QSize(0,0)) ); // can we set QSize(0,0) ?
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
        newImage->fill(qRgba(0,0,0,0));
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

// Euclidean Color distance
int BitmapImage::rgbDistance(QRgb rgba1, QRgb rgba2)
{
    int result = pow(qRed(rgba1)-qRed(rgba2))
               + pow(qGreen(rgba1)-qGreen(rgba2))
               + pow(qBlue(rgba1)-qBlue(rgba2))
               + pow(qAlpha(rgba1)-qAlpha(rgba2));
    return sqrt(result);
}

// ----- Queue based flood fill
// ----- http://lodev.org/cgtutor/floodfill.html
void BitmapImage::floodFill(BitmapImage* targetImage, BitmapImage* fillImage, QPoint point, QRgb targetColour, QRgb replacementColour, int tolerance, bool extendFillImage)
{
    QList<QPoint> queue; // queue all the pixels of the filled area (as they are found)
    int j, k;
    bool condition;
    BitmapImage* replaceImage = nullptr;
    if (extendFillImage) {
        replaceImage = new BitmapImage(targetImage->mBounds.united(fillImage->mBounds), QColor(0,0,0,0));
    } else {
        targetImage->extend(fillImage->mBounds); // not necessary - here just to prevent some bug when we draw outside the targetImage - to be fixed
        replaceImage = new BitmapImage(fillImage->mBounds, QColor(0,0,0,0));
        replaceImage->mExtendable = false;
    }
   
    QPen myPen;
    myPen = QPen( QColor(replacementColour) , 1.0, Qt::SolidLine, Qt::RoundCap,Qt::RoundJoin);

    targetColour = targetImage->pixel(point.x(), point.y());
    queue.append( point );

    j = -1;
    k = 1;
    for(int i=0; i< queue.size(); i++ ) {
        point = queue.at(i);

        if( replaceImage->pixel(point.x(), point.y()) != replacementColour
            && rgbDistance(targetImage->pixel(point.x(), point.y()), targetColour) < tolerance ) {

            j = -1;
            condition =  (point.x() + j > targetImage->left());

            if(!extendFillImage) condition = condition && (point.x() + j > replaceImage->left());

            while( replaceImage->pixel(point.x()+j, point.y()) != replacementColour
                   && rgbDistance(targetImage->pixel( point.x()+j, point.y() ), targetColour) < tolerance
                   && condition) {
                j = j - 1;
                condition =  (point.x() + j > targetImage->left());
                if(!extendFillImage) condition = condition && (point.x() + j > replaceImage->left());
            }

            k = 1;
            condition = ( point.x() + k < targetImage->right()-1);
            if(!extendFillImage) condition = condition && (point.x() + k < replaceImage->right()-1);

            while( replaceImage->pixel(point.x()+k, point.y()) != replacementColour
                   && rgbDistance(targetImage->pixel( point.x()+k, point.y() ), targetColour) < tolerance
                   && condition) {
                k = k + 1;
                condition = ( point.x() + k < targetImage->right()-1);
                if(!extendFillImage) condition = condition && (point.x() + k < replaceImage->right()-1);
            }

            for(int x = j+1; x < k; x++) {
                condition = point.y() - 1 > targetImage->top();
                if(!extendFillImage) condition = condition && (point.y() - 1 > replaceImage->top());

                if( condition && queue.size() < targetImage->height() * targetImage->width() ) {
                    if( replaceImage->pixel(point.x()+x, point.y()-1) != replacementColour) {
                        if( rgbDistance(targetImage->pixel( point.x()+x, point.y() - 1), targetColour) < tolerance) {
                            queue.append( point + QPoint(x,-1) );
                        } else {
                            replaceImage->setPixel( point.x()+x, point.y()-1, replacementColour);
                        }
                    }
                }
                condition = point.y() + 1 < targetImage->bottom();
                if (!extendFillImage) condition = condition && (point.y() + 1 < replaceImage->bottom());

                if( condition && queue.size() < targetImage->height() * targetImage->width() ) {
                    if( replaceImage->pixel(point.x()+x, point.y()+1) != replacementColour) {
                        if( rgbDistance(targetImage->pixel( point.x()+x, point.y() + 1), targetColour) < tolerance) {
                            queue.append( point + QPoint(x, 1) );
                        } else {
                            replaceImage->setPixel( point.x()+x, point.y()+1, replacementColour);
                        }
                    }
                }
            }
            replaceImage->drawLine( QPointF(point.x()+j, point.y()),
                                    QPointF(point.x()+k, point.y()),
                                    myPen,
                                    QPainter::CompositionMode_SourceOver,
                                    false);
        }
    }
    fillImage->paste(replaceImage);
    delete replaceImage;
}

