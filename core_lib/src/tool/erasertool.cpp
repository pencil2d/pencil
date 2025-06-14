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
#include "layermanager.h"
#include "viewmanager.h"
#include "undoredomanager.h"
#include "layervector.h"
#include "vectorimage.h"
#include "pointerevent.h"


EraserTool::EraserTool(QObject* parent) : StrokeTool(parent)
{
}

ToolType EraserTool::type() const
{
    return ERASER;
}

void EraserTool::loadSettings()
{
    StrokeTool::loadSettings();

    QSettings settings(PENCIL2D, PENCIL2D);

    QHash<int, PropertyInfo> info;

    mPropertyUsed[StrokeSettings::WIDTH_VALUE] = { Layer::BITMAP, Layer::VECTOR };
    mPropertyUsed[StrokeSettings::FEATHER_VALUE] = { Layer::BITMAP };
    mPropertyUsed[StrokeSettings::FEATHER_ENABLED] = { Layer::BITMAP };
    mPropertyUsed[StrokeSettings::PRESSURE_ENABLED] = { Layer::BITMAP, Layer::VECTOR };
    mPropertyUsed[StrokeSettings::STABILIZATION_VALUE] = { Layer::BITMAP, Layer::VECTOR };
    mPropertyUsed[StrokeSettings::ANTI_ALIASING_ENABLED] = { Layer::BITMAP };

    info[StrokeSettings::WIDTH_VALUE] = { WIDTH_MIN, WIDTH_MAX, 24.0 };
    info[StrokeSettings::FEATHER_VALUE] = { FEATHER_MIN, FEATHER_MAX, 48.0 };
    info[StrokeSettings::FEATHER_ENABLED] = true;
    info[StrokeSettings::PRESSURE_ENABLED] = true;
    info[StrokeSettings::STABILIZATION_VALUE] = { StabilizationLevel::NONE, StabilizationLevel::STRONG, StabilizationLevel::NONE };
    info[StrokeSettings::ANTI_ALIASING_ENABLED] = true;

    if (mSettings->requireMigration(settings, 1)) {
        mSettings->setBaseValue(StrokeSettings::WIDTH_VALUE, settings.value("eraserWidth", 24.0).toReal());
        mSettings->setBaseValue(StrokeSettings::FEATHER_VALUE, settings.value("eraserFeather", 48.0).toReal());
        mSettings->setBaseValue(StrokeSettings::STABILIZATION_VALUE, settings.value("stabilizerLevel", StabilizationLevel::NONE).toInt());
        mSettings->setBaseValue(StrokeSettings::FEATHER_ENABLED, settings.value("eraserUseFeather", true).toBool());
        mSettings->setBaseValue(StrokeSettings::PRESSURE_ENABLED, settings.value("eraserPressure", true).toBool());
        mSettings->setBaseValue(StrokeSettings::ANTI_ALIASING_ENABLED, settings.value("eraserAA", true).toBool());

        settings.remove("eraserWidth");
        settings.remove("eraserFeather");
        settings.remove("stabilizerLevel");
        settings.remove("eraserUseFeather");
        settings.remove("eraserPressure");
        settings.remove("eraserAA");
    }

    mQuickSizingProperties.insert(Qt::ShiftModifier, StrokeSettings::WIDTH_VALUE);
    mQuickSizingProperties.insert(Qt::ControlModifier, StrokeSettings::FEATHER_VALUE);
}

QCursor EraserTool::cursor()
{
    return QCursor(QPixmap(":icons/general/cross.png"), 10, 10);
}

void EraserTool::pointerPressEvent(PointerEvent *event)
{
    mInterpolator.pointerPressEvent(event);
    if (handleQuickSizing(event)) {
        return;
    }

    startStroke(event->inputType());
    mLastBrushPoint = getCurrentPoint();
    mMouseDownPoint = getCurrentPoint();

    StrokeTool::pointerPressEvent(event);
}

void EraserTool::pointerMoveEvent(PointerEvent* event)
{
    mInterpolator.pointerMoveEvent(event);
    if (handleQuickSizing(event)) {
        return;
    }

    if (event->buttons() & Qt::LeftButton && event->inputType() == mCurrentInputType)
    {
        mCurrentPressure = mInterpolator.getPressure();
        updateStrokes();
        if (mSettings->stabilizerLevel() != mInterpolator.getStabilizerLevel())
        {
            mInterpolator.setStabilizerLevel(mSettings->stabilizerLevel());
        }
    }

    StrokeTool::pointerMoveEvent(event);
}

