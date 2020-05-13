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
    bool  bShowSafeAreaHelperText = true;
    bool  mIsCamera = false;
    QRect mRect = QRect();   // camera rect!
    QPointF mSinglePerspPoint = QPointF(mRect.right() - mRect.width() / 2, mRect.bottom() - mRect.height() / 2);
    QPointF mLeftPerspPoint = QPointF(mRect.left(), mRect.top() + mRect.height() / 2);
    QPointF mRightPerspPoint = QPointF(mRect.right(), mRect.top() + mRect.height() / 2);
    QPointF mMiddlePerspPoint = QPointF(0, 20);

    QPainter::CompositionMode cmBufferBlendMode = QPainter::CompositionMode_SourceOver;
};

class OverlayPainter : public QObject
{
    Q_OBJECT
public:
    explicit OverlayPainter(QObject *parent = nullptr);

    void setViewTransform(const QTransform view, const QTransform viewInverse);
    void setOptions(const OverlayPainterOptions& p) { mOptions = p; }
    OverlayPainterOptions getOptions() { return mOptions; }

    void initPerspectivePainter(QPainter& painter);

    void renderOverlays(QPainter& painter);

    void paintOverlayCenter(QPainter& painter);
    void paintOverlayThirds(QPainter& painter);
    void paintOverlayGolden(QPainter& painter);
    void paintOverlaySafeAreas(QPainter& painter);
    void paintOverlayPerspective1(QPainter& painter);
    void paintOverlayPerspective2(QPainter& painter);
    void paintOverlayPerspective3(QPainter& painter);

    void setCameraRect(QRect rect) { mOptions.mRect = rect; }
    void setIsCamera(bool isCamera) { mOptions.mIsCamera = isCamera; }
    void setSinglePoint(QPoint point) { mOptions.mSinglePerspPoint = point; }
    void setLeftPoint(QPoint point) { mOptions.mLeftPerspPoint = point; }
    void setRightPoint(QPoint point) { mOptions.mRightPerspPoint = point; }
    void setMiddlePoint(QPoint point) { mOptions.mMiddlePerspPoint = point; }

private:

    void resetPerspectives();

    OverlayPainterOptions mOptions;

    QTransform mViewTransform;
    QTransform mViewInverse;


    constexpr static int OVERLAY_SAFE_CENTER_CROSS_SIZE = 25;
};

#endif // OVERLAYPAINTER_H
