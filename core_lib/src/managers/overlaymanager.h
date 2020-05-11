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

    void overlaysChanged();
    void resetPerspectiveOverlays();
    double getAngleLeftRight(QPointF left, QPointF right);

private:
    QPointF mSinglePerspPoint;   // for single point perspective.
    QPointF mLeftPerspPoint;
    QPointF mRightPerspPoint;    // Left, right and middle are for
    QPointF mMiddlePerspPoint;   // two and three point perspective

};

#endif // OVERLAYMANAGER_H
