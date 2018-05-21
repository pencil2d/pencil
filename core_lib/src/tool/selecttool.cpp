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
    return Qt::CrossCursor;
}

void SelectTool::mousePressEvent(QMouseEvent* event)
{
    Layer* layer = mEditor->layers()->currentLayer();
    if (layer == NULL) return;

    mScribbleArea->myRotatedAngle = 0;

    if (event->button() == Qt::LeftButton)
    {

        anchorOriginPoint = getLastPoint();  // Store original click position for help with selection rectangle.

        if (layer->isPaintable())
        {
            if (layer->type() == Layer::VECTOR)
            {
                static_cast<LayerVector*>(layer)->getLastVectorImageAtFrame(mEditor->currentFrame(), 0)->deselectAll();
            }
            mScribbleArea->setMoveMode(ScribbleArea::NONE);

            if (mScribbleArea->somethingSelected)      // there is something selected
            {
                if (BezierCurve::mLength(getLastPoint() - mScribbleArea->myTransformedSelection.topLeft()) < 6)
                {
                    mScribbleArea->setMoveMode(ScribbleArea::TOPLEFT);
                    anchorOriginPoint = mScribbleArea->mySelection.bottomRight();
                }
                if (BezierCurve::mLength(getLastPoint() - mScribbleArea->myTransformedSelection.topRight()) < 6)
                {
                    mScribbleArea->setMoveMode(ScribbleArea::TOPRIGHT);
                    anchorOriginPoint = mScribbleArea->mySelection.bottomLeft();
                }
                if (BezierCurve::mLength(getLastPoint() - mScribbleArea->myTransformedSelection.bottomLeft()) < 6)
                {
                    mScribbleArea->setMoveMode(ScribbleArea::BOTTOMLEFT);
                    anchorOriginPoint = mScribbleArea->mySelection.topRight();
                }
                if (BezierCurve::mLength(getLastPoint() - mScribbleArea->myTransformedSelection.bottomRight()) < 6)
                {
                    mScribbleArea->setMoveMode(ScribbleArea::BOTTOMRIGHT);
                    anchorOriginPoint = mScribbleArea->mySelection.topLeft();
                }

                // the user did not click on one of the corners
                if (mScribbleArea->getMoveMode() == ScribbleArea::NONE)
                {
                    // Deselect all and get ready for a new selection
                    mScribbleArea->deselectAll();

                    mScribbleArea->mySelection.setTopLeft(getLastPoint());
                    mScribbleArea->mySelection.setBottomRight(getLastPoint());
                    mScribbleArea->setSelection(mScribbleArea->mySelection, true);
                }
            }
            else     // there is nothing selected
            {
                mScribbleArea->mySelection.setTopLeft(getLastPoint());
                mScribbleArea->mySelection.setBottomRight(getLastPoint());
                mScribbleArea->setSelection(mScribbleArea->mySelection, true);
            }
            mScribbleArea->update();
        }
    }
}

void SelectTool::mouseReleaseEvent(QMouseEvent* event)
{
    Layer* layer = mEditor->layers()->currentLayer();
    if (layer == NULL) return;

    if (event->button() != Qt::LeftButton) return;
    
    if (layer->type() == Layer::VECTOR)
    {
        if (mScribbleArea->somethingSelected)
        {
            mEditor->tools()->setCurrentTool(MOVE);

            VectorImage* vectorImage = static_cast<LayerVector*>(layer)->getLastVectorImageAtFrame(mEditor->currentFrame(), 0);
            mScribbleArea->setSelection(vectorImage->getSelectionRect(), true);
            if (mScribbleArea->mySelection.width() <= 0 && mScribbleArea->mySelection.height() <= 0)
            {
                mScribbleArea->deselectAll();
            }
        }
        mScribbleArea->updateCurrentFrame();
        mScribbleArea->setAllDirty();
    }
    else if (layer->type() == Layer::BITMAP)
    {
        if (mScribbleArea->mySelection.width() <= 0 && mScribbleArea->mySelection.height() <= 0)
        {
            mScribbleArea->deselectAll();
        }
        mScribbleArea->updateCurrentFrame();
        mScribbleArea->setAllDirty();
    }
}

void SelectTool::mouseMoveEvent(QMouseEvent* event)
{
    Layer* layer = mEditor->layers()->currentLayer();
    if (layer == NULL) { return; }

    if ((event->buttons() & Qt::LeftButton) &&
        mScribbleArea->somethingSelected &&
        layer->isPaintable())
    {
        mScribbleArea->manageSelectionOrigin(getCurrentPoint(), anchorOriginPoint);

        mScribbleArea->myTransformedSelection = mScribbleArea->mySelection.adjusted(0, 0, 0, 0);
        mScribbleArea->myTempTransformedSelection = mScribbleArea->mySelection.adjusted(0, 0, 0, 0);

        if (layer->type() == Layer::VECTOR)
        {
            static_cast<LayerVector*>(layer)->
                    getLastVectorImageAtFrame(mEditor->currentFrame(), 0)->
                    select(mScribbleArea->mySelection);
        }
        mScribbleArea->update();
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
