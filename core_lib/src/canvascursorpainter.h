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
#ifndef CANVASCURSORPAINTER_H
#define CANVASCURSORPAINTER_H

#include <QPainter>
#include <QPen>

struct CanvasCursorPainterOptions
{
    QRectF widthRect;
    QRectF featherRect;
    bool isAdjusting;
    bool useFeather = false;
    bool showCursor = false;
};

class CanvasCursorPainter
{

public:
    CanvasCursorPainter();
    void paint(QPainter& painter, const QRect& blitRect);

    void preparePainter(CanvasCursorPainterOptions& painterOptions, QTransform viewTransform);

    const QRectF dirtyRect() { return mDirtyRect; }
private:

    void setupPen();
    void mapToView(QRectF& widthRect, QRectF& featherRect);

    /// @brief precision circular cursor: used for drawing a cursor on the canvas.
    void paintWidthCursor(QPainter& painter, const QRect& blitRect, const QRectF& widthCircleBounds);
    void paintFeatherCursor(QPainter& painter, const QRect& blitRect, const QRectF& featherCircleBounds, const QRectF& widthCircleBounds);

    CanvasCursorPainterOptions mOptions;
    QRectF mDirtyRect;
    QTransform mViewTransform;

    QPen mCursorPen;
};

#endif // CANVASCURSORPAINTER_H
