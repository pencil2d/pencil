#ifndef OVERLAYPAINTER_H
#define OVERLAYPAINTER_H

#include <QPainter>
#include <QPalette>

struct OverlayPainterOptions
{
    int   nFrameIndex = 1;
    bool  bCenter = false;
    bool  bThirds = false;
    bool  bGoldenRatio = false;
    bool  bActionSafe = true;
    int   nActionSafe = 5;
    bool  bSafeArea = false;
    bool  bTitleSafe = true;
    int   nTitleSafe = 10;
    bool  bPerspective1 = false;
    bool  bPerspective2 = false;
    bool  bPerspective3 = false;
    int   nOverlayAngle = 15; // for perspective overlays
    bool  bShowSafeAreaHelperText = true;
    bool  mIsCamera = false;
    QPointF mSinglePerspPoint;
    QPointF mLeftPerspPoint;
    QPointF mRightPerspPoint;
    QPointF mMiddlePerspPoint;

    QPainter::CompositionMode cmBufferBlendMode = QPainter::CompositionMode_SourceOver;
};

class LayerCamera;
class Layer;

class OverlayPainter
{
public:
    explicit OverlayPainter();

    void setViewTransform(const QTransform view);
    void setOptions(const OverlayPainterOptions& p) { mOptions = p; }

    void initializePainter(QPainter& painter);
    void preparePainter(Layer* cameraLayer, QPalette palette);

    void renderOverlays(QPainter& painter);

    void paintOverlayCenter(QPainter& painter, QTransform& camTransform, QRect& camRect) const;
    void paintOverlayThirds(QPainter& painter, QTransform& camTransform, QRect& camRect) const;
    void paintOverlayGolden(QPainter& painter, QTransform& camTransform, QRect& camRect) const;
    void paintOverlaySafeAreas(QPainter& painter, QTransform& camTransform, QRect& camRect) const;
    void paintOverlayPerspectiveOnePoint(QPainter& painter, QTransform& camTransform, QRect& camRect) const;
    void paintOverlayPerspectiveTwoPoints(QPainter& painter, QTransform& camTransform, QRect& camRect) const;
    void paintOverlayPerspectiveThreePoints(QPainter& painter, QTransform& camTransform, QRect& camRect) const;

private:
    OverlayPainterOptions mOptions;

    QTransform mViewTransform;

    LayerCamera* mCameraLayer = nullptr;
    QPalette mPalette;

    Q_CONSTEXPR static qreal LINELENGTHFACTOR = 2.0;
    Q_CONSTEXPR static int LEFTANGLEOFFSET = 90;
    Q_CONSTEXPR static int RIGHTANGLEOFFSET = -90;
    Q_CONSTEXPR static int HANDLE_WIDTH = 12;
};

#endif // OVERLAYPAINTER_H
