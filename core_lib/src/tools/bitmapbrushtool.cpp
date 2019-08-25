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

#include "bitmapbrushtool.h"

#include <cmath>
#include <QSettings>

#include "editor.h"
#include "colormanager.h"
#include "strokemanager.h"
#include "layermanager.h"
#include "viewmanager.h"
#include "scribblearea.h"
#include "blitrect.h"
#include "pointerevent.h"


BitmapBrushTool::BitmapBrushTool(QObject* parent) : StrokeTool(parent)
{
}

ToolType BitmapBrushTool::type()
{
    return BRUSH;
}

void BitmapBrushTool::loadSettings()
{
    mPropertyEnabled[WIDTH] = true;
    mPropertyEnabled[FEATHER] = true;
    mPropertyEnabled[USEFEATHER] = true;
    mPropertyEnabled[PRESSURE] = true;
    mPropertyEnabled[INVISIBILITY] = true;
    mPropertyEnabled[STABILIZATION] = true;
    mPropertyEnabled[ANTI_ALIASING] = true;

    QSettings settings(PENCIL2D, PENCIL2D);

    properties.width = settings.value("brushWidth", 24.0).toDouble();
    properties.feather = settings.value("brushFeather", 48.0).toDouble();
    properties.useFeather = settings.value("brushUseFeather", true).toBool();
    properties.pressure = settings.value("brushPressure", true).toBool();
    properties.stabilizerLevel = settings.value("brushLineStabilization", StabilizationLevel::STRONG).toInt();
    properties.useAA = settings.value("brushAA", 1).toInt();

    if (properties.useFeather == true) { properties.useAA = -1; }
    if (properties.width <= 0) { setWidth(15); }
    if (std::isnan(properties.feather)) { setFeather(15); }
}

void BitmapBrushTool::resetToDefault()
{
    setWidth(24.0);
    setFeather(48.0);
    setStabilizerLevel(StabilizationLevel::STRONG);
    setUseFeather(true);
}

void BitmapBrushTool::setWidth(const qreal width)
{
    // Set current property
    properties.width = width;

    // Update settings
    QSettings settings(PENCIL2D, PENCIL2D);
    settings.setValue("brushWidth", width);
    settings.sync();
}

void BitmapBrushTool::setUseFeather(const bool usingFeather)
{
    // Set current property
    properties.useFeather = usingFeather;

    // Update settings
    QSettings settings(PENCIL2D, PENCIL2D);
    settings.setValue("brushUseFeather", usingFeather);
    settings.sync();
}

void BitmapBrushTool::setFeather(const qreal feather)
{
    // Set current property
    properties.feather = feather;

    // Update settings
    QSettings settings(PENCIL2D, PENCIL2D);
    settings.setValue("brushFeather", feather);
    settings.sync();
}

void BitmapBrushTool::setPressure(const bool pressure)
{
    // Set current property
    properties.pressure = pressure;

    // Update settings
    QSettings settings(PENCIL2D, PENCIL2D);
    settings.setValue("brushPressure", pressure);
    settings.sync();
}

void BitmapBrushTool::setStabilizerLevel(const int level)
{
    properties.stabilizerLevel = level;

    QSettings settings(PENCIL2D, PENCIL2D);
    settings.setValue("brushLineStabilization", level);
    settings.sync();
}

void BitmapBrushTool::setAA(const int AA)
{
    // Set current property
    properties.useAA = AA;

    // Update settings
    QSettings settings(PENCIL2D, PENCIL2D);
    settings.setValue("brushAA", AA);
    settings.sync();
}

QCursor BitmapBrushTool::cursor()
{
    if (mEditor->preference()->isOn(SETTING::TOOL_CURSOR))
    {
        return QCursor(QPixmap(":icons/brush.png"), 0, 13);
    }
    return Qt::CrossCursor;
}

void BitmapBrushTool::pointerPressEvent(PointerEvent*)
{
    mScribbleArea->setAllDirty();
    mMouseDownPoint = getCurrentPoint();
    mLastBrushPoint = getCurrentPoint();

    startStroke();
}

void BitmapBrushTool::pointerMoveEvent(PointerEvent* event)
{
    if (event->buttons() & Qt::LeftButton)
    {
        mCurrentPressure = strokeManager()->getPressure();
        drawStroke();
        if (properties.stabilizerLevel != strokeManager()->getStabilizerLevel())
            strokeManager()->setStabilizerLevel(properties.stabilizerLevel);
    }
}

void BitmapBrushTool::pointerReleaseEvent(PointerEvent*)
{
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

    paintStroke();
    endStroke();
}

// draw a single paint dab at the given location
void BitmapBrushTool::paintAt(QPointF point)
{
    //qDebug() << "Made a single dab at " << point;
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
                                 mEditor->color()->frontColor(),
                                 opacity,
                                 properties.useFeather,
                                 properties.useAA);

        int rad = qRound(brushWidth) / 2 + 2;
        mScribbleArea->refreshBitmap(rect, rad);
    }
}

void BitmapBrushTool::drawStroke()
{
    StrokeTool::drawStroke();
    QList<QPointF> p = strokeManager()->interpolateStroke();

    auto viewMan = mEditor->view();
    for (int i = 0; i < p.size(); i++)
    {
        p[i] = viewMan->mapScreenToCanvas(p[i]);
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
                                 mEditor->color()->frontColor(),
                                 opacity,
                                 properties.useFeather,
                                 properties.useAA);
        if (i == (steps - 1))
        {
            mLastBrushPoint = getCurrentPoint();
        }
    }

    int rad = qRound(brushWidth / 2 + 2);

    mScribbleArea->paintBitmapBufferRect(rect);
    mScribbleArea->refreshBitmap(rect, rad);

        // Line visualizer
        // for debugging
//        QPainterPath tempPath;

//        QPointF mappedMousePos = mEditor->view()->mapScreenToCanvas(strokeManager()->getMousePos());
//        tempPath.moveTo(getCurrentPoint());
//        tempPath.lineTo(mappedMousePos);

//        QPen pen( Qt::black,
//                   1,
//                   Qt::SolidLine,
//                   Qt::RoundCap,
//                   Qt::RoundJoin );
//        mScribbleArea->drawPolyline(tempPath, pen, true);

}

void BitmapBrushTool::paintStroke()
{
    mScribbleArea->paintBitmapBuffer();
    mScribbleArea->setAllDirty();
    mScribbleArea->clearBitmapBuffer();
}
