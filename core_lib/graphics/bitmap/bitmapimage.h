/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2012-2015 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation;

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/
#ifndef BITMAP_IMAGE_H
#define BITMAP_IMAGE_H

#include <memory>
#include <QtXml>
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

    void paintImage( QPainter& painter );

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
    void clear();
    void clear( QRect rectangle );
    void clear( QRectF rectangle ) { clear( rectangle.toRect() ); }

    static int sqr( int );
    static int rgbDistance( QRgb rgba1, QRgb rgba2 );
    static void floodFill( BitmapImage* targetImage, BitmapImage* fillImage, QPoint point, QRgb targetColour, QRgb replacementColour, int tolerance, bool extendFillImage );

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
