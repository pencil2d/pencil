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
#include "selectionmanager.h"
#include "viewmanager.h"
#include "editor.h"

#include "layerbitmap.h"
#include "vectorimage.h"
#include "bitmapimage.h"

#include "layervector.h"
#include "mathutils.h"

//#ifdef QT_DEBUG
#include <QDebug>
//#endif


SelectionManager::SelectionManager(Editor* editor) : BaseManager(editor, __FUNCTION__)
{
}

SelectionManager::~SelectionManager()
{
}

bool SelectionManager::init()
{
    return true;
}

Status SelectionManager::load(Object*)
{
    resetSelectionProperties();
    return Status::OK;
}

Status SelectionManager::save(Object*)
{
    return Status::OK;
}

void SelectionManager::workingLayerChanged(Layer *)
{
}

void SelectionManager::resetSelectionTransformProperties()
{
    mOffset = QPointF(0, 0);
    mRotatedAngle = 0;
    mSelectionTransform.reset();
}

void SelectionManager::updatePolygons()
{
    mCurrentSelectionPolygonF = mTempTransformedSelection;
    mLastSelectionPolygonF = mTransformedSelection;
}

void SelectionManager::resetSelectionTransform()
{
    mSelectionTransform.reset();
}

bool SelectionManager::isOutsideSelectionArea(const QPointF point)
{
    return (!mTransformedSelection.contains(point)
            && validateMoveMode(point) == MoveMode::NONE);
}

bool SelectionManager::transformHasBeenModified() const
{
    return (mSelection != mTempTransformedSelection) || rotationHasBeenModified();
}

bool SelectionManager::rotationHasBeenModified() const
{
    return !qFuzzyCompare(mRotatedAngle, 0);
}

void SelectionManager::deleteSelection()
{
    emit needDeleteSelection();
}

void SelectionManager::clearCurves()
{
    mClosestCurves.clear();
}

void SelectionManager::clearVertices()
{
    mClosestVertices.clear();
}

qreal SelectionManager::selectionTolerance() const
{
    return qAbs(mSelectionTolerance * editor()->view()->getViewScaleInverse());
}

MoveMode SelectionManager::validateMoveMode(const QPointF pos)
{
    return moveModeForAnchorInRange(pos);
}

MoveMode SelectionManager::moveModeForAnchorInRange(const QPointF lastPos)
{
    const QRectF transformRect = mTempTransformedSelection;
    const QPointF lastPoint = lastPos;

    const double calculatedSelectionTol = selectionTolerance();

    MoveMode mode;
    if (QLineF(lastPoint, transformRect.topLeft()).length() < calculatedSelectionTol)
    {
        mode = MoveMode::TOPLEFT;
    }
    else if (QLineF(lastPoint, transformRect.topRight()).length() < calculatedSelectionTol)
    {
        mode = MoveMode::TOPRIGHT;
    }
    else if (QLineF(lastPoint, transformRect.bottomLeft()).length() < calculatedSelectionTol)
    {
        mode = MoveMode::BOTTOMLEFT;

    }
    else if (QLineF(lastPoint, transformRect.bottomRight()).length() < calculatedSelectionTol)
    {
        mode = MoveMode::BOTTOMRIGHT;
    }
    else if (mTransformedSelection.contains(lastPoint))
    {
        mode = MoveMode::MIDDLE;
    }
    else {
        mode = MoveMode::NONE;
    }
    mMoveMode = mode;
    return mode;
}

