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
#include "editor.h"

#include "vectorimage.h"

#include "mathutils.h"

#include <QVector2D>


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
    mRotatedAngle = 0;
    mTranslation = QPointF(0, 0);
    mScaleX = 1;
    mScaleY = 1;
    mAnchorPoint = QPoint();
    mSelectionTransform.reset();
}

void SelectionManager::resetSelectionTransform()
{
    mSelectionTransform.reset();
}

bool SelectionManager::isOutsideSelectionArea(const QPointF& point) const
{
    return (!mSelectionTransform.map(mSelectionPolygon).containsPoint(point, Qt::WindingFill)) && mMoveMode == MoveMode::NONE;
}

void SelectionManager::deleteSelection()
{
    emit needDeleteSelection();
}

qreal SelectionManager::selectionTolerance() const
{
    return qAbs(mSelectionTolerance * editor()->viewScaleInversed());
}

QPointF SelectionManager::getSelectionAnchorPoint() const
{
    QPointF anchorPoint;
    if (mSelectionPolygon.count() < 3) { return anchorPoint; }

    if (mMoveMode == MoveMode::BOTTOMRIGHT)
    {
        anchorPoint = mSelectionPolygon[0];
    }
    else if (mMoveMode == MoveMode::BOTTOMLEFT)
    {
        anchorPoint = mSelectionPolygon[1];
    }
    else if (mMoveMode == MoveMode::TOPLEFT)
    {
        anchorPoint = mSelectionPolygon[2];
    }
    else if (mMoveMode == MoveMode::TOPRIGHT)
    {
        anchorPoint = mSelectionPolygon[3];
    } else {
        anchorPoint = QLineF(mSelectionPolygon[0], mSelectionPolygon[2]).center();
    }
    return anchorPoint;
}


void SelectionManager::setMoveModeForAnchorInRange(const QPointF& point)
{
    if (mSelectionPolygon.count() < 3) { return; }

    QPolygonF projectedPolygon = mapToSelection(mSelectionPolygon);

    const double calculatedSelectionTol = selectionTolerance();

    MoveMode mode;
    if (QLineF(point, projectedPolygon[0]).length() < calculatedSelectionTol)
    {
        mode = MoveMode::TOPLEFT;
    }
    else if (QLineF(point, projectedPolygon[1]).length() < calculatedSelectionTol)
    {
        mode = MoveMode::TOPRIGHT;
    }
    else if (QLineF(point, projectedPolygon[2]).length() < calculatedSelectionTol)
    {
        mode = MoveMode::BOTTOMRIGHT;
    }
    else if (QLineF(point, projectedPolygon[3]).length() < calculatedSelectionTol)
    {
        mode = MoveMode::BOTTOMLEFT;
    }
    else if (projectedPolygon.containsPoint(point, Qt::WindingFill))
    {
        mode = MoveMode::MIDDLE;
    }
    else {
        mode = MoveMode::NONE;
    }

    mMoveMode = mode;
}

void SelectionManager::adjustSelection(const QPointF& currentPoint, const QPointF& offset, qreal rotationOffset, int rotationIncrement)
{
    switch (mMoveMode)
    {
    case MoveMode::MIDDLE: {
        translate(currentPoint - offset);
        break;
    }
    case MoveMode::TOPLEFT:
    case MoveMode::TOPRIGHT:
    case MoveMode::BOTTOMRIGHT:
    case MoveMode::BOTTOMLEFT: {

        QPolygonF projectedPolygon = mapToSelection(mSelectionPolygon);
        QVector2D currentPVec = QVector2D(currentPoint);

        qreal originWidth = mSelectionPolygon[1].x() - mSelectionPolygon[0].x();
        qreal originHeight = mSelectionPolygon[3].y() - mSelectionPolygon[0].y();

        QVector2D staticXAnchor;
        QVector2D staticYAnchor;
        QVector2D movingAnchor;
        if (mMoveMode == MoveMode::TOPLEFT) {
            movingAnchor = QVector2D(projectedPolygon[0]);
            staticXAnchor = QVector2D(projectedPolygon[1]);
            staticYAnchor = QVector2D(projectedPolygon[3]);
        } else if (mMoveMode == MoveMode::TOPRIGHT) {
            movingAnchor = QVector2D(projectedPolygon[1]);
            staticXAnchor = QVector2D(projectedPolygon[0]);
            staticYAnchor = QVector2D(projectedPolygon[2]);
        } else if (mMoveMode == MoveMode::BOTTOMRIGHT) {
            movingAnchor = QVector2D(projectedPolygon[2]);
            staticXAnchor = QVector2D(projectedPolygon[3]);
            staticYAnchor = QVector2D(projectedPolygon[1]);
        } else {
            movingAnchor = QVector2D(projectedPolygon[3]);
            staticXAnchor = QVector2D(projectedPolygon[2]);
            staticYAnchor = QVector2D(projectedPolygon[0]);
        }

        QVector2D directionVecX = staticXAnchor - currentPVec;
        QVector2D directionVecY = staticYAnchor - currentPVec;

        // Calculates the signed distance
        qreal distanceX = QVector2D::dotProduct(directionVecX, (staticXAnchor - movingAnchor).normalized());
        qreal distanceY = QVector2D::dotProduct(directionVecY, (staticYAnchor - movingAnchor).normalized());

        qreal scaleX = distanceX / originWidth;
        qreal scaleY = distanceY / originHeight;
        if (mAspectRatioFixed) {
            scaleY = scaleX;
        }

        scale(scaleX, scaleY);

        break;
    }
    case MoveMode::ROTATION: {
        rotate(rotationOffset, rotationIncrement);
        break;
    }
    default:
        break;
    }
    calculateSelectionTransformation();
}

