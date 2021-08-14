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
#include <QDebug>
#include <QPixmap>
#include <QPalette>

#include "object.h"
#include "layercamera.h"
#include "keyframe.h"

CameraPainter::CameraPainter()
{

}

void CameraPainter::preparePainter(const Object* object, int layerIndex, int frameIndex, QTransform transform, bool isPlaying, QPalette palette)
{
    mObject = object;
    mCurrentLayerIndex = layerIndex;
    mFrameIndex = frameIndex;
    mViewTransform = transform;
    mViewScaling = transform.m11();
    mIsPlaying = isPlaying;

    mHighlightColor = palette.color(QPalette::Highlight);
    mHighlightedTextColor = palette.color(QPalette::HighlightedText);
    mTextColor = palette.color(QPalette::Text);
}

void CameraPainter::paint() const
{
    QPainter painter;
    initializePainter(painter, *mCanvas);
    paintVisuals(painter);
}

void CameraPainter::paintCached()
{
    QPainter tempPainter;
    QPainter painter;
    QPixmap cachedPixmap(mCanvas->size());
    cachedPixmap.fill(Qt::transparent);
    initializePainter(tempPainter, cachedPixmap);
    initializePainter(painter, *mCanvas);

    if (mCachedPaint) {
        painter.setWorldMatrixEnabled(false);
        painter.drawPixmap(0, 0, *mCachedPaint.get());
    } else {
        paintVisuals(tempPainter);
        mCachedPaint.reset(new QPixmap(cachedPixmap));

        painter.setWorldMatrixEnabled(false);
        painter.drawPixmap(0, 0, cachedPixmap);
    }
    painter.end();
    tempPainter.end();
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
    painter.setWorldMatrixEnabled(true);
    painter.setWorldTransform(mViewTransform);
}

void CameraPainter::paintVisuals(QPainter& painter) const
{
    bool isCameraMode = false;
    LayerCamera* cameraLayer = static_cast<LayerCamera*>(mObject->getFirstVisibleLayer(mCurrentLayerIndex, Layer::CAMERA));

    if (cameraLayer && cameraLayer == mObject->getLayer(mCurrentLayerIndex)) {
        isCameraMode = true;
    }

    if (cameraLayer == nullptr) { return; }

    painter.save();
    painter.setWorldMatrixEnabled(false);

    QTransform camTransform = cameraLayer->getViewAtFrame(mFrameIndex);
    QRect cameraRect = cameraLayer->getViewRect();

    if (isCameraMode) {
        paintInterpolations(painter, cameraLayer);

        if (cameraLayer->keyExists(mFrameIndex) && !mIsPlaying) {
            paintHandles(painter, camTransform, cameraRect);
        }
    }

    paintBorder(painter, camTransform, cameraRect);
}

void CameraPainter::paintBorder(QPainter& painter, const QTransform& camTransform, const QRect& camRect) const
{
    QRectF viewRect = painter.viewport();

    painter.setOpacity(1.0);
    painter.setWorldMatrixEnabled(true);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(0, 0, 0, 80));
    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);

    QRegion rg2(camRect);
    QTransform viewInverse = mViewTransform.inverted();
    QRect boundingRect = viewInverse.mapRect(viewRect).toAlignedRect();

    rg2 = camTransform.inverted().map(rg2);

    QRegion rg1(boundingRect);
    QRegion rg3 = rg1.subtracted(rg2);

    painter.setClipRegion(rg3);
    painter.drawRect(boundingRect);

    // paint top triangle
    QPolygon cameraViewPoly = camTransform.inverted().mapToPolygon(camRect);
    QPointF cameraMidPoint = camTransform.inverted().map(camRect.center());

    QPen trianglePen(Qt::black);
    QLineF topLine(cameraViewPoly.at(0), cameraViewPoly.at(1));
    QLineF centerLine(cameraMidPoint, topLine.pointAt(0.5));
    QPointF points[3] = {centerLine.pointAt(1.1), topLine.pointAt(0.55), topLine.pointAt(0.45)};
    painter.setPen(trianglePen);
    painter.setBrush(Qt::NoBrush);
    painter.drawPolygon(points, 3);

    painter.restore();
}

void CameraPainter::paintHandles(QPainter& painter, const QTransform& camTransform, const QRect& cameraRect) const
{
    painter.save();
    painter.setWorldMatrixEnabled(false);
    QPolygonF camPolygon = mViewTransform.map(camTransform.inverted().mapToPolygon(cameraRect));
    // if the current view is narrower than the camera field

    painter.setBrush(Qt::NoBrush);
    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
    // Indicates that the quality of the output will be degraded
    if (cameraRect.width()-2 > QLineF(camPolygon.at(0), camPolygon.at(1)).length() / mViewScaling)
    {
        painter.setPen(Qt::red);
    }
    else
    {
        painter.setPen(QColor(0, 0, 0, 255));
    }
    painter.drawPolygon(camPolygon);

    painter.setPen(mHighlightedTextColor);
    painter.setBrush(mHighlightColor);
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

    QPointF rotatePointR = QLineF(camPolygon.at(1), camPolygon.at(2)).pointAt(0.5);
    const QRectF rightSideCircle= QRectF(rotatePointR.x() - radius,
                                         rotatePointR.y() - radius,
                                         handleW, handleW);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.drawEllipse(rightSideCircle);

    QPointF rotatePointL = QLineF(camPolygon.at(0), camPolygon.at(3)).pointAt(0.5);
    const QRectF leftSideCircle= QRectF(rotatePointL.x() - radius,
                                         rotatePointL.y() - radius,
                                         handleW, handleW);
    painter.drawEllipse(leftSideCircle);

    painter.restore();
}

