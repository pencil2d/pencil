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
#include <QPalette>
#include "object.h"
#include "layercamera.h"
#include "camera.h"
#include "keyframe.h"

#include "transform.h"

#include "painterutils.h"

CameraPainter::CameraPainter()
{

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

void CameraPainter::paint() const
{
    QPainter painter;
    initializePainter(painter, *mCanvas);
    paintVisuals(painter);
}

void CameraPainter::paintCached()
{
    if (!mCachedPaint) {
        QPainter tempPainter;
        QPixmap cachedPixmap(mCanvas->size());
        cachedPixmap.fill(Qt::transparent);
        initializePainter(tempPainter, cachedPixmap);

        paintVisuals(tempPainter);
        mCachedPaint.reset(new QPixmap(cachedPixmap));
        tempPainter.end();
    }

    QPainter painter;
    initializePainter(painter, *mCanvas);
    painter.drawPixmap(0, 0, *mCachedPaint.get());
    painter.end();
}

void CameraPainter::setCanvas(QPixmap* canvas)
{
    mCanvas = canvas;
}

void CameraPainter::resetCache()
{
    mCachedPaint.reset();
}

void CameraPainter::initializePainter(QPainter& painter, QPixmap& pixmap) const
{
    painter.begin(&pixmap);
    painter.setWorldTransform(mViewTransform);
    painter.setWorldMatrixEnabled(false);
}

void CameraPainter::paintVisuals(QPainter& painter) const
{
    LayerCamera* cameraLayerBelow = static_cast<LayerCamera*>(mObject->getLayerBelow(mCurrentLayerIndex, Layer::CAMERA));

    if (cameraLayerBelow == nullptr) { return; }

    const Layer* currentLayer = mObject->getLayer(mCurrentLayerIndex);

    if (mLayerVisibility == LayerVisibility::CURRENTONLY && currentLayer->type() != Layer::CAMERA) { return; }

    if (!mIsPlaying || mOnionSkinOptions.enabledWhilePlaying) {

        int startLayerI = 0;
        int endLayerI = mObject->getLayerCount() - 1;
        for (int i = startLayerI; i <= endLayerI; i++) {
            Layer* layer = mObject->getLayer(i);
            if (layer->type() != Layer::CAMERA) { continue; }

            LayerCamera* cameraLayer = static_cast<LayerCamera*>(layer);

            bool isCurrentLayer = cameraLayer == cameraLayerBelow;

            painter.save();
            painter.setOpacity(1);
            if (mLayerVisibility == LayerVisibility::RELATED && !isCurrentLayer) {
                painter.setOpacity(calculateRelativeOpacityForLayer(mCurrentLayerIndex, i, mRelativeLayerOpacityThreshold));
            }

            paintOnionSkinning(painter, cameraLayer);

            painter.restore();
        }
    }

    if (!cameraLayerBelow->visible()) { return; }

    QTransform camTransform = cameraLayerBelow->getViewAtFrame(mFrameIndex);
    QRect cameraRect = cameraLayerBelow->getViewRect();
    paintBorder(painter, camTransform, cameraRect);
}

void CameraPainter::paintBorder(QPainter& painter, const QTransform& camTransform, const QRect& camRect) const
{
    painter.save();
    QRectF viewRect = painter.viewport();

    painter.setOpacity(1.0);
    painter.setWorldMatrixEnabled(true);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(0, 0, 0, 80));
    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);

    QTransform viewInverse = mViewTransform.inverted();
    QRect boundingRect = viewInverse.mapRect(viewRect).toAlignedRect();

    QRegion rg1(boundingRect);
    QRegion rg2 = camTransform.inverted().map(QRegion(camRect));
    QRegion rg3 = rg1.subtracted(rg2);

    painter.setClipRegion(rg3);
    painter.drawRect(boundingRect);

    painter.restore();
}

void CameraPainter::paintOnionSkinning(QPainter& painter, const LayerCamera* cameraLayer) const
{
    QPolygon cameraViewPoly = cameraLayer->getViewRect();
    QPen onionSkinPen;

    painter.save();
    painter.setBrush(Qt::NoBrush);

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
