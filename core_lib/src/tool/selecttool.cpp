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
#include "selecttool.h"

#include <QMouseEvent>

#include "vectorimage.h"
#include "editor.h"
#include "strokemanager.h"
#include "layervector.h"
#include "scribblearea.h"
#include "layermanager.h"
#include "toolmanager.h"

SelectTool::SelectTool(QObject* parent) :
    BaseTool(parent)
{
}

void SelectTool::loadSettings()
{
    properties.width = -1;
    properties.feather = -1;
    properties.stabilizerLevel = -1;
    properties.useAA = -1;

}

QCursor SelectTool::cursor()
{
    MoveMode mode = mScribbleArea->getMoveModeForSelectionAnchor();
    return mScribbleArea->currentTool()->selectMoveCursor(mode, type());
}

bool first = false;
void SelectTool::tabletPressEvent(QTabletEvent *)
{
    mCurrentLayer = mEditor->layers()->currentLayer();
    beginSelection();
}

void SelectTool::tabletMoveEvent(QTabletEvent *)
{
    mCurrentLayer = mEditor->layers()->currentLayer();
    if (!mScribbleArea->isSomethingSelected()) { return; }

    mScribbleArea->updateToolCursor();

    if (m_pStrokeManager->isPenPressed())
    {
        controlOffsetOrigin();

        if (mCurrentLayer->type() == Layer::VECTOR)
        {
            static_cast<LayerVector*>(mCurrentLayer)->
                    getLastVectorImageAtFrame(mEditor->currentFrame(), 0)->
                    select(mScribbleArea->myTempTransformedSelection);
        }
    }

    mScribbleArea->updateCurrentFrame();
}

void SelectTool::tabletReleaseEvent(QTabletEvent *)
{
    mCurrentLayer = mEditor->layers()->currentLayer();
    if (maybeDeselect())
    {
        mScribbleArea->deselectAll();
    } else {
        keepSelection();
    }

    mScribbleArea->updateToolCursor();
    mScribbleArea->updateCurrentFrame();
    mScribbleArea->setAllDirty();
}

void SelectTool::mousePressEvent(QMouseEvent* event)
{

    mCurrentLayer = mEditor->layers()->currentLayer();
    if (mCurrentLayer == NULL) return;
    if (!mCurrentLayer->isPaintable()) { return; }
    if (event->button() != Qt::LeftButton) { return; }

    beginSelection();
}

void SelectTool::beginSelection()
{
    // Store original click position for help with selection rectangle.
    mAnchorOriginPoint = getLastPoint();

    mScribbleArea->calculateSelectionTransformation();

    // paint and apply the transformation
    mScribbleArea->paintTransformedSelection();
    mScribbleArea->applyTransformedSelection();

    if (mScribbleArea->isSomethingSelected())// there is something selected
    {
        if (mCurrentLayer->type() == Layer::VECTOR)
        {
            static_cast<LayerVector*>(mCurrentLayer)->getLastVectorImageAtFrame(mEditor->currentFrame(), 0)->deselectAll();
        }

        mScribbleArea->findMoveModeOfCornerInRange();
        mAnchorOriginPoint = whichAnchorPoint();

        // the user did not click on one of the corners
        if (mScribbleArea->getMoveMode() == MoveMode::NONE)
        {
            mScribbleArea->mySelection.setTopLeft(getLastPoint());
            mScribbleArea->mySelection.setBottomRight(getLastPoint());

        }
    }
    else
    {
        mScribbleArea->setSelection(QRectF(getCurrentPoint().x(),
                                           getCurrentPoint().y(),1,1));
    }
    mScribbleArea->update();
}

QPointF SelectTool::whichAnchorPoint()
{
    return mScribbleArea->whichAnchorPoint(mAnchorOriginPoint);
}

void SelectTool::mouseReleaseEvent(QMouseEvent* event)
{
    mCurrentLayer = mEditor->layers()->currentLayer();
    if (mCurrentLayer == NULL) return;
    if (event->button() != Qt::LeftButton) return;

    if (maybeDeselect())
    {
        mScribbleArea->deselectAll();
    } else {
        keepSelection();
    }

    mScribbleArea->updateToolCursor();

    mScribbleArea->updateCurrentFrame();
    mScribbleArea->setAllDirty();
}

void SelectTool::mouseMoveEvent(QMouseEvent* event)
{
    mCurrentLayer = mEditor->layers()->currentLayer();
    if (mCurrentLayer == NULL) { return; }
    if (!mCurrentLayer->isPaintable()) { return; }
    if (!mScribbleArea->isSomethingSelected()) { return; }

    mScribbleArea->updateToolCursor();

    if (event->buttons() & Qt::LeftButton)
    {
        controlOffsetOrigin();

        if (mCurrentLayer->type() == Layer::VECTOR)
        {
            static_cast<LayerVector*>(mCurrentLayer)->
                    getLastVectorImageAtFrame(mEditor->currentFrame(), 0)->
                    select(mScribbleArea->myTempTransformedSelection);
        }
    }

    mScribbleArea->updateCurrentFrame();
}

bool SelectTool::maybeDeselect()
{
    return (!isSelectionPointValid() && mScribbleArea->getMoveMode() == MoveMode::NONE);
}

void SelectTool::controlOffsetOrigin()
{
    QPointF offset = QPointF(mScribbleArea->getTransformOffset().x(),
                 mScribbleArea->getTransformOffset().y()).toPoint();

    if (mScribbleArea->getMoveMode() != MoveMode::NONE)
    {
        if (mCurrentLayer->type() == Layer::BITMAP) {
            offset = QPointF(mScribbleArea->getTransformOffset());
        }

        mScribbleArea->adjustSelection(offset.x(),offset.y(), mScribbleArea->myRotatedAngle);
    }
    else
    {
        // when the selection is none, manage the selection Origin
        mScribbleArea->manageSelectionOrigin(getCurrentPoint(), mAnchorOriginPoint);
    }
}

/**
 * @brief SelectTool::keepSelection
 * Keep selection rect and normalize if invalid
 */
void SelectTool::keepSelection()
{
    if (mCurrentLayer->type() == Layer::BITMAP) {
        if (!mScribbleArea->myTempTransformedSelection.isValid())
        {
            mScribbleArea->setSelection(mScribbleArea->myTempTransformedSelection.normalized());
        }
        else
        {
            mScribbleArea->setSelection(mScribbleArea->myTempTransformedSelection);
        }
    }
    else if (mCurrentLayer->type() == Layer::VECTOR)
    {
        VectorImage* vectorImage = static_cast<LayerVector*>(mCurrentLayer)->getLastVectorImageAtFrame(mEditor->currentFrame(), 0);
        mScribbleArea->setSelection(vectorImage->getSelectionRect());
    }
}

bool SelectTool::keyPressEvent(QKeyEvent* event)
{
    switch (event->key())
    {
    case Qt::Key_Alt:
        mScribbleArea->setTemporaryTool(MOVE);
        break;
    default:
        break;
    }

    // Follow the generic behaviour anyway
    return false;
}
