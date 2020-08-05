/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2012-2018 Matthew Chiawen Chang

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
#include "camera.h"
#include "layercamera.h"

const static qreal mMinScale = 0.01;
const static qreal mMaxScale = 100.0;

const std::vector<qreal> gZoomLevels
{
    0.01, 0.02, 0.04, 0.06, 0.08, 0.12,
    0.16, 0.25, 0.33, 0.5, 0.75, 1.0,
    1.5, 2.0, 3.0, 4.0, 5.0, 6.0,
    8.0, 16.0, 32.0, 48.0, 64.0, 96.0
};


ViewManager::ViewManager(Editor* editor) : BaseManager(editor)
{
    mDefaultEditorCamera = new Camera;
    mCurrentCamera = mDefaultEditorCamera;
}

ViewManager::~ViewManager() {
    delete mDefaultEditorCamera;
}

bool ViewManager::init()
{
    connect(editor(), &Editor::currentFrameChanged, this, &ViewManager::onCurrentFrameChanged);
    return true;
}

Status ViewManager::load(Object*)
{
    mCameraLayer = nullptr;
    mCurrentCamera = mDefaultEditorCamera;
    mCurrentCamera->reset();
    updateViewTransforms();

    return Status::OK;
}

Status ViewManager::save(Object* o)
{
    o->data()->setCurrentView(mView);
    return Status::OK;
}

void ViewManager::workingLayerChanged(Layer* layer)
{
    if (layer->type() == Layer::CAMERA)
    {
        setCameraLayer(layer);
    }
    else
    {
        setCameraLayer(nullptr);
    }
}

QPointF ViewManager::mapCanvasToScreen(QPointF p)
{
    return mViewCanvas.map(p);
}

QPointF ViewManager::mapScreenToCanvas(QPointF p)
{
    return mViewCanvasInverse.map(p);
}

QPainterPath ViewManager::mapCanvasToScreen(const QPainterPath& path)
{
    return mViewCanvas.map(path);
}

QRectF ViewManager::mapCanvasToScreen(const QRectF& rect)
{
    return mViewCanvas.mapRect(rect);
}

QRectF ViewManager::mapScreenToCanvas(const QRectF& rect)
{
    return mViewCanvasInverse.mapRect(rect);
}

QPolygonF ViewManager::mapPolygonToScreen(const QPolygonF &polygon)
{
    return mViewCanvas.map(polygon);
}

QPolygonF ViewManager::mapPolygonToCanvas(const QPolygonF &polygon)
{
    return mViewCanvasInverse.map(polygon);
}

QPainterPath ViewManager::mapScreenToCanvas(const QPainterPath& path)
{
    return mViewCanvasInverse.map(path);
}

QTransform ViewManager::getView()
{
    return mViewCanvas;
}

QTransform ViewManager::getViewInverse()
{
    return mViewCanvasInverse;
}

void ViewManager::updateViewTransforms()
{
    if (mCameraLayer)
    {
        int frame = editor()->currentFrame();
        mCurrentCamera = mCameraLayer->getCameraAtFrame(frame);
        if (mCurrentCamera)
        {
            mCurrentCamera->updateViewTransform();
        }
        mView = mCameraLayer->getViewAtFrame(frame);
    }
    else
    {
        mCurrentCamera = mDefaultEditorCamera;
        mCurrentCamera->updateViewTransform();

        mView = mCurrentCamera->getView();
    }

    mViewInverse = mView.inverted();

    float flipX = mIsFlipHorizontal ? -1.f : 1.f;
    float flipY = mIsFlipVertical ? -1.f : 1.f;
    QTransform f = QTransform::fromScale(static_cast<qreal>(flipX), static_cast<qreal>(flipY));

    mViewCanvas = mView * f * mCentre;
    mViewCanvasInverse = mViewCanvas.inverted();
}

QPointF ViewManager::translation()
{
    if (mCurrentCamera)
    {
        return mCurrentCamera->translation();
    }
    return QPointF(0, 0);
}

void ViewManager::translate(float dx, float dy)
{
    if (mCurrentCamera)
    {
        mCurrentCamera->translate(static_cast<qreal>(dx), static_cast<qreal>(dy));
        updateViewTransforms();

        Q_EMIT viewChanged();
    }
}

void ViewManager::translate(QPointF offset)
{
    translate(static_cast<float>(offset.x()), static_cast<float>(offset.y()));
}

