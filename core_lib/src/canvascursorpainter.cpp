#include "canvascursorpainter.h"

#include <QDebug>

#include <QRadialGradient>

CanvasCursorPainter::CanvasCursorPainter()
{
}

void CanvasCursorPainter::paint(QPainter& painter, const QRect& blitRect)
{
    if (mOptions.isAdjusting || mOptions.showCursor) {
        paintFeatherCursor(painter, blitRect, mOptions.featherRect);
        paintWidthCursor(
                    painter,
                    blitRect,
                    mOptions.widthRect
                    );
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

//    qreal maxWidth = width - 2;
//    qreal factor = (((feather - 2.0) / (100 - 2.0)) * 1.0);
//    qreal whB = qMax(0.0, maxWidth * factor);

//    qreal circleLeft = posX-(whB*0.5);
//    qreal circleTop = posY-(whB*0.5);
//    const QRectF& circleBounds = mViewTransform.mapRect(QRectF(circleLeft, circleTop, feather, feather));

    painter.drawEllipse(mViewTransform.mapRect(featherRect));
    if (mOptions.isAdjusting) {

//        qreal opacity = pressure;
//        qreal offset = feather;
//        QColor color = Qt::black;
//        int r = color.red();
//        int g = color.green();
//        int b = color.blue();
//        qreal a = color.alphaF();

//        int mainColorAlpha = qRound(a * 255 * opacity);

//        // the more feather (offset), the more softness (opacity)
//        int alphaAdded = qRound((mainColorAlpha * offset) / 100);

////        qDebug() << "main: " << mainColorAlpha;
////        qDebug() << "alpha: " << alphaAdded;

//        QRadialGradient gradient = QRadialGradient(QPointF(whB*0.5,whB*0.5), whB);
//        gradient.setColorAt(0.0, QColor(r, g, b, mainColorAlpha - alphaAdded));
//        gradient.setColorAt(1.0, QColor(r, g, b, 0));
//        gradient.setColorAt(1.0 - (offset / 100.0), QColor(r, g, b, mainColorAlpha - alphaAdded));

//        painter.setPen(Qt::NoPen);
//        painter.setBrush(QBrush(gradient));

//        painter.drawEllipse(posX, posX, width, width);

//        qreal offset = feather;
//        qreal opacity = factor;

//        QColor color = Qt::black;
//        int r = color.red();
//        int g = color.green();
//        int b = color.blue();
//        qreal a = color.alpha();

//        int mainColorAlpha = qRound(a * opacity);
//        // the more feather (offset), the more softness (opacity)
//        int alphaAdded = qRound((mainColorAlpha * offset) / 100);
//        gradient.setColorAt(0.0, QColor(r, g, b, mainColorAlpha - alphaAdded));
//        gradient.setColorAt(1.0, QColor(r, g, b, 0));
//        gradient.setColorAt(1.0 - (offset / 100.0), QColor(r, g, b, mainColorAlpha - alphaAdded));

//        painter.setCompositionMode(QPainter::CompositionMode_SourceOver);

    } else {
//        QPen cursorPen = QPen(Qt::gray);
//        painter.setClipRect(blitRect);
//        cursorPen.setWidthF(1);
//        cursorPen.setCosmetic(true);

//        painter.setPen(cursorPen);
//        painter.setCompositionMode(QPainter::RasterOp_SourceXorDestination);
    }

    // Decrement by two pixels to avoid overlap

//    qDebug() << "maxWidth: " << maxWidth;
//    qDebug() << "feather: " << feather;
//    qDebug() << "feather: " << feather;
//    qDebug() << "feather factor: " << feather * factor;
//    qDebug() << "whB: " << width - (feather * factor);
//    qDebug() << "factor: " << factor;

//    painter.drawEllipse(circleBounds);
    painter.restore();

//    mDirtyRect = circleBounds;
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

//void CanvasCursorPainter::paintAdjustmentSlider(QPainter& painter, const QRect& blitRect, float value, float min, float max)
//{

//}

// */
//void CanvasCursorPainter::paintQuickSizeCursor(QPainter& painter, const QRect& blitRect, qreal posX, qreal posY, qreal width, qreal scalingFac)
//{
//    painter.restore();
//    qreal propSize = qMax(0., width) * scalingFac;
////    qreal propFeather = qMax(0., properties.feather) * scalingFac;

////    qreal cursorWidth = (propWidth + 0.5);
////    qreal whA = qMax<float>(0, propWidth - 1);

//    QRectF cursorRect(posX-(propSize*0.5), posY-(propSize*0.5), propSize, propSize);

////    QRectF sizeRect = cursorRect.adjusted(1, 1, -1, -1);
////    qreal featherRadius = (1 - propFeather / 100) * propSize / 2.;

////        cursorPixmap.fill(QColor(255, 255, 255, 0));
////        QPainter cursorPainter(&cursorPixmap);
//    painter.setCompositionMode(QPainter::RasterOp_NotSourceAndNotDestination);

//    QPen cursorPen = QPen(Qt::black);
//    cursorPen.setWidthF(1);
//    cursorPen.setCosmetic(true);
//    cursorPen.setColor(QColor(127, 127, 127, 127));
//    // Draw width (outside circle)
//    painter.setPen(cursorPen);
////    painter.setBrush(QColor(0, 255, 127, 255));
//    painter.drawEllipse(cursorRect);

////        // Draw feather (inside circle)
////        painter.setCompositionMode(QPainter::RasterOp_NotSourceAndNotDestination);
////        painter.setPen(QColor(0, 0, 0, 0));
////        painter.setBrush(QColor(0, 191, 95, 127));
////        painter.drawEllipse(cursorRect.center(), featherRadius, featherRadius);

//    // Draw cursor in center
//    painter.setPen(QColor(0, 0, 0, 255));
//    painter.drawLine(cursorRect.center() - QPoint(2, 0), cursorRect.center() + QPoint(2, 0));
//    painter.drawLine(cursorRect.center() - QPoint(0, 2), cursorRect.center() + QPoint(0, 2));
//    painter.save();

//    mDirtyRect = cursorRect.adjusted(-2, -2, 2, 2);
//}
