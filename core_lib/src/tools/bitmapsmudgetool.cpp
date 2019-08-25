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
#include "bitmapsmudgetool.h"

#include "pointerevent.h"
#include "editor.h"
#include "scribblearea.h"

#include "layermanager.h"
#include "strokemanager.h"
#include "viewmanager.h"
#include "selectionmanager.h"

#include "layerbitmap.h"
#include "blitrect.h"

BitmapSmudgeTool::BitmapSmudgeTool(QObject* parent) : StrokeTool(parent)
{
    toolMode = 0; // tool mode
}

ToolType BitmapSmudgeTool::type()
{
    return SMUDGE;
}

void BitmapSmudgeTool::loadSettings()
{
    mPropertyEnabled[WIDTH] = true;
    mPropertyEnabled[FEATHER] = true;
    mPropertyEnabled[ANTI_ALIASING] = true;

    QSettings settings(PENCIL2D, PENCIL2D);
    properties.width = settings.value("smudgeWidth", 24.0).toDouble();
    properties.feather = settings.value("smudgeFeather", 48.0).toDouble();
}

void BitmapSmudgeTool::resetToDefault()
{
    setWidth(24.0);
    setFeather(48.0);
}

void BitmapSmudgeTool::setWidth(const qreal width)
{
    // Set current property
    properties.width = width;

    // Update settings
    QSettings settings(PENCIL2D, PENCIL2D);
    settings.setValue("smudgeWidth", width);
    settings.sync();
}

void BitmapSmudgeTool::setFeather(const qreal feather)
{
    // Set current property
    properties.feather = feather;

    // Update settings
    QSettings settings(PENCIL2D, PENCIL2D);
    settings.setValue("smudgeFeather", feather);
    settings.sync();
}

void BitmapSmudgeTool::setPressure(const bool pressure)
{
    // Set current property
    properties.pressure = pressure;

    // Update settings
    QSettings settings(PENCIL2D, PENCIL2D);
    settings.setValue("smudgePressure", pressure);
    settings.sync();
}

bool BitmapSmudgeTool::emptyFrameActionEnabled()
{
    // Disabled till we get it working for vector layers...
    return false;
}

QCursor BitmapSmudgeTool::cursor()
{
    if (toolMode == 0) { //normal mode
        return QCursor(QPixmap(":icons/smudge.png"), 0, 16);
    }
    else { // blured mode
        return QCursor(QPixmap(":icons/liquify.png"), -4, 16);
    }
}

bool BitmapSmudgeTool::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Alt)
    {
        toolMode = 1; // alternative mode
        mScribbleArea->setCursor(cursor()); // update cursor
        return true;
    }
    return false;
}

bool BitmapSmudgeTool::keyReleaseEvent(QKeyEvent*)
{
    toolMode = 0; // default mode
    mScribbleArea->setCursor(cursor()); // update cursor

    return true;
}

void BitmapSmudgeTool::pointerPressEvent(PointerEvent* event)
{
    Layer* layer = mEditor->layers()->currentLayer();
    if (layer == nullptr) { return; }

    if (event->button() == Qt::LeftButton)
    {
        mScribbleArea->setAllDirty();
        startStroke();
        mLastBrushPoint = getCurrentPoint();
    }
}

void BitmapSmudgeTool::pointerMoveEvent(PointerEvent* event)
{
    Layer* layer = mEditor->layers()->currentLayer();
    if (layer == nullptr) { return; }

    if (event->buttons() & Qt::LeftButton)   // the user is also pressing the mouse (dragging) {
    {
        drawStroke();
    }
    mScribbleArea->update();
    mScribbleArea->setAllDirty();
}

void BitmapSmudgeTool::pointerReleaseEvent(PointerEvent* event)
{
    Layer* layer = mEditor->layers()->currentLayer();
    if (layer == nullptr) { return; }

    if (event->button() == Qt::LeftButton)
    {
        mEditor->backup(typeName());
        drawStroke();
        mScribbleArea->setAllDirty();
        endStroke();
    }
}

void BitmapSmudgeTool::drawStroke()
{
    if (!mScribbleArea->isLayerPaintable()) return;

    Layer* layer = mEditor->layers()->currentLayer();
    if (layer == nullptr) { return; }

    StrokeTool::drawStroke();

    BitmapImage *targetImage = static_cast<LayerBitmap*>(layer)->getLastBitmapImageAtFrame(mEditor->currentFrame(), 0);
    QList<QPointF> p = strokeManager()->interpolateStroke();

    for (int i = 0; i < p.size(); i++)
    {
        p[i] = mEditor->view()->mapScreenToCanvas(p[i]);
    }

    qreal opacity = 1.0;
    mCurrentWidth = properties.width;
    qreal brushWidth = mCurrentWidth + 0.0 * properties.feather;
    qreal offset = qMax(0.0, mCurrentWidth - 0.5 * properties.feather) / brushWidth;

    BlitRect rect;
    QPointF a = mLastBrushPoint;
    QPointF b = getCurrentPoint();


    if (toolMode == 1) // liquify hard
    {
        qreal brushStep = 2;
        qreal distance = QLineF(b, a).length() / 2.0;
        int steps = qRound(distance / brushStep);
        int rad = qRound(brushWidth / 2.0) + 2;

        QPointF sourcePoint = mLastBrushPoint;
        for (int i = 0; i < steps; i++)
        {
            QPointF targetPoint = mLastBrushPoint + (i + 1) * (brushStep) * (b - mLastBrushPoint) / distance;
            rect.extend(targetPoint.toPoint());
            mScribbleArea->liquifyBrush(targetImage,
                                        sourcePoint,
                                        targetPoint,
                                        brushWidth,
                                        offset,
                                        opacity);

            if (i == (steps - 1))
            {
                mLastBrushPoint = targetPoint;
            }
            sourcePoint = targetPoint;
            mScribbleArea->paintBitmapBufferRect(rect);
            mScribbleArea->refreshBitmap(rect, rad);
        }
    }
    else // liquify smooth
    {
        qreal brushStep = 2.0;
        qreal distance = QLineF(b, a).length();
        int steps = qRound(distance / brushStep);
        int rad = qRound(brushWidth / 2.0) + 2;

        QPointF sourcePoint = mLastBrushPoint;
        for (int i = 0; i < steps; i++)
        {
            QPointF targetPoint = mLastBrushPoint + (i + 1) * (brushStep) * (b - mLastBrushPoint) / distance;
            rect.extend(targetPoint.toPoint());
            mScribbleArea->blurBrush(targetImage,
                                     sourcePoint,
                                     targetPoint,
                                     brushWidth,
                                     offset,
                                     opacity);

            if (i == (steps - 1))
            {
                mLastBrushPoint = targetPoint;
            }
            sourcePoint = targetPoint;
            mScribbleArea->paintBitmapBufferRect(rect);
            mScribbleArea->refreshBitmap(rect, rad);
        }
    }
}

QPointF BitmapSmudgeTool::offsetFromPressPos()
{
    return getCurrentPoint() - getCurrentPressPoint();
}

