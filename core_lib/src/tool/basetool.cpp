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

#include "basetool.h"

#include <array>
#include <QtMath>
#include <QPixmap>
#include "editor.h"
#include "viewmanager.h"
#include "toolmanager.h"
#include "scribblearea.h"
#include "strokemanager.h"
#include "pointerevent.h"

// ---- shared static variables ---- ( only one instance for all the tools )
qreal BaseTool::msOriginalPropertyValue;  // start value (width, feather ..)
bool BaseTool::msIsAdjusting = false;


QString BaseTool::TypeName(ToolType type)
{
    static std::array<QString, TOOL_TYPE_COUNT> map;

    if (map[0].isEmpty())
    {
        map[PENCIL] = tr("Pencil");
        map[ERASER] = tr("Eraser");
        map[SELECT] = tr("Select");
        map[MOVE] = tr("Move");
        map[HAND] = tr("Hand");
        map[SMUDGE] = tr("Smudge");
        map[PEN] = tr("Pen");
        map[POLYLINE] = tr("Polyline");
        map[BUCKET] = tr("Bucket");
        map[EYEDROPPER] = tr("Eyedropper");
        map[BRUSH] = tr("Brush");
    }
    return map.at(type);
}

BaseTool::BaseTool(QObject* parent) : QObject(parent)
{
    mPropertyEnabled.insert(WIDTH, false);
    mPropertyEnabled.insert(FEATHER, false);
    mPropertyEnabled.insert(USEFEATHER, false);
    mPropertyEnabled.insert(PRESSURE, false);
    mPropertyEnabled.insert(INVISIBILITY, false);
    mPropertyEnabled.insert(PRESERVEALPHA, false);
    mPropertyEnabled.insert(BEZIER, false);
    mPropertyEnabled.insert(ANTI_ALIASING, false);
    mPropertyEnabled.insert(FILL_MODE, false);
    mPropertyEnabled.insert(STABILIZATION, false);
}

QCursor BaseTool::cursor()
{
    return Qt::ArrowCursor;
}

void BaseTool::initialize(Editor* editor)
{
    Q_ASSERT(editor);
    mEditor = editor;
    mScribbleArea = editor->getScribbleArea();
    Q_ASSERT(mScribbleArea);

    mStrokeManager = mEditor->getScribbleArea()->getStrokeManager();
    loadSettings();
}

void BaseTool::pointerPressEvent(PointerEvent* event)
{
    event->accept();
}

void BaseTool::pointerMoveEvent(PointerEvent* event)
{
    event->accept();
}

void BaseTool::pointerReleaseEvent(PointerEvent* event)
{
    event->accept();
}

void BaseTool::pointerDoubleClickEvent(PointerEvent* event)
{
    pointerPressEvent(event);
}

/**
 * @brief BaseTool::isDrawingTool - A drawing tool is anything that applies something to the canvas.
 * SELECT and MOVE does not count here because they modify already applied content.
 * @return true if not a drawing tool and false otherwise
 */
bool BaseTool::isDrawingTool()
{
    if (type() == ToolType::HAND || type() == ToolType::MOVE || type() == ToolType::SELECT )
    {
        return false;
    }
    return true;
}

/**
 * @brief precision circular cursor: used for drawing a cursor within scribble area.
 * @return QPixmap
 */
