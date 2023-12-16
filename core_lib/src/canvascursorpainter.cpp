#include "canvascursorpainter.h"

CanvasCursorPainter::CanvasCursorPainter()
{
}

void CanvasCursorPainter::paint(QPainter& painter, const QRect& blitRect)
{
    if (mOptions.isAdjusting || mOptions.showCursor) {
        paintFeatherCursor(painter, blitRect, mOptions.featherRect);
        paintWidthCursor(painter, blitRect, mOptions.widthRect);
    }
}

void CanvasCursorPainter::preparePainter(CanvasCursorPainterOptions& painterOptions, QTransform viewTransform)
{
    mOptions = painterOptions;
    mViewTransform = viewTransform;
}

void CanvasCursorPainter::paintFeatherCursor(QPainter& painter, const QRect& blitRect, const QRectF& featherRect)
{
    painter.save();

    QPen cursorPen = QPen(Qt::gray);
    painter.setClipRect(blitRect);
    cursorPen.setWidthF(1);
    cursorPen.setCosmetic(true);

    painter.setPen(cursorPen);
    painter.setCompositionMode(QPainter::RasterOp_SourceXorDestination);
    painter.drawEllipse(mViewTransform.mapRect(featherRect));

    painter.restore();
}

void CanvasCursorPainter::paintWidthCursor(QPainter& painter, const QRect& blitRect, const QRectF& widthRect)
{
    painter.save();

    QPen cursorPen = QPen(Qt::gray);
    painter.setClipRect(blitRect);
    cursorPen.setWidthF(1);
    cursorPen.setCosmetic(true);

    painter.setPen(cursorPen);

    painter.setCompositionMode(QPainter::RasterOp_SourceXorDestination);

    const QRectF& circleBounds = mViewTransform.mapRect(widthRect);

    // Only draw the cross when the width is bigger than the cross itself
    if (circleBounds.width() > 8) {
        const QPointF& pos = circleBounds.center();
        painter.drawLine(QPointF(pos.x() - 2, pos.y()), QPointF(pos.x() + 2, pos.y()));
        painter.drawLine(QPointF(pos.x(), pos.y() - 2), QPointF(pos.x(), pos.y() + 2));
    }

    painter.drawEllipse(circleBounds);
    painter.restore();

    mDirtyRect = circleBounds;
}