void EraserTool::pointerReleaseEvent(PointerEvent *event)
{
    mInterpolator.pointerReleaseEvent(event);
    if (handleQuickSizing(event)) {
        return;
    }

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

    StrokeTool::pointerReleaseEvent(event);
}

// draw a single paint dab at the given location
void EraserTool::paintAt(QPointF point)
{
    Layer* layer = mEditor->layers()->currentLayer();
    if (layer->type() == Layer::BITMAP)
    {
        qreal pressure = (mSettings->pressureEnabled()) ? mCurrentPressure : 1.0;
        qreal opacity = (mSettings->pressureEnabled()) ? (mCurrentPressure * 0.5) : 1.0;
        qreal brushWidth = mSettings->width() * pressure;
        mCurrentWidth = brushWidth;

        mScribbleArea->drawBrush(point,
                                 brushWidth,
                                 mSettings->feather(),
                                 QColor(255, 255, 255, 255),
                                 QPainter::CompositionMode_SourceOver,
                                 opacity,
                                 mSettings->featherEnabled(),
                                 mSettings->AntiAliasingEnabled() == ON);
    }
}

void EraserTool::drawStroke()
{
    StrokeTool::drawStroke();
    QList<QPointF> p = mInterpolator.interpolateStroke();

    Layer* layer = mEditor->layers()->currentLayer();

    if (layer->type() == Layer::BITMAP)
    {
        qreal pressure = (mSettings->pressureEnabled()) ? mCurrentPressure : 1.0;
        qreal opacity = (mSettings->pressureEnabled()) ? (mCurrentPressure * 0.5) : 1.0;
        qreal brushWidth = mSettings->width() * pressure;
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

            mScribbleArea->drawBrush(point,
                                     brushWidth,
                                     mSettings->feather(),
                                     Qt::white,
                                     QPainter::CompositionMode_SourceOver,
                                     opacity,
                                     mSettings->featherEnabled(),
                                     mSettings->AntiAliasingEnabled() == ON);
            if (i == (steps - 1))
            {
                mLastBrushPoint = getCurrentPoint();
            }
        }
    }
    else if (layer->type() == Layer::VECTOR)
    {
        mCurrentWidth = mSettings->width();
        if (mSettings->pressureEnabled())
        {
            mCurrentWidth = (mCurrentWidth + (mInterpolator.getPressure() * mCurrentWidth)) * 0.5;
        }
        qreal brushWidth = mCurrentWidth;

        QPen pen(Qt::white, brushWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);

        if (p.size() == 4)
        {
            QPainterPath path(p[0]);
            path.cubicTo(p[1],
                         p[2],
                         p[3]);
            mScribbleArea->drawPath(path, pen, Qt::NoBrush, QPainter::CompositionMode_Source);
        }
    }
}

void EraserTool::removeVectorPaint()
{
    Layer* layer = mEditor->layers()->currentLayer();
    if (layer->type() == Layer::VECTOR)
    {
        mScribbleArea->clearDrawingBuffer();
        VectorImage* vectorImage = static_cast<LayerVector*>(layer)->getLastVectorImageAtFrame(mEditor->currentFrame(), 0);
        if (vectorImage == nullptr) { return; } // Can happen if the first frame is deleted while drawing
        // Clear the area containing the last point
        //vectorImage->removeArea(lastPoint);
        // Clear the temporary pixel path
        vectorImage->deleteSelectedPoints();

        mEditor->setModified(mEditor->layers()->currentLayerIndex(), mEditor->currentFrame());
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
        qreal radius = mSettings->width() / 2;

        VectorImage* currKey = static_cast<VectorImage*>(layer->getLastKeyFrameAtPosition(mEditor->currentFrame()));
        QList<VertexRef> nearbyVertices = currKey->getVerticesCloseTo(getCurrentPoint(), radius);
        for (auto nearbyVertice : nearbyVertices)
        {
            currKey->setSelected(nearbyVertice, true);
        }
    }
}
