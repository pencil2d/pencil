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

#include "stroketool.h"

#include <QKeyEvent>
#include "scribblearea.h"
#include "strokemanager.h"
#include "viewmanager.h"
#include "preferencemanager.h"
#include "editor.h"
#include "toolmanager.h"

#include "canvascursorpainter.h"

#ifdef Q_OS_MAC
extern "C" {
    void detectWhichOSX();
    void disableCoalescing();
    void enableCoalescing();
}
#else
extern "C" {
    void detectWhichOSX() {}
    void disableCoalescing() {}
    void enableCoalescing() {}
}
#endif

// ---- shared static variables ---- ( only one instance for all the tools )
//qreal StrokeTool::msOriginalPropertyValue;  // start value (width, feather ..)
bool StrokeTool::msIsAdjusting = false;
bool StrokeTool::mQuickSizingEnabled = false;
//qreal StrokeTool::mAdjustOffset = 0.5;

StrokeTool::StrokeTool(QObject* parent) : BaseTool(parent)
{
    detectWhichOSX();
}

void StrokeTool::loadSettings()
{
    mQuickSizingEnabled = mEditor->preference()->isOn(SETTING::QUICK_SIZING);
    mCanvasCursorEnabled = mEditor->preference()->isOn(SETTING::DOTTED_CURSOR);

    connect(mEditor->view(), &ViewManager::viewChanged, this, &StrokeTool::updateCanvasCursor);
    connect(mEditor->preference(), &PreferenceManager::optionChanged, this, &StrokeTool::onPreferenceChanged);
}

void StrokeTool::onPreferenceChanged(SETTING setting)
{
    if (setting == SETTING::QUICK_SIZING) {
        mQuickSizingEnabled = mEditor->preference()->isOn(setting);
    } else if (setting == SETTING::DOTTED_CURSOR) {
        mCanvasCursorEnabled = mEditor->preference()->isOn(setting);
    }
}

void StrokeTool::startStroke(PointerEvent::InputType inputType)
{
    if (emptyFrameActionEnabled())
    {
        mScribbleArea->handleDrawingOnEmptyFrame();
    }

    mFirstDraw = true;
    mLastPixel = getCurrentPixel();

    mStrokePoints.clear();

    //Experimental
    QPointF startStrokes = strokeManager()->interpolateStart(mLastPixel);
    mStrokePoints << mEditor->view()->mapScreenToCanvas(startStrokes);

    mStrokePressures.clear();
    mStrokePressures << strokeManager()->getPressure();

    mCurrentInputType = inputType;

    disableCoalescing();
}

bool StrokeTool::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Alt:
        if (mEditor->tools()->setTemporaryTool(EYEDROPPER, {}, Qt::AltModifier))
        {
            return true;
        }
        break;
    case Qt::Key_Space:
        if (mEditor->tools()->setTemporaryTool(HAND, Qt::Key_Space, Qt::NoModifier))
        {
            return true;
        }
        break;
    }
    return BaseTool::keyPressEvent(event);
}

bool StrokeTool::emptyFrameActionEnabled()
{
    return true;
}

void StrokeTool::endStroke()
{
    strokeManager()->interpolateEnd();
    mStrokePressures << strokeManager()->getPressure();
    mStrokePoints.clear();
    mStrokePressures.clear();

    enableCoalescing();

    mEditor->setModified(mEditor->currentLayerIndex(), mEditor->currentFrame());
    mScribbleArea->endStroke();
}

void StrokeTool::drawStroke()
{
    QPointF pixel = getCurrentPixel();
    if (pixel != mLastPixel || !mFirstDraw)
    {
        // get last pixel before interpolation initializes
        QPointF startStrokes = strokeManager()->interpolateStart(getLastPixel());
        mStrokePoints << mEditor->view()->mapScreenToCanvas(startStrokes);
        mStrokePressures << strokeManager()->getPressure();
    }
    else
    {
        mFirstDraw = false;
    }
}