void CameraPainter::paintInterpolations(QPainter& painter, LayerCamera* cameraLayer) const
{
    if (mIsPlaying && !mOnionSkinOptions.enabledWhilePlaying) { return; }

    painter.save();
    QColor cameraDotColor = cameraLayer->getDotColor();

    QPolygon cameraViewPoly = cameraLayer->getViewRect();
    QPen onionSkinPen;

    bool keyExistsOnCurrentFrame = cameraLayer->keyExists(mFrameIndex);

    cameraLayer->foreachKeyFrame([&] (KeyFrame* keyframe) {

        int frame = keyframe->pos();
        int nextFrame = cameraLayer->getNextKeyFramePosition(frame);

        if (cameraLayer->getShowCameraPath() && !cameraLayer->hasSameTranslation(frame, nextFrame)) {

            QPointF cameraMidPoint = mViewTransform.map(cameraLayer->getPathMidPoint(mFrameIndex));
            painter.setBrush(cameraDotColor);
            painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
            painter.setRenderHint(QPainter::Antialiasing);

            // Highlight current dot
            QPen pen(Qt::black);
            pen.setWidth(2);
            painter.setPen(pen);
            cameraMidPoint = mViewTransform.map(cameraLayer->getViewAtFrame(mFrameIndex).inverted().map(QRectF(cameraLayer->getViewRect()).center()));
            painter.drawEllipse(cameraMidPoint, DOT_WIDTH/2., DOT_WIDTH/2.);

            if (!keyExistsOnCurrentFrame)
            {
                cameraMidPoint = mViewTransform.map(cameraLayer->getPathMidPoint(frame + 1));
                paintPath(painter, cameraLayer, frame, cameraMidPoint);
            }

            QColor color = cameraDotColor;
            if (mFrameIndex > frame && mFrameIndex < nextFrame)
                color.setAlphaF(0.5);
            else
                color.setAlphaF(0.2);
            painter.setPen(Qt::black);
            painter.setBrush(color);

            int next = cameraLayer->getNextKeyFramePosition(frame);
            for (int frameInBetween = frame; frameInBetween <= next ; frameInBetween++)
            {
                QTransform transform = cameraLayer->getViewAtFrame(frameInBetween);
                QPointF center = mViewTransform.map(transform.inverted().map(QRectF(cameraLayer->getViewRect()).center()));
                painter.drawEllipse(center, DOT_WIDTH/2., DOT_WIDTH/2.);
            }
        }

        painter.save();
        painter.setBrush(Qt::NoBrush);

        onionSkinPen.setStyle(Qt::PenStyle::DashLine);
        int prevFrame = cameraLayer->getPreviousFrameNumber(mFrameIndex, true);
        mOnionSkinPainter.paint(painter, cameraLayer, mOnionSkinOptions, mFrameIndex, [&] (OnionSkinPaintState state, int onionSkinNumber) {
            if (state == OnionSkinPaintState::PREV) {
                onionSkinPen.setColor(Qt::red);
            }
            if (state == OnionSkinPaintState::NEXT) {
                onionSkinPen.setColor(Qt::blue);
            }

            painter.setPen(onionSkinPen);

            // paint the "current" absolute frame
            painter.drawPolygon(mViewTransform.map(cameraLayer->getViewAtFrame(prevFrame).inverted().map(cameraViewPoly)));

            // paint normal onion skinning
            painter.drawPolygon(mViewTransform.map(cameraLayer->getViewAtFrame(onionSkinNumber).inverted().map(cameraViewPoly)));
        });
        painter.restore();
    });

    painter.restore();
}

void CameraPainter::paintPath(QPainter& painter, const LayerCamera* cameraLayer, const int frameIndex, const QPointF& midPoint) const
{
    painter.save();
    // draw movemode in text
    painter.setPen(Qt::black);
    QString pathType = cameraLayer->getInterpolationText(frameIndex);
    painter.drawText(midPoint - QPoint(0, 10), pathType);

    // if active path, draw bezier help lines for active path
    QList<QPointF> points = cameraLayer->getBezierPoints(frameIndex + 1);

    QList<QPointF> mappedPoints;
    for (QPointF point : points) {
        mappedPoints << mViewTransform.map(point);
    }
    if (mappedPoints.size() == 3)
    {
        painter.save();
        QPen pen (mHighlightColor, 0.5, Qt::PenStyle::DashLine);
        painter.setPen(pen);
        painter.drawLine(mappedPoints.at(0), mappedPoints.at(1));
        painter.drawLine(mappedPoints.at(1), mappedPoints.at(2));
        painter.restore();
    }

    // if active path, draw move handle
    painter.save();
    painter.setRenderHint(QPainter::Antialiasing, false);
    painter.setPen(mHighlightedTextColor);
    painter.setBrush(mHighlightColor);
    painter.drawRect(static_cast<int>(midPoint.x() - HANDLE_WIDTH/2),
                     static_cast<int>(midPoint.y() - HANDLE_WIDTH/2),
                     HANDLE_WIDTH, HANDLE_WIDTH);
    painter.restore();
    painter.restore();
}
