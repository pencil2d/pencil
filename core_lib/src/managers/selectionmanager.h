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

    QVector<QPointF> calcSelectionCenterPoints();

    void updatePolygons();
    void updateTransformedSelection() { mTransformedSelection = mTempTransformedSelection; }

    QRectF mappedSelection();

    QPointF whichAnchorPoint(QPointF currentPoint);
    QPointF getTransformOffset() { return mOffset; }
    QPointF offsetFromAspectRatio(qreal offsetX, qreal offsetY);

    void flipSelection(bool flipVertical);

    void setSelection(QRectF rect);

    void translate(QPointF point);

    MoveMode getMoveModeForSelectionAnchor(QPointF pos);
    MoveMode validateMoveMode(QPointF pos);
    MoveMode getMoveMode() const { return mMoveMode; }
    void setMoveMode(MoveMode moveMode) { mMoveMode = moveMode; }

    bool somethingSelected() const { return mSomethingSelected; }

    void calculateSelectionTransformation();
    void adjustSelection(const QPointF& currentPoint, qreal offsetX, qreal offsetY, qreal rotationOffset, int rotationIncrement=0);
    MoveMode moveModeForAnchorInRange(QPointF lastPos);
    void setCurves(QList<int> curves) { mClosestCurves = curves; }
    void setVertices(QList<VertexRef> vertices) { mClosestVertices = vertices; }

    void clearCurves();
    void clearVertices();

    const QList<int> closestCurves() { return mClosestCurves; }
    const QList<VertexRef> closestVertices() { return mClosestVertices; }

    QTransform selectionTransform() { return mSelectionTransform; }
    void setSelectionTransform(QTransform transform) { mSelectionTransform = transform; }
    void resetSelectionTransform();

    bool transformHasBeenModified();
    bool rotationHasBeenModified();

    /** @brief SelectionManager::resetSelectionTransformProperties
     * should be used whenever translate, rotate, transform, scale
     * has been applied to a selection, but don't want to reset size nor position
     */
    void resetSelectionTransformProperties();

    void resetSelectionProperties();
    void deleteSelection();

    bool isOutsideSelectionArea(QPointF point);

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
    void needPaintAndApply();
    void needDeleteSelection();

private:

    int constrainRotationToAngle(const qreal& rotatedAngle, const int& rotationIncrement) const;

    QRectF mSelection;
    QRectF mTempTransformedSelection;
    QRectF mTransformedSelection;
    qreal mRotatedAngle;

    bool mSomethingSelected;
    QPolygonF mLastSelectionPolygonF;
    QPolygonF mCurrentSelectionPolygonF;
    QPointF mOffset;

    QList<int> mClosestCurves;
    QList<VertexRef> mClosestVertices;

    MoveMode mMoveMode;

    QTransform mSelectionTransform;

    const qreal mSelectionTolerance = 8.0;
};

#endif // SELECTIONMANAGER_H
