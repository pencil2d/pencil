#ifndef OVERLAYMANAGER_H
#define OVERLAYMANAGER_H

//#include <QObject>
#include "pencildef.h"
#include "movemode.h"
#include "basemanager.h"

#include <QPointF>
#include <QTransform>
#include <QPainter>

class Editor;
class ViewManager;
class OverlayPainter;

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

    MoveMode getMoveModeForOverlayAnchor(QPointF pos);
    double selectionTolerance();

    void updatePerspOverlay(int i, QPointF point);
    void updatePerspOverlay(MoveMode mode, QPoint point);
    void overlaysChanged();
    void resetPerspectiveOverlays();

    void setOverlayCenter(bool b);
    void setOverlayThirds(bool b);
    void setOverlayGoldenRatio(bool b);
    void setOverlaySafeAreas(bool b);
    void setOverlayPerspective1(bool b);
    void setOverlayPerspective2(bool b);
    void setOverlayPerspective3(bool b);

    bool getOverlayCenter() { return mOverlayCenter; }
    bool getOverlayThirds() { return mOverlayThirds; }
    bool getOverlayGoldenRatio() { return mOverlayGoldenRatio; }
    bool getOverlaySafeAreas() { return mOverlaySafeAreas; }
    bool getOverlayPerspective1() { return mOverlayPerspective1; }
    bool getOverlayPerspective2() { return mOverlayPerspective2; }
    bool getOverlayPerspective3() { return mOverlayPerspective3; }

    MoveMode getMoveMode() { return mMoveMode; }
    void setActivePoint(QPointF point) { mActivePoint = point; }
    void setSinglePerspPoint(QPointF point) { mSinglePerspPoint = point; }
    QPointF getSinglePerspPoint() { return mSinglePerspPoint; }
    void setLeftPerspPoint(QPointF point) { mLeftPerspPoint = point; }
    QPointF getLeftPerspPoint() { return mLeftPerspPoint; }
    void setRightPerspPoint(QPointF point) { mRightPerspPoint = point; }
    QPointF getRightPerspPoint() { return mRightPerspPoint; }
    void setMiddlePerspPoint(QPointF point) { mMiddlePerspPoint = point; }
    QPointF getMiddlePerspPoint() { return mMiddlePerspPoint; }

    void setPerpsOverlayActive(int perspType) { mActivePerspOverlays.append(perspType); }
    void removePerspOverlayActive(int perspType) { mActivePerspOverlays.removeOne(perspType); }
    QList<int> getActivePerspOverlays() { return mActivePerspOverlays; }
    bool isPerspOverlaysActive() { return !mActivePerspOverlays.isEmpty(); }

private:
    Editor* mEditor = nullptr;
    OverlayPainter* op = nullptr;

    QPointF mActivePoint;
    QPointF mSinglePerspPoint;   // for single point perspective.
    QPointF mLeftPerspPoint;
    QPointF mRightPerspPoint;    // Left, right and middle are for
    QPointF mMiddlePerspPoint;   // two and three point perspective

    bool mOverlayCenter = false;
    bool mOverlayThirds = false;
    bool mOverlayGoldenRatio = false;
    bool mOverlaySafeAreas = false;
    bool mOverlayPerspective1 = false;
    bool mOverlayPerspective2 = false;
    bool mOverlayPerspective3 = false;

    MoveMode mMoveMode = MoveMode::NONE;

    QList<int> mActivePerspOverlays;
    const qreal mSelectionTolerance = 8.0;
};

#endif // OVERLAYMANAGER_H
