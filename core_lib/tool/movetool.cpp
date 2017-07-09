/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2012-2017 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/

#include "editor.h"
#include "toolmanager.h"
#include "scribblearea.h"
#include "layervector.h"
#include "layermanager.h"

#include "movetool.h"

MoveTool::MoveTool(QObject *parent) :
    BaseTool(parent)
{
}

ToolType MoveTool::type()
{
    return MOVE;
}

void MoveTool::loadSettings()
{
    properties.width = -1;
    properties.feather = -1;
    properties.useFeather = -1;
    properties.inpolLevel = -1;
    properties.useAA = -1;
}

QCursor MoveTool::cursor()
{
    return Qt::ArrowCursor;
}

void MoveTool::mousePressEvent( QMouseEvent *event )
{
    Layer *layer = mEditor->layers()->currentLayer();
    if ( layer == NULL ) { return; }

    if ( event->button() == Qt::LeftButton )
    {
        pressOperation(event, layer);
    }
}

void MoveTool::pressOperation(QMouseEvent* event, Layer* layer)
{
    if ( (layer->type() == Layer::BITMAP || layer->type() == Layer::VECTOR) )
    {
        mEditor->backup( tr( "Move" ) );
        mScribbleArea->setMoveMode( ScribbleArea::MIDDLE ); // was MIDDLE

        if ( mScribbleArea->somethingSelected ) // there is an area selection
        {
            whichTransformationPoint();
        }

        if ( mScribbleArea->getMoveMode() == ScribbleArea::MIDDLE )
        {
            if ( event->modifiers() == Qt::ControlModifier ) // --- rotation
            {
                mScribbleArea->setMoveMode( ScribbleArea::ROTATION );
            }
            else if (event->modifiers() == Qt::AltModifier ) // --- symmetry
            {
                mScribbleArea->setMoveMode(ScribbleArea::SYMMETRY );
            }

            if ( layer->type() == Layer::VECTOR )
            {
                actionOnVector(event, layer);
            }
            if ( !(mScribbleArea->myTransformedSelection.contains( getLastPoint() )) )    // click is outside the transformed selection with the MOVE tool
            {
                applyChanges();
            }
        }
    }
}

void MoveTool::actionOnVector(QMouseEvent* event, Layer* layer)
{
    VectorImage *vectorImage = ((LayerVector *)layer)->getLastVectorImageAtFrame( mEditor->currentFrame(), 0 );
    if ( mScribbleArea->mClosestCurves.size() > 0 ) // the user clicks near a curve
    {
        // editor->backup();
        if ( !vectorImage->isSelected( mScribbleArea->mClosestCurves ) )
        {
            if ( event->modifiers() != Qt::ShiftModifier )
            {
                applyChanges();
            }
            vectorImage->setSelected( mScribbleArea->mClosestCurves, true );
            mScribbleArea->setSelection( vectorImage->getSelectionRect(), true );
            mScribbleArea->update();
        }
    }
    else
    {
        int areaNumber = vectorImage->getLastAreaNumber( getLastPoint() );
        if ( areaNumber != -1 )   // the user clicks on an area
        {
            if ( !vectorImage->isAreaSelected( areaNumber ) )
            {
                if ( event->modifiers() != Qt::ShiftModifier )
                {
                    applyChanges();
                }
                vectorImage->setAreaSelected( areaNumber, true );
                mScribbleArea->setSelection( QRectF( 0, 0, 0, 0 ), true );
                mScribbleArea->update();
            }
        }
    }
}

void MoveTool::whichTransformationPoint()
{
    QRectF transformPoint = mScribbleArea->myTransformedSelection;

    if ( QLineF( getLastPoint(), transformPoint.topLeft() ).length() < 10 )
    {
        mScribbleArea->setMoveMode( ScribbleArea::TOPLEFT );
    }
    if ( QLineF( getLastPoint(), transformPoint.topRight() ).length() < 10 )
    {
        mScribbleArea->setMoveMode( ScribbleArea::TOPRIGHT );
    }
    if ( QLineF( getLastPoint(), transformPoint.bottomLeft() ).length() < 10 )
    {
        mScribbleArea->setMoveMode( ScribbleArea::BOTTOMLEFT );
    }
    if ( QLineF( getLastPoint(), transformPoint.bottomRight() ).length() < 10 )
    {
        mScribbleArea->setMoveMode( ScribbleArea::BOTTOMRIGHT );
    }
}

