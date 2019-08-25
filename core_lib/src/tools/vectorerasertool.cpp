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
#include "vectorerasertool.h"

#include <QSettings>

#include "editor.h"
#include "scribblearea.h"
#include "strokemanager.h"
#include "layermanager.h"
#include "viewmanager.h"
#include "layervector.h"
#include "vectorimage.h"
#include "pointerevent.h"


VectorEraserTool::VectorEraserTool(QObject* parent) : StrokeTool(parent)
{
}


QCursor VectorEraserTool::cursor()
{
    return Qt::CrossCursor;
}

void VectorEraserTool::loadSettings()
{
    mPropertyEnabled[WIDTH] = true;
    mPropertyEnabled[PRESSURE] = true;
    mPropertyEnabled[STABILIZATION] = true;

    QSettings settings(PENCIL2D, PENCIL2D);

    properties.width = settings.value("eraserWidth", 24.0).toDouble();
    properties.pressure = settings.value("eraserPressure", true).toBool();
    properties.invisibility = DISABLED;
    properties.stabilizerLevel = settings.value("stabilizerLevel", StabilizationLevel::NONE).toInt();
}

void VectorEraserTool::resetToDefault()
{
    setWidth(24.0);
    setPressure(true);
    setStabilizerLevel(StabilizationLevel::NONE);
}

void VectorEraserTool::setWidth(const qreal width)
{
    // Set current property
    properties.width = width;

    // Update settings
    QSettings settings(PENCIL2D, PENCIL2D);
    settings.setValue("eraserWidth", width);
    settings.sync();
}

void VectorEraserTool::setPressure(const bool pressure)
{
    // Set current property
    properties.pressure = pressure;

    // Update settings
    QSettings settings(PENCIL2D, PENCIL2D);
    settings.setValue("eraserPressure", pressure);
    settings.sync();
}

void VectorEraserTool::setStabilizerLevel(const int level)
{
    properties.stabilizerLevel = level;

    QSettings settings(PENCIL2D, PENCIL2D);
    settings.setValue("stabilizerLevel", level);
    settings.sync();
}

void VectorEraserTool::pointerPressEvent(PointerEvent*)
{
    mScribbleArea->setAllDirty();

    startStroke();
    mLastBrushPoint = getCurrentPoint();
    mMouseDownPoint = getCurrentPoint();

    Q_ASSERT(mEditor->layers()->currentLayer()->type() == Layer::VECTOR);
}

void VectorEraserTool::pointerMoveEvent(PointerEvent* event)
{
    if (event->buttons() & Qt::LeftButton)
    {
        mCurrentPressure = strokeManager()->getPressure();
        updateStrokes();
        if (properties.stabilizerLevel != strokeManager()->getStabilizerLevel())
            strokeManager()->setStabilizerLevel(properties.stabilizerLevel);
    }
}

void VectorEraserTool::pointerReleaseEvent(PointerEvent*)
{
    mEditor->backup(typeName());

    drawStroke();
    eraseStroke();
    endStroke();
}

void VectorEraserTool::drawStroke()
{
    StrokeTool::drawStroke();
    QList<QPointF> p = strokeManager()->interpolateStroke();

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
        mScribbleArea->drawPath(path, pen, Qt::NoBrush, QPainter::CompositionMode_Source);
        mScribbleArea->refreshVector(path.boundingRect().toRect(), rad);
    }
}

void VectorEraserTool::eraseStroke()
{
    Layer* layer = mEditor->layers()->currentLayer();
    VectorImage* vectorImage = static_cast<LayerVector*>(layer)->getLastVectorImageAtFrame(mEditor->currentFrame(), 0);
    // Clear the area containing the last point
    //vectorImage->removeArea(lastPoint);
    // Clear the temporary pixel path
    mScribbleArea->clearBitmapBuffer();
    vectorImage->deleteSelectedPoints();

    mScribbleArea->setModified(mEditor->layers()->currentLayerIndex(), mEditor->currentFrame());
    mScribbleArea->setAllDirty();
}

void VectorEraserTool::updateStrokes()
{
    Layer* layer = mEditor->layers()->currentLayer();
    drawStroke();

    qreal radius = properties.width / 2;

    VectorImage* currKey = static_cast<VectorImage*>(layer->getLastKeyFrameAtPosition(mEditor->currentFrame()));
    QList<VertexRef> nearbyVertices = currKey->getVerticesCloseTo(getCurrentPoint(), radius);
    for (auto nearbyVertice : nearbyVertices)
    {
        currKey->setSelected(nearbyVertice, true);
    }
    mScribbleArea->setAllDirty();
}
