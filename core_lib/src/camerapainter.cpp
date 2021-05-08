#include "camerapainter.h"

#include <QPainter>
#include <QDebug>
#include <QPixmap>

#include "object.h"
#include "layercamera.h"

CameraPainter::CameraPainter()
{

}

void CameraPainter::preparePainter(const Object* object, int layerIndex, int frameIndex, QTransform transform, bool isPlaying)
{
    mObject = object;
    mCurrentLayerIndex = layerIndex;
    mFrameIndex = frameIndex;
    mViewTransform = transform;
    mIsPlaying = isPlaying;
}

void CameraPainter::paint() const
{
    QPainter painter;
    initializePainter(painter, *mCanvas);
    paintCameraBorder(painter);
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
        paintCameraBorder(tempPainter);
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

void CameraPainter::paintCameraBorder(QPainter& painter) const
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
            qDebug() << layer->name() << " index: " << i;
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
    painter.setWorldMatrixEnabled(true);

    QRectF viewRect = painter.viewport();
    QTransform mCamTransform = cameraLayer->getViewAtFrame(mFrameIndex);
    QRect mCameraRect = cameraLayer->getViewRect();

    // Draw camera paths
    paintCameraPath(painter, cameraLayer);

    // Draw Field polygon
    if (isCameraMode)
    {
        painter.save();
        painter.setWorldMatrixEnabled(true);
        QPolygonF camPolygon = mCamTransform.inverted().mapToPolygon(mCameraRect);
        // if the current view is narrower than the camera field
        painter.setBrush(Qt::NoBrush);
        if (mCameraRect.width() > QLineF(camPolygon.at(0), camPolygon.at(1)).length())
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

        painter.setPen(Qt::white);
        painter.setBrush(Qt::black);
        painter.setCompositionMode(QPainter::RasterOp_NotDestination);
        int handleW = mCameraRect.width() / 40; // width of handles is 2.5 % of output size
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
        painter.drawEllipse(rightSideCircle);

        QPointF rotatePointL = QLineF(camPolygon.at(0), camPolygon.at(3)).pointAt(0.5);
        const QRectF leftSideCircle= QRectF(rotatePointL.x() - width,
                                             rotatePointL.y() - width,
                                             radius, radius);
        painter.drawEllipse(leftSideCircle);

        painter.restore();
    }

    painter.setOpacity(1.0);
    painter.setWorldMatrixEnabled(true);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(0, 0, 0, 80));
    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);

    QRegion rg2(mCameraRect);
    QTransform viewInverse = mViewTransform.inverted();
    QRect boundingRect = viewInverse.mapRect(viewRect).toAlignedRect();

    rg2 = mCamTransform.inverted().map(rg2);

    QRegion rg1(boundingRect);
    QRegion rg3 = rg1.subtracted(rg2);

    painter.setClipRegion(rg3);
    painter.drawRect(boundingRect);

    painter.restore();
}


void CameraPainter::paintCameraPath(QPainter& painter, LayerCamera* cameraLayer) const
{
    if (!cameraLayer->getShowCameraPath() || mIsPlaying) { return; }

    painter.save();
    QColor cameraDotColor = cameraLayer->getDotColor();
    int max = cameraLayer->getMaxKeyFramePosition();
    for (int frame = 1; frame <= max; frame++)
    {
        bool activepath = false;
        QString pathType = "";
        if (!cameraLayer->keyExists(frame)) { continue; }

        int nextFrame = cameraLayer->getNextKeyFramePosition(frame);
        if (nextFrame == frame)
            break;

        if (frame < mFrameIndex && mFrameIndex < nextFrame)
        {
            activepath = true;
        }

        QPointF center = cameraLayer->getPathMidPoint(mFrameIndex);
        painter.setPen(cameraDotColor);
        painter.setCompositionMode(QPainter::CompositionMode_SourceOver);

        if (activepath && !cameraLayer->hasSameTranslation(frame, nextFrame))
        {
            // if active path, draw movemode in text
            pathType = cameraLayer->getInterpolationText(frame);
            painter.drawText(center - QPoint(0, 10), pathType);

            // if active path, draw bezier help lines for active path
            QList<QPointF> points = cameraLayer->getBezierPoints(mFrameIndex);
            if (points.size() == 3)
            {
                painter.drawLine(points.at(0), points.at(1));
                painter.drawLine(points.at(1), points.at(2));
            }

            // if active path, draw move handle
            painter.setBrush(Qt::white);
            painter.drawRect(center.x() - HANDLE_WIDTH/2,
                             center.y() - HANDLE_WIDTH/2,
                             HANDLE_WIDTH, HANDLE_WIDTH);
        }

        // draw dots
        if (!activepath)
            painter.setBrush(cameraDotColor);
        else
            painter.setBrush(Qt::NoBrush);
        int next = cameraLayer->getNextKeyFramePosition(frame);
        for (int j = frame; j <= next ; j++)
        {
            QTransform transform = cameraLayer->getViewAtFrame(j);
            QPointF center = transform.inverted().map(QRectF(mCameraRect).center());
            painter.drawEllipse(center, DOT_WIDTH/2., DOT_WIDTH/2.);
        }

        // highligth current dot
        if (cameraDotColor != Qt::white)
            painter.setBrush(Qt::white);
        else
            painter.setBrush(Qt::black);
        center = cameraLayer->getViewAtFrame(mFrameIndex).inverted().map(QRectF(mCameraRect).center());
        painter.drawEllipse(center, DOT_WIDTH/2., DOT_WIDTH/2.);
    }
        painter.restore();
}
