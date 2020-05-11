#include "overlaypainter.h"
#include <QDebug>
#include <QSettings>

#include "pencildef.h"

OverlayPainter::OverlayPainter(QObject *parent) : QObject(parent)
{
    activeOverlays.clear();
}

void OverlayPainter::setViewTransform(const QTransform view, const QTransform viewInverse)
{
    mViewTransform = view;
    mViewInverse = viewInverse;
}

void OverlayPainter::initPerspectivePainter(QPainter &painter)
{
    painter.setCompositionMode(QPainter::CompositionMode_Difference);
    QPen pen(QColor(180, 220, 255));
    pen.setCosmetic(true);
    painter.setPen(pen);
    painter.setWorldMatrixEnabled(true);
    painter.setBrush(Qt::NoBrush);
}

void OverlayPainter::renderOverlays(QPainter &painter, QRect cameraRect)
{
    if (mOptions.bCenter)
    {
        painter.setWorldTransform(mViewTransform);
        paintOverlayCenter(painter, cameraRect);
    }
    if (mOptions.bThirds)
    {
        painter.setWorldTransform(mViewTransform);
        paintOverlayThirds(painter, cameraRect);
    }
    if (mOptions.bGoldenRatio)
    {
        painter.setWorldTransform(mViewTransform);
        paintOverlayGolden(painter, cameraRect);
    }
    if (mOptions.bSafeArea)
    {
        painter.setWorldTransform(mViewTransform);
        paintOverlaySafeAreas(painter, cameraRect);
    }
    QSettings settings(PENCIL2D, PENCIL2D);
    mOptions.nOverlayAngle = settings.value("OverlayAngle").toInt();

    if (mOptions.bPerspective1)
    {
        painter.setWorldTransform(mViewTransform);
        paintOverlayPerspective1(painter, cameraRect, mOptions.nOverlayAngle);
    }
    if (mOptions.bPerspective2)
    {
        painter.setWorldTransform(mViewTransform);
        paintOverlayPerspective2(painter, cameraRect, mOptions.nOverlayAngle);
    }
    if (mOptions.bPerspective3)
    {
        painter.setWorldTransform(mViewTransform);
        paintOverlayPerspective3(painter, cameraRect, mOptions.nOverlayAngle);
    }
}

void OverlayPainter::paintOverlayCenter(QPainter &painter, QRect rect)
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

    int offset = OVERLAY_SAFE_CENTER_CROSS_SIZE;
    painter.drawLine(rect.center().x()-offset, rect.center().y(), rect.center().x()+offset, rect.center().y());
    painter.drawLine(rect.center().x(), rect.center().y()-offset, rect.center().x(), rect.center().y()+offset);

    painter.restore();
}

void OverlayPainter::paintOverlayThirds(QPainter &painter, QRect rect)
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
    QPainter::RenderHints previous_renderhints = painter.renderHints();
    painter.setRenderHint(QPainter::Antialiasing, false);

    painter.drawLine(rect.x(), rect.y() + (rect.height()/3), rect.right(), rect.y() + (rect.height()/3));
    painter.drawLine(rect.x(), rect.y() + (rect.height() * 2/3), rect.x() + rect.width(), rect.y() + (rect.height() * 2/3));
    painter.drawLine(rect.x() + rect.width()/3, rect.y(), rect.x() + rect.width()/3, rect.y() + rect.height());
    painter.drawLine(rect.x() + rect.width() *2/3, rect.y(), rect.x() + rect.width() *2/3, rect.y() + rect.height());

    painter.setRenderHints(previous_renderhints);
    painter.restore();
}

void OverlayPainter::paintOverlayGolden(QPainter &painter, QRect rect)
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
    QPainter::RenderHints previous_renderhints = painter.renderHints();
    painter.setRenderHint(QPainter::Antialiasing, false);

    painter.drawLine(rect.x(), static_cast<int>(rect.y() + (rect.height() * 0.38)), rect.right(), static_cast<int>(rect.y() + (rect.height() * 0.38)));
    painter.drawLine(rect.x(), static_cast<int>(rect.y() + (rect.height() * 0.62)), rect.x() + rect.width(), static_cast<int>(rect.y() + (rect.height() * 0.62)));
    painter.drawLine(static_cast<int>(rect.x() + rect.width() * 0.38), rect.y(), static_cast<int>(rect.x() + rect.width() * 0.38), rect.bottom());
    painter.drawLine(static_cast<int>(rect.x() + rect.width() * 0.62), rect.y(), static_cast<int>(rect.x() + rect.width() * 0.62), rect.bottom());

    painter.setRenderHints(previous_renderhints);
    painter.restore();
}

