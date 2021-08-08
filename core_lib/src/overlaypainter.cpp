#include "overlaypainter.h"
#include <QDebug>

#include "layercamera.h"
#include "layer.h"

#include "pencildef.h"

OverlayPainter::OverlayPainter()
{
}

void OverlayPainter::initializePainter(QPainter& painter)
{
    painter.setCompositionMode(QPainter::CompositionMode_Difference);
    QPen pen(QColor(180, 220, 255));
    pen.setCosmetic(true);
    painter.setPen(pen);
    painter.setWorldMatrixEnabled(true);
    painter.setBrush(Qt::NoBrush);
}

void OverlayPainter::preparePainter(Layer* cameraLayer, QPalette palette)
{
    mPalette = palette;
    mCameraLayer = static_cast<LayerCamera*>(cameraLayer);
}

void OverlayPainter::setViewTransform(const QTransform view)
{
    mViewTransform = view;
}

void OverlayPainter::paint(QPainter &painter)
{
    painter.save();
    initializePainter(painter);
    LayerCamera* cameraLayer = mCameraLayer;

    if (cameraLayer == nullptr) { return; }

    painter.setWorldMatrixEnabled(false);

    QTransform camTransform = cameraLayer->getViewAtFrame(mOptions.nFrameIndex);
    QRect cameraRect = cameraLayer->getViewRect();

    if (mOptions.bCenter)
    {
        painter.setWorldTransform(mViewTransform);
        paintOverlayCenter(painter, camTransform, cameraRect);
    }
    if (mOptions.bThirds)
    {
        painter.setWorldTransform(mViewTransform);
        paintOverlayThirds(painter, camTransform, cameraRect);
    }
    if (mOptions.bGoldenRatio)
    {
        painter.setWorldTransform(mViewTransform);
        paintOverlayGolden(painter, camTransform, cameraRect);
    }
    if (mOptions.bSafeArea)
    {
        painter.setWorldTransform(mViewTransform);
        paintOverlaySafeAreas(painter, camTransform, cameraRect);
    }

    if (mOptions.bPerspective1)
    {
        painter.setWorldTransform(mViewTransform);
        paintOverlayPerspectiveOnePoint(painter, camTransform, cameraRect);
    }
    if (mOptions.bPerspective2)
    {
        painter.setWorldTransform(mViewTransform);
        paintOverlayPerspectiveTwoPoints(painter, camTransform, cameraRect);
    }
    if (mOptions.bPerspective3)
    {
        painter.setWorldTransform(mViewTransform);
        paintOverlayPerspectiveThreePoints(painter, camTransform, cameraRect);
    }

    if (mOptions.bGrid)
    {
        painter.setWorldTransform(mViewTransform);
        paintGrid(painter);
    }

    painter.restore();
}

void OverlayPainter::paintOverlayCenter(QPainter &painter, QTransform& camTransform, QRect& camRect) const
{
    painter.save();
    painter.setCompositionMode(QPainter::RasterOp_NotSourceAndNotDestination);

    QPen pen(Qt::DashLine);
    qreal space = 10;
    QVector<qreal> dashes;
    dashes << 10 << space << 10 << space << 10 << space;
    pen.setDashPattern(dashes);
    pen.setCosmetic(true);
    painter.setPen(pen);
    painter.setWorldMatrixEnabled(true);
    painter.setBrush(Qt::NoBrush);
    painter.setRenderHint(QPainter::Antialiasing, false);

    QPolygon poly = camTransform.inverted().mapToPolygon(camRect);
    QPoint centerTop = QLineF(poly.at(0), poly.at(1)).pointAt(0.5).toPoint();
    QPoint centerBottom = QLineF(poly.at(2), poly.at(3)).pointAt(0.5).toPoint();
    QPoint centerLeft = QLineF(poly.at(0), poly.at(3)).pointAt(0.5).toPoint();
    QPoint centerRight = QLineF(poly.at(1), poly.at(2)).pointAt(0.5).toPoint();
    painter.drawLine(QLineF(centerTop, centerBottom).pointAt(0.4).toPoint(),
                     QLineF(centerTop, centerBottom).pointAt(0.6).toPoint());
    painter.drawLine(QLineF(centerLeft, centerRight).pointAt(0.4).toPoint(),
                     QLineF(centerLeft, centerRight).pointAt(0.6).toPoint());

    painter.restore();
}

