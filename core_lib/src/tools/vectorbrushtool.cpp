/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2012-2018 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/

#include "vectorbrushtool.h"

#include <cmath>
#include <QSettings>

#include "colormanager.h"
#include "strokemanager.h"
#include "layermanager.h"
#include "viewmanager.h"
#include "selectionmanager.h"
#include "scribblearea.h"
#include "pointerevent.h"

#include "beziercurve.h"
#include "vectorimage.h"
#include "layervector.h"
#include "editor.h"


VectorBrushTool::VectorBrushTool(QObject* parent) : StrokeTool(parent)
{
}

void VectorBrushTool::loadSettings()
{
    mPropertyEnabled[WIDTH] = true;
    mPropertyEnabled[PRESSURE] = true;
    mPropertyEnabled[INVISIBILITY] = true;
    mPropertyEnabled[STABILIZATION] = true;

    QSettings settings(PENCIL2D, PENCIL2D);

    properties.width = settings.value("brushWidth", 24.0).toDouble();
    properties.pressure = settings.value("brushPressure", true).toBool();
    properties.invisibility = settings.value("brushInvisibility", false).toBool();
    properties.stabilizerLevel = settings.value("brushLineStabilization", StabilizationLevel::STRONG).toInt();

    if (properties.width <= 0) { setWidth(1); }
}

void VectorBrushTool::resetToDefault()
{
    setWidth(24.0);
    setFeather(48.0);
    setStabilizerLevel(StabilizationLevel::STRONG);
}

void VectorBrushTool::setWidth(const qreal width)
{
    // Set current property
    properties.width = width;

    // Update settings
    QSettings settings(PENCIL2D, PENCIL2D);
    settings.setValue("brushWidth", width);
    settings.sync();
}

void VectorBrushTool::setInvisibility(const bool invisibility)
{
    // force value
    properties.invisibility = invisibility;

    QSettings settings(PENCIL2D, PENCIL2D);
    settings.setValue("brushInvisibility", invisibility);
    settings.sync();
}

void VectorBrushTool::setPressure(const bool pressure)
{
    // Set current property
    properties.pressure = pressure;

    // Update settings
    QSettings settings(PENCIL2D, PENCIL2D);
    settings.setValue("brushPressure", pressure);
    settings.sync();
}

void VectorBrushTool::setStabilizerLevel(const int level)
{
    properties.stabilizerLevel = level;

    QSettings settings(PENCIL2D, PENCIL2D);
    settings.setValue("brushLineStabilization", level);
    settings.sync();
}

QCursor VectorBrushTool::cursor()
{
    if (mEditor->preference()->isOn(SETTING::TOOL_CURSOR))
    {
        return QCursor(QPixmap(":icons/brush.png"), 0, 13);
    }
    return Qt::CrossCursor;
}

void VectorBrushTool::pointerPressEvent(PointerEvent*)
{
    mScribbleArea->setAllDirty();
    mMouseDownPoint = getCurrentPoint();
    mLastBrushPoint = getCurrentPoint();

    Q_ASSERT(mEditor->layers()->currentLayer()->type() == Layer::VECTOR);

    startStroke();
}

void VectorBrushTool::pointerMoveEvent(PointerEvent* event)
{
    if (event->buttons() & Qt::LeftButton)
    {
        mCurrentPressure = strokeManager()->getPressure();
        drawStroke();
        if (properties.stabilizerLevel != strokeManager()->getStabilizerLevel())
            strokeManager()->setStabilizerLevel(properties.stabilizerLevel);
    }
}

void VectorBrushTool::pointerReleaseEvent(PointerEvent*)
{
    mEditor->backup(typeName());

    drawStroke();
    paintStroke();
    endStroke();
}

void VectorBrushTool::drawStroke()
{
    StrokeTool::drawStroke();
    QList<QPointF> p = strokeManager()->interpolateStroke();

    qreal pressure = (properties.pressure) ? mCurrentPressure : 1;
    qreal brushWidth = properties.width * pressure;

    int rad = qRound((brushWidth / 2 + 2) * mEditor->view()->scaling());

    QPen pen(mEditor->color()->frontColor(),
             brushWidth * mEditor->view()->scaling(),
             Qt::SolidLine,
             Qt::RoundCap,
             Qt::RoundJoin);

    if (p.size() == 4)
    {
        QPainterPath path(p[0]);
        path.cubicTo(p[1], p[2], p[3]);

        mScribbleArea->drawPath(path, pen, Qt::NoBrush, QPainter::CompositionMode_Source);
        mScribbleArea->refreshVector(path.boundingRect().toRect(), rad);
    }
}

// This function uses the points from DrawStroke
// and turns them into vector lines.
void VectorBrushTool::paintStroke()
{
    if (mStrokePoints.empty())
        return;

    Layer* layer = mEditor->layers()->currentLayer();

    if (mStrokePoints.size() > -1)
    {
        // Clear the temporary pixel path
        mScribbleArea->clearBitmapBuffer();
        qreal tol = mScribbleArea->getCurveSmoothing() / mEditor->view()->scaling();

        BezierCurve curve(mStrokePoints, mStrokePressures, tol);
        curve.setWidth(properties.width);
        curve.setFeather(properties.feather);
        curve.setFilled(false);
        curve.setInvisibility(properties.invisibility);
        curve.setVariableWidth(properties.pressure);
        curve.setColourNumber(mEditor->color()->frontColorNumber());

        VectorImage* vectorImage = static_cast<VectorImage*>(layer->getLastKeyFrameAtPosition(mEditor->currentFrame()));
        vectorImage->addCurve(curve, mEditor->view()->scaling(), false);

        if (vectorImage->isAnyCurveSelected() || mEditor->select()->somethingSelected())
        {
            mEditor->deselectAll();
        }

        vectorImage->setSelected(vectorImage->getLastCurveNumber(), true);

        mScribbleArea->setModified(mEditor->layers()->currentLayerIndex(), mEditor->currentFrame());
        mScribbleArea->setAllDirty();
    }
}
