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
#include "canvascursorpainter.h"

#include <QPainter>
#include <QtMath>

CanvasCursorPainter::CanvasCursorPainter()
{
    setupPen();
}

void CanvasCursorPainter::setupPen()
{
    mCursorPen = QPen(Qt::gray);
    mCursorPen.setWidthF(1);
    mCursorPen.setCosmetic(true);
}

void CanvasCursorPainter::paint(QPainter& painter, const QRect& blitRect)
{
    if (mOptions.showCursor) {
        paintWidthCursor(painter, blitRect, mOptions.circleRect);
        mIsDirty = true;
    }
}

void CanvasCursorPainter::preparePainter(const CanvasCursorPainterOptions& painterOptions)
{
    mOptions = painterOptions;
}

void CanvasCursorPainter::paintWidthCursor(QPainter& painter, const QRect& blitRect, const QRectF& widthCircleBounds)
{
    painter.save();

    painter.setClipRect(painter.transform().inverted().mapRect(blitRect));
    painter.setPen(mCursorPen);

    painter.setCompositionMode(QPainter::RasterOp_SourceXorDestination);

    // Only draw the cross when the width is bigger than the cross itself
    if (widthCircleBounds.width() > 8 && mOptions.showCross) {
        painter.save();

        const QPointF& pos = painter.transform().mapRect(widthCircleBounds).center();
        painter.resetTransform();
        painter.drawLine(QPointF(pos.x() - 2, pos.y()), QPointF(pos.x() + 2, pos.y()));
        painter.drawLine(QPointF(pos.x(), pos.y() - 2), QPointF(pos.x(), pos.y() + 2));
        painter.restore();
    }

    painter.drawEllipse(widthCircleBounds);
    painter.restore();

    mDirtyRect = widthCircleBounds.toAlignedRect();
}


void CanvasCursorPainter::clearDirty()
{
    mDirtyRect = QRect();
    mIsDirty = false;
}
