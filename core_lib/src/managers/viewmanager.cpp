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

#include <QPainterPath>
#include "viewmanager.h"
#include "editor.h"
#include "object.h"

const static qreal mMinScale = 0.01;
const static qreal mMaxScale = 100.0;

const std::vector<qreal> gZoomLevels
{
    0.01, 0.02, 0.04, 0.06, 0.08, 0.12,
    0.16, 0.25, 0.33, 0.5, 0.75, 1.0,
    1.5, 2.0, 3.0, 4.0, 5.0, 6.0,
    8.0, 16.0, 32.0, 48.0, 64.0, 96.0
};


ViewManager::ViewManager(Editor* editor) : BaseManager(editor, __FUNCTION__)
{
}

ViewManager::~ViewManager() {
}

bool ViewManager::init()
{
    connect(editor(), &Editor::scrubbed, this, &ViewManager::onCurrentFrameChanged);
    return true;
}

Status ViewManager::load(Object*)
{
    updateViewTransforms();

    return Status::OK;
}

Status ViewManager::save(Object* o)
{
    o->data()->setCurrentView(mView);
    return Status::OK;
}

QPointF ViewManager::mapCanvasToScreen(QPointF p) const
{
    return mViewCanvas.map(p);
}

QPointF ViewManager::mapScreenToCanvas(QPointF p) const
{
    return mViewCanvasInverse.map(p);
}

QPainterPath ViewManager::mapCanvasToScreen(const QPainterPath& path) const
{
    return mViewCanvas.map(path);
}

QRectF ViewManager::mapCanvasToScreen(const QRectF& rect) const
{
    return mViewCanvas.mapRect(rect);
}

QRectF ViewManager::mapScreenToCanvas(const QRectF& rect) const
{
    return mViewCanvasInverse.mapRect(rect);
}

QPolygonF ViewManager::mapPolygonToScreen(const QPolygonF &polygon) const
{
    return mViewCanvas.map(polygon);
}

QPolygonF ViewManager::mapPolygonToCanvas(const QPolygonF &polygon) const
{
    return mViewCanvasInverse.map(polygon);
}

QPainterPath ViewManager::mapScreenToCanvas(const QPainterPath& path) const
{
    return mViewCanvasInverse.map(path);
}

QTransform ViewManager::getView() const
{
    return mViewCanvas;
}

QTransform ViewManager::getViewInverse() const
{
    return mViewCanvasInverse;
}

qreal ViewManager::getViewScaleInverse() const
{
    return mViewCanvasInverse.m11();
}

void ViewManager::updateViewTransforms()
{
    QTransform t;
    t.translate(mTranslation.x(), mTranslation.y());

    QTransform r;
    r.rotate(mRotation);

    QTransform s;
    s.scale(mScaling, mScaling);

    mView = t * r * s;
    mViewInverse = mView.inverted();

    float flipX = mIsFlipHorizontal ? -1.f : 1.f;
    float flipY = mIsFlipVertical ? -1.f : 1.f;
    QTransform f = QTransform::fromScale(static_cast<qreal>(flipX), static_cast<qreal>(flipY));

    mViewCanvas = mView * f * mCentre;
    mViewCanvasInverse = mViewCanvas.inverted();
}

QPointF ViewManager::translation() const
{
    return mTranslation;
}

void ViewManager::translate(float dx, float dy)
{
    mTranslation = QPointF(dx, dy);
    updateViewTransforms();

    emit viewChanged();
}

void ViewManager::translate(QPointF offset)
{
    translate(static_cast<float>(offset.x()), static_cast<float>(offset.y()));
}

void ViewManager::centerView()
{
    translate(0, 0);
}

float ViewManager::rotation()
{
    return mRotation;
}

void ViewManager::rotate(float degree)
{
    mRotation = degree;
    updateViewTransforms();

    emit viewChanged();
}

void ViewManager::rotateRelative(float delta)
{
    mRotation = mRotation + delta;
    updateViewTransforms();

    emit viewChanged();
}