void OverlayPainter::paintOverlaySafeAreas(QPainter &painter, QRect rect)
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
    QPainter::RenderHints previous_renderhints = painter.renderHints();
    painter.setRenderHint(QPainter::TextAntialiasing, true);

    if (mOptions.bActionSafe)
    {
        int action = mOptions.nActionSafe;
        QRect safeAction = QRect(rect.x() + rect.width()*action/200, rect.y() + rect.height()*action/200, rect.width()*(100-action)/100, rect.height()*(100-action)/100);
        painter.drawRect(safeAction);

        if (mOptions.bShowSafeAreaHelperText) {
            painter.drawText(safeAction.x(), safeAction.y()-1, tr("Safe Action area %1 %").arg(action));
        }
    }
    if (mOptions.bTitleSafe)
    {
        int title = mOptions.nTitleSafe;
        QRect safeTitle = QRect(rect.x() + rect.width()*title/200, rect.y() + rect.height()*title/200, rect.width()*(100-title)/100, rect.height()*(100-title)/100);
        painter.drawRect(safeTitle);

        if (mOptions.bShowSafeAreaHelperText) {
            painter.drawText(safeTitle.x(), safeTitle.y()-1, tr("Safe Title area %1 %").arg(title));
        }
    }

    painter.setRenderHints(previous_renderhints);
    painter.restore();
}

void OverlayPainter::paintOverlayPerspective1(QPainter &painter, QRect rect, int angle)
{
    painter.save();
    painter.setCompositionMode(QPainter::CompositionMode_Difference);
    QPen pen(QColor(180, 220, 255));
    pen.setCosmetic(true);
    painter.setPen(pen);
    painter.setWorldMatrixEnabled(true);
    painter.setBrush(Qt::NoBrush);
    QPainter::RenderHints previous_renderhints = painter.renderHints();
    painter.setRenderHint(QPainter::Antialiasing, false);

    qreal degrees = static_cast<qreal>(angle);
    if (degrees == 7.0) { degrees = 7.5; }
    int repeats = static_cast<int>(360 / degrees);
    QPoint center = QPoint(rect.right() - rect.width() / 2, rect.bottom() - rect.height() / 2);
    QLineF angleLine;
    angleLine.setP1(center);
    for (int i = 0; i < repeats; i++)
    {
        angleLine.setAngle(i * degrees);
        angleLine.setLength(rect.width() * 2);
        painter.drawLine(angleLine);
    }

    painter.setRenderHints(previous_renderhints);
    painter.restore();
}

void OverlayPainter::paintOverlayPerspective2(QPainter &painter, QRect rect, int angle)
{
    painter.save();
    painter.setCompositionMode(QPainter::CompositionMode_Difference);
    QPen pen(QColor(180, 220, 255));
    pen.setCosmetic(true);
    painter.setPen(pen);
    painter.setWorldMatrixEnabled(true);
    painter.setBrush(Qt::NoBrush);
    QPainter::RenderHints previous_renderhints = painter.renderHints();
    painter.setRenderHint(QPainter::Antialiasing, false);

    qreal degrees = static_cast<qreal>(angle);
    if (degrees == 7.0) { degrees = 7.5; }
    int repeats = static_cast<int>(180 / degrees);

    mOptions.mLeftPerspPoint = QPoint(rect.left(), 0);
    mOptions.mRightPerspPoint = QPoint(rect.right(), 0);
    QLineF leftAngleLine;
    leftAngleLine.setP1(mOptions.mLeftPerspPoint);
    QLineF rightAngleLine;
    rightAngleLine.setP1(mOptions.mRightPerspPoint);
    for (int i = 0; i <= repeats ; i++)
    {
        leftAngleLine.setAngle(i * degrees - 90);
        leftAngleLine.setLength(rect.width() * 2);
        painter.drawLine(leftAngleLine);
        rightAngleLine.setAngle(i * degrees + 90);
        rightAngleLine.setLength(rect.width() * 2);
        painter.drawLine(rightAngleLine);
    }

    painter.setRenderHints(previous_renderhints);
    painter.restore();
}

void OverlayPainter::paintOverlayPerspective3(QPainter &painter, QRect rect, int angle)
{

}

void OverlayPainter::resetPerspectives()
{

}

