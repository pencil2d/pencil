#ifndef OVERLAYPAINTER_H
#define OVERLAYPAINTER_H

#include <QObject>
#include <QPainter>
#include <QDebug>

struct OverlayPainterOptions
{
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
    QPoint mLeftPerspPoint = QPoint(-20, 0);
    QPoint mRightPerspPoint = QPoint(20, 0);
    QPoint mBottomPerspPoint = QPoint(0, 20);
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

    void renderOverlays(QPainter& painter, QRect cameraRect);

    void paintOverlayCenter(QPainter& painter, QRect cameraRect);
    void paintOverlayThirds(QPainter& painter, QRect cameraRect);
    void paintOverlayGolden(QPainter& painter, QRect rect);
    void paintOverlaySafeAreas(QPainter& painter, QRect rect);
    void paintOverlayPerspective1(QPainter& painter, QRect rect, int angle);
    void paintOverlayPerspective2(QPainter& painter, QRect rect, int angle);
    void paintOverlayPerspective3(QPainter& painter, QRect rect, int angle);

signals:

private:
    void resetPerspectives();

    OverlayPainterOptions mOptions;

    QTransform mViewTransform;
    QTransform mViewInverse;

    QList<bool> activeOverlays;
    constexpr static int OVERLAY_SAFE_CENTER_CROSS_SIZE = 25;
};

#endif // OVERLAYPAINTER_H
