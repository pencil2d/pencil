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

#include "polylinetool.h"

#include <QSettings>
#include "editor.h"
#include "scribblearea.h"

#include "layermanager.h"
#include "colormanager.h"
#include "viewmanager.h"
#include "undoredomanager.h"
#include "pointerevent.h"
#include "layervector.h"
#include "layerbitmap.h"
#include "vectorimage.h"


PolylineTool::PolylineTool(QObject* parent) : StrokeTool(parent)
{
}

ToolType PolylineTool::type()
{
    return POLYLINE;
}

void PolylineTool::loadSettings()
{
    StrokeTool::loadSettings();

    mPropertyEnabled[WIDTH] = true;
    mPropertyEnabled[BEZIER] = true;
    mPropertyEnabled[CLOSEDPATH] = true;
    mPropertyEnabled[ANTI_ALIASING] = true;
    mPropertyEnabled[SNAPTOANGLE] = true;

    QSettings settings(PENCIL2D, PENCIL2D);

    properties.width = settings.value("polyLineWidth", 8.0).toDouble();
    properties.feather = -1;
    properties.useFeather = false;
    properties.pressure = false;
    properties.invisibility = OFF;
    properties.preserveAlpha = OFF;
    properties.closedPolylinePath = settings.value("closedPolylinePath").toBool();
    properties.useAA = settings.value("brushAA").toBool();
    properties.stabilizerLevel = -1;

    mQuickSizingProperties.insert(Qt::ShiftModifier, WIDTH);
}

void PolylineTool::saveSettings()
{
    QSettings settings(PENCIL2D, PENCIL2D);

    settings.setValue("polyLineWidth", properties.width);
    settings.setValue("brushAA", properties.useAA);
    settings.setValue("closedPolylinePath", properties.closedPolylinePath);

    settings.sync();
}

void PolylineTool::resetToDefault()
{
    setWidth(8.0);
    setBezier(false);
    setClosedPath(false);
}

void PolylineTool::setWidth(const qreal width)
{
    // Set current property
    properties.width = width;
}

void PolylineTool::setFeather(const qreal feather)
{
    Q_UNUSED(feather);
    properties.feather = -1;
}

void PolylineTool::setAA(const int AA)
{
    // Set current property
    properties.useAA = AA;
}

void PolylineTool::setClosedPath(const bool closed)
{
    BaseTool::setClosedPath(closed);
}

bool PolylineTool::leavingThisTool()
{
    StrokeTool::leavingThisTool();
    if (mPoints.size() > 0)
    {
        cancelPolyline();
    }
    return true;
}

bool PolylineTool::isActive() const
{
    return !mPoints.isEmpty();
}

QCursor PolylineTool::cursor()
{
    return QCursor(QPixmap(":icons/general/cross.png"), 10, 10);
}

void PolylineTool::clearToolData()
{
    if (mPoints.empty()) {
        return;
    }

    mPoints.clear();
    emit isActiveChanged(POLYLINE, false);

    // Clear the in-progress polyline from the bitmap buffer.
    mScribbleArea->clearDrawingBuffer();
    mScribbleArea->updateFrame();
}

void PolylineTool::pointerPressEvent(PointerEvent* event)
{
    mInterpolator.pointerPressEvent(event);
    if (handleQuickSizing(event)) {
        return;
    }

    Layer* layer = mEditor->layers()->currentLayer();

    if (event->button() == Qt::LeftButton)
    {
        if (layer->type() == Layer::BITMAP || layer->type() == Layer::VECTOR)
        {
            mScribbleArea->handleDrawingOnEmptyFrame();

            if (layer->type() == Layer::VECTOR)
            {
                VectorImage* vectorImage = static_cast<LayerVector*>(layer)->getLastVectorImageAtFrame(mEditor->currentFrame(), 0);
                Q_CHECK_PTR(vectorImage);
                vectorImage->deselectAll();
                if (mScribbleArea->makeInvisible() && !mEditor->preference()->isOn(SETTING::INVISIBLE_LINES))
                {
                    mScribbleArea->toggleThinLines();
                }
            }

            QPointF currentPoint = getCurrentPoint();
            if (properties.snapAngleState && !mPoints.isEmpty()) {
                currentPoint = getSnappedPoint(mPoints.last(), currentPoint);
            }

            mPoints << currentPoint;
            emit isActiveChanged(POLYLINE, true);
        }
    }

    StrokeTool::pointerPressEvent(event);
}

void PolylineTool::pointerMoveEvent(PointerEvent* event)
{
    mInterpolator.pointerMoveEvent(event);
    if (handleQuickSizing(event)) {
        return;
    }

    Layer* layer = mEditor->layers()->currentLayer();
    if (layer->type() == Layer::BITMAP || layer->type() == Layer::VECTOR)
    {
        QPointF currentPoint = getCurrentPoint();
        if (properties.snapAngleState && !mPoints.isEmpty()) {
            currentPoint = getSnappedPoint(mPoints.last(), currentPoint);
        }
        drawPolyline(mPoints, currentPoint);

    }

    StrokeTool::pointerMoveEvent(event);
}

void PolylineTool::pointerReleaseEvent(PointerEvent* event)
{
    mInterpolator.pointerReleaseEvent(event);
    if (handleQuickSizing(event)) {
        return;
    }

    StrokeTool::pointerReleaseEvent(event);
}

