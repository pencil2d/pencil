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
#include "vectorpentool.h"

#include <QPixmap>

#include "vectorimage.h"
#include "layervector.h"
#include "colormanager.h"
#include "strokemanager.h"
#include "layermanager.h"
#include "viewmanager.h"
#include "selectionmanager.h"
#include "editor.h"
#include "scribblearea.h"
#include "blitrect.h"
#include "pointerevent.h"


VectorPenTool::VectorPenTool(QObject* parent) : StrokeTool(parent)
{
}

void VectorPenTool::loadSettings()
{
    mPropertyEnabled[WIDTH] = true;
    mPropertyEnabled[PRESSURE] = true;
    mPropertyEnabled[VECTORMERGE] = true;
    mPropertyEnabled[STABILIZATION] = true;

    QSettings settings(PENCIL2D, PENCIL2D);

    properties.width = settings.value("penWidth", 12.0).toDouble();
    properties.pressure = settings.value("penPressure", true).toBool();
    properties.invisibility = OFF;
    properties.stabilizerLevel = settings.value("penLineStabilization", StabilizationLevel::STRONG).toInt();
}

void VectorPenTool::resetToDefault()
{
    setWidth(12.0);
    setPressure(true);
    setStabilizerLevel(StabilizationLevel::STRONG);
}

void VectorPenTool::setWidth(const qreal width)
{
    // Set current property
    properties.width = width;

    // Update settings
    QSettings settings(PENCIL2D, PENCIL2D);
    settings.setValue("penWidth", width);
    settings.sync();
}

void VectorPenTool::setPressure(const bool pressure)
{
    // Set current property
    properties.pressure = pressure;

    // Update settings
    QSettings settings(PENCIL2D, PENCIL2D);
    settings.setValue("penPressure", pressure);
    settings.sync();
}

void VectorPenTool::setStabilizerLevel(const int level)
{
    properties.stabilizerLevel = level;

    QSettings settings(PENCIL2D, PENCIL2D);
    settings.setValue("penLineStabilization", level);
    settings.sync();
}

QCursor VectorPenTool::cursor()
{
    if (mEditor->preference()->isOn(SETTING::TOOL_CURSOR))
    {
        return QCursor(QPixmap(":icons/pen.png"), -5, 0);
    }
    return Qt::CrossCursor;
}

void VectorPenTool::pointerPressEvent(PointerEvent *)
{
    mScribbleArea->setAllDirty();

    mMouseDownPoint = getCurrentPoint();
    mLastBrushPoint = getCurrentPoint();

    Q_ASSERT(mEditor->layers()->currentLayer()->type() != Layer::VECTOR);

    startStroke();
}

void VectorPenTool::pointerMoveEvent(PointerEvent* event)
{
    if (event->buttons() & Qt::LeftButton)
    {
        mCurrentPressure = strokeManager()->getPressure();
        drawStroke();
        if (properties.stabilizerLevel != strokeManager()->getStabilizerLevel())
            strokeManager()->setStabilizerLevel(properties.stabilizerLevel);
    }
}

void VectorPenTool::pointerReleaseEvent(PointerEvent*)
{
    mEditor->backup(typeName());

    Layer* layer = mEditor->layers()->currentLayer();

    drawStroke();
    paintVectorStroke(layer);
    endStroke();
}

void VectorPenTool::drawStroke()
{
    StrokeTool::drawStroke();
    QList<QPointF> p = strokeManager()->interpolateStroke();

    qreal pressure = (properties.pressure) ? mCurrentPressure : 1.0;
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

void VectorPenTool::paintVectorStroke(Layer* layer)
{
    if (mStrokePoints.empty())
        return;

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

    auto pLayerVector = static_cast<LayerVector*>(layer);
    VectorImage* vectorImage = pLayerVector->getLastVectorImageAtFrame(mEditor->currentFrame(), 0);
    vectorImage->addCurve(curve, mEditor->view()->scaling(), false);

    if (vectorImage->isAnyCurveSelected() || mEditor->select()->somethingSelected())
    {
        mEditor->deselectAll();
    }

    vectorImage->setSelected(vectorImage->getLastCurveNumber(), true);

    mScribbleArea->setModified(mEditor->layers()->currentLayerIndex(), mEditor->currentFrame());
    mScribbleArea->setAllDirty();
}
