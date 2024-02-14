/*

Pencil2D - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2012-2020 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/

#ifndef VIEWMANAGER_H
#define VIEWMANAGER_H

#include <QTransform>
#include "basemanager.h"


class ViewManager : public BaseManager
{
    Q_OBJECT

public:
    explicit ViewManager(Editor* editor);
    ~ViewManager() override;

    bool init() override;
    Status load(Object*) override;
    Status save(Object*) override;

    QTransform getView() const;
    QTransform getViewInverse() const;
    qreal getViewScaleInverse() const;
    void resetView();

    QPointF mapCanvasToScreen(QPointF p) const;
    QPointF mapScreenToCanvas(QPointF p) const;

    QRectF mapCanvasToScreen(const QRectF& rect) const;
    QRectF mapScreenToCanvas(const QRectF& rect) const;

    QPolygonF mapPolygonToScreen(const QPolygonF& polygon) const;
    QPolygonF mapPolygonToCanvas(const QPolygonF& polygon) const;

    QPainterPath mapCanvasToScreen(const QPainterPath& path) const;
    QPainterPath mapScreenToCanvas(const QPainterPath& path) const;

    QPointF translation() const;
    void translate(float dx, float dy);
    void translate(QPointF offset);
    void centerView();

    float rotation();
    void rotate(float degree);
    void rotateRelative(float delta);
    void resetRotation();

    qreal scaling();
    void scale(qreal scaleValue);
    void scaleAtOffset(qreal scaleValue, QPointF offset);
    void scaleUp();
    void scaleDown();
    void scale400();
    void scale300();
    void scale200();
    void scale100();
    void scale50();
    void scale33();
    void scale25();

    void flipHorizontal(bool b);
    void flipVertical(bool b);

    void setOverlayCenter(bool b);
    void setOverlayThirds(bool b);
    void setOverlayGoldenRatio(bool b);
    void setOverlaySafeAreas(bool b);

    bool isFlipHorizontal() const { return mIsFlipHorizontal; }
    bool isFlipVertical() const { return mIsFlipVertical; }
    bool getOverlayCenter() const { return mOverlayCenter; }
    bool getOverlayThirds() const { return mOverlayThirds; }
    bool getOverlayGoldenRatio() const { return mOverlayGoldenRatio; }
    bool getOverlaySafeAreas() const { return mOverlaySafeAreas; }

    void setCanvasSize(QSize size);

    QTransform getImportView() { return mImportView; }
    void setImportView(const QTransform& newView) { mImportView = newView; }

    void setImportFollowsCamera(bool b) { mImportFollowsCamera = b; }
    bool getImportFollowsCamera() { return mImportFollowsCamera; }

    void forceUpdateViewTransform();

signals:
    void viewChanged();
    void viewFlipped();

private:
    void updateViewTransforms();
    void onCurrentFrameChanged();

    QTransform mView;
    QTransform mViewInverse;
    QTransform mViewCanvas;
    QTransform mViewCanvasInverse;
    QTransform mCentre;
    QTransform mImportView;

    QPointF mTranslation = QPointF();
    qreal mScaling = 1.0;
    qreal mRotation = 0.0;

    QSize mCanvasSize = { 1, 1 };

    bool mIsFlipHorizontal = false;
    bool mIsFlipVertical = false;
    bool mOverlayCenter = false;
    bool mOverlayThirds = false;
    bool mOverlayGoldenRatio = false;
    bool mOverlaySafeAreas = false;

    bool mImportFollowsCamera = false;
};

#endif // VIEWMANAGER_H