void OverlayPainter::paintOverlayThirds(QPainter &painter, QTransform& camTransform, QRect& camRect) const
{
    painter.save();
    painter.setCompositionMode(QPainter::RasterOp_NotSourceAndNotDestination);

    QPen pen(Qt::DashLine);
    qreal space = 10;
    QVector<qreal> dashes;
    dashes << 10 << space << 10 << space << 10 << space;
    pen.setDashPattern(dashes);
    pen.setCosmetic(true);
    painter.setPen(pen);
    painter.setWorldMatrixEnabled(true);
    painter.setBrush(Qt::NoBrush);
    painter.setRenderHint(QPainter::Antialiasing, false);

    QPolygon poly = camTransform.inverted().mapToPolygon(camRect);
    QLineF topLine(poly.at(0), poly.at(1));
    QLineF bottomLine(poly.at(3), poly.at(2));
    QLineF leftLine(poly.at(0), poly.at(3));
    QLineF rightLine(poly.at(1), poly.at(2));
    painter.drawLine(topLine.pointAt(0.333).toPoint(), bottomLine.pointAt(0.333));
    painter.drawLine(topLine.pointAt(0.667).toPoint(), bottomLine.pointAt(0.667));
    painter.drawLine(leftLine.pointAt(0.333).toPoint(), rightLine.pointAt(0.333));
    painter.drawLine(leftLine.pointAt(0.667).toPoint(), rightLine.pointAt(0.667));

    painter.restore();
}

void OverlayPainter::paintOverlayGolden(QPainter &painter, QTransform& camTransform, QRect& camRect) const
{
    painter.save();
    painter.setCompositionMode(QPainter::RasterOp_NotSourceAndNotDestination);

    QPen pen(Qt::DashLine);
    qreal space = 10;
    QVector<qreal> dashes;
    dashes << 10 << space << 10 << space << 10 << space;
    pen.setDashPattern(dashes);
    pen.setCosmetic(true);
    painter.setPen(pen);
    painter.setWorldMatrixEnabled(true);
    painter.setBrush(Qt::NoBrush);
    painter.setRenderHint(QPainter::Antialiasing, false);

    QPolygon poly = camTransform.inverted().mapToPolygon(camRect);
    QLineF topLine(poly.at(0), poly.at(1));
    QLineF bottomLine(poly.at(3), poly.at(2));
    QLineF leftLine(poly.at(0), poly.at(3));
    QLineF rightLine(poly.at(1), poly.at(2));
    painter.drawLine(topLine.pointAt(0.382).toPoint(), bottomLine.pointAt(0.382));
    painter.drawLine(topLine.pointAt(0.618).toPoint(), bottomLine.pointAt(0.618));
    painter.drawLine(leftLine.pointAt(0.382).toPoint(), rightLine.pointAt(0.382));
    painter.drawLine(leftLine.pointAt(0.618).toPoint(), rightLine.pointAt(0.618));

    painter.restore();
}

void OverlayPainter::paintOverlaySafeAreas(QPainter &painter, QTransform& camTransform, QRect& camRect) const
{
    painter.save();
    painter.setCompositionMode(QPainter::RasterOp_NotSourceAndNotDestination);
    QPen pen(Qt::DashLine);
    qreal space = 10;
    QVector<qreal> dashes;
    dashes << 10 << space << 10 << space << 10 << space;
    pen.setDashPattern(dashes);
    pen.setCosmetic(true);
    painter.setPen(pen);
    painter.setWorldMatrixEnabled(true);
    painter.setBrush(Qt::NoBrush);
    painter.setRenderHint(QPainter::TextAntialiasing, true);

    QPolygon poly = camTransform.inverted().mapToPolygon(camRect);
    QLineF topLeftCrossLine(poly.at(0), poly.at(2));
    QLineF bottomLeftCrossLine(poly.at(3), poly.at(1));

    if (mOptions.bActionSafe)
    {
        int action = mOptions.nActionSafe;
        painter.drawLine(topLeftCrossLine.pointAt((action / 2.0) / 100.0).toPoint(),
                         bottomLeftCrossLine.pointAt((100 - (action / 2.0)) / 100).toPoint());
        painter.drawLine(topLeftCrossLine.pointAt((action / 2.0) / 100.0).toPoint(),
                         bottomLeftCrossLine.pointAt((action / 2.0) / 100).toPoint());
        painter.drawLine(topLeftCrossLine.pointAt((100 - (action / 2.0)) / 100.0).toPoint(),
                         bottomLeftCrossLine.pointAt((100 - (action / 2.0)) / 100).toPoint());
        painter.drawLine(topLeftCrossLine.pointAt((100 - (action / 2.0)) / 100.0).toPoint(),
                         bottomLeftCrossLine.pointAt((action / 2.0) / 100).toPoint());

        if (mOptions.bShowSafeAreaHelperText)
        {
            painter.drawText(topLeftCrossLine.pointAt((action / 2.0) / 100.0).toPoint(), QObject::tr("Safe Action area %1 %").arg(action));
        }
    }
    if (mOptions.bTitleSafe)
    {
        int title = mOptions.nTitleSafe;
        painter.drawLine(topLeftCrossLine.pointAt((title / 2.0) / 100.0).toPoint(),
                         bottomLeftCrossLine.pointAt((100 - (title / 2.0)) / 100).toPoint());
        painter.drawLine(topLeftCrossLine.pointAt((title / 2.0) / 100.0).toPoint(),
                         bottomLeftCrossLine.pointAt((title / 2.0) / 100).toPoint());
        painter.drawLine(topLeftCrossLine.pointAt((100 - (title / 2.0)) / 100.0).toPoint(),
                         bottomLeftCrossLine.pointAt((100 - (title / 2.0)) / 100).toPoint());
        painter.drawLine(topLeftCrossLine.pointAt((100 - (title / 2.0)) / 100.0).toPoint(),
                         bottomLeftCrossLine.pointAt((title / 2.0) / 100).toPoint());

        if (mOptions.bShowSafeAreaHelperText)
        {
            painter.drawText(bottomLeftCrossLine.pointAt((title / 2.0) / 100), QObject::tr("Safe Title area %1 %").arg(title));
        }
    }
    painter.restore();
}

