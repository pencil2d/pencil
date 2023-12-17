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
#include "mathutils.h"

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
bool StrokeTool::msIsAdjusting = false;
bool StrokeTool::mQuickSizingEnabled = false;

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
        if (event->buttons() & Qt::LeftButton && msIsAdjusting) {
            adjustCursor(event->modifiers());
            return true;
        }
    } else if (event->eventType() == PointerEvent::Release) {
        if (msIsAdjusting) {
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
    const qreal brushWidth = properties.width;
    const qreal brushFeather = properties.feather;

    const QPointF& cursorPos = msIsAdjusting ? mAdjustPosition : getCurrentPoint();
    const qreal cursorRad = brushWidth * 0.5;
    const QPointF& cursorOffset = QPointF(cursorPos.x() - cursorRad, cursorPos.y() - cursorRad);

    CanvasCursorPainterOptions options;
    options.widthRect = QRectF(cursorOffset, QSizeF(brushWidth, brushWidth));

    const qreal featherWidthFactor = MathUtils::normalize(brushFeather, 0.0, 99.0);
    options.featherRect = QRectF(options.widthRect.center().x() - (cursorRad * featherWidthFactor),
                                 options.widthRect.center().y() - (cursorRad * featherWidthFactor),
                                 brushWidth * featherWidthFactor,
                                 brushWidth * featherWidthFactor);
    options.showCursor = mCanvasCursorEnabled;
    options.isAdjusting = msIsAdjusting && mQuickSizingEnabled;
    options.useFeather = properties.useFeather;

    mCanvasCursorPainter.preparePainter(options, mEditor->view()->getView());

    const QRect& dirtyRect = mCanvasCursorPainter.dirtyRect().toAlignedRect();
    const QRect& updateRect = mEditor->view()->getView().mapRect(QRectF(cursorOffset, QSizeF(brushWidth, brushWidth))).toAlignedRect();

    if (msIsAdjusting || mCanvasCursorEnabled) {
        // Adjusted to account for some pixel bleeding outside the update rect
        mScribbleArea->update(updateRect.united(dirtyRect).adjusted(-2, -2, 2, 2));
    }
}

bool StrokeTool::startAdjusting(Qt::KeyboardModifiers modifiers, qreal step)
{
    Q_UNUSED(step)

    if (mQuickSizingProperties.contains(modifiers))
    {
        const QPointF& currentPressPoint = getCurrentPressPoint();
        const QPointF& currentPoint = getCurrentPoint();
        auto propertyType = mQuickSizingProperties.value(modifiers);
        switch (propertyType) {
        case WIDTH: {
            const qreal factor = 0.5;
            const qreal rad = properties.width * factor;
            const qreal distance = QLineF(currentPressPoint - QPointF(rad, rad), currentPoint).length();
            mAdjustPosition = currentPressPoint - QPointF(distance * factor, distance * factor);
            break;
        }
        case FEATHER: {
            const qreal factor = 0.5;
            const qreal cursorRad = properties.width * factor;
            const qreal featherWidthFactor = MathUtils::normalize(properties.feather, 0.0, 99.0);
            const qreal offset = (cursorRad * featherWidthFactor) * factor;
            const qreal distance = QLineF(currentPressPoint - QPointF(offset, offset), currentPoint).length();
            mAdjustPosition = currentPressPoint - QPointF(distance, distance);
            break;
        }
        case TOLERANCE:
//            msOriginalPropertyValue = properties.tolerance;
            break;
        default:
            qDebug() << "Unhandled quick sizing property for tool" << typeName();
            Q_ASSERT(false);
            return false;
        }

        msIsAdjusting = true;
        updateCanvasCursor();
        return true;
    }
    return false;
}

void StrokeTool::stopAdjusting()
{
    msIsAdjusting = false;
    mAdjustPosition = QPointF();
    updateCanvasCursor();
}

void StrokeTool::adjustCursor(Qt::KeyboardModifiers modifiers)
{
    switch (mQuickSizingProperties.value(modifiers))
    {
    case WIDTH: {
        // The adjusted position is based on the radius of the circle, so in order to
        // map it back to its original value, we can multiply by the factor we divided with
        const qreal newValue = QLineF(mAdjustPosition, getCurrentPoint()).length() * 2.0;

        mEditor->tools()->setWidth(qBound(1., newValue, 200.));
        break;
    }
    case FEATHER: {
        // The radius of the width is the max value we can get
        const qreal inputMin = 0.0;
        const qreal inputMax = properties.width * 0.5;
        const qreal distance = QLineF(mAdjustPosition, getCurrentPoint()).length();
        const qreal outputMax = 99.0;
        const qreal outputMin = 0.0;

        // We flip min and max here in order to get the inverted value for the UI
        const qreal mappedValue = MathUtils::map(distance, inputMin, inputMax, outputMax, outputMin);

        mEditor->tools()->setFeather(qBound(1., mappedValue, 99.));
        break;
    }
    case TOLERANCE:
    // TODO: should we reimplement or remove?
//        mEditor->tools()->setTolerance(qBound(0., newValue, 100.));
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
}


