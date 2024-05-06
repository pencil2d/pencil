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
#include "smudgetool.h"
#include <QPixmap>
#include <QSettings>

#include "pointerevent.h"
#include "vectorimage.h"
#include "editor.h"
#include "scribblearea.h"

#include "layermanager.h"
#include "viewmanager.h"
#include "selectionmanager.h"

#include "layerbitmap.h"
#include "layervector.h"
#include "blitrect.h"

SmudgeTool::SmudgeTool(QObject* parent) : StrokeTool(parent)
{
    toolMode = 0; // tool mode
}

ToolType SmudgeTool::type()
{
    return SMUDGE;
}

void SmudgeTool::loadSettings()
{
    StrokeTool::loadSettings();

    mPropertyEnabled[WIDTH] = true;
    mPropertyEnabled[FEATHER] = true;

    QSettings settings(PENCIL2D, PENCIL2D);
    properties.width = settings.value("smudgeWidth", 24.0).toDouble();
    properties.feather = settings.value("smudgeFeather", 48.0).toDouble();
    properties.pressure = false;
    properties.stabilizerLevel = -1;

    mQuickSizingProperties.insert(Qt::ShiftModifier, WIDTH);
    mQuickSizingProperties.insert(Qt::ControlModifier, FEATHER);
}

void SmudgeTool::resetToDefault()
{
    setWidth(24.0);
    setFeather(48.0);
}

void SmudgeTool::setWidth(const qreal width)
{
    // Set current property
    properties.width = width;

    // Update settings
    QSettings settings(PENCIL2D, PENCIL2D);
    settings.setValue("smudgeWidth", width);
    settings.sync();
}

void SmudgeTool::setFeather(const qreal feather)
{
    // Set current property
    properties.feather = feather;

    // Update settings
    QSettings settings(PENCIL2D, PENCIL2D);
    settings.setValue("smudgeFeather", feather);
    settings.sync();
}

void SmudgeTool::setPressure(const bool pressure)
{
    // Set current property
    properties.pressure = pressure;

    // Update settings
    QSettings settings(PENCIL2D, PENCIL2D);
    settings.setValue("smudgePressure", pressure);
    settings.sync();
}

bool SmudgeTool::emptyFrameActionEnabled()
{
    // Disabled till we get it working for vector layers...
    return false;
}

QCursor SmudgeTool::cursor()
{
    if (toolMode == 0) { //normal mode
        return QCursor(QPixmap(":icons/general/cursor-smudge.svg"), 4, 18);

    }
    else { // blured mode
        return QCursor(QPixmap(":icons/general/cursor-smudge-liquify.svg"), 4, 18);
    }
}

bool SmudgeTool::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Alt)
    {
        toolMode = 1; // alternative mode
        mScribbleArea->setCursor(cursor()); // update cursor
        return true;
    }
    return StrokeTool::keyPressEvent(event);
}

bool SmudgeTool::keyReleaseEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Alt)
    {
        toolMode = 0; // default mode
        mScribbleArea->setCursor(cursor()); // update cursor
        return true;
    }
    return StrokeTool::keyReleaseEvent(event);
}

void SmudgeTool::pointerPressEvent(PointerEvent* event)
{
    mInterpolator.pointerPressEvent(event);
    if (handleQuickSizing(event)) {
        return;
    }

    Layer* layer = mEditor->layers()->currentLayer();
    auto selectMan = mEditor->select();
    if (layer == nullptr) { return; }

    if (event->button() == Qt::LeftButton)
    {
        startStroke(event->inputType());
        if (layer->type() == Layer::BITMAP)
        {
            mLastBrushPoint = getCurrentPoint();
        }
        else if (layer->type() == Layer::VECTOR)
        {
            const int currentFrame = mEditor->currentFrame();
            const float distanceFrom = selectMan->selectionTolerance();
            VectorImage* vectorImage = static_cast<LayerVector*>(layer)->getLastVectorImageAtFrame(currentFrame, 0);
            if (vectorImage == nullptr) { return; }
            selectMan->setCurves(vectorImage->getCurvesCloseTo(getCurrentPoint(), distanceFrom));
            selectMan->setVertices(vectorImage->getVerticesCloseTo(getCurrentPoint(), distanceFrom));
;
            if (selectMan->closestCurves().size() > 0 || selectMan->closestCurves().size() > 0)      // the user clicks near a vertex or a curve
            {
                // Since startStroke() isn't called, handle empty frame behaviour here.
                // Commented out for now - leads to segfault on mouse-release event.
//                if(emptyFrameActionEnabled())
//                {
//                    mScribbleArea->handleDrawingOnEmptyFrame();
//                }

                //qDebug() << "closestCurves:" << closestCurves << " | closestVertices" << closestVertices;
                if (event->modifiers() != Qt::ShiftModifier && !vectorImage->isSelected(selectMan->closestVertices()))
                {
                    mEditor->deselectAll();
                }

                vectorImage->setSelected(selectMan->closestVertices(), true);
                selectMan->vectorSelection.add(selectMan->closestCurves());
                selectMan->vectorSelection.add(selectMan->closestVertices());

                mEditor->frameModified(mEditor->currentFrame());
            }
            else
            {
                mEditor->deselectAll();
            }
        }
    }

    StrokeTool::pointerPressEvent(event);
}

