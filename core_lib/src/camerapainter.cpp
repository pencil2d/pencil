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


#include "painterutils.h"

const int DOT_WIDTH = 6;
const int HANDLE_WIDTH = 12;

CameraPainter::CameraPainter()
{

}

void CameraPainter::preparePainter(const Object* object,
                                   int layerIndex,
                                   int frameIndex,
                                   const QTransform& transform,
                                   bool isPlaying,
                                   bool showHandles,
                                   LayerVisibility layerVisibility,
                                   float relativeLayerOpacityThreshold,
                                   qreal viewScale)
{
    mObject = object;
    mCurrentLayerIndex = layerIndex;
    mFrameIndex = frameIndex;
    mViewTransform = transform;
    mIsPlaying = isPlaying;
    mShowHandles = showHandles;
    mLayerVisibility = layerVisibility;
    mRelativeLayerOpacityThreshold = relativeLayerOpacityThreshold;
    mViewScale = viewScale;

    mHandleColor = Qt::gray;
    mHandleTextColor = QColor(0, 0, 0);
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
    auto cameraLayers = mObject->getLayersByType<LayerCamera>();

    LayerCamera* cameraLayerBelow = static_cast<LayerCamera*>(mObject->getLayerBelow(mCurrentLayerIndex, Layer::CAMERA));

    if (cameraLayerBelow == nullptr) { return; }

    int startLayerI = 0;
    int endLayerI = mObject->getLayerCount() - 1;
    for (int i = startLayerI; i <= endLayerI; i++) {
        Layer* layer = mObject->getLayer(i);
        if (layer->type() != Layer::CAMERA) { continue; }

        LayerCamera* cameraLayer = static_cast<LayerCamera*>(layer);

        bool isCurrentLayer = cameraLayer == cameraLayerBelow;

        if (!cameraLayer->visible() || (mLayerVisibility == LayerVisibility::CURRENTONLY && !isCurrentLayer)) { continue; }

        painter.save();
        painter.setOpacity(1);
        if (mLayerVisibility == LayerVisibility::RELATED && !isCurrentLayer) {
            painter.setOpacity(calculateRelativeOpacityForLayer(mCurrentLayerIndex, i, mRelativeLayerOpacityThreshold));
        }

        paintInterpolations(painter, cameraLayer);

        painter.restore();
    }

    if (!mIsPlaying) {
        QTransform camTransform = cameraLayerBelow->getViewAtFrame(mFrameIndex);
        QRect cameraRect = cameraLayerBelow->getViewRect();
        if (mShowHandles) {
            int frame = cameraLayerBelow->getPreviousKeyFramePosition(mFrameIndex);
            Camera* cam = cameraLayerBelow->getLastCameraAtFrame(qMax(frame, mFrameIndex), 0);
            Q_ASSERT(cam);
            qreal scale = cam->scaling();
            qreal rotation = cam->rotation();
            QPointF translation = cam->translation();
            paintHandles(painter, camTransform, cameraRect, translation, scale, rotation, !cameraLayerBelow->keyExists(mFrameIndex));
        }
        paintBorder(painter, camTransform, cameraRect);
    }
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

void CameraPainter::paintHandles(QPainter& painter, const QTransform& camTransform, const QRect& cameraRect, const QPointF translation, const qreal scale, const qreal rotation, bool hollowHandles) const
{
    painter.save();

    painter.setBrush(Qt::NoBrush);
    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);

    // if the current view is narrower than the camera field
    // Indicates that the quality of the output will be degraded
    if (scale > 1)
    {
        painter.setPen(Qt::red);
    }
    else
    {
        painter.setPen(QColor(0, 0, 0, 255));
    }

    QPolygonF camPolygon = mViewTransform.map(camTransform.inverted().map(QPolygon(cameraRect)));
    painter.drawPolygon(camPolygon);


    painter.setPen(QColor(0, 0, 0, 100));

    QTransform scaleT;
    scaleT.scale(1, 1);
    scaleT.rotate(rotation);
    scaleT.translate(translation.x(), translation.y());

    QPolygon nonScaledCamPoly = mViewTransform.map(scaleT.inverted().map(QPolygon(cameraRect)));
    painter.drawPolygon(nonScaledCamPoly);
    painter.drawText(nonScaledCamPoly[0]-QPoint(0, 2), "100%");

    painter.setPen(mHandleTextColor);
    if (hollowHandles) {
        painter.setBrush(Qt::NoBrush);
    } else {
        painter.setBrush(mHandleColor);
    }
    int handleW = HANDLE_WIDTH;
    int radius = handleW / 2;

    const QRectF topRightCorner = QRectF(camPolygon.at(1).x() - radius,
                                            camPolygon.at(1).y() - radius,
                                            handleW, handleW);
    painter.drawRect(topRightCorner);

    const QRectF bottomRightCorner = QRectF(camPolygon.at(2).x() - radius,
                                            camPolygon.at(2).y() - radius,
                                            handleW, handleW);
    painter.drawRect(bottomRightCorner);
    const QRectF topLeftCorner = QRectF(camPolygon.at(0).x() - radius,
                                            camPolygon.at(0).y() - radius,
                                            handleW, handleW);
    painter.drawRect(topLeftCorner);

    const QRectF bottomLeftCorner = QRectF(camPolygon.at(3).x() - radius,
                                            camPolygon.at(3).y() - radius,
                                            handleW, handleW);
    painter.drawRect(bottomLeftCorner);

    // Paint rotation handle
    QPointF topCenter = QLineF(camPolygon.at(0), camPolygon.at(1)).center();

    qreal offsetLimiter = (0.8 * mViewScale);
    QPointF rotationHandle = mViewTransform.map(camTransform.inverted().map(QPoint(0, (-cameraRect.height()*0.5 - (offsetLimiter) * RotationHandleOffset))));

    painter.drawLine(topCenter, QPoint(rotationHandle.x(),
                                       (rotationHandle.y())));

    painter.drawEllipse(QRectF((rotationHandle.x() - handleW*0.5),
                               (rotationHandle.y() - handleW*0.5),
                               handleW, handleW));

    painter.restore();
}