bool StrokeTool::interruptPointerEvent(PointerEvent* event)
{
    if (event->eventType() == PointerEvent::Press) {
        if (mQuickSizingEnabled) {
            return startAdjusting(event->modifiers(), 1);
        }
    } else if (event->eventType() == PointerEvent::Move) {
        if (event->buttons() & Qt::LeftButton && isAdjusting()) {
            adjustCursor(event->modifiers());
            return true;
        }
    } else if (event->eventType() == PointerEvent::Release) {
        if (isAdjusting()) {
            mEditor->tools()->setWidth(static_cast<float>(properties.width));
            stopAdjusting();
            return true;
        }
    }
    return false;
}

void StrokeTool::pointerPressEvent(PointerEvent*)
{
    updateCanvasCursor();
}

void StrokeTool::pointerMoveEvent(PointerEvent*)
{
    updateCanvasCursor();
}

void StrokeTool::pointerReleaseEvent(PointerEvent*)
{
    updateCanvasCursor();
}

void StrokeTool::updateCanvasCursor()
{
    qreal brushWidth = properties.width;
//    qreal brushFeather = properties.feather;

    QPointF cursorPos = getCurrentPoint();
    const qreal cursorRad = brushWidth * 0.5;

//    switch (mAdjustingPropertyType) {
//        case WIDTH: {
//            cursorPos = isAdjusting() ? mAdjustPosition : cursorPos;
//            break;
//        }
//        case FEATHER: {
            cursorPos = isAdjusting() ? mAdjustPosition : cursorPos;
//            break;
//        }
//        default:
//            break;
//    }

    CanvasCursorPainterOptions options;
    options.widthRect = QRectF(cursorPos.x() - cursorRad, cursorPos.y() - cursorRad, brushWidth, brushWidth);

//    qDebug() << "width rect: " << options.widthRect;

//    qreal featherToRelativeDistance = QLineF(adjustFromPosition(msOriginalPropertyValue, 0.5), getCurrentPoint()).length();
    qreal featherWidthFactor = ((properties.feather - 0.0) / (100.0 - 0.0));
    options.featherRect = QRectF(options.widthRect.center().x() - (cursorRad * featherWidthFactor),
                                 options.widthRect.center().y() - (cursorRad * featherWidthFactor),
                                 brushWidth * featherWidthFactor,
                                 brushWidth * featherWidthFactor);
    options.showCursor = mCanvasCursorEnabled;
    options.isAdjusting = isAdjusting() && mQuickSizingEnabled;

    mCanvasCursorPainter.preparePainter(options, mEditor->view()->getView());

    QRect dirtyRect = mCanvasCursorPainter.dirtyRect().toAlignedRect();
    QRect updateRect = mEditor->view()->getView().mapRect(QRectF(cursorPos.x()-cursorRad, cursorPos.y()-cursorRad, brushWidth, brushWidth)).toAlignedRect();

    if (isAdjusting() || mCanvasCursorEnabled) {
        mScribbleArea->update(updateRect.united(dirtyRect).adjusted(-2, -2, 2, 2));
    }
}

bool StrokeTool::startAdjusting(Qt::KeyboardModifiers modifiers, qreal step)
{
    if (mQuickSizingProperties.contains(modifiers))
    {
        auto propertyType = mQuickSizingProperties.value(modifiers);
        mAdjustingPropertyType = propertyType;
        switch (propertyType) {
        case WIDTH: {
            const qreal factor = 0.5;
            qreal rad = properties.width * factor;
            qreal distance = QLineF(getCurrentPressPoint() - QPointF(rad, rad), getCurrentPoint()).length();
            mAdjustPosition = getCurrentPressPoint() - QPointF(distance * factor, distance * factor);
            break;
        }
        case FEATHER: {
            const qreal factor = 0.5;
            qreal cursorRad = properties.width * factor;
            qreal featherWidthFactor = ((properties.feather - 0.0) / (99.0 - 0.0));
            qreal distance = QLineF(getCurrentPressPoint() - QPointF((cursorRad * featherWidthFactor) * factor, (cursorRad * featherWidthFactor) * factor), getCurrentPoint()).length();
            mAdjustPosition = getCurrentPressPoint() - QPointF(distance, distance);
            break;
        }
        case TOLERANCE:
//            msOriginalPropertyValue = properties.tolerance;
            break;
        default:
            mAdjustingPropertyType = ToolPropertyType::NULL_PROPERTY;
            qDebug() << "Unhandled quick sizing property for tool" << typeName();
            Q_ASSERT(false);
            return false;
        }

        msIsAdjusting = true;
        mAdjustmentStep = step;
        updateCanvasCursor();
        return true;
    }
    return false;
}