void SmudgeTool::pointerMoveEvent(PointerEvent* event)
{
    mInterpolator.pointerMoveEvent(event);
    if (handleQuickSizing(event)) {
        return;
    }

    if (event->inputType() != mCurrentInputType) return;

    Layer* layer = mEditor->layers()->currentLayer();
    if (layer == nullptr) { return; }

    if (layer->type() != Layer::BITMAP && layer->type() != Layer::VECTOR)
    {
        return;
    }

    auto selectMan = mEditor->select();
    if (event->buttons() & Qt::LeftButton)   // the user is also pressing the mouse (dragging) {
    {
        if (layer->type() == Layer::BITMAP)
        {
            drawStroke();
        }
        else //if (layer->type() == Layer::VECTOR)
        {
            if (event->modifiers() != Qt::ShiftModifier)    // (and the user doesn't press shift)
            {
                VectorImage* vectorImage = static_cast<LayerVector*>(layer)->getLastVectorImageAtFrame(mEditor->currentFrame(), 0);
                if (vectorImage == nullptr) { return; }
                // transforms the selection

                BlitRect blit;

                // Use the previous dirty bound and extend it with the current dirty bound
                // this ensures that we won't get painting artifacts
                blit.extend(vectorImage->getBoundsOfTransformedCurves().toRect());
                selectMan->setSelectionTransform(QTransform().translate(offsetFromPressPos().x(), offsetFromPressPos().y()));
                vectorImage->setSelectionTransformation(selectMan->selectionTransform());
                blit.extend(vectorImage->getBoundsOfTransformedCurves().toRect());

                // And now tell the widget to update the portion in local coordinates
                mScribbleArea->update(mEditor->view()->mapCanvasToScreen(blit).toRect().adjusted(-1, -1, 1, 1));
            }
        }
    }
    else     // the user is moving the mouse without pressing it
    {
        if (layer->type() == Layer::VECTOR)
        {
            VectorImage* vectorImage = static_cast<LayerVector*>(layer)->getLastVectorImageAtFrame(mEditor->currentFrame(), 0);
            if (vectorImage == nullptr) { return; }

            selectMan->setVertices(vectorImage->getVerticesCloseTo(getCurrentPoint(), selectMan->selectionTolerance()));
            mScribbleArea->update();
        }
    }

    StrokeTool::pointerMoveEvent(event);
}

void SmudgeTool::pointerReleaseEvent(PointerEvent* event)
{
    mInterpolator.pointerReleaseEvent(event);
    if (handleQuickSizing(event)) {
        return;
    }

    if (event->inputType() != mCurrentInputType) return;

    Layer* layer = mEditor->layers()->currentLayer();
    if (layer == nullptr) { return; }

    if (event->button() == Qt::LeftButton)
    {
        mEditor->backup(typeName());

        if (layer->type() == Layer::BITMAP)
        {
            drawStroke();
            mScribbleArea->paintBitmapBuffer();
            mScribbleArea->clearDrawingBuffer();
            endStroke();
        }
        else if (layer->type() == Layer::VECTOR)
        {
            VectorImage *vectorImage = ((LayerVector *)layer)->getLastVectorImageAtFrame(mEditor->currentFrame(), 0);
            if (vectorImage == nullptr) { return; }
            vectorImage->applySelectionTransformation();

            auto selectMan = mEditor->select();
            selectMan->resetSelectionTransform();
            for (int k = 0; k < selectMan->vectorSelection.curve.size(); k++)
            {
                int curveNumber = selectMan->vectorSelection.curve.at(k);
                vectorImage->curve(curveNumber).smoothCurve();
            }
            mEditor->setModified(mEditor->layers()->currentLayerIndex(), mEditor->currentFrame());
        }
    }

    StrokeTool::pointerReleaseEvent(event);
}

void SmudgeTool::drawStroke()
{
    Layer* layer = mEditor->layers()->currentLayer();
    if (layer == nullptr || !layer->isPaintable()) { return; }

    BitmapImage *sourceImage = static_cast<LayerBitmap*>(layer)->getLastBitmapImageAtFrame(mEditor->currentFrame(), 0);
    if (sourceImage == nullptr) { return; } // Can happen if the first frame is deleted while drawing
    BitmapImage targetImage = sourceImage->copy();
    StrokeTool::drawStroke();
    QList<QPointF> p = mInterpolator.interpolateStroke();

    for (int i = 0; i < p.size(); i++)
    {
        p[i] = mEditor->view()->mapScreenToCanvas(p[i]);
    }

    qreal opacity = 1.0;
    mCurrentWidth = properties.width;
    qreal brushWidth = mCurrentWidth + 0.0 * properties.feather;
    qreal offset = qMax(0.0, mCurrentWidth - 0.5 * properties.feather) / brushWidth;
    //opacity = currentPressure; // todo: Probably not interesting?!
    //brushWidth = brushWidth * opacity;

    QPointF a = mLastBrushPoint;
    QPointF b = getCurrentPoint();


    if (toolMode == 1) // liquify hard
    {
        qreal brushStep = 2;
        qreal distance = QLineF(b, a).length() / 2.0;
        int steps = qRound(distance / brushStep);

        QPointF sourcePoint = mLastBrushPoint;
        for (int i = 0; i < steps; i++)
        {
            targetImage.paste(&mScribbleArea->mTiledBuffer);
            QPointF targetPoint = mLastBrushPoint + (i + 1) * (brushStep) * (b - mLastBrushPoint) / distance;
            mScribbleArea->liquifyBrush(&targetImage,
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
        }
    }
    else // liquify smooth
    {
        qreal brushStep = 2.0;
        qreal distance = QLineF(b, a).length();
        int steps = qRound(distance / brushStep);

        QPointF sourcePoint = mLastBrushPoint;
        for (int i = 0; i < steps; i++)
        {
            targetImage.paste(&mScribbleArea->mTiledBuffer);
            QPointF targetPoint = mLastBrushPoint + (i + 1) * (brushStep) * (b - mLastBrushPoint) / distance;
            mScribbleArea->blurBrush(&targetImage,
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
        }
    }
}

QPointF SmudgeTool::offsetFromPressPos()
{
    return getCurrentPoint() - getCurrentPressPoint();
}

