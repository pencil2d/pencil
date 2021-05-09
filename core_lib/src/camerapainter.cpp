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
    mIsPlaying = isPlaying;

    mHighlightColor = palette.color(QPalette::Highlight);
    mHighlightedTextColor = palette.color(QPalette::HighlightedText);
    mTextColor = palette.color(QPalette::Text);
}

void CameraPainter::paint() const
{
    QPainter painter;
    initializePainter(painter, *mCanvas);
    paintCameraVisuals(painter);
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
        paintCameraVisuals(tempPainter);
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

void CameraPainter::paintCameraVisuals(QPainter& painter) const
{
    LayerCamera* cameraLayer = nullptr;
    bool isCameraMode = false;

    Layer* layer = mObject->getLayer(mCurrentLayerIndex);
    if (layer->type() == Layer::CAMERA && layer->visible())
    {
        cameraLayer = static_cast<LayerCamera*>(layer);
        isCameraMode = true;
    }
    else
    {
        // Find the first visible camera layers
        for (int i = 0; i < mObject->getLayerCount(); ++i)
        {
            layer = mObject->getLayer(i);
            if (layer->type() == Layer::CAMERA && layer->visible())
            {
                cameraLayer = static_cast<LayerCamera*>(layer);
                isCameraMode = (i == mCurrentLayerIndex);
                break;
            }
        }
    }


    if (cameraLayer == nullptr) { return; }

    painter.save();
    painter.setWorldMatrixEnabled(false);

    QTransform camTransform = cameraLayer->getViewAtFrame(mFrameIndex);
    QRect cameraRect = cameraLayer->getViewRect();

    // Draw Field polygon

    if (isCameraMode) {
        // Draw camera paths
        paintCameraPath(painter, cameraLayer);

        if (cameraLayer->keyExists(mFrameIndex)) {
            paintCameraHandles(painter, camTransform, cameraRect);
        }
    }

    paintCameraBorder(painter, camTransform, cameraRect);
}

void CameraPainter::paintCameraBorder(QPainter& painter, const QTransform& camTransform, const QRect& camRect) const
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

    painter.restore();
}

void CameraPainter::paintCameraHandles(QPainter& painter, const QTransform& camTransform, const QRect& cameraRect) const
{
    painter.save();
    painter.setWorldMatrixEnabled(false);
    QPolygonF camPolygon = mViewTransform.map(camTransform.inverted().mapToPolygon(cameraRect));
    // if the current view is narrower than the camera field

    painter.setBrush(Qt::NoBrush);
    if (cameraRect.width() > QLineF(camPolygon.at(0), camPolygon.at(1)).length())
    {
        painter.setPen(Qt::red);
        painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
    }
    else
    {
        painter.setPen(QColor(0, 0, 0, 80));
        painter.setCompositionMode(QPainter::RasterOp_NotDestination);
    }
    painter.drawLine(camPolygon.at(3), camPolygon.at(0));
    painter.drawLine(camPolygon.at(0), camPolygon.at(1));
    painter.drawLine(camPolygon.at(1), camPolygon.at(2));
    painter.setPen(Qt::blue);
    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
    painter.drawLine(camPolygon.at(3), camPolygon.at(2));

    painter.setPen(mHighlightedTextColor);
    painter.setBrush(mHighlightColor);
    int handleW = 15;
    int radius = handleW / 2;
    int width = radius / 2;

    const QRectF topRightCorner = QRectF(camPolygon.at(1).x() - width,
                                            camPolygon.at(1).y() - width,
                                            radius, radius);
    painter.drawRect(topRightCorner);

    const QRectF bottomRightCorner = QRectF(camPolygon.at(2).x() - width,
                                            camPolygon.at(2).y() - width,
                                            radius, radius);
    painter.drawRect(bottomRightCorner);
    const QRectF topLeftCorner = QRectF(camPolygon.at(0).x() - width,
                                            camPolygon.at(0).y() - width,
                                            radius, radius);
    painter.drawRect(topLeftCorner);

    const QRectF bottomLeftCorner = QRectF(camPolygon.at(3).x() - width,
                                            camPolygon.at(3).y() - width,
                                            radius, radius);
    painter.drawRect(bottomLeftCorner);

    QPointF rotatePointR = QLineF(camPolygon.at(1), camPolygon.at(2)).pointAt(0.5);
    const QRectF rightSideCircle= QRectF(rotatePointR.x() - width,
                                         rotatePointR.y() - width,
                                         radius, radius);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.drawEllipse(rightSideCircle);

    QPointF rotatePointL = QLineF(camPolygon.at(0), camPolygon.at(3)).pointAt(0.5);
    const QRectF leftSideCircle= QRectF(rotatePointL.x() - width,
                                         rotatePointL.y() - width,
                                         radius, radius);
    painter.drawEllipse(leftSideCircle);

    painter.restore();
}

void CameraPainter::paintCameraPath(QPainter& painter, LayerCamera* cameraLayer) const
{
    if (!cameraLayer->getShowCameraPath() || mIsPlaying) { return; }

    painter.save();
    QColor cameraDotColor = cameraLayer->getDotColor();

    QPen pen(Qt::black);
    pen.setWidth(2);

    cameraLayer->foreachKeyFrame([this, &painter, &cameraLayer, &cameraDotColor, &pen] (KeyFrame* keyframe) {
        bool activepath = false;

        int frame = keyframe->pos();
        int nextFrame = cameraLayer->getNextKeyFramePosition(frame);
        if (nextFrame == frame)
            return;

        if (mFrameIndex > frame && mFrameIndex < nextFrame)
        {
            activepath = true;
        }

        QPointF center = mViewTransform.map(cameraLayer->getPathMidPoint(mFrameIndex));
        painter.setBrush(cameraDotColor);
        painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
        painter.setRenderHint(QPainter::Antialiasing);

        if (activepath && !cameraLayer->hasSameTranslation(frame, nextFrame))
        {
            // if active path, draw movemode in text
            painter.setPen(Qt::black);
            QString pathType = cameraLayer->getInterpolationText(frame);
            painter.drawText(center - QPoint(0, 10), pathType);

            // if active path, draw bezier help lines for active path
            QList<QPointF> points = cameraLayer->getBezierPoints(mFrameIndex);

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
            painter.drawRect(static_cast<int>(center.x() - HANDLE_WIDTH/2),
                             static_cast<int>(center.y() - HANDLE_WIDTH/2),
                             HANDLE_WIDTH, HANDLE_WIDTH);
            painter.restore();
        }

        QColor color = cameraDotColor;
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

        // Highlight current dot
        painter.setPen(pen);
        painter.setBrush(cameraDotColor);
        center = mViewTransform.map(cameraLayer->getViewAtFrame(mFrameIndex).inverted().map(QRectF(cameraLayer->getViewRect()).center()));
        painter.drawEllipse(center, DOT_WIDTH/2., DOT_WIDTH/2.);
    });

    painter.restore();
}