MoveMode SelectionManager::getMoveModeForSelectionAnchor(const QPointF pos) const
{
    const double calculatedSelectionTol = selectionTolerance();

    if (mCurrentSelectionPolygonF.count() < 4) { return MoveMode::NONE; }

    QPointF topLeftCorner = mCurrentSelectionPolygonF[0];

    QPointF topRightCorner = mCurrentSelectionPolygonF[1];

    QPointF bottomRightCorner = mCurrentSelectionPolygonF[2];

    QPointF bottomLeftCorner = mCurrentSelectionPolygonF[3];

    QPointF currentPos = pos;

    if (QLineF(currentPos, topLeftCorner).length() < calculatedSelectionTol)
    {
        return MoveMode::TOPLEFT;
    }
    else if (QLineF(currentPos, topRightCorner).length() < calculatedSelectionTol)
    {
        return MoveMode::TOPRIGHT;
    }
    else if (QLineF(currentPos, bottomLeftCorner).length() < calculatedSelectionTol)
    {
        return MoveMode::BOTTOMLEFT;

    }
    else if (QLineF(currentPos, bottomRightCorner).length() < calculatedSelectionTol)
    {
        return MoveMode::BOTTOMRIGHT;
    }
    else if (mTempTransformedSelection.contains(currentPos))
    {
        return MoveMode::MIDDLE;
    }

    return MoveMode::NONE;
}

QPointF SelectionManager::whichAnchorPoint(const QPointF currentPoint) const
{
    QPointF anchorPoint;
    MoveMode mode = getMoveModeForSelectionAnchor(currentPoint);
    if (mode == MoveMode::TOPLEFT)
    {
        anchorPoint = mSelection.bottomRight();
    }
    else if (mode == MoveMode::TOPRIGHT)
    {
        anchorPoint = mSelection.bottomLeft();
    }
    else if (mode == MoveMode::BOTTOMLEFT)
    {
        anchorPoint = mSelection.topRight();
    }
    else if (mode == MoveMode::BOTTOMRIGHT)
    {
        anchorPoint = mSelection.topLeft();
    }
    return anchorPoint;
}

void SelectionManager::adjustSelection(const QPointF& currentPoint, qreal offsetX, qreal offsetY, qreal rotationOffset, int rotationIncrement)
{
    offsetX = qRound(offsetX);
    offsetY = qRound(offsetY);
    QRectF& transformedSelection = mTransformedSelection;

    switch (mMoveMode)
    {
    case MoveMode::MIDDLE:
    {
        mTempTransformedSelection = transformedSelection.translated(QPointF(offsetX, offsetY));
        break;
    }
    case MoveMode::TOPRIGHT:
    {
        mTempTransformedSelection = transformedSelection.adjusted(0, offsetY, offsetX, 0);
        break;
    }
    case MoveMode::TOPLEFT:
    {
        mTempTransformedSelection = transformedSelection.adjusted(offsetX, offsetY, 0, 0);
        break;
    }
    case MoveMode::BOTTOMLEFT:
    {
        mTempTransformedSelection = transformedSelection.adjusted(offsetX, 0, 0, offsetY);
        break;
    }
    case MoveMode::BOTTOMRIGHT:
    {
        mTempTransformedSelection = transformedSelection.adjusted(0, 0, offsetX, offsetY);
        break;

    }
    case MoveMode::ROTATION:
    {
        mTempTransformedSelection = transformedSelection;
        QPointF anchorPoint = transformedSelection.center();
        qreal rotatedAngle = qRadiansToDegrees(MathUtils::getDifferenceAngle(anchorPoint, currentPoint)) - rotationOffset;
        if (rotationIncrement > 0) {
            mRotatedAngle = constrainRotationToAngle(rotatedAngle, rotationIncrement);
        } else {
            mRotatedAngle = rotatedAngle;
        }
        break;
    }
    default:
        break;
    }
}

int SelectionManager::constrainRotationToAngle(const qreal rotatedAngle, const int rotationIncrement) const
{
    return qRound(rotatedAngle / rotationIncrement) * rotationIncrement;
}

void SelectionManager::setSelection(QRectF rect, bool roundPixels)
{
    resetSelectionTransformProperties();
    if (roundPixels)
    {
        rect = QRect(rect.topLeft().toPoint(), rect.bottomRight().toPoint() - QPoint(1,1));
    }
    mSelection = rect;
    mTransformedSelection = rect;
    mTempTransformedSelection = rect;
    mSomethingSelected = (mSelection.isNull() ? false : true);

    emit selectionChanged();
}

