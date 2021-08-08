#ifndef OVERLAYMANAGER_H
#define OVERLAYMANAGER_H

#include "pencildef.h"
#include "movemode.h"
#include "basemanager.h"
#include "overlaypainter.h"

#include <QPointF>
#include <QPainter>

class Editor;
class ViewManager;

class OverlayManager : public BaseManager
{
    Q_OBJECT

public:
    explicit OverlayManager(Editor* editor);
    ~OverlayManager() override;

    bool init() override;
    Status load(Object *o) override;
    Status save(Object *o) override;

    void workingLayerChanged(Layer *) override;

    MoveMode getMoveModeForPoint(const QPointF& pos, QTransform transform);
    double selectionTolerance();

    void updatePerspective(const int persp);
    void updatePerspective(const QPointF& point);

    void setCenterEnabled(bool b) { mOverlayCenter = b; }
    void setThirdsEnabled(bool b) { mOverlayThirds = b; }
    void setGoldenRatioEnabled(bool b) { mOverlayGoldenRatio = b; }
    void setSafeAreasEnabled(bool b) { mOverlaySafeAreas = b; }
    void setOnePointPerspectiveEnabled(bool b);
    void setTwoPointPerspectiveEnabled(bool b);
    void setThreePointPerspectiveEnabled(bool b);

    MoveMode getMoveMode() const { return mMoveMode; }
    void setMoveMode(MoveMode mode) { mMoveMode = mode; }
    void setSinglePerspPoint(QPointF point) { mSinglePerspPoint = point; }
    QPointF getSinglePerspPoint() const { return mSinglePerspPoint; }
    void setLeftPerspPoint(QPointF point) { mLeftPerspPoint = point; }
    QPointF getLeftPerspPoint() const { return mLeftPerspPoint; }
    void setRightPerspPoint(QPointF point) { mRightPerspPoint = point; }
    QPointF getRightPerspPoint() const { return mRightPerspPoint; }
    void setMiddlePerspPoint(QPointF point) { mMiddlePerspPoint = point; }
    QPointF getMiddlePerspPoint() const { return mMiddlePerspPoint; }

    bool isPerspOverlaysActive() const { return !mActivePerspOverlays.isEmpty(); }
    void updatePerspOverlayActiveList();

private:
    Editor* mEditor = nullptr;

    QPointF mSinglePerspPoint;   // for single point perspective.
    QPointF mLeftPerspPoint;
    QPointF mRightPerspPoint;    // Left, right and middle are for
    QPointF mMiddlePerspPoint;   // two and three point perspective

    MoveMode mMoveMode = MoveMode::NONE;

    bool mOverlayCenter = false;
    bool mOverlayThirds = false;
    bool mOverlayGoldenRatio = false;
    bool mOverlaySafeAreas = false;
    bool mOverlayPerspective1 = false;
    bool mOverlayPerspective2 = false;
    bool mOverlayPerspective3 = false;

    QList<int> mActivePerspOverlays;
    const qreal mSelectionTolerance = 8.0;
};

#endif // OVERLAYMANAGER_H
