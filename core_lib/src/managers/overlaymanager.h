#ifndef OVERLAYMANAGER_H
#define OVERLAYMANAGER_H

//#include <QObject>
#include "pencildef.h"
#include "movemode.h"
#include "basemanager.h"
#include "overlaypainter.h"

#include <QPointF>
#include <QTransform>
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

    MoveMode getMoveModeForOverlayAnchor(QPointF pos);
    double selectionTolerance();

    void initPerspOverlay();
    void updatePerspOverlay(int persp);
    void updatePerspOverlay(QPointF point);

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

    MoveMode getMoveMode();
    void setMoveMode(MoveMode mode);
    void setSinglePerspPoint(QPointF point);
    QPointF getSinglePerspPoint();
    void setLeftPerspPoint(QPointF point);
    QPointF getLeftPerspPoint();
    void setRightPerspPoint(QPointF point);
    QPointF getRightPerspPoint();
    void setMiddlePerspPoint(QPointF point);
    QPointF getMiddlePerspPoint();

    void setPerpsOverlayActive(int perspType) { mActivePerspOverlays.append(perspType); }
    void removePerspOverlayActive(int perspType) { mActivePerspOverlays.removeOne(perspType); }
    QList<int> getActivePerspOverlays() { return mActivePerspOverlays; }
    bool isPerspOverlaysActive() { return !mActivePerspOverlays.isEmpty(); }
    void updatePerspOverlayActiveList();

private:
    Editor* mEditor = nullptr;
    OverlayPainter op;

    QPointF mSinglePerspPoint;   // for single point perspective.
    QPointF mLeftPerspPoint;
    QPointF mRightPerspPoint;    // Left, right and middle are for
    QPointF mMiddlePerspPoint;   // two and three point perspective

    QPointF mLastLeftPoint;
    QPointF mLastRightPoint;
    QPointF mLastMiddlePoint;

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
