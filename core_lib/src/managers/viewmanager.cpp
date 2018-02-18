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

#include "viewmanager.h"
#include "editor.h"
#include "object.h"
#include "camera.h"
#include "layercamera.h"

const static float mMinScale = 0.01f;
const static float mMaxScale = 100.0f;


ViewManager::ViewManager(Editor* editor) : BaseManager(editor)
{
    mDefaultEditorCamera = new Camera;
    mCurrentCamera = mDefaultEditorCamera;
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
    return Status();
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
    QTransform f = QTransform::fromScale(flipX, flipY);

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
        mCurrentCamera->translate(dx, dy);
        updateViewTransforms();

        Q_EMIT viewChanged();
    }
}

void ViewManager::translate(QPointF offset)
{
    translate(offset.x(), offset.y());
}

float ViewManager::rotation()
{
    if (mCurrentCamera)
    {
        return mCurrentCamera->rotation();
    }
    return 0.0f;
}

void ViewManager::rotate(float degree)
{
    if (mCurrentCamera)
    {
        mCurrentCamera->rotate(degree);
        updateViewTransforms();

        Q_EMIT viewChanged();
    }
}

float ViewManager::scaling()
{
    if (mCurrentCamera)
    {
        return mCurrentCamera->scaling();
    }
    return 0.0f;
}

void ViewManager::scaleUp()
{
    for (size_t i = 0; i < mZoomLevels.size(); i++)
    {
        if (mZoomLevels[i] > scaling())
        {
            scale(mZoomLevels[i]);
            return;
        }
    }

    // out of pre-defined zoom levels
    scale(scaling() * 1.18f);
}

void ViewManager::scaleDown()
{
    for (int i = mZoomLevels.size() - 1; i >= 0; --i)
    {
        if (mZoomLevels[i] < scaling())
        {
            scale(mZoomLevels[i]);
            return;
        }
    }
    scale(scaling() * 0.8333f);
}

void ViewManager::scale(float scaleValue)
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

void ViewManager::setCanvasSize(QSize size)
{
    mCanvasSize = size;
    mCentre = QTransform::fromTranslate(mCanvasSize.width() / 2.f, mCanvasSize.height() / 2.f);

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
