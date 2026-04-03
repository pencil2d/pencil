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

#include "basemanager.h"
#include "movemode.h"
#include "vertexref.h"
#include "vectorselection.h"

#include <QPointF>
#include <QRectF>
#include <QPolygonF>
#include <QTransform>

class Editor;

/**
 * @brief The SelectionManager class acts as the "Brain" of the selection system.
 * 
 * It is responsible for:
 * 1. Storing the "Truth" of the selection:
 *    - The original shape (mOriginalRect)
 *    - The current transformation state (mSelectionTransform) including position, rotation, and scale.
 * 
 * 2. Performing the Math:
 *    - Calculates new transformations based on user input from the SelectTool.
 *    - Handles complex matrix operations for rotation and scaling.
 * 
 * 3. Coordinate Space Management:
 *    - Converts points between "Screen Space" (mouse coordinates) and "Selection Space" (drawing coordinates).
 *    - Maps operations from the UI (SelectTool) to the underlying data.
 * 
 * The SelectTool (the "Hand") delegates all state tracking and heavy calculation to this manager.
 */
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

    void flipSelection(bool flipVertical);

    /** @brief Defines the selection area. 
     *  @param roundPixels Set to true for Bitmap layers to ensure integer coordinates. */
    void setSelection(QRectF rect, bool roundPixels=false);

    void translate(QPointF point);
    void rotate(qreal angle, qreal lockedAngle);
    void scale(qreal sX, qreal sY);
    void maintainAspectRatio(bool state) { mAspectRatioFixed = state; }

    /** @brief Locks movement either horizontally or vertically depending on drag direction
     *  @param state */
    void alignPositionToAxis(bool state) { mLockAxis = state; }

    /** @brief Checks if the point is over a handle (corner) or body and sets the MoveMode accordingly. */
    void setMoveModeForAnchorInRange(const QPointF& point);
    MoveMode getMoveMode() const { return mMoveMode; }
    void setMoveMode(const MoveMode moveMode) { mMoveMode = moveMode; }

    bool somethingSelected() const { return mOriginalRect.isValid() || mMoveMode != MoveMode::NONE; }

    /** @brief Updates the selection transform (move, scale, rotate) based on input delta.
     *  This is the core logic for interactive manipulation. */
    void adjustSelection(const QPointF& currentPoint, const QPointF& offset, qreal rotationOffset, int rotationIncrement = 0);

    QTransform selectionTransform() const { return mSelectionTransform; }
    void setSelectionTransform(const QTransform& transform) { mSelectionTransform = transform; }
    void resetSelectionTransform();

    /** @brief SelectionManager::resetSelectionTransformProperties
     * should be used whenever translate, rotate, transform, scale
     * has been applied to a selection, but don't want to reset size nor position
     */
    void resetSelectionTransformProperties();

    void resetSelectionProperties();
    void deleteSelection();

    bool isOutsideSelectionArea(const QPointF& point) const;

    qreal selectionTolerance() const;

    qreal selectionWidth() const { return (mSelectionPolygon[1] - mSelectionPolygon[0]).x(); }
    qreal selectionHeight() const { return (mSelectionPolygon[3] - mSelectionPolygon[0]).y(); }

    QPointF currentTransformAnchor() const { return mAnchorPoint; }
    QPointF getSelectionAnchorPoint() const;

    void setTransformAnchor(const QPointF& point);

    const QRectF& mySelectionRect() const { return mOriginalRect; }
    const qreal& myRotation() const { return mRotatedAngle; }
    const qreal& myScaleX() const { return mScaleX; }
    const qreal& myScaleY() const { return mScaleY; }
    const QPointF& myTranslation() const { return mTranslation; }

    void setRotation(const qreal& rotation) { mRotatedAngle = rotation; }
    void setScale(const qreal scaleX, const qreal scaleY) { mScaleX = scaleX; mScaleY = scaleY; }
    void setTranslation(const QPointF& translation) { mTranslation = translation; }

    qreal angleFromPoint(const QPointF& point, const QPointF& anchorPoint) const;

    // Coordinate Space Conversions
    // "Local Space" = The Layer/Canvas coordinate system.
    // "Selection Space" = The local coordinate system of the selection itself (0,0 is usually top-left of un-transformed rect).
    
    /** @brief Maps a point from Canvas/Layer space INTO the Selection's transformed space. */
    QPointF mapToSelection(const QPointF& point) const { return mSelectionTransform.map(point); };
    
    /** @brief Maps a point FROM the Selection's transformed space BACK to Canvas/Layer space. */
    QPointF mapFromLocalSpace(const QPointF& point) const { return mSelectionTransform.inverted().map(point); }
    
    QPolygonF mapToSelection(const QPolygonF& polygon) const { return mSelectionTransform.map(polygon); }
    QPolygonF mapFromLocalSpace(const QPolygonF& polygon) const { return mSelectionTransform.inverted().map(polygon); }

    // Vector selection
    VectorSelection vectorSelection;

    void setCurves(const QList<int>& curves) { mClosestCurves = curves; }
    void setVertices(const QList<VertexRef>& vertices) { mClosestVertices = vertices; }

    void clearCurves() { mClosestCurves.clear(); };
    void clearVertices() { mClosestVertices.clear(); };

    /// The point from where the dragging will be based of inside the selection area.
    /// Not to be confused with the selection origin
    void setDragOrigin(const QPointF point) { mDragOrigin = point; }

    const QList<int> closestCurves() const { return mClosestCurves; }
    const QList<VertexRef> closestVertices() const { return mClosestVertices; }

    /// This should be called to update the selection transform
    void calculateSelectionTransformation();

signals:
    void selectionChanged();
    void selectionReset();
    void needDeleteSelection();

private:
    /** @brief Aligns the input position to the nearest axis.
     *  Eg. draggin along the x axis, will keep the selection to that axis.
     * @param currentPosition the position of the cursor
     * @return A point that is either horizontally or vertically aligned with the current position.
     */
    QPointF alignPositionToAxis(QPointF currentPoint) const;
    int constrainRotationToAngle(const qreal rotatedAngle, const int rotationIncrement) const;

    bool mAspectRatioFixed = false;
    bool mLockAxis = false;
    QPolygonF mSelectionPolygon;
    QRectF mOriginalRect;

    qreal mScaleX;
    qreal mScaleY;
    QPointF mTranslation;
    qreal mRotatedAngle = 0.0;

    QList<int> mClosestCurves;
    QList<VertexRef> mClosestVertices;

    QPointF mDragOrigin;

    MoveMode mMoveMode = MoveMode::NONE;
    QTransform mSelectionTransform;
    const qreal mSelectionTolerance = 10.0;

    QPointF mAnchorPoint;
};

#endif // SELECTIONMANAGER_H
