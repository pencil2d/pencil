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

#include "camerapainter.h"

#include <QPainter>
#include <QPixmap>
#include "object.h"
#include "layercamera.h"
#include "camera.h"
#include "keyframe.h"

#include "transform.h"

#include "painterutils.h"

CameraPainter::CameraPainter(QPixmap& canvas) : mCanvas(canvas)
{
    reset();
}

void CameraPainter::reset()
{
    mCameraPixmap = QPixmap(mCanvas.size());
    mCameraPixmap.setDevicePixelRatio(mCanvas.devicePixelRatioF());
    mCameraPixmap.fill(Qt::transparent);
}

void CameraPainter::resetCache()
{
    mCameraCacheValid = false;
}

void CameraPainter::preparePainter(const Object* object,
                                   int layerIndex,
                                   int frameIndex,
                                   const QTransform& transform,
                                   bool isPlaying,
                                   LayerVisibility layerVisibility,
                                   float relativeLayerOpacityThreshold,
                                   qreal viewScale)
{
    mObject = object;
    mCurrentLayerIndex = layerIndex;
    mFrameIndex = frameIndex;
    mViewTransform = transform;
    mIsPlaying = isPlaying;
    mLayerVisibility = layerVisibility;
    mRelativeLayerOpacityThreshold = relativeLayerOpacityThreshold;
    mViewScale = viewScale;
}

void CameraPainter::paint(const QRect& blitRect)
{
    QPainter painter;
    initializePainter(painter, mCanvas, blitRect, false);
    paintVisuals(painter, blitRect);

    mCameraCacheValid = true;
}

void CameraPainter::paintCached(const QRect& blitRect)
{
    QPainter painter;
    // As always, initialize the painter with the canvas image, as this is what we'll paint on
    // In this case though because the canvas has already been painted, we're not interested in
    // having the blitter clear the image again, as that would remove our previous painted data, ie. strokes...
    initializePainter(painter, mCanvas, blitRect, false);
    if (!mCameraCacheValid) {
        paintVisuals(painter, blitRect);
        painter.end();
        mCameraCacheValid = true;
    } else {
        painter.setWorldMatrixEnabled(false);
        painter.drawPixmap(mZeroPoint, mCameraPixmap);
        painter.setWorldMatrixEnabled(true);
        painter.end();
    }
}

void CameraPainter::initializePainter(QPainter& painter, QPixmap& pixmap, const QRect& blitRect, bool blitEnabled)
{
    painter.begin(&pixmap);

    if (blitEnabled) {
        painter.setCompositionMode(QPainter::CompositionMode_Clear);
        painter.fillRect(blitRect, Qt::transparent);
        // Surface has been cleared and is ready to be painted on
        painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
    }

    painter.setClipRect(blitRect);
    painter.setWorldMatrixEnabled(true);
    painter.setWorldTransform(mViewTransform);
}

void CameraPainter::paintVisuals(QPainter& painter, const QRect& blitRect)
{
    LayerCamera* cameraLayerBelow = static_cast<LayerCamera*>(mObject->getLayerBelow(mCurrentLayerIndex, Layer::CAMERA));

    if (cameraLayerBelow == nullptr) { return; }

    const Layer* currentLayer = mObject->getLayer(mCurrentLayerIndex);

    if (mLayerVisibility == LayerVisibility::CURRENTONLY && currentLayer->type() != Layer::CAMERA) { return; }

    QPainter visualsPainter;
    initializePainter(visualsPainter, mCameraPixmap, blitRect, true);

    if (!mIsPlaying || mOnionSkinOptions.enabledWhilePlaying) {

        int startLayerI = 0;
        int endLayerI = mObject->getLayerCount() - 1;
        for (int i = startLayerI; i <= endLayerI; i++) {
            Layer* layer = mObject->getLayer(i);
            if (layer->type() != Layer::CAMERA) { continue; }

            LayerCamera* cameraLayer = static_cast<LayerCamera*>(layer);

            bool isCurrentLayer = cameraLayer == cameraLayerBelow;

            visualsPainter.save();
            visualsPainter.setOpacity(1);
            if (mLayerVisibility == LayerVisibility::RELATED && !isCurrentLayer) {
                visualsPainter.setOpacity(calculateRelativeOpacityForLayer(mCurrentLayerIndex, i, mRelativeLayerOpacityThreshold));
            }

            paintOnionSkinning(visualsPainter, cameraLayer);

            visualsPainter.restore();
        }
    }

    if (!cameraLayerBelow->visible()) { return; }

    QTransform camTransform = cameraLayerBelow->getViewAtFrame(mFrameIndex);
    QRect cameraRect = cameraLayerBelow->getViewRect();
    paintBorder(visualsPainter, camTransform, cameraRect);

    painter.setWorldMatrixEnabled(false);
    painter.drawPixmap(mZeroPoint, mCameraPixmap);
}

void CameraPainter::paintBorder(QPainter& painter, const QTransform& camTransform, const QRect& camRect)
{
    painter.save();
    QRect viewRect = painter.viewport();

    painter.setOpacity(1.0);
    painter.setWorldMatrixEnabled(true);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(0, 0, 0, 80));
    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);

    QTransform viewInverse = mViewTransform.inverted();
    QRect boundingRect = viewInverse.mapRect(viewRect);

    QPolygon camPoly = camTransform.inverted().map(QPolygon(camRect));
    QPolygon boundingRectPoly = boundingRect;
    QPolygon visibleCanvasPoly = boundingRectPoly.subtracted(camPoly);

    painter.drawPolygon(visibleCanvasPoly);

    painter.restore();
}

void CameraPainter::paintOnionSkinning(QPainter& painter, const LayerCamera* cameraLayer)
{
    QPen onionSkinPen;

    painter.save();
    painter.setBrush(Qt::NoBrush);
    painter.setWorldMatrixEnabled(false);

    onionSkinPen.setStyle(Qt::PenStyle::DashLine);
    mOnionSkinPainter.paint(painter, cameraLayer, mOnionSkinOptions, mFrameIndex, [&] (OnionSkinPaintState state, int onionSkinNumber) {

        const QTransform& cameraTransform = cameraLayer->getViewAtFrame(onionSkinNumber);
        const QPolygonF& cameraPolygon = Transform::mapToWorldPolygon(cameraTransform, mViewTransform, cameraLayer->getViewRect());
        if (state == OnionSkinPaintState::PREV) {

            if (mOnionSkinOptions.colorizePrevFrames) {
                onionSkinPen.setColor(Qt::red);
            }

            painter.setPen(onionSkinPen);
            painter.drawPolygon(cameraPolygon);
        } else if (state == OnionSkinPaintState::NEXT) {
            if (mOnionSkinOptions.colorizeNextFrames) {
                onionSkinPen.setColor(Qt::blue);
            }

            painter.setPen(onionSkinPen);
            painter.drawPolygon(cameraPolygon);
        } else if (state == OnionSkinPaintState::CURRENT) {
            painter.save();
            painter.setPen(Qt::black);
            painter.drawPolygon(cameraPolygon);
            painter.restore();
        }
    });
    painter.restore();
}
