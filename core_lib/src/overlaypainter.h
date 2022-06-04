#ifndef OVERLAYPAINTER_H
#define OVERLAYPAINTER_H

#include <QCoreApplication>
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
    bool  bShowHandle = false;
    bool  bGrid = false;
    int   nGridSizeW = 50; /* This is the grid Width IN PIXELS. The grid will scale with the image, though */
    int   nGridSizeH = 50; /* This is the grid Height IN PIXELS. The grid will scale with the image, though */
    QPointF mSinglePerspPoint;
    QPointF mLeftPerspPoint;
    QPointF mRightPerspPoint;
    QPointF mMiddlePerspPoint;

    QPainter::CompositionMode cmBufferBlendMode = QPainter::CompositionMode_SourceOver;
};

class LayerCamera;
class Camera;
class Layer;

class OverlayPainter
{
    Q_DECLARE_TR_FUNCTIONS(OverlayPainter)
public:
    explicit OverlayPainter();

    void setViewTransform(const QTransform view);
    void setOptions(const OverlayPainterOptions& p) { mOptions = p; }

    void initializePainter(QPainter& painter);
    void preparePainter(Layer* cameraLayer, QPalette palette);

    void paint(QPainter& painter);
private:

    void paintGrid(QPainter& painter) const;
    void paintOverlayCenter(QPainter& painter, QTransform& camTransform, QRect& camRect) const;
    void paintOverlayThirds(QPainter& painter, QTransform& camTransform, QRect& camRect) const;
    void paintOverlayGolden(QPainter& painter, QTransform& camTransform, QRect& camRect) const;
    void paintOverlaySafeAreas(QPainter& painter, QTransform& camTransform, QRect& camRect, Camera* camera) const;
    void paintOverlayPerspectiveOnePoint(QPainter& painter, QTransform& camTransform, QRect& camRect) const;
    void paintOverlayPerspectiveTwoPoints(QPainter& painter, const Camera* camera, QTransform& camTransform, QRect& camRect) const;
    void paintOverlayPerspectiveThreePoints(QPainter& painter, const Camera* camera, QTransform& camTransform, QRect& camRect) const;

    int round100(double f, int gridSize) const;

    OverlayPainterOptions mOptions;

    QTransform mViewTransform;

    LayerCamera* mCameraLayer = nullptr;
    QPalette mPalette;
};

#endif // OVERLAYPAINTER_H
