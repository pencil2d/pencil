#ifndef OVERLAYPAINTER_H
#define OVERLAYPAINTER_H


#include <QCoreApplication>
#include <QPainter>
#include "movemode.h"

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
    QRect mRect;   // camera rect!
    QPointF mSinglePerspPoint = QPointF(0.0, 0.0);
    QPointF mLeftPerspPoint = QPointF(mRect.left(), 0.0);// = QPointF(-300, 0);
    QPointF mRightPerspPoint = QPointF(mRect.right(), 0.0);// = QPointF(300, 0);
    QPointF mMiddlePerspPoint = QPointF(0.0, mRect.bottom());// = QPointF(0, 200);

    QPainter::CompositionMode cmBufferBlendMode = QPainter::CompositionMode_SourceOver;
};

class OverlayPainter
{
    Q_DECLARE_TR_FUNCTIONS(OverlayPainter)
public:
    explicit OverlayPainter();

    void setViewTransform(const QTransform view);
    void setOptions(const OverlayPainterOptions& p) { mOptions = p; }
    OverlayPainterOptions getOptions() { return mOptions; }

    void renderOverlays(QPainter& painter, MoveMode mode);

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
    QPoint getSinglePoint() const { return mOptions.mSinglePerspPoint.toPoint(); }
    void setLeftPoint(QPoint point) { mOptions.mLeftPerspPoint = point; }
    QPoint getLeftPoint() const { return mOptions.mLeftPerspPoint.toPoint(); }
    void setRightPoint(QPoint point) { mOptions.mRightPerspPoint = point; }
    QPoint getRightPoint() const { return mOptions.mRightPerspPoint.toPoint(); }
    void setMiddlePoint(QPoint point) { mOptions.mMiddlePerspPoint = point; }
    QPoint getMiddlePoint() const { return mOptions.mMiddlePerspPoint.toPoint(); }
    void setMoveMode(MoveMode mode) { mMoveMode = mode; }
    MoveMode getMoveMode() const { return mMoveMode; }

private:
    OverlayPainterOptions mOptions;
    MoveMode mMoveMode = MoveMode::NONE;

    QTransform mViewTransform;
};

#endif // OVERLAYPAINTER_H
