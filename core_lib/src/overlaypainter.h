#ifndef OVERLAYPAINTER_H
#define OVERLAYPAINTER_H

#include <QObject>
#include <QPainter>
#include <QDebug>

struct OverlayPainterOptions
{
    bool  bIsCamera = false;
    QRect mCameraRect = QRect();
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
    QPointF mCenterPerspPoint = QPointF(0.0, 0.0);
    QPointF mLeftPerspPoint = QPointF(-20.0, 0.0);
    QPointF mRightPerspPoint = QPointF(20.0, 0.0);
    QPointF mBottomPerspPoint = QPointF(0.0, 20.0);
    bool  bShowSafeAreaHelperText = true;

    QPainter::CompositionMode cmBufferBlendMode = QPainter::CompositionMode_SourceOver;
};

class OverlayPainter : public QObject
{
    Q_OBJECT
public:
    explicit OverlayPainter(QObject *parent = nullptr);

    void setViewTransform(const QTransform view, const QTransform viewInverse);
    void setOptions(const OverlayPainterOptions& p) { mOptions = p; }

    void initPerspectivePainter(QPainter& painter);

    void renderOverlays(QPainter& painter);

    void paintOverlayCenter(QPainter& painter);
    void paintOverlayThirds(QPainter& painter);
    void paintOverlayGolden(QPainter& painter);
    void paintOverlaySafeAreas(QPainter& painter);
    void paintOverlayPerspective1(QPainter& painter);
    void paintOverlayPerspective2(QPainter& painter);
    void paintOverlayPerspective3(QPainter& painter);

signals:

private:

    void resetPerspectives();

    OverlayPainterOptions mOptions;

    QTransform mViewTransform;
    QTransform mViewInverse;

    constexpr static int OVERLAY_SAFE_CENTER_CROSS_SIZE = 25;
};

#endif // OVERLAYPAINTER_H
