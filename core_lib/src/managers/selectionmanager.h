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
#ifndef SELECTIONMANAGER_H
#define SELECTIONMANAGER_H

#include "pencildef.h"
#include "basemanager.h"
#include "movemode.h"
#include "vertexref.h"
#include "vectorselection.h"

#include <QPointF>
#include <QRectF>
#include <QPolygonF>
#include <QTransform>

class Editor;

class SelectionManager : public BaseManager
{
    Q_OBJECT
public:
    explicit SelectionManager(Editor* editor);
    ~SelectionManager() override;

    bool init() override;
    Status load(Object*) override;
    Status save(Object*) override;
    void workingLayerChanged(Layer*) override;

    QVector<QPointF> calcSelectionCenterPoints() const;

    void updatePolygons();
    void updateTransformedSelection() { mTransformedSelection = mTempTransformedSelection; }

    QPointF whichAnchorPoint(QPointF currentPoint) const;
    QPointF getTransformOffset() const { return mOffset; }
    QPointF offsetFromAspectRatio(qreal offsetX, qreal offsetY) const;

    void flipSelection(bool flipVertical);

    void setSelection(QRectF rect, bool roundPixels=false);

    void translate(QPointF point);

    MoveMode getMoveModeForSelectionAnchor(const QPointF pos) const;
    MoveMode validateMoveMode(const QPointF pos);
    MoveMode getMoveMode() const { return mMoveMode; }
    void setMoveMode(const MoveMode moveMode) { mMoveMode = moveMode; }

    bool somethingSelected() const { return mSomethingSelected; }

    void calculateSelectionTransformation();
    void adjustSelection(const QPointF& currentPoint, qreal offsetX, qreal offsetY, qreal rotationOffset, int rotationIncrement);
    MoveMode moveModeForAnchorInRange(const QPointF lastPos);
    void setCurves(const QList<int>& curves) { mClosestCurves = curves; }
    void setVertices(const QList<VertexRef>& vertices) { mClosestVertices = vertices; }

    void clearCurves();
    void clearVertices();

    const QList<int> closestCurves() const { return mClosestCurves; }
    const QList<VertexRef> closestVertices() const { return mClosestVertices; }

    QTransform selectionTransform() const { return mSelectionTransform; }
    void setSelectionTransform(const QTransform& transform) { mSelectionTransform = transform; }
    void resetSelectionTransform();

    bool transformHasBeenModified() const;
    bool rotationHasBeenModified() const;

    /** @brief SelectionManager::resetSelectionTransformProperties
     * should be used whenever translate, rotate, transform, scale
     * has been applied to a selection, but don't want to reset size nor position
     */
    void resetSelectionTransformProperties();

    void resetSelectionProperties();
    void deleteSelection();

    bool isOutsideSelectionArea(const QPointF point);

    qreal selectionTolerance() const;


    QPolygonF currentSelectionPolygonF() const { return mCurrentSelectionPolygonF; }
    QPolygonF lastSelectionPolygonF() const { return mLastSelectionPolygonF; }

    void setSomethingSelected(bool selected) { mSomethingSelected = selected; }

    VectorSelection vectorSelection;

    const QRectF& mySelectionRect() { return mSelection; }
    const QRectF& myTempTransformedSelectionRect() { return mTempTransformedSelection; }
    const QRectF& myTransformedSelectionRect() { return mTransformedSelection; }
    const qreal& myRotation() { return mRotatedAngle; }

    void setSelectionRect(const QRectF& rect) { mSelection = rect; }
    void setTempTransformedSelectionRect(const QRectF& rect) { mTempTransformedSelection = rect; }
    void setTransformedSelectionRect(const QRectF& rect) { mTransformedSelection = rect; }
    void setRotation(const qreal& rotation) { mRotatedAngle = rotation; }

signals:
    void selectionChanged();
    void selectionReset();
    void needPaintAndApply();
    void needDeleteSelection();

private:
    int constrainRotationToAngle(const qreal rotatedAngle, const int rotationIncrement) const;

    QRectF mSelection;
    QRectF mTempTransformedSelection;
    QRectF mTransformedSelection;
    qreal mRotatedAngle = 0.0;

    bool mSomethingSelected = false;
    QPolygonF mLastSelectionPolygonF;
    QPolygonF mCurrentSelectionPolygonF;
    QPointF mOffset;

    QList<int> mClosestCurves;
    QList<VertexRef> mClosestVertices;

    MoveMode mMoveMode = MoveMode::NONE;
    QTransform mSelectionTransform;
    const qreal mSelectionTolerance = 8.0;
};

#endif // SELECTIONMANAGER_H
