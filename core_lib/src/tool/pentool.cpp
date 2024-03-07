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
#include "pentool.h"

#include <QPixmap>
#include <QSettings>

#include "vectorimage.h"
#include "layervector.h"
#include "colormanager.h"
#include "layermanager.h"
#include "viewmanager.h"
#include "selectionmanager.h"
#include "editor.h"
#include "scribblearea.h"
#include "blitrect.h"
#include "pointerevent.h"


PenTool::PenTool(QObject* parent) : StrokeTool(parent)
{
}

void PenTool::loadSettings()
{
    StrokeTool::loadSettings();

    mPropertyEnabled[WIDTH] = true;
    mPropertyEnabled[PRESSURE] = true;
    mPropertyEnabled[VECTORMERGE] = true;
    mPropertyEnabled[ANTI_ALIASING] = true;
    mPropertyEnabled[STABILIZATION] = true;

    QSettings settings(PENCIL2D, PENCIL2D);

    properties.width = settings.value("penWidth", 12.0).toDouble();
    properties.pressure = settings.value("penPressure", true).toBool();
    properties.invisibility = OFF;
    properties.preserveAlpha = OFF;
    properties.useAA = settings.value("penAA", true).toBool();
    properties.stabilizerLevel = settings.value("penLineStabilization", StabilizationLevel::STRONG).toInt();

    mQuickSizingProperties.insert(Qt::ShiftModifier, WIDTH);
}

void PenTool::resetToDefault()
{
    setWidth(12.0);
    setUseFeather(false);
    setPressure(true);
    setStabilizerLevel(StabilizationLevel::STRONG);
    setAA(1);
}

void PenTool::setWidth(const qreal width)
{
    // Set current property
    properties.width = width;

    // Update settings
    QSettings settings(PENCIL2D, PENCIL2D);
    settings.setValue("penWidth", width);
    settings.sync();
}

void PenTool::setPressure(const bool pressure)
{
    // Set current property
    properties.pressure = pressure;

    // Update settings
    QSettings settings(PENCIL2D, PENCIL2D);
    settings.setValue("penPressure", pressure);
    settings.sync();
}

void PenTool::setAA(const int AA)
{
    // Set current property
    properties.useAA = AA;

    // Update settings
    QSettings settings(PENCIL2D, PENCIL2D);
    settings.setValue("penAA", AA);
    settings.sync();
}

void PenTool::setStabilizerLevel(const int level)
{
    properties.stabilizerLevel = level;

    QSettings settings(PENCIL2D, PENCIL2D);
    settings.setValue("penLineStabilization", level);
    settings.sync();
}

QCursor PenTool::cursor()
{
    if (mEditor->preference()->isOn(SETTING::TOOL_CURSOR))
    {
        return QCursor(QPixmap(":icons/general/cursor-pen.svg"), 5, 14);
    }
    return QCursor(QPixmap(":icons/general/cross.png"), 10, 10);
}

void PenTool::pointerPressEvent(PointerEvent *event)
{
    mInterpolator.pointerPressEvent(event);
    if (handleQuickSizing(event)) {
        return;
    }

    mMouseDownPoint = getCurrentPoint();
    mLastBrushPoint = getCurrentPoint();

    startStroke(event->inputType());

    StrokeTool::pointerPressEvent(event);
}

void PenTool::pointerMoveEvent(PointerEvent* event)
{
    mInterpolator.pointerMoveEvent(event);
    if (handleQuickSizing(event)) {
        return;
    }

    if (event->buttons() & Qt::LeftButton && event->inputType() == mCurrentInputType)
    {
        mCurrentPressure = mInterpolator.getPressure();
        drawStroke();
        if (properties.stabilizerLevel != mInterpolator.getStabilizerLevel())
        {
            mInterpolator.setStabilizerLevel(properties.stabilizerLevel);
        }
    }

    StrokeTool::pointerMoveEvent(event);
}

