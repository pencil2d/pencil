#include "overlaypainter.h"
#include <QDebug>
#include <QSettings>

#include "pencildef.h"


Q_CONSTEXPR static int OVERLAY_SAFE_CENTER_CROSS_SIZE = 25;
Q_CONSTEXPR static qreal LINELENGTHFACTOR = 2.0;
Q_CONSTEXPR static int LR_DIFF = 10;         // minimum difference for Left and Right point
Q_CONSTEXPR static int MID_DIFF = 2;         // minimum difference for Middle point
Q_CONSTEXPR static int LEFTANGLEOFFSET = 90;
Q_CONSTEXPR static int RIGHTANGLEOFFSET = -90;


void initPerspectivePainter(QPainter& painter)
{
	painter.setCompositionMode(QPainter::CompositionMode_Difference);
	QPen pen(QColor(180, 220, 255));
	pen.setCosmetic(true);
	painter.setPen(pen);
	painter.setWorldMatrixEnabled(true);
	painter.setBrush(Qt::NoBrush);
}

OverlayPainter::OverlayPainter()
{
}

void OverlayPainter::setViewTransform(const QTransform view)
{
    mViewTransform = view;
}

void OverlayPainter::renderOverlays(QPainter &painter, MoveMode mode)
{
    mMoveMode = mode;
    painter.save();

    if (mOptions.bCenter)
    {
        painter.setWorldTransform(mViewTransform);
        paintOverlayCenter(painter);
    }
    if (mOptions.bThirds)
    {
        painter.setWorldTransform(mViewTransform);
        paintOverlayThirds(painter);
    }
    if (mOptions.bGoldenRatio)
    {
        painter.setWorldTransform(mViewTransform);
        paintOverlayGolden(painter);
    }
    if (mOptions.bSafeArea)
    {
        painter.setWorldTransform(mViewTransform);
        paintOverlaySafeAreas(painter);
    }

    QSettings settings(PENCIL2D, PENCIL2D);
    mOptions.nOverlayAngle = settings.value("OverlayAngle").toInt();

    if (mOptions.bPerspective1)
    {
        painter.setWorldTransform(mViewTransform);
        paintOverlayPerspective1(painter);
    }
    if (mOptions.bPerspective2)
    {
        painter.setWorldTransform(mViewTransform);
        paintOverlayPerspective2(painter);
    }
    if (mOptions.bPerspective3)
    {
        painter.setWorldTransform(mViewTransform);
        paintOverlayPerspective3(painter);
    }
    painter.restore();
}

void OverlayPainter::paintOverlayCenter(QPainter &painter)
{
    QRect rect = mOptions.mRect;

    painter.save();
    painter.setCompositionMode(QPainter::RasterOp_NotSourceAndNotDestination);

    QPen pen(Qt::DashLine);
    qreal space = 10;
    QVector<qreal> dashes;
    dashes << 10 << space << 10 << space << 10 << space;
    pen.setDashPattern(dashes);
    pen.setCosmetic(true);
    painter.setPen(pen);
    painter.setWorldMatrixEnabled(!mOptions.mIsCamera);
    painter.setBrush(Qt::NoBrush);
    painter.setRenderHint(QPainter::Antialiasing, false);

    int offset = OVERLAY_SAFE_CENTER_CROSS_SIZE;
    painter.drawLine(rect.center().x()-offset, rect.center().y(), rect.center().x()+offset, rect.center().y());
    painter.drawLine(rect.center().x(), rect.center().y()-offset, rect.center().x(), rect.center().y()+offset);

    painter.restore();
}

