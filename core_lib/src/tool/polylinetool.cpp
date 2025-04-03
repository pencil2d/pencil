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

ToolType PolylineTool::type() const
{
    return POLYLINE;
}

void PolylineTool::createSettings(ToolSettings *)
{
    mSettings = new PolylineSettings();
    StrokeTool::createSettings(mSettings);
}

void PolylineTool::loadSettings()
{
    StrokeTool::loadSettings();

    mPropertyUsed[StrokeSettings::WIDTH_VALUE] = { Layer::BITMAP, Layer::VECTOR };
    mPropertyUsed[PolylineSettings::CLOSEDPATH_ENABLED] = { Layer::BITMAP, Layer::VECTOR };
    mPropertyUsed[PolylineSettings::BEZIERPATH_ENABLED] = { Layer::BITMAP };
    mPropertyUsed[StrokeSettings::ANTI_ALIASING_ENABLED] = { Layer::BITMAP };

    QSettings settings(PENCIL2D, PENCIL2D);

    QHash<int, PropertyInfo> info;

    info[StrokeSettings::WIDTH_VALUE] = { WIDTH_MIN, WIDTH_MAX, 8.0 };
    info[PolylineSettings::CLOSEDPATH_ENABLED] = false;
    info[PolylineSettings::BEZIERPATH_ENABLED] = false;
    info[StrokeSettings::ANTI_ALIASING_ENABLED] = true;

    mSettings->load(typeName(), settings, info);

    if (mSettings->requireMigration(settings, 1)) {
        mSettings->setBaseValue(StrokeSettings::WIDTH_VALUE, settings.value("polylineWidth", 8.0).toReal());
        mSettings->setBaseValue(StrokeSettings::ANTI_ALIASING_ENABLED, settings.value("brushAA", true).toBool());
        mSettings->setBaseValue(PolylineSettings::CLOSEDPATH_ENABLED, settings.value("closedPolylinePath", false).toBool());

        settings.remove("polylineWidth");
        settings.remove("brushAA");
        settings.remove("closedPolylinePath");
    }

    mQuickSizingProperties.insert(Qt::ShiftModifier, StrokeSettings::WIDTH_VALUE);
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
            mPoints << getCurrentPoint();
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
        drawPolyline(mPoints, getCurrentPoint());
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

void PolylineTool::drawPolyline(QList<QPointF> points, QPointF endPoint)
{
    if (points.size() > 0)
    {
        QPen pen(mEditor->color()->frontColor(),
                 mSettings->width(),
                 Qt::SolidLine,
                 Qt::RoundCap,
                 Qt::RoundJoin);
        Layer* layer = mEditor->layers()->currentLayer();

        // Bitmap by default
        QPainterPath tempPath;
        if (mSettings->bezierPathEnabled())
        {
            tempPath = BezierCurve(points).getSimplePath();
        }
        else
        {
            tempPath = BezierCurve(points).getStraightPath();
        }
        tempPath.lineTo(endPoint);

        // Ctrl key inverts closed behavior while held (XOR)
        if ((mSettings->closedPathEnabled() == !mClosedPathOverrideEnabled) && points.size() > 1)
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
                    pen.setWidth(mSettings->width());
                }
            }
        }

        mScribbleArea->drawPolyline(tempPath, pen, mSettings->AntiAliasingEnabled());
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
        BezierCurve curve = BezierCurve(points, mSettings->bezierPathEnabled());
        if (mScribbleArea->makeInvisible() == true)
        {
            curve.setWidth(0);
        }
        else
        {
            curve.setWidth(mSettings->width());
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

void PolylineTool::setUseBezier(bool useBezier)
{
    mSettings->setBaseValue(PolylineSettings::BEZIERPATH_ENABLED, useBezier);
    emit bezierPathEnabledChanged(useBezier);
}

void PolylineTool::setClosePath(bool closePath)
{
    mSettings->setBaseValue(PolylineSettings::CLOSEDPATH_ENABLED, closePath);
    emit closePathChanged(closePath);
}
