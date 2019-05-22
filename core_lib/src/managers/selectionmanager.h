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

    QVector<QPoint> calcSelectionCenterPoints();

    void updatePolygons();

    QRectF mappedSelection();

    QPointF whichAnchorPoint(QPointF currentPoint, QPointF anchorPoint);
    QPointF getTransformOffset() { return mOffset; }
    QPointF offsetFromAspectRatio(qreal offsetX, qreal offsetY);

    void flipSelection(bool flipVertical);

    void setSelection(QRectF rect);

    MoveMode getMoveModeForSelectionAnchor(QPointF pos);
    MoveMode validateMoveMode(QPointF pos);
    MoveMode getMoveMode() const { return mMoveMode; }
    void setMoveMode(MoveMode moveMode) { mMoveMode = moveMode; }

    bool somethingSelected() const { return mSomethingSelected; }

    void calculateSelectionTransformation();
    void adjustSelection(float offsetX, float offsetY, qreal rotatedAngle);
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

    inline bool transformHasBeenModified() { return (mySelection != myTempTransformedSelection) || myRotatedAngle != 0; }

    /** @brief SelectionManager::resetSelectionTransformProperties
     * should be used whenever translate, rotate, transform, scale
     * has been applied to a selection, but don't want to reset size nor position
     */
    void resetSelectionTransformProperties();

    void resetSelectionProperties();
    void deleteSelection();

    bool isOutsideSelectionArea(QPointF point);

    float selectionTolerance() const;


    QPolygonF currentSelectionPolygonF() const { return mCurrentSelectionPolygonF; }
    QPolygonF lastSelectionPolygonF() const { return mLastSelectionPolygonF; }

    QRectF mySelection;
    QRectF myTempTransformedSelection;
    QRectF myTransformedSelection;

    int myRotatedAngle;

    VectorSelection vectorSelection;

signals:
    void selectionChanged();
    void needPaintAndApply();
    void needDeleteSelection();

private:

    bool mSomethingSelected;
    QPolygonF mLastSelectionPolygonF;
    QPolygonF mCurrentSelectionPolygonF;
    QPointF mOffset;

    QList<int> mClosestCurves;
    QList<VertexRef> mClosestVertices;

    MoveMode mMoveMode;

    QTransform mSelectionTransform;

    float mSelectionTolerance = 8.0;
};

#endif // SELECTIONMANAGER_H