void CameraPainter::paintInterpolations(QPainter& painter, const LayerCamera* cameraLayer) const
{
    if (mIsPlaying && !mOnionSkinOptions.enabledWhilePlaying) { return; }

    QColor cameraDotColor = cameraLayer->getDotColor();

    QPolygon cameraViewPoly = cameraLayer->getViewRect();
    QPen onionSkinPen;

    cameraLayer->foreachKeyFrame([&] (KeyFrame* keyframe) {
        int frame = keyframe->pos();
        int nextFrame = cameraLayer->getNextKeyFramePosition(frame);

        if (cameraLayer->getShowCameraPath() && !cameraLayer->hasSameTranslation(frame, nextFrame)) {
            painter.save();

            QPointF cameraPathPoint = mViewTransform.map(cameraLayer->getPathControlPointAtFrame(mFrameIndex));
            painter.setBrush(cameraDotColor);
            painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
            painter.setRenderHint(QPainter::Antialiasing);

            // Highlight current dot
            QPen pen(Qt::black);
            pen.setWidth(2);
            painter.setPen(pen);
            cameraPathPoint = mViewTransform.map(cameraLayer->getViewAtFrame(mFrameIndex).inverted().map(QRectF(cameraLayer->getViewRect()).center()));
            painter.drawEllipse(cameraPathPoint, DOT_WIDTH/2., DOT_WIDTH/2.);

            cameraPathPoint = mViewTransform.map(cameraLayer->getPathControlPointAtFrame(frame + 1));

            int distance = nextFrame - frame;
            // It makes no sense to paint the path when there's no interpolation.
            if (distance >= 2) {
                paintPath(painter, cameraLayer, frame, cameraPathPoint);
            }

            QColor color = cameraDotColor;
            if (mFrameIndex > frame && mFrameIndex < nextFrame)
                color.setAlphaF(0.5);
            else
                color.setAlphaF(0.2);
            painter.setPen(Qt::black);
            painter.setBrush(color);

            for (int frameInBetween = frame; frameInBetween <= nextFrame ; frameInBetween++)
            {
                QTransform transform = cameraLayer->getViewAtFrame(frameInBetween);
                QPointF center = mViewTransform.map(transform.inverted().map(QRectF(cameraLayer->getViewRect()).center()));
                painter.drawEllipse(center, DOT_WIDTH/2., DOT_WIDTH/2.);
            }

            painter.restore();
        }

        painter.save();
        painter.setBrush(Qt::NoBrush);

        onionSkinPen.setStyle(Qt::PenStyle::DashLine);
        mOnionSkinPainter.paint(painter, cameraLayer, mOnionSkinOptions, mFrameIndex, [&] (OnionSkinPaintState state, int onionSkinNumber) {

            QPolygon cameraPoly = mViewTransform.map(cameraLayer->getViewAtFrame(onionSkinNumber).inverted().map(cameraViewPoly));
            if (state == OnionSkinPaintState::PREV) {

                // TODO: should not be fixed colors.. we have preferences for this!
                onionSkinPen.setColor(Qt::red);

                painter.setPen(onionSkinPen);
                painter.drawPolygon(cameraPoly);
            } else if (state == OnionSkinPaintState::NEXT) {
                onionSkinPen.setColor(Qt::blue);

                painter.setPen(onionSkinPen);
                painter.drawPolygon(cameraPoly);
            } else if (state == OnionSkinPaintState::CURRENT) {
                painter.save();
                painter.setPen(Qt::black);
                painter.drawPolygon(cameraPoly);
                painter.restore();
            }
        });
        painter.restore();
    });
}

void CameraPainter::paintPath(QPainter& painter, const LayerCamera* cameraLayer, const int frameIndex, const QPointF& pathPoint) const
{

    // if active path, draw bezier help lines for active path
    QList<QPointF> points = cameraLayer->getBezierPointsAtFrame(frameIndex + 1);

    if (!points.empty())
    {
        Q_ASSERT(points.size() == 3);
        QPointF p0 = mViewTransform.map(points.at(0));
        QPointF p1 = mViewTransform.map(points.at(1));
        QPointF p2 = mViewTransform.map(points.at(2));

        painter.save();
        QPen pen (mHandleColor, 0.5, Qt::PenStyle::DashLine);
        painter.setPen(pen);
        painter.drawLine(p0, p1);
        painter.drawLine(p1, p2);
        painter.restore();
    }

    if (mShowHandles) {
        painter.save();
        // draw movemode in text
        painter.setPen(Qt::black);
        QString pathType = cameraLayer->getInterpolationTextAtFrame(frameIndex);

        // Space text according to path point so it doesn't overlap
        painter.drawText(pathPoint - QPoint(0, HANDLE_WIDTH), pathType);
        painter.restore();

        // if active path, draw move handle
        painter.save();
        painter.setRenderHint(QPainter::Antialiasing, false);
        painter.setPen(mHandleTextColor);
        painter.setBrush(mHandleColor);
        painter.drawRect(static_cast<int>(pathPoint.x() - HANDLE_WIDTH/2),
                         static_cast<int>(pathPoint.y() - HANDLE_WIDTH/2),
                         HANDLE_WIDTH, HANDLE_WIDTH);
        painter.restore();
    }
}