QPixmap BaseTool::canvasCursor(float width, float feather, bool useFeather, float scalingFac, int windowWidth)
{
    float propWidth = width * scalingFac;
    float propFeather = feather * scalingFac;

    float cursorWidth = 0.0f;
    float xyA = 0.0f;
    float xyB = 0.0f;
    float whA = 0.0f;
    float whB = 0.0f;

    if (useFeather)
    {
        cursorWidth = propWidth + 0.5 * propFeather;
        xyA = 1 + propFeather / 2;
        xyB = 1 + propFeather / 8;
        whA = qMax<float>(0, propWidth - xyA - 1);
        whB = qMax<float>(0, cursorWidth - propFeather / 4 - 2);
    }
    else
    {
        cursorWidth = (propWidth + 0.5);
        whA = qMax<float>(0, propWidth - 1);
        whB = qMax<float>(0, cursorWidth / 4 - 2);
    }

    float radius = cursorWidth / 2;

    // deallocate when cursor width gets some value larger than the widget
    if (cursorWidth > windowWidth * 2)
    {
        return QPixmap(0, 0);
    }

    if (cursorWidth < 1) { cursorWidth = 1; }

    QPixmap cursorPixmap = QPixmap(cursorWidth, cursorWidth);
    if (!cursorPixmap.isNull())
    {
        cursorPixmap.fill(QColor(255, 255, 255, 0));
        QPainter cursorPainter(&cursorPixmap);
        QPen cursorPen = cursorPainter.pen();
        cursorPainter.setRenderHint(QPainter::HighQualityAntialiasing);

        // Draw cross in center
        cursorPen.setStyle(Qt::SolidLine);
        cursorPen.setColor(QColor(0, 0, 0, 127));
        cursorPainter.setPen(cursorPen);
        cursorPainter.drawLine(QPointF(radius - 2, radius), QPointF(radius + 2, radius));
        cursorPainter.drawLine(QPointF(radius, radius - 2), QPointF(radius, radius + 2));

        // Draw outer circle
        if (useFeather)
        {
            cursorPen.setStyle(Qt::DotLine);
            cursorPen.setColor(QColor(0, 0, 0, 255));
            cursorPainter.setPen(cursorPen);
            cursorPainter.drawEllipse(QRectF(xyB, xyB, whB, whB));
            cursorPen.setDashOffset(4);
            cursorPen.setColor(QColor(255, 255, 255, 255));
            cursorPainter.setPen(cursorPen);
            cursorPainter.drawEllipse(QRectF(xyB, xyB, whB, whB));
        }

        // Draw inner circle
        cursorPen.setStyle(Qt::DotLine);
        cursorPen.setColor(QColor(0, 0, 0, 255));
        cursorPainter.setPen(cursorPen);
        cursorPainter.drawEllipse(QRectF(xyA, xyA, whA, whA));
        cursorPen.setDashOffset(4);
        cursorPen.setColor(QColor(255, 255, 255, 255));
        cursorPainter.setPen(cursorPen);
        cursorPainter.drawEllipse(QRectF(xyA, xyA, whA, whA));

        cursorPainter.end();
    }
    return cursorPixmap;
}

QCursor BaseTool::selectMoveCursor(MoveMode mode, ToolType type)
{
    QPixmap cursorPixmap = QPixmap(24, 24);
    if (!cursorPixmap.isNull())
    {
        cursorPixmap.fill(QColor(255, 255, 255, 0));
        QPainter cursorPainter(&cursorPixmap);
        cursorPainter.setRenderHint(QPainter::HighQualityAntialiasing);

        switch(mode)
        {
        case MoveMode::PERSP_LEFT:
        case MoveMode::PERSP_RIGHT:
        case MoveMode::PERSP_MIDDLE:
        case MoveMode::PERSP_SINGLE:
        {
            cursorPainter.drawImage(QPoint(6,6),QImage("://icons/new/arrow-selectmove.png"));
            break;
        }
        case MoveMode::MIDDLE:
        {
            if (type == SELECT) {
                cursorPainter.drawImage(QPoint(6,6),QImage("://icons/new/arrow-selectmove.png"));
            } else {
                return Qt::ArrowCursor;
            }
            break;
        }
        case MoveMode::TOPLEFT:
        case MoveMode::BOTTOMRIGHT:
        {
            cursorPainter.drawImage(QPoint(6,6),QImage("://icons/new/arrow-diagonalleft.png"));
            break;
        }
        case MoveMode::TOPRIGHT:
        case MoveMode::BOTTOMLEFT:
        {
            cursorPainter.drawImage(QPoint(6,6),QImage("://icons/new/arrow-diagonalright.png"));
            break;
        }

        default:
            return (type == SELECT) ? Qt::CrossCursor : Qt::ArrowCursor;
            break;
        }
        cursorPainter.end();
    }
    return QCursor(cursorPixmap);
}