void MoveTool::transformSelection(qreal offsetX, qreal offsetY)
{
    switch ( mScribbleArea->mMoveMode )
    {
    case ScribbleArea::MIDDLE:
        if ( QLineF( getLastPressPixel(), getCurrentPixel() ).length() > 4 )
        {
            mScribbleArea->myTempTransformedSelection = mScribbleArea->myTransformedSelection.translated( QPointF(offsetX, offsetY) );
        }
        break;

    case ScribbleArea::TOPRIGHT:
        mScribbleArea->myTempTransformedSelection =
        mScribbleArea->myTransformedSelection.adjusted( 0, offsetY, offsetX, 0 );
        break;

    case ScribbleArea::TOPLEFT:
        mScribbleArea->myTempTransformedSelection =
        mScribbleArea->myTransformedSelection.adjusted( offsetX, offsetY, 0, 0 );
        break;

    case ScribbleArea::BOTTOMLEFT:
        mScribbleArea->myTempTransformedSelection =
        mScribbleArea->myTransformedSelection.adjusted( offsetX, 0, 0, offsetY );
        break;

    case ScribbleArea::BOTTOMRIGHT:
        mScribbleArea->myTempTransformedSelection =
        mScribbleArea->myTransformedSelection.adjusted( 0, 0, offsetX, offsetY );
        break;

    case ScribbleArea::ROTATION:
        mScribbleArea->myTempTransformedSelection =
        mScribbleArea->myTransformedSelection; // @ necessary?
        mScribbleArea->myRotatedAngle = getCurrentPixel().x() - getLastPressPixel().x();
        break;
    default:
        break;
    }
    mScribbleArea->update();

}

QPointF MoveTool::maintainAspectRatio(qreal offsetX, qreal offsetY)
{
    qreal factor = mScribbleArea->mySelection.width() / mScribbleArea->mySelection.height();

    if (mScribbleArea->mMoveMode == ScribbleArea::TOPLEFT || mScribbleArea->mMoveMode == ScribbleArea::BOTTOMRIGHT) {
        offsetY = offsetX / factor;
    }
    else if (mScribbleArea->mMoveMode == ScribbleArea::TOPRIGHT || mScribbleArea->mMoveMode == ScribbleArea::BOTTOMLEFT) {
        offsetY = -(offsetX / factor);
    }
    else if (mScribbleArea->mMoveMode == ScribbleArea::MIDDLE) {

        qreal absX = offsetX;
        if (absX < 0) {absX = -absX;}

        qreal absY = offsetY;
        if (absY < 0) {absY = -absY;}

        if (absX > absY) {
            offsetY = 0;
        }
        if (absY > absX) {
            offsetX = 0;
        }
    }
    return QPointF(offsetX,offsetY);
}

void MoveTool::onHoverOutlineStroke(Layer* layer)
{
    auto layerVector = static_cast< LayerVector* >( layer );
    VectorImage* pVecImg = layerVector->getLastVectorImageAtFrame( mEditor->currentFrame(), 0 );
    mScribbleArea->mClosestCurves = pVecImg->getCurvesCloseTo( getCurrentPoint(), mScribbleArea->tol / mEditor->view()->scaling() );
}

void MoveTool::mouseReleaseEvent( QMouseEvent* )
{
    mScribbleArea->myTransformedSelection = mScribbleArea->myTempTransformedSelection;
    // Don't do anything more on mouse release.
    // The modifications are only applied on deselect or press enter.
}

void MoveTool::mouseMoveEvent( QMouseEvent *event )
{
    Layer* layer = mEditor->layers()->currentLayer();
    if ( layer == NULL ) {
        return;
    }

    if ( layer->type() != Layer::BITMAP && layer->type() != Layer::VECTOR )
    {
        return;
    }

    if ( event->buttons() & Qt::LeftButton )   // the user is also pressing the mouse (dragging)
    {
        if( mScribbleArea->somethingSelected )
        {
            QPointF offset = QPointF(mScribbleArea->mOffset.x(),
                                     mScribbleArea->mOffset.y());

            if ( event->modifiers() == Qt::ShiftModifier )    // maintain aspect ratio
            {
                offset = maintainAspectRatio(offset.x(), offset.y());
            }

            transformSelection(offset.x(), offset.y());

            mScribbleArea->calculateSelectionTransformation();

            mScribbleArea->paintTransformedSelection();
        }
        else // there is nothing selected
        {
            // we switch to the select tool
            //mEditor->tools()->setCurrentTool( SELECT );
            mScribbleArea->deselectAll();
            mScribbleArea->mMoveMode = ScribbleArea::NONE;
        }
    }
    else // the user is moving the mouse without pressing it
    {
        if ( layer->type() == Layer::VECTOR )
        {
            onHoverOutlineStroke(layer);
        }
        mScribbleArea->update();
    }
}

void MoveTool::cancelChanges()
{
    mScribbleArea->cancelTransformedSelection();
}

void MoveTool::applyChanges()
{
    mScribbleArea->applyTransformedSelection();
}

void MoveTool::leavingThisTool(){
    applyChanges();
}

void MoveTool::switchingLayers(){
    applyChanges();
}