void StrokeTool::stopAdjusting()
{
    msIsAdjusting = false;
    mAdjustmentStep = 0;
//    msOriginalPropertyValue = 0;
    mAdjustPosition = QPointF();
    mAdjustingPropertyType = NULL_PROPERTY;
    updateCanvasCursor();
}

void StrokeTool::adjustCursor(Qt::KeyboardModifiers modifiers)
{
    // The adjusted position is based on the radius of the circle, so in order to
    // map it back to its original value, we multiply by the factor we divided with
    qreal newValue = QLineF(mAdjustPosition, getCurrentPoint()).length() * 2.0;

    // Problem breakdown:
    // 1. The feather value goes from 2 to 100
    // 2. No matter the brush width, the feather value should always map between 2 and 200
    // 3. When calculating the value for the slider, we need to map it from 0 to 1
    //    where the current position in the circle, is the percentage from center
    // 4. msOriginalPropertyValue stores the value that tells us the distance from the adjusting cursor pos to our current pos

//    qDebug() << "stuff";
    switch (mQuickSizingProperties.value(modifiers))
    {
    case WIDTH:
        mEditor->tools()->setWidth(qBound(1., newValue, 200.));
        break;
    case FEATHER: {
        qreal circleRad = properties.width * 0.5;
        qreal maxWidth = circleRad;
        newValue = QLineF(mAdjustPosition, getCurrentPoint()).length();
        qreal mapped = (((newValue - 0.0) / (maxWidth - 0.0)) * 99.0);

        qDebug() << "mapped: " << newValue;
        mEditor->tools()->setFeather(qBound(1., mapped, 99.));
        break;
    }
    case TOLERANCE:
        mEditor->tools()->setTolerance(qBound(0., newValue, 100.));
        break;
    default:
        qDebug() << "Unhandled quick sizing property for tool" << typeName();
        Q_ASSERT(false);
        break;
    }
    updateCanvasCursor();
}

void StrokeTool::paint(QPainter& painter, const QRect& blitRect)
{
    mCanvasCursorPainter.paint(painter, blitRect);

    QPointF cursorPos = getCurrentPoint();

//    QLineF testLine
    painter.drawLine(mEditor->view()->getView().map(QLineF(mAdjustPosition, getCurrentPoint())));
//    switch (mAdjustingPropertyType) {
//        case WIDTH: {
//            cursorPos = isAdjusting() ? mAdjustPosition : cursorPos;
//            break;
//        }
//        case FEATHER: {
            cursorPos = isAdjusting() ? mAdjustPosition : cursorPos;
//            break;
//        }
//        default:
//            break;
//    }

//    CanvasCursorPainterOptions options;
//    QRectF widthCircle = QRectF(cursorPos.x() - cursorRad, cursorPos.y() - cursorRad, properties.width, properties.width);

////    qDebug() << "width rect: " << options.widthRect;

////    qreal featherToRelativeDistance = QLineF(adjustFromPosition(msOriginalPropertyValue, 0.5), getCurrentPoint()).length();
//    qreal featherWidthFactor = ((properties.feather - 2.0) / (99.0 - 2.0));
//    QRectF featherCircle =  QRectF(widthCircle.center().x() - (cursorRad * featherWidthFactor),
//                                 widthCircle.center().y() - (cursorRad * featherWidthFactor),
//                                 properties.width * featherWidthFactor,
//                                 properties.width * featherWidthFactor);
//    painter.setPen(Qt::blue);
//    painter.drawEllipse(mEditor->view()->getView().mapRect(featherCircle));
//    painter.drawEllipse(mEditor->view()->getView().mapRect(QRectF(mAdjustPosition-QPoint(5,5), QSize(10,10))));
}


