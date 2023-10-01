#include "overlaypainter.h"

#include "layercamera.h"
#include "camera.h"
#include "layer.h"
#include "util.h"

Q_CONSTEXPR static int LEFT_ANGLE_OFFSET = 90;
Q_CONSTEXPR static int RIGHT_ANGLE_OFFSET = -90;
Q_CONSTEXPR static int HANDLE_WIDTH = 12;

OverlayPainter::OverlayPainter()
{
}

void OverlayPainter::initializePainter(QPainter& painter)
{
    painter.setCompositionMode(QPainter::CompositionMode_Difference);
    QPen pen(QColor(180, 220, 255));
    pen.setCosmetic(true);
    painter.setPen(pen);
    painter.setBrush(Qt::NoBrush);
}

void OverlayPainter::preparePainter(const LayerCamera* cameraLayer, const QPalette& palette)
{
    mPalette = palette;
    mCameraLayer = cameraLayer;
}

void OverlayPainter::setViewTransform(const QTransform view)
{
    mViewTransform = view;
}

void OverlayPainter::paint(QPainter &painter, const QRect& viewport)
{
    if (mCameraLayer == nullptr) { return; }

    painter.save();
    initializePainter(painter);

    QTransform camTransform = mCameraLayer->getViewAtFrame(mOptions.nFrameIndex);
    QRect cameraRect = mCameraLayer->getViewRect();
    Camera* camera = mCameraLayer->getLastCameraAtFrame(mOptions.nFrameIndex, 0);

    if (camera == nullptr) { return; }


    painter.setWorldTransform(mViewTransform);
    if (mOptions.bCenter)
    {
        paintOverlayCenter(painter, camTransform, cameraRect);
    }
    if (mOptions.bThirds)
    {
        paintOverlayThirds(painter, camTransform, cameraRect);
    }
    if (mOptions.bGoldenRatio)
    {
        paintOverlayGolden(painter, camTransform, cameraRect);
    }
    if (mOptions.bSafeArea)
    {
        paintOverlaySafeAreas(painter, *camera, camTransform, cameraRect);
    }

    const QRect mappedViewport = mViewTransform.inverted().mapRect(viewport);
    if (mOptions.bPerspective1)
    {
        paintOverlayPerspectiveOnePoint(painter, mappedViewport, camTransform);
    }
    if (mOptions.bPerspective2 || mOptions.bPerspective3)
    {
        paintOverlayPerspectiveTwoPoints(painter, mappedViewport, *camera, camTransform);
    }
    if (mOptions.bPerspective3)
    {
        paintOverlayPerspectiveThreePoints(painter, mappedViewport, *camera, camTransform);
    }

    if (mOptions.bGrid)
    {
        paintGrid(painter);
    }

    painter.restore();
}

void OverlayPainter::paintOverlayCenter(QPainter &painter, const QTransform& camTransform, const QRect& camRect) const
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

void OverlayPainter::paintOverlayThirds(QPainter &painter, const QTransform& camTransform, const QRect& camRect) const
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

void OverlayPainter::paintOverlayGolden(QPainter &painter, const QTransform& camTransform, const QRect& camRect) const
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

void OverlayPainter::paintOverlaySafeAreas(QPainter &painter, const Camera& camera, const QTransform& camTransform, const QRect& camRect) const
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
            painter.save();
            QPointF topLeft = topLeftCrossLine.pointAt((action / 2.0) / 100 ).toPoint();

            QTransform trans = QTransform::fromTranslate(topLeft.x(), topLeft.y());
            QTransform rot = QTransform().rotate(-camera.rotation());
            QTransform scale = QTransform::fromScale(camera.scaling(), camera.scaling());

            QTransform t = scale.inverted() * rot * trans;
            painter.setTransform(t, true);
            painter.drawText(QPoint(), tr("Safe Action area %1 %").arg(action));
            painter.restore();
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
            QPointF bottomLeft = bottomLeftCrossLine.pointAt((title / 2.0) / 100);
            painter.save();

            QTransform trans = QTransform::fromTranslate(bottomLeft.x(), bottomLeft.y());
            QTransform rot = QTransform().rotate(-camera.rotation());
            QTransform scale = QTransform::fromScale(camera.scaling(), camera.scaling());

            QTransform t = scale.inverted() * rot * trans;
            painter.setTransform(t, true);
            painter.drawText(QPoint(), tr("Safe Title area %1 %").arg(title));
            painter.restore();
        }
    }
    painter.restore();
}

