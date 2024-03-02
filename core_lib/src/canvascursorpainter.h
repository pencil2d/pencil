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

#include <QPen>

class QPainter;

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

    void preparePainter(const CanvasCursorPainterOptions& painterOptions, const QTransform& viewTransform);

    const QRect dirtyRect() { return mDirtyRect; }
    bool isDirty() const { return mIsDirty; }
    void clearDirty();

private:

    void setupPen();

    /// @brief precision circular cursor: used for drawing a cursor on the canvas.
    void paintWidthCursor(QPainter& painter, const QRect& blitRect, const QRectF& widthCircleBounds);
    void paintFeatherCursor(QPainter& painter, const QRect& blitRect, const QRectF& widthCircleBounds, const QRectF& featherCircleBounds);

    CanvasCursorPainterOptions mOptions;
    QRect mDirtyRect;
    bool mIsDirty = false;

    QPen mCursorPen;
};

#endif // CANVASCURSORPAINTER_H
