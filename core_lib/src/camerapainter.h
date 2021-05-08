#ifndef CAMERAPAINTER_H
#define CAMERAPAINTER_H

#include <QRect>
#include <QTransform>
#include <QColor>

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
    void preparePainter(const Object* object, int layerIndex, int frameIndex, QTransform transform, bool isPlaying);
    void resetCache();

private:

    void initializePainter(QPainter& painter, QPixmap& pixmap) const;
    void paintCameraBorder(QPainter& painter) const;
    void paintCameraPath(QPainter& painter, LayerCamera* cameraLayer) const;

    const Object* mObject = nullptr;
    QPixmap* mCanvas = nullptr;

    std::unique_ptr<QPixmap> mCachedPaint = nullptr;
    QTransform mViewTransform;
    QTransform mViewInverse;

    QRect mCameraRect;
    qreal mCamRotation = 0.;
    QTransform mCamTransform = QTransform();

    const int DOT_WIDTH = 4;
    const int HANDLE_WIDTH = 8;

    int mFrameIndex = 0;
    int mCurrentLayerIndex = 0;

    bool mIsPlaying = false;
};

#endif // CAMERAPAINTER_H