void OverlayPainter::paintOverlayThirds(QPainter &painter)
{
    QRect rect = mOptions.mRect;

    painter.save();
    painter.setCompositionMode(QPainter::RasterOp_NotSourceAndNotDestination);

    QPen pen(Qt::DashLine);
    qreal space = 10;
    QVector<qreal> dashes;
    dashes << 10 << space << 10 << space << 10 << space;
    pen.setDashPattern(dashes);
    pen.setCosmetic(true);
    painter.setPen(pen);
    painter.setWorldMatrixEnabled(!mOptions.mIsCamera);
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

void OverlayPainter::paintOverlayGolden(QPainter &painter)
{
    QRect rect = mOptions.mRect;

    painter.save();
    painter.setCompositionMode(QPainter::RasterOp_NotSourceAndNotDestination);

    QPen pen(Qt::DashLine);
    qreal space = 10;
    QVector<qreal> dashes;
    dashes << 10 << space << 10 << space << 10 << space;
    pen.setDashPattern(dashes);
    pen.setCosmetic(true);
    painter.setPen(pen);
    painter.setWorldMatrixEnabled(!mOptions.mIsCamera);
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

void OverlayPainter::paintOverlaySafeAreas(QPainter &painter)
{
    QRect rect = mOptions.mRect;

    painter.save();
    painter.setCompositionMode(QPainter::RasterOp_NotSourceAndNotDestination);
    QPen pen(Qt::DashLine);
    qreal space = 10;
    QVector<qreal> dashes;
    dashes << 10 << space << 10 << space << 10 << space;
    pen.setDashPattern(dashes);
    pen.setCosmetic(true);
    painter.setPen(pen);
    painter.setWorldMatrixEnabled(!mOptions.mIsCamera);
    painter.setBrush(Qt::NoBrush);
    QPainter::RenderHints previous_renderhints = painter.renderHints();
    painter.setRenderHint(QPainter::TextAntialiasing, true);

    if (mOptions.bActionSafe)
    {
        int action = mOptions.nActionSafe;
        QRect safeAction(rect.x() + rect.width() * action / 200,
                         rect.y() + rect.height() * action/200,
                         rect.width() * (100 - action) / 100,
                         rect.height() * (100 - action) / 100);
        painter.drawRect(safeAction);

        if (mOptions.bShowSafeAreaHelperText)
        {
            painter.drawText(safeAction.x(), safeAction.y() - 1, tr("Safe Action area %1 %").arg(action));
        }
    }
    if (mOptions.bTitleSafe)
    {
        int title = mOptions.nTitleSafe;
        QRect safeTitle(rect.x() + rect.width() * title / 200,
                        rect.y() + rect.height() * title / 200,
                        rect.width() * (100 - title) / 100,
                        rect.height() * (100 - title) / 100);
        painter.drawRect(safeTitle);

        if (mOptions.bShowSafeAreaHelperText)
        {
            painter.drawText(safeTitle.x(), safeTitle.y()-1, tr("Safe Title area %1 %").arg(title));
        }
    }

    painter.setRenderHints(previous_renderhints);
    painter.restore();
}

void OverlayPainter::paintOverlayPerspective1(QPainter& painter)
{
    QRect rect = mOptions.mRect;

    painter.save();
    initPerspectivePainter(painter);
    QPainter::RenderHints previous_renderhints = painter.renderHints();
    painter.setRenderHint(QPainter::Antialiasing, false);

    qreal degrees = static_cast<qreal>(mOptions.nOverlayAngle);
    if (degrees == 7.0) { degrees = 7.5; }
    int repeats = static_cast<int>(360 / degrees);
    QLineF angleLine;
    if (mOptions.mSinglePerspPoint == QPointF(0, 0))    // TODO: bug in QT prevents
        mOptions.mSinglePerspPoint = QPointF(0.1, 0.1); // point to be (0,0)...
    angleLine.setP1(mOptions.mSinglePerspPoint);
    QVector<QLineF> lines;
    for (int i = 0; i < repeats; i++)
    {
        angleLine.setAngle(i * degrees);
        angleLine.setLength(rect.width() * 2.0);
        lines.append(angleLine);
    }
    painter.drawLines(lines);

    painter.setRenderHints(previous_renderhints);
    painter.restore();
}

void OverlayPainter::paintOverlayPerspective2(QPainter& painter)
{
    QRect rect = mOptions.mRect;
    painter.save();
    initPerspectivePainter(painter);
    QPainter::RenderHints previous_renderhints = painter.renderHints();
    painter.setRenderHint(QPainter::Antialiasing, false);

    qreal degrees = static_cast<qreal>(mOptions.nOverlayAngle);
    if (degrees == 7.0) { degrees = 7.5; }
    int repeats = static_cast<int>(180 / degrees);

    if (mOptions.mLeftPerspPoint == QPointF(0.0, 0.0))      // TODO: bug in QT prevents
        mOptions.mLeftPerspPoint = QPointF(0.1, 0.1);       // point to be (0,0)...
    if (mOptions.mRightPerspPoint == QPointF(0.0, 0.0))     // TODO: bug in QT prevents
        mOptions.mRightPerspPoint = QPointF(0.1, 0.1);      // point to be (0,0)...

    switch (mMoveMode) {
    case MoveMode::PERSP_LEFT:
        setRightPoint(QPoint(mOptions.mRightPerspPoint.x(), mOptions.mLeftPerspPoint.y()));
        break;
    case MoveMode::PERSP_RIGHT:
        setLeftPoint(QPoint(mOptions.mLeftPerspPoint.x(), mOptions.mRightPerspPoint.y()));
        break;
    default:
        break;
    }

    QLineF angleLine;
    angleLine.setAngle(LEFTANGLEOFFSET);
    angleLine.setP1(mOptions.mLeftPerspPoint);
    QVector<QLineF> lines;
    for (int i = 0; i <= repeats; i++)
    {
        angleLine.setAngle(LEFTANGLEOFFSET - i * degrees);
        angleLine.setLength(rect.width() * LINELENGTHFACTOR);
        lines.append(angleLine);
    }

    angleLine.setAngle(RIGHTANGLEOFFSET);
    angleLine.setP1(mOptions.mRightPerspPoint);
    for (int i = 0; i <= repeats; i++)
    {
        angleLine.setAngle(RIGHTANGLEOFFSET - i * degrees);
        angleLine.setLength(rect.width() * LINELENGTHFACTOR);
        lines.append(angleLine);
    }
    painter.drawLines(lines);

    painter.setRenderHints(previous_renderhints);
    painter.restore();
}

void OverlayPainter::paintOverlayPerspective3(QPainter& painter)
{
    if (!mOptions.bPerspective2)
        paintOverlayPerspective2(painter);

    QRect rect = mOptions.mRect;
    painter.save();
    initPerspectivePainter(painter);
    QPainter::RenderHints previous_renderhints = painter.renderHints();
    painter.setRenderHint(QPainter::Antialiasing, false);

    qreal degrees = static_cast<qreal>(mOptions.nOverlayAngle);
    if (degrees == 7.0) { degrees = 7.5; }
    int repeats = static_cast<int>(180 / degrees);

    if (mOptions.mMiddlePerspPoint == QPointF(0, 0))    // TODO: bug in QT prevents
        mOptions.mMiddlePerspPoint = QPointF(0.1, 0.1); // point to be (0,0)...

    const int middleAngleOffset = mOptions.mLeftPerspPoint.y() < mOptions.mMiddlePerspPoint.y() ? 180 : 0;

    QLineF angleLine;
    angleLine.setAngle(middleAngleOffset);
    angleLine.setP1(mOptions.mMiddlePerspPoint);
    QVector<QLineF> lines;
    for (int i = 0; i <= repeats; i++)
    {
        angleLine.setAngle(middleAngleOffset - i * degrees);
        angleLine.setLength(rect.width() * LINELENGTHFACTOR);
        lines.append(angleLine);
    }
    painter.drawLines(lines);

    painter.setRenderHints(previous_renderhints);
    painter.restore();
}
