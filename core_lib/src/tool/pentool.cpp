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
#include "pentool.h"

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


PenTool::PenTool(QObject* parent) : StrokeTool(parent)
{
}

void PenTool::loadSettings()
{
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
        return QCursor(QPixmap(":icons/pen.png"), -5, 0);
    }
    return Qt::CrossCursor;
}

void PenTool::pointerPressEvent(PointerEvent *)
{
    mScribbleArea->setAllDirty();

    mMouseDownPoint = getCurrentPoint();
    mLastBrushPoint = getCurrentPoint();

    startStroke();
}

void PenTool::pointerMoveEvent(PointerEvent* event)
{
    if (event->buttons() & Qt::LeftButton)
    {
        mCurrentPressure = strokeManager()->getPressure();
        drawStroke();
        if (properties.stabilizerLevel != strokeManager()->getStabilizerLevel())
            strokeManager()->setStabilizerLevel(properties.stabilizerLevel);
    }
}

void PenTool::pointerReleaseEvent(PointerEvent*)
{
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

    if (layer->type() == Layer::BITMAP)
        paintBitmapStroke();
    else if (layer->type() == Layer::VECTOR)
        paintVectorStroke(layer);
    endStroke();
}

// draw a single paint dab at the given location
void PenTool::paintAt(QPointF point)
{
    //qDebug() << "Made a single dab at " << point;

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

        int rad = qRound(brushWidth) / 2 + 2;

        BlitRect rect(point.toPoint());
        mScribbleArea->refreshBitmap(rect, rad);
    }
}

void PenTool::drawStroke()
{
    StrokeTool::drawStroke();
    QList<QPointF> p = strokeManager()->interpolateStroke();

    Layer* layer = mEditor->layers()->currentLayer();

    if (layer->type() == Layer::BITMAP)
    {
        for (int i = 0; i < p.size(); i++)
        {
            p[i] = mEditor->view()->mapScreenToCanvas(p[i]);
        }

        qreal pressure = (properties.pressure) ? mCurrentPressure : 1.0;
        qreal brushWidth = properties.width * pressure;
        mCurrentWidth = brushWidth;

        // TODO: Make popup widget for less important properties,
        // Eg. stepsize should be a slider.. will have fixed (0.3) value for now.
        qreal brushStep = (0.5 * brushWidth);
        brushStep = qMax(1.0, brushStep);

        BlitRect rect;

        QPointF a = mLastBrushPoint;
        QPointF b = getCurrentPoint();

        qreal distance = 4 * QLineF(b, a).length();
        int steps = qRound(distance / brushStep);

        for (int i = 0; i < steps; i++)
        {
            QPointF point = mLastBrushPoint + (i + 1) * brushStep * (getCurrentPoint() - mLastBrushPoint) / distance;
            rect.extend(point.toPoint());
            mScribbleArea->drawPen(point,
                                   brushWidth,
                                   mEditor->color()->frontColor(),
                                   properties.useAA);

            if (i == (steps - 1))
            {
                mLastBrushPoint = getCurrentPoint();
            }
        }

        int rad = qRound(brushWidth) / 2 + 2;

        mScribbleArea->paintBitmapBufferRect(rect);
        mScribbleArea->refreshBitmap(rect, rad);
    }
    else if (layer->type() == Layer::VECTOR)
    {
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
}

void PenTool::paintBitmapStroke()
{
    mScribbleArea->paintBitmapBuffer();
    mScribbleArea->setAllDirty();
    mScribbleArea->clearBitmapBuffer();
}

void PenTool::paintVectorStroke(Layer* layer)
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
