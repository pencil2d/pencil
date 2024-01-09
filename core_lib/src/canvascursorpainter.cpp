#include "canvascursorpainter.h"

CanvasCursorPainter::CanvasCursorPainter()
{
    setupPen();
}

void CanvasCursorPainter::setupPen()
{
    mCursorPen = Qt::gray;
    mCursorPen.setWidthF(1);
    mCursorPen.setCosmetic(true);
}

void CanvasCursorPainter::paint(QPainter& painter, const QRect& blitRect)
{
    if (mOptions.isAdjusting || mOptions.showCursor) {
        QRectF& widthRect = mOptions.widthRect;
        QRectF& featherRect = mOptions.featherRect;
        mapToView(widthRect, featherRect);

        if (mOptions.useFeather) {
            paintFeatherCursor(painter, blitRect, featherRect, widthRect);
        }
        paintWidthCursor(painter, blitRect, widthRect);
    }
}

void CanvasCursorPainter::mapToView(QRectF& widthRect, QRectF& featherRect)
{
    widthRect = mViewTransform.mapRect(widthRect);
    featherRect = mViewTransform.mapRect(featherRect);
}

void CanvasCursorPainter::preparePainter(CanvasCursorPainterOptions& painterOptions, QTransform viewTransform)
{
    mOptions = painterOptions;
    mViewTransform = viewTransform;
}

void CanvasCursorPainter::paintFeatherCursor(QPainter& painter, const QRect& blitRect, const QRectF& featherCircleBounds, const QRectF& widthCircleBounds)
{
    // When the circles are too close to each other, the rendering will appear dotted or almost
    // invisible at certain zoom levels.
    if (widthCircleBounds.width() - featherCircleBounds.width() <= 1) {
        return;
    }

    painter.save();

    painter.setClipRect(blitRect);
    painter.setPen(mCursorPen);
    painter.setCompositionMode(QPainter::RasterOp_SourceXorDestination);
    painter.drawEllipse(featherCircleBounds);

    painter.restore();
}

void CanvasCursorPainter::paintWidthCursor(QPainter& painter, const QRect& blitRect, const QRectF& widthCircleBounds)
{
    painter.save();

    painter.setClipRect(blitRect);
    painter.setPen(mCursorPen);

    painter.setCompositionMode(QPainter::RasterOp_SourceXorDestination);

    // Only draw the cross when the width is bigger than the cross itself
    if (widthCircleBounds.width() > 8) {
        const QPointF& pos = widthCircleBounds.center();
        painter.drawLine(QPointF(pos.x() - 2, pos.y()), QPointF(pos.x() + 2, pos.y()));
        painter.drawLine(QPointF(pos.x(), pos.y() - 2), QPointF(pos.x(), pos.y() + 2));
    }

    painter.drawEllipse(widthCircleBounds);
    painter.restore();

    mDirtyRect = widthCircleBounds;
}