void SelectionManager::calculateSelectionTransformation()
{
    QVector<QPointF> centerPoints = calcSelectionCenterPoints();

    mSelectionTransform.reset();

    mSelectionTransform.translate(centerPoints[0].x(), centerPoints[0].y());
    mSelectionTransform.rotate(mRotatedAngle);

    if (mSelection.width() > 0 && mSelection.height() > 0) // can't divide by 0
    {
        qreal scaleX = mTempTransformedSelection.width() / mSelection.width();
        qreal scaleY = mTempTransformedSelection.height() / mSelection.height();
        mSelectionTransform.scale(scaleX, scaleY);
    }
    mSelectionTransform.translate(-centerPoints[1].x(), -centerPoints[1].y());
}

QVector<QPointF> SelectionManager::calcSelectionCenterPoints() const
{
    QVector<QPointF> centerPoints;
    qreal selectionCenterX,
        selectionCenterY,
        tempSelectionCenterX,
        tempSelectionCenterY;

    tempSelectionCenterX = mTempTransformedSelection.center().x();
    tempSelectionCenterY = mTempTransformedSelection.center().y();
    selectionCenterX = mSelection.center().x();
    selectionCenterY = mSelection.center().y();
    centerPoints.append(QPointF(tempSelectionCenterX, tempSelectionCenterY));
    centerPoints.append(QPointF(selectionCenterX, selectionCenterY));
    return centerPoints;
}


QPointF SelectionManager::offsetFromAspectRatio(qreal offsetX, qreal offsetY) const
{
    qreal factor = mTransformedSelection.width() / mTransformedSelection.height();

    if (mMoveMode == MoveMode::TOPLEFT || mMoveMode == MoveMode::BOTTOMRIGHT)
    {
        offsetY = offsetX / factor;
    }
    else if (mMoveMode == MoveMode::TOPRIGHT || mMoveMode == MoveMode::BOTTOMLEFT)
    {
        offsetY = -(offsetX / factor);
    }
    else if (mMoveMode == MoveMode::MIDDLE)
    {
        qreal absX = offsetX;
        if (absX < 0) { absX = -absX; }

        qreal absY = offsetY;
        if (absY < 0) { absY = -absY; }

        if (absX > absY) { offsetY = 0; }
        if (absY > absX) { offsetX = 0; }
    }
    return QPointF(offsetX, offsetY);
}

/**
 * @brief ScribbleArea::flipSelection
 * flip selection along the X or Y axis
*/
void SelectionManager::flipSelection(bool flipVertical)
{
    if (flipVertical)
    {
        editor()->backup(tr("Flip selection vertically"));
    }
    else
    {
        editor()->backup(tr("Flip selection horizontally"));
    }

    qreal scaleX = mTempTransformedSelection.width() / mSelection.width();
    qreal scaleY = mTempTransformedSelection.height() / mSelection.height();
    QVector<QPointF> centerPoints = calcSelectionCenterPoints();

    QTransform translate = QTransform::fromTranslate(centerPoints[0].x(), centerPoints[0].y());
    QTransform _translate = QTransform::fromTranslate(-centerPoints[1].x(), -centerPoints[1].y());
    QTransform scale = QTransform::fromScale(-scaleX, scaleY);

    if (flipVertical)
    {
        scale = QTransform::fromScale(scaleX, -scaleY);
    }

    // reset transformation for vector selections
    mSelectionTransform.reset();
    mSelectionTransform *= _translate * scale * translate;

    emit needPaintAndApply();
}

void SelectionManager::translate(QPointF point)
{
    mTempTransformedSelection.translate(point);
    mTransformedSelection = mTempTransformedSelection;
    calculateSelectionTransformation();
}

void SelectionManager::resetSelectionProperties()
{
    resetSelectionTransformProperties();
    mSelection = QRectF();
    mTransformedSelection = QRectF();
    mTempTransformedSelection = QRectF();
    mCurrentSelectionPolygonF = QPolygonF();
    mLastSelectionPolygonF = QPolygonF();

    mSomethingSelected = false;
    vectorSelection.clear();
    emit selectionReset();
    emit selectionChanged();
}