float ViewManager::rotation()
{
    if (mCurrentCamera)
    {
        return static_cast<float>(mCurrentCamera->rotation());
    }
    return 0.0f;
}

void ViewManager::rotate(float degree)
{
    if (mCurrentCamera)
    {
        mCurrentCamera->rotate(static_cast<qreal>(degree));
        updateViewTransforms();

        Q_EMIT viewChanged();
    }
}

qreal ViewManager::scaling()
{
    if (mCurrentCamera)
    {
        return mCurrentCamera->scaling();
    }
    return 0.0;
}

void ViewManager::scaleUp()
{
    for (size_t i = 0; i < gZoomLevels.size(); i++)
    {
        if (gZoomLevels[i] > scaling())
        {
            scale(gZoomLevels[i]);
            return;
        }
    }

    // out of pre-defined zoom levels
    scale(scaling() * 1.18);
}

void ViewManager::scaleDown()
{
    for (int i = static_cast<int>(gZoomLevels.size()) - 1; i >= 0; --i)
    {
        if (gZoomLevels[static_cast<unsigned>(i)] < scaling())
        {
            scale(gZoomLevels[static_cast<unsigned>(i)]);
            return;
        }
    }
    scale(scaling() * 0.8333);
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

    if (mCurrentCamera)
    {
        mCurrentCamera->scale(scaleValue);
        updateViewTransforms();

        Q_EMIT viewChanged();
    }
}

void ViewManager::scaleWithOffset(qreal scaleValue, QPointF offset)
{
    if (scaleValue < mMinScale)
    {
        scaleValue = mMinScale;
    }
    else if (scaleValue > mMaxScale)
    {
        scaleValue = mMaxScale;
    }

    if (mCurrentCamera)
    {
        mCurrentCamera->scaleWithOffset(scaleValue, offset);
        updateViewTransforms();

        Q_EMIT viewChanged();
    }
}

void ViewManager::flipHorizontal(bool b)
{
    if (b != mIsFlipHorizontal)
    {
        mIsFlipHorizontal = b;
        updateViewTransforms();

        Q_EMIT viewChanged();
        Q_EMIT viewFlipped();
    }
}

void ViewManager::flipVertical(bool b)
{
    if (b != mIsFlipVertical)
    {
        mIsFlipVertical = b;
        updateViewTransforms();

        Q_EMIT viewChanged();
        Q_EMIT viewFlipped();
    }
}

void ViewManager::setOverlayCenter(bool b)
{
    if (b != mOverlayCenter)
    {
        mOverlayCenter = b;
        updateViewTransforms();
        Q_EMIT viewChanged();
    }
}

void ViewManager::setOverlayThirds(bool b)
{
    if (b != mOverlayThirds)
    {
        mOverlayThirds = b;
        updateViewTransforms();
        Q_EMIT viewChanged();
    }
}

void ViewManager::setOverlayGoldenRatio(bool b)
{
    if (b != mOverlayGoldenRatio)
    {
        mOverlayGoldenRatio = b;
        updateViewTransforms();
        Q_EMIT viewChanged();
    }
}

void ViewManager::setOverlaySafeAreas(bool b)
{
    if (b != mOverlaySafeAreas)
    {
        mOverlaySafeAreas = b;
        updateViewTransforms();
        Q_EMIT viewChanged();
    }
}

void ViewManager::setCanvasSize(QSize size)
{
    mCanvasSize = size;
    mCentre = QTransform::fromTranslate(mCanvasSize.width() / 2., mCanvasSize.height() / 2.);

    updateViewTransforms();
    Q_EMIT viewChanged();
}

void ViewManager::setCameraLayer(Layer* layer)
{
    if (layer != nullptr)
    {
        if (layer->type() != Layer::CAMERA)
        {
            Q_ASSERT(false && "Only camera layers allowed pls");
            return;
        }
        mCameraLayer = static_cast<LayerCamera*>(layer);
    }
    else
    {
        mCameraLayer = nullptr;
    }

    updateViewTransforms();
}

void ViewManager::onCurrentFrameChanged()
{
    if (mCameraLayer)
    {
        updateViewTransforms();
    }
}

void ViewManager::resetView()
{
    if (mCurrentCamera)
    {
        mCurrentCamera->reset();
        updateViewTransforms();
        Q_EMIT viewChanged();
        Q_EMIT viewFlipped();
    }
}
