#include "selectionmanager.h"
#include "editor.h"

#include "layerbitmap.h"
#include "vectorimage.h"
#include "bitmapimage.h"

#include "layervector.h"

//#ifdef QT_DEBUG
#include <QDebug>
//#endif


SelectionManager::SelectionManager(Editor* editor) : BaseManager(editor),
    mySelection(QRectF()), myTempTransformedSelection(QRectF()), myTransformedSelection(QRectF()),
    mSomethingSelected(false), mLastSelectionPolygonF(QPolygonF()), mCurrentSelectionPolygonF(QPolygonF()),
    myRotatedAngle(0), mOffset(QPointF()), mMoveMode(MoveMode::NONE)
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
    mOffset = QPoint(0, 0);
    myRotatedAngle = 0;
    mSelectionTransform.reset();
}


void SelectionManager::updatePolygons()
{
    mCurrentSelectionPolygonF = myTempTransformedSelection;
    mLastSelectionPolygonF = myTransformedSelection;
}

void SelectionManager::resetSelectionTransform()
{
    mSelectionTransform.reset();
}

bool SelectionManager::isOutsideSelectionArea(QPointF point)
{
    return (!myTransformedSelection.contains(point)
            && validateMoveMode(point) == MoveMode::NONE);
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

float SelectionManager::selectionTolerance() const
{
    return mSelectionTolerance * editor()->viewScaleInversed();
}

MoveMode SelectionManager::validateMoveMode(QPointF pos)
{
    return moveModeForAnchorInRange(pos);
}

MoveMode SelectionManager::moveModeForAnchorInRange(QPointF lastPos)
{
    const double marginInPixels = 15;
    const double scale = editor()->viewScaleInversed();
    const double scaledMargin = qAbs(marginInPixels * scale);

    qDebug() << editor()->viewScaleInversed();

    QRectF transformRect = myTempTransformedSelection;
    QPointF lastPoint = lastPos;

    MoveMode mode;
    if (QLineF(lastPoint, transformRect.topLeft()).length() < scaledMargin)
    {
        mode = MoveMode::TOPLEFT;
    }
    else if (QLineF(lastPoint, transformRect.topRight()).length() < scaledMargin)
    {
        mode = MoveMode::TOPRIGHT;
    }
    else if (QLineF(lastPoint, transformRect.bottomLeft()).length() < scaledMargin)
    {
        mode = MoveMode::BOTTOMLEFT;

    }
    else if (QLineF(lastPoint, transformRect.bottomRight()).length() < scaledMargin)
    {
        mode = MoveMode::BOTTOMRIGHT;
    }
    else if (myTransformedSelection.contains(lastPoint))
    {
        mode = MoveMode::MIDDLE;
    }
    else {
        mode = MoveMode::NONE;
    }
    mMoveMode = mode;
    return mode;
}

MoveMode SelectionManager::getMoveModeForSelectionAnchor(QPointF pos)
{

//    BaseTool* currentTool = editor()->tools()->currentTool();
    const double marginInPixels = 15;
    const double radius = marginInPixels / 2;
    const double scale = editor()->viewScaleInversed();
    const double scaledMargin = qAbs(marginInPixels * scale);

    if (mCurrentSelectionPolygonF.count() < 4) { return MoveMode::NONE; }


    QRectF topLeftCorner = QRectF(mCurrentSelectionPolygonF[0].x() - radius,
                                                                     mCurrentSelectionPolygonF[0].y() - radius,
                                                                     marginInPixels, marginInPixels);

    QRectF topRightCorner = QRectF(mCurrentSelectionPolygonF[1].x() - radius,
                                                                      mCurrentSelectionPolygonF[1].y() - radius,
                                                                      marginInPixels, marginInPixels);

    QRectF bottomRightCorner = QRectF(mCurrentSelectionPolygonF[2].x() - radius,
                                                                         mCurrentSelectionPolygonF[2].y() - radius,
                                                                         marginInPixels, marginInPixels);

    QRectF bottomLeftCorner = QRectF(mCurrentSelectionPolygonF[3].x() - radius,
                                                                        mCurrentSelectionPolygonF[3].y() - radius,
                                                                        marginInPixels, marginInPixels);

    QPointF currentPos = pos;

    if (QLineF(currentPos, topLeftCorner.center()).length() < scaledMargin)
    {
        return MoveMode::TOPLEFT;
    }
    else if (QLineF(currentPos, topRightCorner.center()).length() < scaledMargin)
    {
        return MoveMode::TOPRIGHT;
    }
    else if (QLineF(currentPos, bottomLeftCorner.center()).length() < scaledMargin)
    {
        return MoveMode::BOTTOMLEFT;

    }
    else if (QLineF(currentPos, bottomRightCorner.center()).length() < scaledMargin)
    {
        return MoveMode::BOTTOMRIGHT;
    }
    else if (myTempTransformedSelection.contains(currentPos))
    {
        return MoveMode::MIDDLE;
    }

    return MoveMode::NONE;
}

QPointF SelectionManager::whichAnchorPoint(QPointF currentPoint, QPointF anchorPoint)
{
    MoveMode mode = getMoveModeForSelectionAnchor(currentPoint);
    if (mode == MoveMode::TOPLEFT)
    {
        anchorPoint = mySelection.bottomRight();
    }
    else if (mode == MoveMode::TOPRIGHT)
    {
        anchorPoint = mySelection.bottomLeft();
    }
    else if (mode == MoveMode::BOTTOMLEFT)
    {
        anchorPoint = mySelection.topRight();
    }
    else if (mode == MoveMode::BOTTOMRIGHT)
    {
        anchorPoint = mySelection.topLeft();
    }
    return anchorPoint;
}

void SelectionManager::adjustSelection(const QPointF& currentPoint, qreal offsetX, qreal offsetY, qreal rotationOffset)
{
    QRectF& transformedSelection = myTransformedSelection;

    switch (mMoveMode)
    {
    case MoveMode::MIDDLE:
    {
        myTempTransformedSelection = transformedSelection.translated(QPointF(offsetX, offsetY));
        break;
    }
    case MoveMode::TOPRIGHT:
    {
        myTempTransformedSelection = transformedSelection.adjusted(0, offsetY, offsetX, 0);
        break;
    }
    case MoveMode::TOPLEFT:
    {
        myTempTransformedSelection = transformedSelection.adjusted(offsetX, offsetY, 0, 0);
        break;
    }
    case MoveMode::BOTTOMLEFT:
    {
        myTempTransformedSelection = transformedSelection.adjusted(offsetX, 0, 0, offsetY);
        break;
    }
    case MoveMode::BOTTOMRIGHT:
    {
        myTempTransformedSelection = transformedSelection.adjusted(0, 0, offsetX, offsetY);
        break;

    }
    case MoveMode::ROTATION:
    {
        myTempTransformedSelection =
            transformedSelection; // @ necessary?
        QPointF anchorPoint = transformedSelection.center();
        myRotatedAngle = ( atan2( currentPoint.y() - anchorPoint.y(), currentPoint.x() - anchorPoint.x() ) ) * 180.0 / M_PI - rotationOffset;
        qDebug() << currentPoint << anchorPoint << myRotatedAngle << rotationOffset;
        break;
    }
    default:
        break;
    }
}


void SelectionManager::setSelection(QRectF rect)
{
    resetSelectionTransformProperties();
    mySelection = rect;
    myTransformedSelection = rect;
    myTempTransformedSelection = rect;
    mSomethingSelected = (mySelection.isNull() ? false : true);

    emit selectionChanged();
}

void SelectionManager::calculateSelectionTransformation()
{
    QVector<QPoint> centerPoints = calcSelectionCenterPoints();

    mSelectionTransform.reset();

    mSelectionTransform.translate(centerPoints[0].x(), centerPoints[0].y());
    mSelectionTransform.rotate(myRotatedAngle);

    if (mySelection.width() > 0 && mySelection.height() > 0) // can't divide by 0
    {
        float scaleX = myTempTransformedSelection.width() / mySelection.width();
        float scaleY = myTempTransformedSelection.height() / mySelection.height();
        mSelectionTransform.scale(scaleX, scaleY);
    }
    mSelectionTransform.translate(-centerPoints[1].x(), -centerPoints[1].y());
}

QVector<QPoint> SelectionManager::calcSelectionCenterPoints()
{
    QVector<QPoint> centerPoints;
    float selectionCenterX,
        selectionCenterY,
        tempSelectionCenterX,
        tempSelectionCenterY;

    tempSelectionCenterX = myTempTransformedSelection.center().x();
    tempSelectionCenterY = myTempTransformedSelection.center().y();
    selectionCenterX = mySelection.center().x();
    selectionCenterY = mySelection.center().y();
    centerPoints.append(QPoint(tempSelectionCenterX, tempSelectionCenterY));
    centerPoints.append(QPoint(selectionCenterX, selectionCenterY));
    return centerPoints;
}


QPointF SelectionManager::offsetFromAspectRatio(qreal offsetX, qreal offsetY)
{
    qreal factor = myTransformedSelection.width() / myTransformedSelection.height();

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
    int scaleX = myTempTransformedSelection.width() / mySelection.width();
    int scaleY = myTempTransformedSelection.height() / mySelection.height();
    QVector<QPoint> centerPoints = calcSelectionCenterPoints();

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

void SelectionManager::resetSelectionProperties()
{
    resetSelectionTransformProperties();
    mySelection = QRectF();
    myTransformedSelection = QRectF();
    myTempTransformedSelection = QRectF();
    mCurrentSelectionPolygonF = QPolygonF();
    mLastSelectionPolygonF = QPolygonF();

    mSomethingSelected = false;
    vectorSelection.clear();
}

