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
#include "vectorsmudgetool.h"
#include <QPixmap>

#include "pointerevent.h"
#include "vectorimage.h"
#include "editor.h"
#include "scribblearea.h"

#include "layermanager.h"
#include "strokemanager.h"
#include "viewmanager.h"
#include "selectionmanager.h"

#include "layerbitmap.h"
#include "layervector.h"
#include "blitrect.h"

VectorSmudgeTool::VectorSmudgeTool(QObject* parent) : StrokeTool(parent)
{
    toolMode = 0; // tool mode
}

void VectorSmudgeTool::loadSettings()
{
    mPropertyEnabled[WIDTH] = true;
    mPropertyEnabled[FEATHER] = true;

    QSettings settings(PENCIL2D, PENCIL2D);
    properties.width = settings.value("smudgeWidth", 24.0).toDouble();
    properties.pressure = false;
    properties.stabilizerLevel = -1;
}

void VectorSmudgeTool::resetToDefault()
{
    setWidth(24.0);
    setFeather(48.0);
}

void VectorSmudgeTool::setWidth(const qreal width)
{
    // Set current property
    properties.width = width;

    // Update settings
    QSettings settings(PENCIL2D, PENCIL2D);
    settings.setValue("smudgeWidth", width);
    settings.sync();
}

void VectorSmudgeTool::setPressure(const bool pressure)
{
    // Set current property
    properties.pressure = pressure;

    // Update settings
    QSettings settings(PENCIL2D, PENCIL2D);
    settings.setValue("smudgePressure", pressure);
    settings.sync();
}

bool VectorSmudgeTool::emptyFrameActionEnabled()
{
    // Disabled till we get it working for vector layers...
    return false;
}

QCursor VectorSmudgeTool::cursor()
{
    if (toolMode == 0) { //normal mode
        return QCursor(QPixmap(":icons/smudge.png"), 0, 16);
    }
    else { // blured mode
        return QCursor(QPixmap(":icons/liquify.png"), -4, 16);
    }
}

bool VectorSmudgeTool::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Alt)
    {
        toolMode = 1; // alternative mode
        mScribbleArea->setCursor(cursor()); // update cursor
        return true;
    }
    return false;
}

bool VectorSmudgeTool::keyReleaseEvent(QKeyEvent*)
{

    toolMode = 0; // default mode
    mScribbleArea->setCursor(cursor()); // update cursor

    return true;
}

void VectorSmudgeTool::pointerPressEvent(PointerEvent* event)
{
    Layer* layer = mEditor->layers()->currentLayer();
    auto selectMan = mEditor->select();
    if (layer == nullptr) { return; }

    Q_ASSERT(mEditor->layers()->currentLayer()->type() == Layer::VECTOR);

    if (event->button() == Qt::LeftButton)
    {
        const int currentFrame = mEditor->currentFrame();
        const float distanceFrom = selectMan->selectionTolerance();
        VectorImage* vectorImage = static_cast<LayerVector*>(layer)->getLastVectorImageAtFrame(currentFrame, 0);
        selectMan->setCurves(vectorImage->getCurvesCloseTo(getCurrentPoint(), distanceFrom));
        selectMan->setVertices(vectorImage->getVerticesCloseTo(getCurrentPoint(), distanceFrom));

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
                mScribbleArea->paintTransformedSelection();
                mEditor->deselectAll();
            }

            vectorImage->setSelected(selectMan->closestVertices(), true);
            selectMan->vectorSelection.add(selectMan->closestCurves());
            selectMan->vectorSelection.add(selectMan->closestVertices());

            mScribbleArea->update();
        }
        else
        {
            mEditor->deselectAll();
        }
    }
}

void VectorSmudgeTool::pointerMoveEvent(PointerEvent* event)
{
    Layer* layer = mEditor->layers()->currentLayer();
    if (layer == nullptr) { return; }

    auto selectMan = mEditor->select();
    if (event->buttons() & Qt::LeftButton)   // the user is also pressing the mouse (dragging) {
    {
        if (event->modifiers() != Qt::ShiftModifier)    // (and the user doesn't press shift)
        {
            VectorImage* vectorImage = static_cast<LayerVector*>(layer)->getLastVectorImageAtFrame(mEditor->currentFrame(), 0);
            // transforms the selection

            selectMan->setSelectionTransform(QTransform().translate(offsetFromPressPos().x(), offsetFromPressPos().y()));
            vectorImage->setSelectionTransformation(selectMan->selectionTransform());
        }
    }
    else     // the user is moving the mouse without pressing it
    {
        VectorImage* vectorImage = static_cast<LayerVector*>(layer)->getLastVectorImageAtFrame(mEditor->currentFrame(), 0);
        selectMan->setVertices(vectorImage->getVerticesCloseTo(getCurrentPoint(), selectMan->selectionTolerance()));
    }
    mScribbleArea->update();
    mScribbleArea->setAllDirty();
}

void VectorSmudgeTool::pointerReleaseEvent(PointerEvent* event)
{
    Layer* layer = mEditor->layers()->currentLayer();
    if (layer == nullptr) { return; }

    if (event->button() == Qt::LeftButton)
    {
        mEditor->backup(typeName());

        VectorImage *vectorImage = static_cast<LayerVector*>(layer)->getLastVectorImageAtFrame(mEditor->currentFrame(), 0);
        vectorImage->applySelectionTransformation();

        auto selectMan = mEditor->select();
        selectMan->resetSelectionTransform();
        for (int k = 0; k < selectMan->vectorSelection.curve.size(); k++)
        {
            int curveNumber = selectMan->vectorSelection.curve.at(k);
            vectorImage->curve(curveNumber).smoothCurve();
        }
        mScribbleArea->setModified(mEditor->layers()->currentLayerIndex(), mEditor->currentFrame());
    }
}

QPointF VectorSmudgeTool::offsetFromPressPos()
{
    return getCurrentPoint() - getCurrentPressPoint();
}