bool BaseTool::isActive()
{
    return strokeManager()->isActive();
}

/**
 * @brief precision circular cursor: used for drawing stroke size while adjusting
 * @return QPixmap
 */
QPixmap BaseTool::quickSizeCursor(qreal scalingFac)
{
    qreal propSize = qMax(0., properties.width) * scalingFac;
    qreal propFeather = qMax(0., properties.feather) * scalingFac;
    QRectF cursorRect(0, 0, propSize+2, propSize+2);

    QRectF sizeRect = cursorRect.adjusted(1, 1, -1, -1);
    qreal featherRadius = (1 - propFeather / 100) * propSize / 2.;

    QPixmap cursorPixmap = QPixmap(cursorRect.size().toSize());
    if (!cursorPixmap.isNull())
    {
        cursorPixmap.fill(QColor(255, 255, 255, 0));
        QPainter cursorPainter(&cursorPixmap);
        cursorPainter.setRenderHints(QPainter::Antialiasing, true);

        // Draw width (outside circle)
        cursorPainter.setPen(QColor(255, 127, 127, 127));
        cursorPainter.setBrush(QColor(0, 255, 127, 127));
        cursorPainter.drawEllipse(sizeRect);

        // Draw feather (inside circle)
        cursorPainter.setCompositionMode(QPainter::CompositionMode_Darken);
        cursorPainter.setPen(QColor(0, 0, 0, 0));
        cursorPainter.setBrush(QColor(0, 191, 95, 127));
        cursorPainter.drawEllipse(cursorRect.center(), featherRadius, featherRadius);

        // Draw cursor in center
        cursorPainter.setRenderHints(QPainter::Antialiasing, false);
        cursorPainter.setPen(QColor(0, 0, 0, 255));
        cursorPainter.drawLine(cursorRect.center() - QPoint(2, 0), cursorRect.center() + QPoint(2, 0));
        cursorPainter.drawLine(cursorRect.center() - QPoint(0, 2), cursorRect.center() + QPoint(0, 2));

        cursorPainter.end();
    }
    return cursorPixmap;
}

bool BaseTool::startAdjusting(Qt::KeyboardModifiers modifiers, qreal step)
{
    if (mQuickSizingProperties.contains(modifiers))
    {
        switch (mQuickSizingProperties.value(modifiers)) {
        case WIDTH:
            msOriginalPropertyValue = properties.width;
            break;
        case FEATHER:
            msOriginalPropertyValue = properties.feather;
            break;
        case TOLERANCE:
            msOriginalPropertyValue = properties.tolerance;
            break;
        default:
            qDebug() << "Unhandled quick sizing property for tool" << typeName();
            Q_ASSERT(false);
            return false;
        }

        msIsAdjusting = true;
        mAdjustmentStep = step;
        mScribbleArea->updateCanvasCursor();
        return true;
    }
    return false;
}

void BaseTool::stopAdjusting()
{
    msIsAdjusting = false;
    mAdjustmentStep = 0;
    msOriginalPropertyValue = 0;
    mEditor->getScribbleArea()->updateCanvasCursor();
}