void OverlayPainter::paintOverlayPerspectiveOnePoint(QPainter& painter, QTransform& camTransform, QRect& camRect) const
{
    painter.save();
    painter.setRenderHint(QPainter::Antialiasing, true);

    qreal degrees = static_cast<qreal>(mOptions.nOverlayAngle);
    if (degrees == 7.0) { degrees = 7.5; }
    int repeats = static_cast<int>(360 / degrees);
    QLineF angleLine;

    QPointF singlePoint = camTransform.inverted().map(mOptions.mSinglePerspPoint);
    if (singlePoint == QPointF(0, 0))
    {
        singlePoint += QPointF(0.1, 0.1);
    }

    angleLine.setP1(singlePoint);
    QVector<QLineF> lines;
    for (int i = 0; i < repeats; i++)
    {
        angleLine.setAngle(i * degrees);
        angleLine.setLength(camRect.width() * 2.0);
        lines.append(angleLine);
    }
    painter.drawLines(lines);

    painter.setWorldMatrixEnabled(false);

    singlePoint = mViewTransform.map(singlePoint);

    if (!mOptions.bIsCamera) {
        painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
        painter.setPen(mPalette.color(QPalette::HighlightedText));
        painter.setBrush(mPalette.color(QPalette::Highlight));
        painter.drawEllipse(QRectF(singlePoint.x()-HANDLE_WIDTH*.5, singlePoint.y()-HANDLE_WIDTH*.5, HANDLE_WIDTH, HANDLE_WIDTH));
    }

    painter.restore();

}

void OverlayPainter::paintOverlayPerspectiveTwoPoints(QPainter& painter, QTransform& camTransform, QRect& camRect) const
{
    painter.save();
    painter.setRenderHint(QPainter::Antialiasing, true);

    qreal degrees = static_cast<qreal>(mOptions.nOverlayAngle);
    if (degrees == 7.0) { degrees = 7.5; }
    int repeats = static_cast<int>(180 / degrees);

    QPointF leftPoint = camTransform.inverted().map(mOptions.mLeftPerspPoint);
    QPointF rightPoint = camTransform.inverted().map(mOptions.mRightPerspPoint);
    if (leftPoint == QPointF(0.0, 0.0))
    {
        leftPoint += QPointF(0.1, 0.1);
    }

    if (rightPoint == QPointF(0.0, 0.0))
    {
        rightPoint += QPointF(0.1, 0.1);
    }

    QLineF angleLine;
    angleLine.setAngle(LEFTANGLEOFFSET);
    angleLine.setP1(leftPoint);
    QVector<QLineF> lines;
    for (int i = 0; i <= repeats; i++)
    {
        angleLine.setAngle(LEFTANGLEOFFSET - i * degrees);
        angleLine.setLength(camRect.width() * LINELENGTHFACTOR);
        lines.append(angleLine);
    }

    angleLine.setAngle(RIGHTANGLEOFFSET);
    angleLine.setP1(rightPoint);
    for (int i = 0; i <= repeats; i++)
    {
        angleLine.setAngle(RIGHTANGLEOFFSET - i * degrees);
        angleLine.setLength(camRect.width() * LINELENGTHFACTOR);
        lines.append(angleLine);
    }
    painter.drawLines(lines);

    painter.setWorldMatrixEnabled(false);

    leftPoint = mViewTransform.map(leftPoint);
    rightPoint = mViewTransform.map(rightPoint);

    if (!mOptions.bIsCamera) {
        painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
        painter.setPen(mPalette.color(QPalette::HighlightedText));
        painter.setBrush(mPalette.color(QPalette::Highlight));
        painter.drawEllipse(QRectF(leftPoint.x()-HANDLE_WIDTH*.5, leftPoint.y()-HANDLE_WIDTH*.5, HANDLE_WIDTH,HANDLE_WIDTH));
        painter.drawEllipse(QRectF(rightPoint.x()-HANDLE_WIDTH*.5, rightPoint.y()-HANDLE_WIDTH*.5, HANDLE_WIDTH, HANDLE_WIDTH));
    }

    painter.restore();
}

