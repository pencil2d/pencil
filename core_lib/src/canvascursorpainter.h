#ifndef CANVASCURSORPAINTER_H
#define CANVASCURSORPAINTER_H

#include <QPainter>

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

    void mapToView(QRectF& widthRect, QRectF& featherRect);

    /// @brief precision circular cursor: used for drawing a cursor on the canvas.
    void paintWidthCursor(QPainter& painter, const QRect& blitRect, const QRectF& widthCircleBounds);
    void paintFeatherCursor(QPainter& painter, const QRect& blitRect, const QRectF& featherCircleBounds, const QRectF& widthCircleBounds);

    CanvasCursorPainterOptions mOptions;
    QRectF mDirtyRect;
    QTransform mViewTransform;
};

#endif // CANVASCURSORPAINTER_H
