#ifndef CAMERAPAINTER_H
#define CAMERAPAINTER_H

#include <QRect>
#include <QTransform>
#include <QColor>
#include <QPalette>

#include "onionskinsubpainter.h"

class LayerCamera;
class Object;
class QPixmap;

class CameraPainter
{
public:
    explicit CameraPainter();

    void paint() const;
    void paintCached();

    void setOnionSkinPaintOptions(const OnionSkinPainterOptions& options) { mOnionSkinOptions = options; }
    void setCanvas(QPixmap* canvas);
    void preparePainter(const Object* object, int layerIndex, int frameIndex, QTransform transform, bool isPlaying, const QPalette palette);
    void resetCache();

private:
    void initializePainter(QPainter& painter, QPixmap& pixmap) const;
    void paintVisuals(QPainter& painter) const;
    void paintBorder(QPainter& painter, const QTransform& camTransform, const QRect& camRect) const;
    void paintInterpolations(QPainter& painter, LayerCamera* cameraLayer) const;
    void paintHandles(QPainter& painter, const QTransform& camTransform, const QRect& cameraRect) const;
    void paintPath(QPainter& painter, const LayerCamera* cameraLayer, const int frameIndex, const QPointF& midPoint) const;

    const Object* mObject = nullptr;
    QPixmap* mCanvas = nullptr;

    std::unique_ptr<QPixmap> mCachedPaint = nullptr;
    QTransform mViewTransform;

    OnionSkinSubPainter mOnionSkinPainter;
    OnionSkinPainterOptions mOnionSkinOptions;

    const int DOT_WIDTH = 6;
    const int HANDLE_WIDTH = 8;

    int mFrameIndex = 0;
    int mCurrentLayerIndex = 0;

    bool mIsPlaying = false;

    QColor mHighlightColor;
    QColor mHighlightedTextColor;
    QColor mTextColor;
};

#endif // CAMERAPAINTER_H