void BaseTool::adjustCursor(Qt::KeyboardModifiers modifiers)
{
    qreal inc = qPow(msOriginalPropertyValue * 100, 0.5);
    qreal newValue = inc + getCurrentPoint().x();

    if (newValue < 0)
    {
        newValue = 0;
    }

    newValue = qPow(newValue, 2) / 100;
    if (mAdjustmentStep > 0)
    {
        int tempValue = static_cast<int>(newValue / mAdjustmentStep); // + 0.5 ?
        newValue = tempValue * mAdjustmentStep;
    }

    switch (mQuickSizingProperties.value(modifiers))
    {
    case WIDTH:
        mEditor->tools()->setWidth(qBound(1., newValue, 200.));
        break;
    case FEATHER:
        mEditor->tools()->setFeather(qBound(2., newValue, 200.));
        break;
    case TOLERANCE:
        mEditor->tools()->setTolerance(qBound(0., newValue, 100.));
        break;
    default:
        qDebug() << "Unhandled quick sizing property for tool" << typeName();
        Q_ASSERT(false);
        break;
    }
}

QPointF BaseTool::getCurrentPressPixel()
{
    return strokeManager()->getCurrentPressPixel();
}

QPointF BaseTool::getCurrentPressPoint()
{
    return mEditor->view()->mapScreenToCanvas(strokeManager()->getCurrentPressPixel());
}

QPointF BaseTool::getCurrentPixel()
{
    return strokeManager()->getCurrentPixel();
}

QPointF BaseTool::getCurrentPoint()
{
    return mEditor->view()->mapScreenToCanvas(getCurrentPixel());
}

QPointF BaseTool::getLastPixel()
{
    return strokeManager()->getLastPixel();
}

QPointF BaseTool::getLastPoint()
{
    return mEditor->view()->mapScreenToCanvas(getLastPixel());
}

QPointF BaseTool::getLastPressPixel()
{
    return strokeManager()->getLastPressPixel();
}

QPointF BaseTool::getLastPressPoint()
{
    return mEditor->view()->mapScreenToCanvas(getLastPressPixel());
}

void BaseTool::setWidth(const qreal width)
{
    properties.width = width;
}

void BaseTool::setFeather(const qreal feather)
{
    properties.feather = feather;
}

void BaseTool::setUseFeather(const bool usingFeather)
{
    properties.useFeather = usingFeather;
}

void BaseTool::setInvisibility(const bool invisibility)
{
    properties.invisibility = invisibility;
}

void BaseTool::setBezier(const bool _bezier_state)
{
    properties.bezier_state = _bezier_state;
}

void BaseTool::setPressure(const bool pressure)
{
    properties.pressure = pressure;
}

void BaseTool::setPreserveAlpha(const bool preserveAlpha)
{
    properties.preserveAlpha = preserveAlpha;
}

void BaseTool::setVectorMergeEnabled(const bool vectorMergeEnabled)
{
    properties.vectorMergeEnabled = vectorMergeEnabled;
}

void BaseTool::setAA(const int useAA)
{
    properties.useAA = useAA;
}

void BaseTool::setFillMode(const int mode)
{
    properties.fillMode = mode;
}

void BaseTool::setStabilizerLevel(const int level)
{
    properties.stabilizerLevel = level;
}

void BaseTool::setTolerance(const int tolerance)
{
    properties.tolerance = tolerance;
}

void BaseTool::setToleranceEnabled(const bool enabled)
{
    properties.toleranceEnabled = enabled;
}

void BaseTool::setFillExpand(const int fillExpandValue)
{
    properties.bucketFillExpand = fillExpandValue;
}

void BaseTool::setFillToLayer(int layerMode)
{
    properties.bucketFillToLayerMode = layerMode;
}

void BaseTool::setFillReferenceMode(int referenceMode)
{
    properties.bucketFillReferenceMode = referenceMode;
}

void BaseTool::setFillExpandEnabled(const bool enabled)
{
    properties.bucketFillExpandEnabled = enabled;
}

void BaseTool::setUseFillContour(const bool useFillContour)
{
    properties.useFillContour = useFillContour;
}

void BaseTool::setShowSelectionInfo(const bool b)
{
    properties.showSelectionInfo = b;
}


