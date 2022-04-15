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
#include "erasertool.h"

#include <QSettings>
#include <QPixmap>
#include <QPainter>

#include "editor.h"
#include "blitrect.h"
#include "scribblearea.h"
#include "strokemanager.h"
#include "layermanager.h"
#include "viewmanager.h"
#include "layervector.h"
#include "vectorimage.h"
#include "pointerevent.h"


EraserTool::EraserTool(QObject* parent) : StrokeTool(parent)
{
}

ToolType EraserTool::type()
{
    return ERASER;
}

void EraserTool::loadSettings()
{
    mPropertyEnabled[WIDTH] = true;
    mPropertyEnabled[USEFEATHER] = true;
    mPropertyEnabled[FEATHER] = true;
    mPropertyEnabled[USEFEATHER] = true;
    mPropertyEnabled[PRESSURE] = true;
    mPropertyEnabled[STABILIZATION] = true;
    mPropertyEnabled[ANTI_ALIASING] = true;

    QSettings settings(PENCIL2D, PENCIL2D);

    properties.width = settings.value("eraserWidth", 24.0).toDouble();
    properties.feather = settings.value("eraserFeather", 48.0).toDouble();
    properties.useFeather = settings.value("eraserUseFeather", true).toBool();
    properties.pressure = settings.value("eraserPressure", true).toBool();
    properties.invisibility = DISABLED;
    properties.preserveAlpha = OFF;
    properties.stabilizerLevel = settings.value("stabilizerLevel", StabilizationLevel::NONE).toInt();
    properties.useAA = settings.value("eraserAA", 1).toInt();

    if (properties.useFeather) { properties.useAA = -1; }

    mQuickSizingProperties.insert(Qt::ShiftModifier, WIDTH);
    mQuickSizingProperties.insert(Qt::ControlModifier, FEATHER);
}

void EraserTool::resetToDefault()
{
    setWidth(24.0);
    setFeather(48.0);
    setUseFeather(true);
    setPressure(true);
    setAA(true);
    setStabilizerLevel(StabilizationLevel::NONE);
}

void EraserTool::setWidth(const qreal width)
{
    // Set current property
    properties.width = width;

    // Update settings
    QSettings settings(PENCIL2D, PENCIL2D);
    settings.setValue("eraserWidth", width);
    settings.sync();
}

void EraserTool::setUseFeather(const bool usingFeather)
{
    // Set current property
    properties.useFeather = usingFeather;

    // Update settings
    QSettings settings(PENCIL2D, PENCIL2D);
    settings.setValue("eraserUseFeather", usingFeather);
    settings.sync();
}

void EraserTool::setFeather(const qreal feather)
{
    // Set current property
    properties.feather = feather;

    // Update settings
    QSettings settings(PENCIL2D, PENCIL2D);
    settings.setValue("eraserFeather", feather);
    settings.sync();
}

void EraserTool::setPressure(const bool pressure)
{
    // Set current property
    properties.pressure = pressure;

    // Update settings
    QSettings settings(PENCIL2D, PENCIL2D);
    settings.setValue("eraserPressure", pressure);
    settings.sync();
}

void EraserTool::setAA(const int AA)
{
    // Set current property
    properties.useAA = AA;

    // Update settings
    QSettings settings(PENCIL2D, PENCIL2D);
    settings.setValue("eraserAA", AA);
    settings.sync();
}

void EraserTool::setStabilizerLevel(const int level)
{
    properties.stabilizerLevel = level;

    QSettings settings(PENCIL2D, PENCIL2D);
    settings.setValue("stabilizerLevel", level);
    settings.sync();
}


QCursor EraserTool::cursor()
{
    return QCursor(QPixmap(":icons/cross.png"), 10, 10);
}

void EraserTool::pointerPressEvent(PointerEvent *event)
{
    startStroke(event->inputType());
    mLastBrushPoint = getCurrentPoint();
    mMouseDownPoint = getCurrentPoint();
}

void EraserTool::pointerMoveEvent(PointerEvent* event)
{
    if (event->buttons() & Qt::LeftButton && event->inputType() == mCurrentInputType)
    {
        mCurrentPressure = strokeManager()->getPressure();
        updateStrokes();
        if (properties.stabilizerLevel != strokeManager()->getStabilizerLevel())
            strokeManager()->setStabilizerLevel(properties.stabilizerLevel);
    }
}

