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

const int DOT_WIDTH = 6;
const int HANDLE_WIDTH = 12;

CameraPainter::CameraPainter()
{

}

void CameraPainter::preparePainter(const Object* object, int layerIndex, int frameIndex, const QTransform& transform, bool isPlaying, const QPalette& palette)
{
    mObject = object;
    mCurrentLayerIndex = layerIndex;
    mFrameIndex = frameIndex;
    mViewTransform = transform;
    mIsPlaying = isPlaying;

    mHighlightColor = palette.color(QPalette::Highlight);
    mHighlightedTextColor = palette.color(QPalette::HighlightedText);
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
    LayerCamera* cameraLayer = static_cast<LayerCamera*>(mObject->getFirstVisibleLayer(mCurrentLayerIndex, Layer::CAMERA));
    if (cameraLayer == nullptr) { return; }

    QTransform camTransform = cameraLayer->getViewAtFrame(mFrameIndex);
    QRect cameraRect = cameraLayer->getViewRect();

    if (cameraLayer == mObject->getLayer(mCurrentLayerIndex)) {
        paintInterpolations(painter, cameraLayer);

        if (!mIsPlaying && cameraLayer->keyExists(mFrameIndex)) {
            int frame = cameraLayer->getPreviousKeyFramePosition(mFrameIndex);
            Camera* cam = cameraLayer->getLastCameraAtFrame(qMax(frame, mFrameIndex), 0);
            Q_ASSERT(cam);
            qreal scale = cam->scaling();
            qreal rotation = cam->rotation();

            paintHandles(painter, camTransform, cameraRect, scale, rotation);
        }
    }

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

    // paint top triangle
    QPolygon cameraViewPoly = camTransform.inverted().map(QPolygon(camRect));
    QPointF cameraPathPoint = camTransform.inverted().map(camRect.center());

    QPen trianglePen(Qt::black);
    QLineF topLine(cameraViewPoly.at(0), cameraViewPoly.at(1));
    QLineF centerLine(cameraPathPoint, topLine.pointAt(0.5));
    QPointF points[3] = {centerLine.pointAt(1.1), topLine.pointAt(0.55), topLine.pointAt(0.45)};
    painter.setPen(trianglePen);
    painter.setBrush(Qt::NoBrush);
    painter.drawPolygon(points, 3);

    painter.restore();
}

void CameraPainter::paintHandles(QPainter& painter, const QTransform& camTransform, const QRect& cameraRect, const qreal scale, const qreal rotation) const
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
    scaleT.translate(camTransform.m31() / scale, camTransform.m32() / scale);
    scaleT.rotate(rotation);
    scaleT.scale(1, 1);

    QPolygon nonScaledCamPoly = mViewTransform.map(scaleT.inverted().map(QPolygon(cameraRect)));
    painter.drawPolygon(nonScaledCamPoly);
    painter.drawText(nonScaledCamPoly[0]-QPoint(0, 2), "100%");

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

void CameraPainter::paintInterpolations(QPainter& painter, const LayerCamera* cameraLayer) const
{
    if (mIsPlaying && !mOnionSkinOptions.enabledWhilePlaying) { return; }

    QColor cameraDotColor = cameraLayer->getDotColor();

    QPolygon cameraViewPoly = cameraLayer->getViewRect();
    QPen onionSkinPen;

    bool keyExistsOnCurrentFrame = cameraLayer->keyExists(mFrameIndex);

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

            if (!keyExistsOnCurrentFrame)
            {
                cameraPathPoint = mViewTransform.map(cameraLayer->getPathControlPointAtFrame(frame + 1));

                int distance = nextFrame - frame;
                // It makes no sense to paint the path when there's no interpolation.
                if (distance >= 2) {
                    paintPath(painter, cameraLayer, frame, cameraPathPoint);
                }
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
        int prevFrame = cameraLayer->getPreviousFrameNumber(mFrameIndex, true);
        mOnionSkinPainter.paint(painter, cameraLayer, mOnionSkinOptions, mFrameIndex, [&] (OnionSkinPaintState state, int onionSkinNumber) {
            if (state == OnionSkinPaintState::PREV) {
                onionSkinPen.setColor(Qt::red);

                painter.setPen(onionSkinPen);
                painter.drawPolygon(mViewTransform.map(cameraLayer->getViewAtFrame(prevFrame).inverted().map(cameraViewPoly)));
            }
            if (state == OnionSkinPaintState::NEXT) {
                onionSkinPen.setColor(Qt::blue);

                painter.setPen(onionSkinPen);
                painter.drawPolygon(mViewTransform.map(cameraLayer->getViewAtFrame(onionSkinNumber).inverted().map(cameraViewPoly)));
            }
        });
        painter.restore();
    });
}

void CameraPainter::paintPath(QPainter& painter, const LayerCamera* cameraLayer, const int frameIndex, const QPointF& pathPoint) const
{
    painter.save();
    // draw movemode in text
    painter.setPen(Qt::black);
    QString pathType = cameraLayer->getInterpolationTextAtFrame(frameIndex);
    painter.drawText(pathPoint - QPoint(0, 10), pathType);
    painter.restore();

    // if active path, draw bezier help lines for active path
    QList<QPointF> points = cameraLayer->getBezierPointsAtFrame(frameIndex + 1);

    if (!points.empty())
    {
        Q_ASSERT(points.size() == 3);
        QPointF p0 = mViewTransform.map(points.at(0));
        QPointF p1 = mViewTransform.map(points.at(1));
        QPointF p2 = mViewTransform.map(points.at(2));

        painter.save();
        QPen pen (mHighlightColor, 0.5, Qt::PenStyle::DashLine);
        painter.setPen(pen);
        painter.drawLine(p0, p1);
        painter.drawLine(p1, p2);
        painter.restore();
    }

    // if active path, draw move handle
    painter.save();
    painter.setRenderHint(QPainter::Antialiasing, false);
    painter.setPen(mHighlightedTextColor);
    painter.setBrush(mHighlightColor);
    painter.drawRect(static_cast<int>(pathPoint.x() - HANDLE_WIDTH/2),
                     static_cast<int>(pathPoint.y() - HANDLE_WIDTH/2),
                     HANDLE_WIDTH, HANDLE_WIDTH);
    painter.restore();
}
