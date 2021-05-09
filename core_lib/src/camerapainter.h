#ifndef CAMERAPAINTER_H
#define CAMERAPAINTER_H

#include <QRect>
#include <QTransform>
#include <QColor>
#include <QPalette>

class LayerCamera;
class Object;
class QPixmap;

class CameraPainter
{
public:
    explicit CameraPainter();

    void paint() const;
    void paintCached();

    void setCanvas(QPixmap* canvas);
    void preparePainter(const Object* object, int layerIndex, int frameIndex, QTransform transform, bool isPlaying, const QPalette palette);
    void resetCache();

private:
    void initializePainter(QPainter& painter, QPixmap& pixmap) const;
    void paintCameraBorder(QPainter& painter) const;
    void paintCameraPath(QPainter& painter, LayerCamera* cameraLayer) const;
    void paintCameraHandles(QPainter& painter, QTransform camTransform, QRect cameraRect) const;

    const Object* mObject = nullptr;
    QPixmap* mCanvas = nullptr;

    std::unique_ptr<QPixmap> mCachedPaint = nullptr;
    QTransform mViewTransform;

    const int DOT_WIDTH = 6;
    const int HANDLE_WIDTH = 8;

    int mFrameIndex = 0;
    int mCurrentLayerIndex = 0;

    bool mIsPlaying = false;

    QColor mHighlightColor;
    QColor mHighlightedTextColor;
    QColor mButtonColor;
    QColor mTextColor;
};

#endif // CAMERAPAINTER_H