void PolylineTool::pointerDoubleClickEvent(PointerEvent* event)
{
    mInterpolator.pointerPressEvent(event);
    // include the current point before ending the line.
    mPoints << getCurrentPoint();

    const UndoSaveState* saveState = mEditor->undoRedo()->state(UndoRedoRecordType::KEYFRAME_MODIFY);
    mEditor->backup(typeName());

    endPolyline(mPoints);
    mEditor->undoRedo()->record(saveState, typeName());
}

void PolylineTool::removeLastPolylineSegment()
{
    if (mPoints.size() > 1)
    {
        mPoints.removeLast();
        drawPolyline(mPoints, getCurrentPoint());
    }
    else if (mPoints.size() == 1)
    {
        cancelPolyline();
        clearToolData();
    }
}

bool PolylineTool::keyPressEvent(QKeyEvent* event)
{
    switch (event->key())
    {
    case Qt::Key_Control:
        mClosedPathOverrideEnabled = true;
        drawPolyline(mPoints, getCurrentPoint());
        return true;
        break;

    case Qt::Key_Return:
        if (mPoints.size() > 0)
        {
            const UndoSaveState* saveState = mEditor->undoRedo()->state(UndoRedoRecordType::KEYFRAME_MODIFY);
            endPolyline(mPoints);
            mEditor->undoRedo()->record(saveState, typeName());
            return true;
        }
        break;
    case Qt::Key_Backspace:
        if (mPoints.size() > 0)
        {
            removeLastPolylineSegment();
            return true;
        }
    case Qt::Key_Escape:
        if (mPoints.size() > 0)
        {
            cancelPolyline();
            return true;
        }
        break;

    default:
        break;
    }

    return BaseTool::keyPressEvent(event);
}

bool PolylineTool::keyReleaseEvent(QKeyEvent* event)
{
    switch (event->key())
    {
    case Qt::Key_Control:
        mClosedPathOverrideEnabled = false;
        drawPolyline(mPoints, getCurrentPoint());
        return true;
        break;

    default:
        break;
    }

    return BaseTool::keyReleaseEvent(event);
}


QPointF PolylineTool::getSnappedPoint(const QPointF& lastPoint, const QPointF& currentPoint)
{
    QPointF delta = currentPoint - lastPoint;
    qreal angle = qAtan2(delta.y(), delta.x());
    qreal snapAngle = qDegreesToRadians(properties.snapAngleDegrees);
    angle = qRound(angle / snapAngle) * snapAngle;
    qreal length = qSqrt(delta.x() * delta.x() + delta.y() * delta.y());
    return lastPoint + QPointF(qCos(angle) * length, qSin(angle) * length);
}

void PolylineTool::drawPolyline(QList<QPointF> points, QPointF endPoint)
{
    if (points.size() > 0)
    {
        if (properties.snapAngleState && !points.isEmpty()) {
            endPoint = getSnappedPoint(points.last(), endPoint);
        }

        QPen pen(mEditor->color()->frontColor(),
                 properties.width,
                 Qt::SolidLine,
                 Qt::RoundCap,
                 Qt::RoundJoin);
        Layer* layer = mEditor->layers()->currentLayer();

        // Bitmap by default
        QPainterPath tempPath;
        if (properties.bezier_state)
        {
            tempPath = BezierCurve(points).getSimplePath();
        }
        else
        {
            tempPath = BezierCurve(points).getStraightPath();
        }
        tempPath.lineTo(endPoint);

        // Ctrl key inverts closed behavior while held (XOR)
        if ((properties.closedPolylinePath == !mClosedPathOverrideEnabled) && points.size() > 1)
        {
            tempPath.closeSubpath();
        }

        // Vector otherwise
        if (layer->type() == Layer::VECTOR)
        {
            if (mEditor->layers()->currentLayer()->type() == Layer::VECTOR)
            {
                if (mScribbleArea->makeInvisible() == true)
                {
                    pen.setWidth(0);
                    pen.setStyle(Qt::DotLine);
                }
                else
                {
                    pen.setWidth(properties.width);
                }
            }
        }

        mScribbleArea->drawPolyline(tempPath, pen, properties.useAA);
    }
}


void PolylineTool::cancelPolyline()
{
    clearToolData();
}

void PolylineTool::endPolyline(QList<QPointF> points)
{
    Layer* layer = mEditor->layers()->currentLayer();

    if (layer->type() == Layer::VECTOR)
    {
        BezierCurve curve = BezierCurve(points, properties.bezier_state);
        if (mScribbleArea->makeInvisible() == true)
        {
            curve.setWidth(0);
        }
        else
        {
            curve.setWidth(properties.width);
        }
        curve.setColorNumber(mEditor->color()->frontColorNumber());
        curve.setVariableWidth(false);
        curve.setInvisibility(mScribbleArea->makeInvisible());

        VectorImage* vectorImage = static_cast<LayerVector*>(layer)->getLastVectorImageAtFrame(mEditor->currentFrame(), 0);
        if (vectorImage == nullptr) { return; } // Can happen if the first frame is deleted while drawing
        vectorImage->addCurve(curve, mEditor->view()->scaling());
    }
    if (layer->type() == Layer::BITMAP)
    {
        drawPolyline(points, points.last());
    }

    mScribbleArea->endStroke();
    mEditor->setModified(mEditor->layers()->currentLayerIndex(), mEditor->currentFrame());

    clearToolData();
}