void ViewManager::resetRotation()
{
    rotate(0);
}

qreal ViewManager::scaling()
{
    return mScaling;
}

void ViewManager::scaleUp()
{
    for (size_t i = 0; i < gZoomLevels.size(); i++)
    {
        if (mScaling < gZoomLevels[i])
        {
            scale(gZoomLevels[i]);
            return;
        }
    }

    scale(mScaling * 1.25);
}

void ViewManager::scaleDown()
{
    for (size_t i = gZoomLevels.size() - 1; i >= 0; --i)
    {
        if (mScaling > gZoomLevels[i])
        {
            scale(gZoomLevels[i]);
            return;
        }
    }

    scale(mScaling * 0.8);
}

void ViewManager::scale100()
{
    scale(1.0);
}

void ViewManager::scale400()
{
    scale(4.0);
}

void ViewManager::scale300()
{
    scale(3.0);
}

void ViewManager::scale200()
{
    scale(2.0);
}

void ViewManager::scale50()
{
    scale(0.5);
}

void ViewManager::scale33()
{
    scale(0.33);
}

void ViewManager::scale25()
{
    scale(0.25);
}

void ViewManager::scale(qreal scaleValue)
{
    if (scaleValue < mMinScale)
    {
        scaleValue = mMinScale;
    }
    else if (scaleValue > mMaxScale)
    {
        scaleValue = mMaxScale;
    }

    mScaling = scaleValue;
    updateViewTransforms();

    emit viewChanged();
}

void ViewManager::scaleAtOffset(qreal scaleValue, QPointF offset)
{
    if (scaleValue < mMinScale)
    {
        scaleValue = mMinScale;
    }
    else if (scaleValue > mMaxScale)
    {
        scaleValue = mMaxScale;
    }
    mTranslation = (mTranslation + offset) * mScaling / scaleValue - offset;
    mScaling = scaleValue;
    updateViewTransforms();

    emit viewChanged();
}

void ViewManager::flipHorizontal(bool b)
{
    if (b != mIsFlipHorizontal)
    {
        mIsFlipHorizontal = b;
        updateViewTransforms();

        emit viewChanged();
        emit viewFlipped();
    }
}

void ViewManager::flipVertical(bool b)
{
    if (b != mIsFlipVertical)
    {
        mIsFlipVertical = b;
        updateViewTransforms();

        emit viewChanged();
        emit viewFlipped();
    }
}

void ViewManager::setOverlayCenter(bool b)
{
    if (b != mOverlayCenter)
    {
        mOverlayCenter = b;
        updateViewTransforms();
        emit viewChanged();
    }
}

void ViewManager::setOverlayThirds(bool b)
{
    if (b != mOverlayThirds)
    {
        mOverlayThirds = b;
        updateViewTransforms();
        emit viewChanged();
    }
}

void ViewManager::setOverlayGoldenRatio(bool b)
{
    if (b != mOverlayGoldenRatio)
    {
        mOverlayGoldenRatio = b;
        updateViewTransforms();
        emit viewChanged();
    }
}

void ViewManager::setOverlaySafeAreas(bool b)
{
    if (b != mOverlaySafeAreas)
    {
        mOverlaySafeAreas = b;
        updateViewTransforms();
        emit viewChanged();
    }
}

void ViewManager::setCanvasSize(QSize size)
{
    mCanvasSize = size;
    mCentre = QTransform::fromTranslate(mCanvasSize.width() / 2., mCanvasSize.height() / 2.);

    updateViewTransforms();
    emit viewChanged();
}

void ViewManager::forceUpdateViewTransform()
{
    updateViewTransforms();
    emit viewChanged();
}

void ViewManager::onCurrentFrameChanged()
{
    // emit because of potential camera interpolation changes
    emit viewChanged();
}

void ViewManager::resetView()
{
    mTranslation = QPointF(0,0);
    mScaling = 1.0;
    mRotation = 0.0;

    updateViewTransforms();
    emit viewChanged();
    emit viewFlipped();

}