void PenTool::pointerReleaseEvent(PointerEvent *event)
{
    mInterpolator.pointerReleaseEvent(event);
    if (handleQuickSizing(event)) {
        return;
    }

    if (event->inputType() != mCurrentInputType) return;

    mEditor->backup(typeName());

    Layer* layer = mEditor->layers()->currentLayer();

    qreal distance = QLineF(getCurrentPoint(), mMouseDownPoint).length();
    if (distance < 1)
    {
        paintAt(mMouseDownPoint);
    }
    else
    {
        drawStroke();
    }

    if (layer->type() == Layer::VECTOR) {
        paintVectorStroke(layer);
    }
    endStroke();

    StrokeTool::pointerReleaseEvent(event);
}

// draw a single paint dab at the given location
void PenTool::paintAt(QPointF point)
{
    Layer* layer = mEditor->layers()->currentLayer();
    if (layer->type() == Layer::BITMAP)
    {
        qreal pressure = (properties.pressure) ? mCurrentPressure : 1.0;
        qreal brushWidth = properties.width * pressure;
        mCurrentWidth = brushWidth;

        mScribbleArea->drawPen(point,
                               brushWidth,
                               mEditor->color()->frontColor(),
                               properties.useAA);
    }
}

void PenTool::drawStroke()
{
    StrokeTool::drawStroke();
    QList<QPointF> p = mInterpolator.interpolateStroke();

    Layer* layer = mEditor->layers()->currentLayer();

    if (layer->type() == Layer::BITMAP)
    {
        qreal pressure = (properties.pressure) ? mCurrentPressure : 1.0;
        qreal brushWidth = properties.width * pressure;
        mCurrentWidth = brushWidth;

        // TODO: Make popup widget for less important properties,
        // Eg. stepsize should be a slider.. will have fixed (0.3) value for now.
        qreal brushStep = (0.5 * brushWidth);
        brushStep = qMax(1.0, brushStep);

        QPointF a = mLastBrushPoint;
        QPointF b = getCurrentPoint();

        qreal distance = 4 * QLineF(b, a).length();
        int steps = qRound(distance / brushStep);

        for (int i = 0; i < steps; i++)
        {
            QPointF point = mLastBrushPoint + (i + 1) * brushStep * (getCurrentPoint() - mLastBrushPoint) / distance;
            mScribbleArea->drawPen(point,
                                   brushWidth,
                                   mEditor->color()->frontColor(),
                                   properties.useAA);

            if (i == (steps - 1))
            {
                mLastBrushPoint = getCurrentPoint();
            }
        }
    }
    else if (layer->type() == Layer::VECTOR)
    {
        qreal pressure = (properties.pressure) ? mCurrentPressure : 1.0;
        qreal brushWidth = properties.width * pressure;

        QPen pen(mEditor->color()->frontColor(),
                 brushWidth,
                 Qt::SolidLine,
                 Qt::RoundCap,
                 Qt::RoundJoin);

        if (p.size() == 4)
        {
            QPainterPath path(p[0]);
            path.cubicTo(p[1], p[2], p[3]);
            mScribbleArea->drawPath(path, pen, Qt::NoBrush, QPainter::CompositionMode_Source);
        }
    }
}

void PenTool::paintVectorStroke(Layer* layer)
{
    if (mStrokePoints.empty())
        return;

    // Clear the temporary pixel path
    mScribbleArea->clearDrawingBuffer();
    qreal tol = mScribbleArea->getCurveSmoothing() / mEditor->view()->scaling();

    BezierCurve curve(mStrokePoints, mStrokePressures, tol);
    curve.setWidth(properties.width);
    curve.setFeather(properties.feather);
    curve.setFilled(false);
    curve.setInvisibility(properties.invisibility);
    curve.setVariableWidth(properties.pressure);
    curve.setColorNumber(mEditor->color()->frontColorNumber());

    auto pLayerVector = static_cast<LayerVector*>(layer);
    VectorImage* vectorImage = pLayerVector->getLastVectorImageAtFrame(mEditor->currentFrame(), 0);
    if (vectorImage == nullptr) { return; } // Can happen if the first frame is deleted while drawing
    vectorImage->addCurve(curve, mEditor->view()->scaling(), false);

    if (vectorImage->isAnyCurveSelected() || mEditor->select()->somethingSelected())
    {
        mEditor->deselectAll();
    }

    vectorImage->setSelected(vectorImage->getLastCurveNumber(), true);

    mEditor->setModified(mEditor->layers()->currentLayerIndex(), mEditor->currentFrame());
}