void SelectionManager::translate(QPointF newPos)
{
    mTranslation += newPos;
}

void SelectionManager::rotate(qreal angle, qreal lockedAngle)
{
    if (lockedAngle > 0) {
        mRotatedAngle = constrainRotationToAngle(angle, lockedAngle);
    } else {
        mRotatedAngle = angle;
    }
}

void SelectionManager::scale(qreal sX, qreal sY)
{
    // Enforce negative scaling
    if (mScaleX < 0) {
        sX = -sX;
    } else if (qFuzzyIsNull(sX)) {
        // Scale must not become 0
        sX = 0.0001;
    }

    // Enforce negative scaling
    if (mScaleY < 0) {
        sY = -sY;
    } else if (qFuzzyIsNull(sY)) {
        // Scale must not become 0
        sY = 0.0001;
    }

    mScaleX = sX;
    mScaleY = sY;
}

int SelectionManager::constrainRotationToAngle(const qreal& rotatedAngle, const int& rotationIncrement) const
{
    return qRound(rotatedAngle / rotationIncrement) * rotationIncrement;
}

qreal SelectionManager::angleFromPoint(const QPointF& point, const QPointF& anchorPoint) const
{
    return qRadiansToDegrees(MathUtils::getDifferenceAngle(mSelectionTransform.map(anchorPoint), point));
}

void SelectionManager::setSelection(QRectF rect, bool roundPixels)
{
    resetSelectionTransformProperties();
    if (roundPixels)
    {
        rect = rect.toAlignedRect();
    }
    mSelectionPolygon = rect;
    mOriginalRect = rect;
    mScaleX = 1;
    mScaleY = 1;
    mRotatedAngle = 0;

    emit selectionChanged();
}

void SelectionManager::setTransformAnchor(const QPointF& point)
{
    QPointF newPos = mapToSelection(point);
    QPointF oldPos = mapToSelection(mAnchorPoint);

    // Adjust translation based on anchor point to avoid moving the selection
    mTranslation = mTranslation - oldPos + newPos;
    mAnchorPoint = point;
}

void SelectionManager::calculateSelectionTransformation()
{
    QTransform t;
    t.translate(-mAnchorPoint.x(), -mAnchorPoint.y());
    QTransform t2;
    t2.translate(mTranslation.x(), mTranslation.y());

    QTransform r;
    r.rotate(mRotatedAngle);
    QTransform s;
    s.scale(mScaleX, mScaleY);
    mSelectionTransform = t * s * r * t2;
}

QPointF SelectionManager::offsetFromAspectRatio(qreal offsetX, qreal offsetY) const
{
    QPolygonF projectedPolygon = mapToSelection(mSelectionPolygon);
    qreal width = QLineF(projectedPolygon[0], projectedPolygon[1]).dx();
    qreal height = QLineF(projectedPolygon[0], projectedPolygon[3]).dy();
    qreal factor = width / height;

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
        mScaleY = -mScaleY;
    }
    else
    {
        mScaleX = -mScaleX;
    }
    setTransformAnchor(mOriginalRect.center());
    calculateSelectionTransformation();
    emit selectionChanged();
}

void SelectionManager::resetSelectionProperties()
{
    resetSelectionTransformProperties();
    mSelectionPolygon = QPolygonF();
    mOriginalRect = QRectF();
    emit selectionChanged();
}

