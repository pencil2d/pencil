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
#ifndef BITMAP_IMAGE_H
#define BITMAP_IMAGE_H

#include <memory>
#include <QPainter>
#include "keyframe.h"


class BitmapImage : public KeyFrame
{
public:
    BitmapImage();
    BitmapImage( const BitmapImage& );
    BitmapImage( const QRect& boundaries, const QColor& colour );
    BitmapImage( const QRect& boundaries, const QImage& image );
    BitmapImage( const QString& path, const QPoint& topLeft );

    ~BitmapImage();
    BitmapImage& operator=( const BitmapImage& a );

    BitmapImage* clone() override;

    void paintImage( QPainter& painter );
    void paintImage(QPainter &painter, QImage &image, QRect sourceRect, QRect destRect);

    QImage* image() { return mImage.get(); }
    void    setImage( QImage* pImg );

    BitmapImage copy();
    BitmapImage copy( QRect rectangle );
    void paste( BitmapImage* );
    void paste( BitmapImage*, QPainter::CompositionMode cm );

    void add( BitmapImage* );
    void compareAlpha( BitmapImage* );
    void moveTopLeft( QPoint point );
    void moveTopLeft( QPointF point ) { moveTopLeft( point.toPoint() ); }
    void transform( QRect rectangle, bool smoothTransform );
    void transform( QRectF rectangle, bool smoothTransform )  { transform( rectangle.toRect(), smoothTransform ); }
    BitmapImage transformed( QRect selection, QTransform transform, bool smoothTransform );
    BitmapImage transformed( QRect rectangle, bool smoothTransform );
    BitmapImage transformed( QRectF rectangle, bool smoothTransform ) { return transformed( rectangle.toRect(), smoothTransform ); }

    bool contains( QPoint P ) { return mBounds.contains( P ); }
    bool contains( QPointF P ) { return contains( P.toPoint() ); }
    void extend( QPoint P );
    void extend( QRect rectangle );

    QRgb pixel( int x, int y );
    QRgb pixel( QPoint P );
    void setPixel( int x, int y, QRgb colour );
    void setPixel( QPoint P, QRgb colour );
    QRgb constScanLine(int x, int y);
    void scanLine( int x, int y, QRgb colour);
    void clear();
    void clear( QRect rectangle );
    void clear( QRectF rectangle ) { clear( rectangle.toRect() ); }

    static int pow( int );
    static bool compareColor(QRgb color1, QRgb color2, int tolerance);
    static void floodFill(BitmapImage* targetImage, QRect cameraRect, QPoint point, QRgb oldColor, QRgb newColor, int tolerance );

    void drawLine( QPointF P1, QPointF P2, QPen pen, QPainter::CompositionMode cm, bool antialiasing );
    void drawRect( QRectF rectangle, QPen pen, QBrush brush, QPainter::CompositionMode cm, bool antialiasing );
    void drawEllipse( QRectF rectangle, QPen pen, QBrush brush, QPainter::CompositionMode cm, bool antialiasing );
    void drawPath( QPainterPath path, QPen pen, QBrush brush, QPainter::CompositionMode cm, bool antialiasing );

    QPoint topLeft() { return mBounds.topLeft(); }
    QPoint topRight() { return mBounds.topRight(); }
    QPoint bottomLeft() { return mBounds.bottomLeft(); }
    QPoint bottomRight() { return mBounds.bottomRight(); }
    int left() { return mBounds.left(); }
    int right() { return mBounds.right(); }
    int top() { return mBounds.top(); }
    int bottom() { return mBounds.bottom(); }
    int width() { return mBounds.width(); }
    int height() { return mBounds.height(); }

    QRect& bounds() { return mBounds; }

private:
    std::shared_ptr< QImage > mImage;
    QRect   mBounds;
    bool    mExtendable = true;
};

#endif
