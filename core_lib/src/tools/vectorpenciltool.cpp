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
#include "vectorpenciltool.h"

#include <QSettings>
#include <QPixmap>
#include "pointerevent.h"

#include "layermanager.h"
#include "colormanager.h"
#include "strokemanager.h"
#include "viewmanager.h"
#include "preferencemanager.h"
#include "selectionmanager.h"

#include "editor.h"
#include "scribblearea.h"
#include "blitrect.h"
#include "layervector.h"
#include "vectorimage.h"


VectorPencilTool::VectorPencilTool(QObject* parent) : StrokeTool(parent)
{
}

void VectorPencilTool::loadSettings()
{
    mPropertyEnabled[WIDTH] = true;
    mPropertyEnabled[PRESSURE] = true;
    mPropertyEnabled[VECTORMERGE] = false;
    mPropertyEnabled[STABILIZATION] = true;
    mPropertyEnabled[FILLCONTOUR] = true;

    QSettings settings(PENCIL2D, PENCIL2D);
    properties.width = settings.value("pencilWidth", 4).toDouble();
    properties.pressure = settings.value("pencilPressure", true).toBool();
    properties.stabilizerLevel = settings.value("pencilLineStabilization", StabilizationLevel::STRONG).toInt();
    properties.useAA = DISABLED;
    properties.useFillContour = false;
}

void VectorPencilTool::resetToDefault()
{
    setWidth(4.0);
    setStabilizerLevel(StabilizationLevel::STRONG);
}

void VectorPencilTool::setWidth(const qreal width)
{
    // Set current property
    properties.width = width;

    // Update settings
    QSettings settings(PENCIL2D, PENCIL2D);
    settings.setValue("pencilWidth", width);
    settings.sync();
}

void VectorPencilTool::setInvisibility(const bool)
{
    // force value
    properties.invisibility = 1;
}

void VectorPencilTool::setPressure(const bool pressure)
{
    // Set current property
    properties.pressure = pressure;

    // Update settings
    QSettings settings(PENCIL2D, PENCIL2D);
    settings.setValue("pencilPressure", pressure);
    settings.sync();
}

void VectorPencilTool::setStabilizerLevel(const int level)
{
    properties.stabilizerLevel = level;

    QSettings settings(PENCIL2D, PENCIL2D);
    settings.setValue("pencilLineStabilization", level);
    settings.sync();
}

void VectorPencilTool::setUseFillContour(const bool useFillContour)
{
    properties.useFillContour = useFillContour;

    QSettings settings(PENCIL2D, PENCIL2D);
    settings.setValue("FillContour", useFillContour);
    settings.sync();
}

QCursor VectorPencilTool::cursor()
{
    if (mEditor->preference()->isOn(SETTING::TOOL_CURSOR))
    {
        return QCursor(QPixmap(":icons/pencil2.png"), 0, 16);
    }
    return Qt::CrossCursor;
}

void VectorPencilTool::pointerPressEvent(PointerEvent*)
{
    mScribbleArea->setAllDirty();

    mMouseDownPoint = getCurrentPoint();
    mLastBrushPoint = getCurrentPoint();

    startStroke();

    Q_ASSERT(mEditor->layers()->currentLayer()->type() == Layer::VECTOR);

    // note: why are we doing this on device press event?
    if ( !mEditor->preference()->isOn(SETTING::INVISIBLE_LINES) )
    {
        mScribbleArea->toggleThinLines();
    }
}

void VectorPencilTool::pointerMoveEvent(PointerEvent* event)
{
    if (event->buttons() & Qt::LeftButton)
    {
        mCurrentPressure = strokeManager()->getPressure();
        drawStroke();
        if (properties.stabilizerLevel != strokeManager()->getStabilizerLevel())
            strokeManager()->setStabilizerLevel(properties.stabilizerLevel);
    }
}

void VectorPencilTool::pointerReleaseEvent(PointerEvent*)
{
    mEditor->backup(typeName());
    drawStroke();

    Layer* layer = mEditor->layers()->currentLayer();
    paintVectorStroke(layer);
    endStroke();
}

void VectorPencilTool::drawStroke()
{
    StrokeTool::drawStroke();
    QList<QPointF> p = strokeManager()->interpolateStroke();

    mCurrentWidth = 0; // FIXME: WTF?
    QPen pen(mEditor->color()->frontColor(),
             1,
             Qt::DotLine,
             Qt::RoundCap,
             Qt::RoundJoin);

    int rad = qRound((mCurrentWidth / 2 + 2) * mEditor->view()->scaling());

    if (p.size() == 4)
    {
        QPainterPath path(p[0]);
        path.cubicTo(p[1],
                     p[2],
                     p[3]);
        mScribbleArea->drawPath(path, pen, Qt::NoBrush, QPainter::CompositionMode_Source);
        mScribbleArea->refreshVector(path.boundingRect().toRect(), rad);
    }
}

void VectorPencilTool::paintVectorStroke(Layer* layer)
{
    if (mStrokePoints.empty())
        return;

    // Clear the temporary pixel path
    mScribbleArea->clearBitmapBuffer();
    qreal tol = mScribbleArea->getCurveSmoothing() / mEditor->view()->scaling();

    BezierCurve curve(mStrokePoints, mStrokePressures, tol);
    curve.setWidth(0);
    curve.setFeather(0);
    curve.setFilled(false);
    curve.setInvisibility(true);
    curve.setVariableWidth(false);
    curve.setColourNumber(mEditor->color()->frontColorNumber());
    VectorImage* vectorImage = ((LayerVector *)layer)->getLastVectorImageAtFrame(mEditor->currentFrame(), 0);

    vectorImage->addCurve(curve, qAbs(mEditor->view()->scaling()), properties.vectorMergeEnabled);

    if (properties.useFillContour)
    {
        vectorImage->fillContour(mStrokePoints,
                                 mEditor->color()->frontColorNumber());
    }

    if (vectorImage->isAnyCurveSelected() || mEditor->select()->somethingSelected())
    {
        mEditor->deselectAll();
    }

    // select last/newest curve
    vectorImage->setSelected(vectorImage->getLastCurveNumber(), true);

    // TODO: selection doesn't apply on enter

    mScribbleArea->setModified(mEditor->layers()->currentLayerIndex(), mEditor->currentFrame());
    mScribbleArea->setAllDirty();
}
