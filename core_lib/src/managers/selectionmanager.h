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
#include <QVector2D>
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

    QPointF getTransformOffset() { return mOffset; }
    QPointF offsetFromAspectRatio(qreal offsetX, qreal offsetY);

    void flipSelection(bool flipVertical);

    void setSelection(QRectF rect, bool roundPixels=false);

    void translate(QPointF point);
    void rotate(qreal angle, qreal lockedAngle);
    void scale(qreal sX, qreal sY);
    void maintainAspectRatio(bool state) { mAspectRatioFixed = state; }

    void setMoveModeForAnchorInRange(QPointF point);
    MoveMode getMoveMode() const { return mMoveMode; }
    void setMoveMode(MoveMode moveMode) { mMoveMode = moveMode; }

    bool somethingSelected() const { return mSomethingSelected; }

    void adjustSelection(const QPointF& currentPoint, qreal offsetX, qreal offsetY, qreal rotationOffset, int rotationIncrement=0);

    QTransform selectionTransform() { return mSelectionTransform; }
    void setSelectionTransform(QTransform transform) { mSelectionTransform = transform; }
    void resetSelectionTransform();

    bool transformHasBeenModified();

    /** @brief SelectionManager::resetSelectionTransformProperties
     * should be used whenever translate, rotate, transform, scale
     * has been applied to a selection, but don't want to reset size nor position
     */
    void resetSelectionTransformProperties();

    void resetSelectionProperties();
    void deleteSelection();

    bool isOutsideSelectionArea(QPointF point);

    qreal selectionTolerance() const;

    qreal selectionWidth() const { return (mSelectionPolygon[1] - mSelectionPolygon[0]).x(); }
    qreal selectionHeight() const { return (mSelectionPolygon[3] - mSelectionPolygon[0]).y(); }

    QPointF currentTransformAnchor() const { return mAnchorPoint; }
    QPointF getSelectionAnchorPoint() const;

    void setSomethingSelected(bool selected) { mSomethingSelected = selected; }
    void setTransformAnchor(QPointF point);

    const QRectF& mySelectionRect() { return mOriginalRect; }
    const qreal& myRotation() { return mRotatedAngle; }
    const qreal& myScaleX() { return mScaleX; }
    const qreal& myScaleY() { return mScaleY; }
    const QPointF& myTranslation() { return mTranslation; }

    void setRotation(const qreal& rotation) { mRotatedAngle = rotation; }
    void setScale(const qreal scaleX, const qreal scaleY) { mScaleX = scaleX; mScaleY = scaleY; }
    void setTranslation(const QPointF& translation) { mTranslation = translation; }

    qreal angleFromPoint(QPointF point, QPointF anchorPoint) const;

    QPointF mapToSelection(QPointF point) const { return mSelectionTransform.map(point); };
    QPointF mapFromLocalSpace(QPointF point) const { return mSelectionTransform.inverted().map(point); }
    QPolygonF mapToSelection(QPolygonF polygon) const { return mSelectionTransform.map(polygon); }
    QPolygonF mapFromLocalSpace(QPolygonF polygon) const { return mSelectionTransform.inverted().map(polygon); }


    // Vector selection
    VectorSelection vectorSelection;

    void setCurves(QList<int> curves) { mClosestCurves = curves; }
    void setVertices(QList<VertexRef> vertices) { mClosestVertices = vertices; }

    void clearCurves() { mClosestCurves.clear(); };
    void clearVertices() { mClosestVertices.clear(); };

    const QList<int> closestCurves() { return mClosestCurves; }
    const QList<VertexRef> closestVertices() { return mClosestVertices; }

    void calculateSelectionTransformation();

signals:
    void selectionChanged();
    void selectionReset();
    void needPaintAndApply();
    void needDeleteSelection();

private:
    int constrainRotationToAngle(const qreal& rotatedAngle, const int& rotationIncrement) const;

    bool mSomethingSelected = false;
    bool mAspectRatioFixed = false;
    QPolygonF mSelectionPolygon;
    QRectF mOriginalRect;

    QPointF mOffset;
    qreal mScaleX;
    qreal mScaleY;
    QPointF mTranslation;
    qreal mRotatedAngle = 0.0;

    QList<int> mClosestCurves;
    QList<VertexRef> mClosestVertices;

    MoveMode mMoveMode = MoveMode::NONE;
    QTransform mSelectionTransform;
    const qreal mSelectionTolerance = 8.0;

    QPointF mAnchorPoint;

    bool mUpdateView = false;
};

#endif // SELECTIONMANAGER_H
