#ifndef OVERLAYPAINTER_H
#define OVERLAYPAINTER_H

#include <QCoreApplication>
#include <QPainter>
#include <QPalette>

class LayerCamera;
class Camera;

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
    bool  bShowHandle = false;
    bool  bGrid = false;
    int   nGridSizeW = 50; /* This is the grid Width IN PIXELS. The grid will scale with the image, though */
    int   nGridSizeH = 50; /* This is the grid Height IN PIXELS. The grid will scale with the image, though */
    QPointF mSinglePerspPoint;
    QPointF mLeftPerspPoint;
    QPointF mRightPerspPoint;
    QPointF mMiddlePerspPoint;
};

class OverlayPainter
{
    Q_DECLARE_TR_FUNCTIONS(OverlayPainter)
public:
    explicit OverlayPainter();

    void setViewTransform(const QTransform view);
    void setOptions(const OverlayPainterOptions& p) { mOptions = p; }

    void preparePainter(const LayerCamera* cameraLayer, const QPalette& palette);

    void paint(QPainter& painter, const QRect& viewport);
private:
    void initializePainter(QPainter& painter);

    void paintGrid(QPainter& painter) const;
    void paintOverlayCenter(QPainter& painter, const QTransform& camTransform, const QRect& camRect) const;
    void paintOverlayThirds(QPainter& painter, const QTransform& camTransform, const QRect& camRect) const;
    void paintOverlayGolden(QPainter& painter, const QTransform& camTransform, const QRect& camRect) const;
    void paintOverlaySafeAreas(QPainter& painter, const Camera& camera, const QTransform& camTransform, const QRect& camRect) const;
    void paintOverlayPerspectiveOnePoint(QPainter& painter, const QRect& viewport, const QTransform& camTransform) const;
    void paintOverlayPerspectiveTwoPoints(QPainter& painter, const QRect& viewport, const Camera& camera, const QTransform& camTransform) const;
    void paintOverlayPerspectiveThreePoints(QPainter& painter, const QRect& viewport, const Camera& camera, const QTransform& camTransform) const;

    int round100(double f, int gridSize) const;

    OverlayPainterOptions mOptions;

    QTransform mViewTransform;

    const LayerCamera* mCameraLayer = nullptr;
    QPalette mPalette;
};

#endif // OVERLAYPAINTER_H