void OverlayPainter::paintOverlayPerspectiveThreePoints(QPainter& painter, QTransform& camTransform, QRect& camRect) const
{
    if (!mOptions.bPerspective2)
        paintOverlayPerspectiveTwoPoints(painter, camTransform, camRect);

    painter.save();
    painter.setRenderHint(QPainter::Antialiasing, true);

    qreal degrees = static_cast<qreal>(mOptions.nOverlayAngle);
    if (degrees == 7.0) { degrees = 7.5; }
    int repeats = static_cast<int>(180 / degrees);

    QPointF middlePoint = camTransform.inverted().map(mOptions.mMiddlePerspPoint);
    if (middlePoint == QPointF(0.0, 0.0))
    {
        middlePoint += QPointF(0.1, 0.1);
    }

    const int middleAngleOffset = mOptions.mLeftPerspPoint.y() < mOptions.mMiddlePerspPoint.y() ? 180 : 0;

    QLineF angleLine;
    angleLine.setAngle(middleAngleOffset);
    angleLine.setP1(middlePoint);
    QVector<QLineF> lines;
    for (int i = 0; i <= repeats; i++)
    {
        angleLine.setAngle(middleAngleOffset - i * degrees);
        angleLine.setLength(camRect.width() * LINELENGTHFACTOR);
        lines.append(angleLine);
    }
    painter.drawLines(lines);

    painter.setWorldMatrixEnabled(false);

    middlePoint = mViewTransform.map(middlePoint);

    if (!mOptions.bIsCamera) {
        painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
        painter.setPen(mPalette.color(QPalette::HighlightedText));
        painter.setBrush(mPalette.color(QPalette::Highlight));
        painter.drawEllipse(QRectF(middlePoint.x()-HANDLE_WIDTH*.5, middlePoint.y()-HANDLE_WIDTH*.5, HANDLE_WIDTH,HANDLE_WIDTH));
    }

    painter.restore();
}

void OverlayPainter::paintGrid(QPainter& painter) const
{
    painter.save();
    int gridSizeW = mOptions.nGridSizeW;
    int gridSizeH = mOptions.nGridSizeH;

    QRectF rect = painter.viewport();
    QRectF boundingRect = mViewTransform.inverted().mapRect(rect);

    int left = round100(boundingRect.left(), gridSizeW) - gridSizeW;
    int right = round100(boundingRect.right(), gridSizeW) + gridSizeW;
    int top = round100(boundingRect.top(), gridSizeH) - gridSizeH;
    int bottom = round100(boundingRect.bottom(), gridSizeH) + gridSizeH;

    QPen pen(Qt::lightGray);
    pen.setCosmetic(true);
    painter.setPen(pen);
    painter.setWorldMatrixEnabled(true);
    painter.setBrush(Qt::NoBrush);
    painter.setRenderHint(QPainter::Antialiasing, false);
    // draw vertical grid lines
    for (int x = left; x < right; x += gridSizeW)
    {
        painter.drawLine(x, top, x, bottom);
    }

    // draw horizontal grid lines
    for (int y = top; y < bottom; y += gridSizeH)
    {
        painter.drawLine(left, y, right, y);
    }

    painter.restore();
}

int OverlayPainter::round100(double f, int gridSize) const
{
    return static_cast<int>(f) / gridSize * gridSize;
}