void EraserTool::pointerReleaseEvent(PointerEvent *event)
{
    if (event->inputType() != mCurrentInputType) return;

    mEditor->backup(typeName());

    qreal distance = QLineF(getCurrentPoint(), mMouseDownPoint).length();
    if (distance < 1)
    {
        paintAt(mMouseDownPoint);
    }
    else
    {
        drawStroke();
    }
    removeVectorPaint();
    endStroke();
}

// draw a single paint dab at the given location
void EraserTool::paintAt(QPointF point)
{
    Layer* layer = mEditor->layers()->currentLayer();
    if (layer->type() == Layer::BITMAP)
    {
        qreal pressure = (properties.pressure) ? mCurrentPressure : 1.0;
        qreal opacity = (properties.pressure) ? (mCurrentPressure * 0.5) : 1.0;
        qreal brushWidth = properties.width * pressure;
        mCurrentWidth = brushWidth;

        BlitRect rect(point.toPoint());
        mScribbleArea->drawBrush(point,
                                 brushWidth,
                                 properties.feather,
                                 QColor(255, 255, 255, 255),
                                 opacity,
                                 properties.useFeather,
                                 properties.useAA == ON);

        int rad = qRound(brushWidth / 2 + 2);

        //continuously update buffer to update stroke behind grid.
        mScribbleArea->paintBitmapBufferRect(rect);

        mScribbleArea->refreshBitmap(rect, rad);
    }
}

void EraserTool::drawStroke()
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
        qreal opacity = (properties.pressure) ? (mCurrentPressure * 0.5) : 1.0;
        qreal brushWidth = properties.width * pressure;
        mCurrentWidth = brushWidth;

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
            mScribbleArea->drawBrush(point,
                                     brushWidth,
                                     properties.feather,
                                     Qt::white,
                                     opacity,
                                     properties.useFeather,
                                     properties.useAA == ON);
            if (i == (steps - 1))
            {
                mLastBrushPoint = getCurrentPoint();
            }
        }

        int rad = qRound(brushWidth / 2 + 2);

        mScribbleArea->paintBitmapBufferRect(rect);
        mScribbleArea->refreshBitmap(rect, rad);
    }
    else if (layer->type() == Layer::VECTOR)
    {
        mCurrentWidth = properties.width;
        if (properties.pressure)
        {
            mCurrentWidth = (mCurrentWidth + (strokeManager()->getPressure() * mCurrentWidth)) * 0.5;
        }
        qreal brushWidth = mCurrentWidth;

        QPen pen(Qt::white, brushWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        int rad = qRound(brushWidth) / 2 + 2;

        if (p.size() == 4)
        {
            QPainterPath path(p[0]);
            path.cubicTo(p[1],
                         p[2],
                         p[3]);
            qDebug() << path;
            mScribbleArea->drawPath(path, pen, Qt::NoBrush, QPainter::CompositionMode_Source);
            mScribbleArea->refreshVector(path.boundingRect().toRect(), rad);
        }
    }
}

void EraserTool::removeVectorPaint()
{
    Layer* layer = mEditor->layers()->currentLayer();
    if (layer->type() == Layer::BITMAP)
    {
        mScribbleArea->paintBitmapBuffer();
        mScribbleArea->clearBitmapBuffer();
    }
    else if (layer->type() == Layer::VECTOR)
    {
        mScribbleArea->clearBitmapBuffer();
        VectorImage* vectorImage = static_cast<LayerVector*>(layer)->getLastVectorImageAtFrame(mEditor->currentFrame(), 0);
        if (vectorImage == nullptr) { return; } // Can happen if the first frame is deleted while drawing
        // Clear the area containing the last point
        //vectorImage->removeArea(lastPoint);
        // Clear the temporary pixel path
        vectorImage->deleteSelectedPoints();

        mScribbleArea->setModified(mEditor->layers()->currentLayerIndex(), mEditor->currentFrame());
    }
}

void EraserTool::updateStrokes()
{
    Layer* layer = mEditor->layers()->currentLayer();
    if (layer->type() == Layer::BITMAP || layer->type() == Layer::VECTOR)
    {
        drawStroke();
    }

    if (layer->type() == Layer::VECTOR)
    {
        qreal radius = properties.width / 2;

        VectorImage* currKey = static_cast<VectorImage*>(layer->getLastKeyFrameAtPosition(mEditor->currentFrame()));
        QList<VertexRef> nearbyVertices = currKey->getVerticesCloseTo(getCurrentPoint(), radius);
        for (auto nearbyVertice : nearbyVertices)
        {
            currKey->setSelected(nearbyVertice, true);
        }
    }
}