void OverlayPainter::paintOverlayPerspectiveOnePoint(QPainter& painter, const QRect& viewport, const QTransform& camTransform) const
{
    qreal degrees = static_cast<qreal>(mOptions.nOverlayAngle);
    if (degrees == 7.0) { degrees = 7.5; }

    QPointF singlePoint = camTransform.inverted().map(mOptions.mSinglePerspPoint);
    QLineF angleLine(singlePoint.x(), singlePoint.y(), singlePoint.x() + 1, singlePoint.y());

    QVector<QLineF> lines;
    for (qreal angle = 0; angle < 180; angle += degrees)
    {
        angleLine.setAngle(angle);
        lines.append(clipLine(angleLine, viewport, -qInf(), qInf()));
    }

    painter.save();
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.drawLines(lines);

    if (mOptions.bShowHandle) {
        singlePoint = mViewTransform.map(singlePoint);
        painter.setWorldMatrixEnabled(false);
        painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
        painter.setPen(mPalette.color(QPalette::HighlightedText));
        painter.setBrush(mPalette.color(QPalette::Highlight));
        painter.drawEllipse(QRectF(singlePoint.x()-HANDLE_WIDTH*.5, singlePoint.y()-HANDLE_WIDTH*.5, HANDLE_WIDTH, HANDLE_WIDTH));
    }

    painter.restore();
}

void OverlayPainter::paintOverlayPerspectiveTwoPoints(QPainter& painter, const QRect& viewport, const Camera& camera, const QTransform& camTransform) const
{
    qreal degrees = static_cast<qreal>(mOptions.nOverlayAngle);
    if (degrees == 7.0) { degrees = 7.5; }

    QPointF leftPoint = camTransform.inverted().map(mOptions.mLeftPerspPoint);
    QPointF rightPoint = camTransform.inverted().map(mOptions.mRightPerspPoint);
    QLineF angleLineLeft(leftPoint.x(), leftPoint.y(), leftPoint.x() + 1, leftPoint.y());
    QLineF angleLineRight(rightPoint.x(), rightPoint.y(), rightPoint.x() + 1, rightPoint.y());

    QVector<QLineF> lines;
    for (qreal angle = 0; angle <= 180; angle += degrees)
    {
        angleLineLeft.setAngle(LEFT_ANGLE_OFFSET - angle + camera.rotation());
        angleLineRight.setAngle(RIGHT_ANGLE_OFFSET - angle + camera.rotation());
        lines.append(clipLine(angleLineLeft, viewport, 0, qInf()));
        lines.append(clipLine(angleLineRight, viewport, 0, qInf()));
    }

    painter.save();
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.drawLines(lines);

    if (mOptions.bShowHandle) {
        leftPoint = mViewTransform.map(leftPoint);
        rightPoint = mViewTransform.map(rightPoint);
        painter.setWorldMatrixEnabled(false);
        painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
        painter.setPen(mPalette.color(QPalette::HighlightedText));
        painter.setBrush(mPalette.color(QPalette::Highlight));
        painter.drawEllipse(QRectF(leftPoint.x()-HANDLE_WIDTH*.5, leftPoint.y()-HANDLE_WIDTH*.5, HANDLE_WIDTH,HANDLE_WIDTH));
        painter.drawEllipse(QRectF(rightPoint.x()-HANDLE_WIDTH*.5, rightPoint.y()-HANDLE_WIDTH*.5, HANDLE_WIDTH, HANDLE_WIDTH));
    }

    painter.restore();
}

void OverlayPainter::paintOverlayPerspectiveThreePoints(QPainter& painter, const QRect& viewport, const Camera& camera, const QTransform& camTransform) const
{
    qreal degrees = static_cast<qreal>(mOptions.nOverlayAngle);
    if (degrees == 7.0) { degrees = 7.5; }

    QPointF middlePoint = camTransform.inverted().map(mOptions.mMiddlePerspPoint);
    QLineF angleLine(middlePoint.x(), middlePoint.y(), middlePoint.x() + 1, middlePoint.y());

    const int middleAngleOffset = mOptions.mLeftPerspPoint.y() < mOptions.mMiddlePerspPoint.y() ? 180 : 0;
    QVector<QLineF> lines;
    for (qreal angle = 0; angle <= 180; angle += degrees)
    {
        angleLine.setAngle(middleAngleOffset - angle + camera.rotation());
        lines.append(clipLine(angleLine, viewport, 0, qInf()));
    }

    painter.save();
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.drawLines(lines);

    if (mOptions.bShowHandle) {
        middlePoint = mViewTransform.map(middlePoint);
        painter.setWorldMatrixEnabled(false);
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

