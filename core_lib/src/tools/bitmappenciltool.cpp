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
#include "bitmappenciltool.h"

#include <QSettings>
#include "pointerevent.h"

#include "layermanager.h"
#include "colormanager.h"
#include "strokemanager.h"
#include "preferencemanager.h"
#include "selectionmanager.h"

#include "editor.h"
#include "scribblearea.h"
#include "blitrect.h"


BitmapPencilTool::BitmapPencilTool(QObject* parent) : StrokeTool(parent)
{
}

QCursor BitmapPencilTool::cursor()
{
    if (mEditor->preference()->isOn(SETTING::TOOL_CURSOR))
    {
        return QCursor(QPixmap(":icons/pencil2.png"), 0, 16);
    }
    return Qt::CrossCursor;
}


void BitmapPencilTool::loadSettings()
{
    mPropertyEnabled[WIDTH] = true;
    mPropertyEnabled[PRESSURE] = true;
    mPropertyEnabled[VECTORMERGE] = false;
    mPropertyEnabled[STABILIZATION] = true;
    mPropertyEnabled[FILLCONTOUR] = true;

    QSettings settings(PENCIL2D, PENCIL2D);
    properties.width = settings.value("pencilWidth", 4).toDouble();
    properties.feather = 50;
    properties.pressure = settings.value("pencilPressure", true).toBool();
    properties.stabilizerLevel = settings.value("pencilLineStabilization", StabilizationLevel::STRONG).toInt();
    properties.useFeather = true;
}

void BitmapPencilTool::resetToDefault()
{
    setWidth(4.0);
    setFeather(50);
    setUseFeather(true);
    setStabilizerLevel(StabilizationLevel::STRONG);
}

void BitmapPencilTool::setWidth(const qreal width)
{
    // Set current property
    properties.width = width;

    // Update settings
    QSettings settings(PENCIL2D, PENCIL2D);
    settings.setValue("pencilWidth", width);
    settings.sync();
}

void BitmapPencilTool::setFeather(const qreal feather)
{
    properties.feather = feather;
}

void BitmapPencilTool::setUseFeather(const bool usingFeather)
{
    // Set current property
    properties.useFeather = usingFeather;

    // Update settings
    QSettings settings(PENCIL2D, PENCIL2D);
    settings.setValue("brushUseFeather", usingFeather);
    settings.sync();
}

void BitmapPencilTool::setPressure(const bool pressure)
{
    // Set current property
    properties.pressure = pressure;

    // Update settings
    QSettings settings(PENCIL2D, PENCIL2D);
    settings.setValue("pencilPressure", pressure);
    settings.sync();
}

void BitmapPencilTool::setPreserveAlpha(const bool preserveAlpha)
{
    // force value
    Q_UNUSED( preserveAlpha );
    properties.preserveAlpha = 0;
}

void BitmapPencilTool::setStabilizerLevel(const int level)
{
    properties.stabilizerLevel = level;

    QSettings settings(PENCIL2D, PENCIL2D);
    settings.setValue("pencilLineStabilization", level);
    settings.sync();
}

void BitmapPencilTool::pointerPressEvent(PointerEvent*)
{
    mScribbleArea->setAllDirty();

    mMouseDownPoint = getCurrentPoint();
    mLastBrushPoint = getCurrentPoint();

    startStroke();

    // note: why are we doing this on device press event?
    if ( !mEditor->preference()->isOn(SETTING::INVISIBLE_LINES) )
    {
        mScribbleArea->toggleThinLines();
    }
}

void BitmapPencilTool::pointerMoveEvent(PointerEvent* event)
{
    if (event->buttons() & Qt::LeftButton)
    {
        mCurrentPressure = strokeManager()->getPressure();
        drawStroke();
        if (properties.stabilizerLevel != strokeManager()->getStabilizerLevel())
            strokeManager()->setStabilizerLevel(properties.stabilizerLevel);
    }
}

void BitmapPencilTool::pointerReleaseEvent(PointerEvent*)
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
void BitmapPencilTool::paintAt(QPointF point)
{
    qreal opacity = (properties.pressure) ? (mCurrentPressure * 0.5) : 1.0;
    qreal pressure = (properties.pressure) ? mCurrentPressure : 1.0;
    qreal brushWidth = properties.width * pressure;
    qreal fixedBrushFeather = properties.feather;

    mCurrentWidth = brushWidth;

    BlitRect rect(point.toPoint());
    mScribbleArea->drawPencil(point,
                              brushWidth,
                              fixedBrushFeather,
                              mEditor->color()->frontColor(),
                              opacity);

    int rad = qRound(brushWidth) / 2 + 2;
    mScribbleArea->refreshBitmap(rect, rad);
}


void BitmapPencilTool::drawStroke()
{
    StrokeTool::drawStroke();
    QList<QPointF> p = strokeManager()->interpolateStroke();

    qreal pressure = (properties.pressure) ? mCurrentPressure : 1.0;
    qreal opacity = (properties.pressure) ? (mCurrentPressure * 0.5) : 1.0;
    qreal brushWidth = properties.width * pressure;
    mCurrentWidth = brushWidth;

    qreal fixedBrushFeather = properties.feather;
    qreal brushStep = qMax(1.0, (0.5 * brushWidth));

    BlitRect rect;

    QPointF a = mLastBrushPoint;
    QPointF b = getCurrentPoint();

    qreal distance = 4 * QLineF(b, a).length();
    int steps = qRound(distance / brushStep);

    for (int i = 0; i < steps; i++)
    {
        QPointF point = mLastBrushPoint + (i + 1) * brushStep * (getCurrentPoint() - mLastBrushPoint) / distance;
        rect.extend(point.toPoint());
        mScribbleArea->drawPencil(point,
                                  brushWidth,
                                  fixedBrushFeather,
                                  mEditor->color()->frontColor(),
                                  opacity);

        if (i == (steps - 1))
        {
            mLastBrushPoint = getCurrentPoint();
        }
    }

    int rad = qRound(brushWidth) / 2 + 2;

    mScribbleArea->paintBitmapBufferRect(rect);
    mScribbleArea->refreshBitmap(rect, rad);
}


void BitmapPencilTool::paintStroke()
{
    mScribbleArea->paintBitmapBuffer();
    mScribbleArea->setAllDirty();
    mScribbleArea->clearBitmapBuffer();
}
