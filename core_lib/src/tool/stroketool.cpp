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

const qreal StrokeTool::FEATHER_MIN = 1.;
const qreal StrokeTool::FEATHER_MAX = 99.;
const qreal StrokeTool::WIDTH_MIN = 1.;
const qreal StrokeTool::WIDTH_MAX = 200.;

// ---- shared static variables ---- ( only one instance for all the tools )
bool StrokeTool::mQuickSizingEnabled = false;

StrokeTool::StrokeTool(QObject* parent) : BaseTool(parent)
{
    detectWhichOSX();
}

void StrokeTool::loadSettings()
{
    mQuickSizingEnabled = mEditor->preference()->isOn(SETTING::QUICK_SIZING);
    mCanvasCursorEnabled = mEditor->preference()->isOn(SETTING::CANVAS_CURSOR);

    /// Given the way that we update preferences currently, this connection should not be removed
    /// when the tool is not active.
    connect(mEditor->preference(), &PreferenceManager::optionChanged, this, &StrokeTool::onPreferenceChanged);

    connect(&mWidthSizingTool, &RadialOffsetTool::offsetChanged, this, [=](qreal offset) {
        mEditor->tools()->setWidth(qBound(WIDTH_MIN, offset * 2.0, WIDTH_MAX));
    });

    connect(&mFeatherSizingTool, &RadialOffsetTool::offsetChanged, this, [=](qreal offset){
        const qreal inputMin = FEATHER_MIN;
        const qreal inputMax = properties.width * 0.5;
        const qreal outputMax = FEATHER_MAX;
        const qreal outputMin = inputMin;

        // We map the feather value to a value between the min width and max width
        const qreal mappedValue = MathUtils::map(offset, inputMin, inputMax, outputMax, outputMin);

        mEditor->tools()->setFeather(qBound(FEATHER_MIN, mappedValue, FEATHER_MAX));
    });
}

bool StrokeTool::enteringThisTool()
{
    mActiveConnections.append(connect(mEditor->view(), &ViewManager::viewChanged, this, &StrokeTool::onViewUpdated));
    return true;
}

bool StrokeTool::leavingThisTool()
{
    return BaseTool::leavingThisTool();
}

void StrokeTool::onPreferenceChanged(SETTING setting)
{
    if (setting == SETTING::QUICK_SIZING) {
        mQuickSizingEnabled = mEditor->preference()->isOn(setting);
    } else if (setting == SETTING::CANVAS_CURSOR) {
        mCanvasCursorEnabled = mEditor->preference()->isOn(setting);
    }
}

void StrokeTool::onViewUpdated()
{
    updateCanvasCursor();
}

QPointF StrokeTool::getCurrentPressPixel() const
{
    return mInterpolator.getCurrentPressPixel();
}

QPointF StrokeTool::getCurrentPressPoint() const
{
    return mEditor->view()->mapScreenToCanvas(mInterpolator.getCurrentPressPixel());
}

QPointF StrokeTool::getCurrentPixel() const
{
    return mInterpolator.getCurrentPixel();
}

QPointF StrokeTool::getCurrentPoint() const
{
    return mEditor->view()->mapScreenToCanvas(getCurrentPixel());
}

QPointF StrokeTool::getLastPixel() const
{
    return mInterpolator.getLastPixel();
}

QPointF StrokeTool::getLastPoint() const
{
    return mEditor->view()->mapScreenToCanvas(getLastPixel());
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
    QPointF startStrokes = mInterpolator.interpolateStart(mLastPixel);
    mStrokePoints << mEditor->view()->mapScreenToCanvas(startStrokes);

    mStrokePressures.clear();
    mStrokePressures << mInterpolator.getPressure();

    mCurrentInputType = inputType;
    mUndoSaveState = mEditor->undoRedo()->state(UndoRedoRecordType::KEYFRAME_MODIFY);

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
    mInterpolator.interpolateEnd();
    mStrokePressures << mInterpolator.getPressure();
    mStrokePoints.clear();
    mStrokePressures.clear();

    enableCoalescing();

    mEditor->setModified(mEditor->currentLayerIndex(), mEditor->currentFrame());
    mScribbleArea->endStroke();

    mEditor->undoRedo()->record(mUndoSaveState, typeName());
}

void StrokeTool::drawStroke()
{
    QPointF pixel = getCurrentPixel();
    if (pixel != mLastPixel || !mFirstDraw)
    {
        // get last pixel before interpolation initializes
        QPointF startStrokes = mInterpolator.interpolateStart(getLastPixel());
        mStrokePoints << mEditor->view()->mapScreenToCanvas(startStrokes);
        mStrokePressures << mInterpolator.getPressure();
    }
    else
    {
        mFirstDraw = false;
    }
}

bool StrokeTool::handleQuickSizing(PointerEvent* event)
{
    if (!mQuickSizingEnabled) { return false; }

    if (!mQuickSizingProperties.contains(event->modifiers())) {
        mWidthSizingTool.stopAdjusting();
        mFeatherSizingTool.stopAdjusting();
        return false;
    }

    ToolPropertyType setting = mQuickSizingProperties[event->modifiers()];
    if (event->eventType() == PointerEvent::Press) {
        switch (setting) {
            case WIDTH: {
                mWidthSizingTool.setOffset(properties.width * 0.5);
                break;
            }
            case FEATHER: {
                const qreal factor = 0.5;
                const qreal cursorRad = properties.width * factor;

                // Pull feather handle closer to center as feather increases
                const qreal featherWidthFactor = MathUtils::normalize(properties.feather, FEATHER_MIN, FEATHER_MAX);
                const qreal offset = (cursorRad * featherWidthFactor);
                mFeatherSizingTool.setOffset(offset);
                break;
            }
            default: break;
        }
    }

    switch (setting) {
        case WIDTH: {
            mWidthSizingTool.pointerEvent(event);
            break;
        }
        case FEATHER: {
            mFeatherSizingTool.pointerEvent(event);
            break;
        }
        default: break;
    }

    updateCanvasCursor();
    return true;
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

bool StrokeTool::enterEvent(QEnterEvent*)
{
    mCanvasCursorEnabled = mEditor->preference()->isOn(SETTING::CANVAS_CURSOR);
    return true;
}

bool StrokeTool::leaveEvent(QEvent*)
{
    if (isActive())
    {
        return false;
    }

    mCanvasCursorEnabled = false;
    updateCanvasCursor();
    return true;
}


QRectF StrokeTool::cursorRect(ToolPropertyType settingType, const QPointF& point)
{
    const qreal brushWidth = properties.width;
    const qreal brushFeather = properties.feather;

    const QPointF& cursorPos = point;
    const qreal cursorRad = brushWidth * 0.5;
    const QPointF& widthCursorTopLeft = QPointF(cursorPos.x() - cursorRad, cursorPos.y() - cursorRad);

    const QRectF widthCircleRect = QRectF(widthCursorTopLeft, QSizeF(brushWidth, brushWidth));
    if (settingType == WIDTH) {
        return widthCircleRect;
    } else if (settingType == FEATHER) {
        const qreal featherWidthFactor =  MathUtils::normalize(brushFeather, FEATHER_MIN, FEATHER_MAX);
        QRectF featherRect = QRectF(widthCircleRect.center().x() - (cursorRad * featherWidthFactor),
                                     widthCircleRect.center().y() - (cursorRad * featherWidthFactor),
                                     brushWidth * featherWidthFactor,
                                     brushWidth * featherWidthFactor);

        // Adjust the feather rect so it doesn't colide with the width rect;
        // as this cancels out both circles when painted
        return featherRect.adjusted(2, 2, -2, -2);
    }

    return QRectF();
}


void StrokeTool::updateCanvasCursor()
{
    CanvasCursorPainterOptions widthOptions;
    widthOptions.circleRect = cursorRect(WIDTH, mWidthSizingTool.isAdjusting() ? mWidthSizingTool.offsetPoint() : getCurrentPoint());
    widthOptions.showCursor = mCanvasCursorEnabled;
    widthOptions.showCross = true;

    CanvasCursorPainterOptions featherOptions;
    featherOptions.circleRect = cursorRect(FEATHER, mFeatherSizingTool.isAdjusting() ? mFeatherSizingTool.offsetPoint() : getCurrentPoint());
    featherOptions.showCursor = mCanvasCursorEnabled;
    featherOptions.showCross = false;

    if (mFeatherSizingTool.isAdjusting()) {
        widthOptions.circleRect = cursorRect(WIDTH, mFeatherSizingTool.offsetPoint());
    } else if (mWidthSizingTool.isAdjusting()) {
        featherOptions.circleRect = cursorRect(FEATHER, mWidthSizingTool.offsetPoint());
    }

    mWidthCursorPainter.preparePainter(widthOptions);
    mFeatherCursorPainter.preparePainter(featherOptions);

    const QRect& dirtyRect = mWidthCursorPainter.dirtyRect();

    // We know that the width rect is bigger than the feather rect
    // so we don't need to change this
    const QRect& updateRect = widthOptions.circleRect.toAlignedRect();

    // Adjusted to account for some pixel bleeding outside the update rect
    mScribbleArea->update(mEditor->view()->getView().mapRect(updateRect.united(dirtyRect).adjusted(-2, -2, 2, 2)));
    mWidthCursorPainter.clearDirty();
}

void StrokeTool::paint(QPainter& painter, const QRect& blitRect)
{
    painter.save();
    painter.setTransform(mEditor->view()->getView());

    if (properties.useFeather) {
        mFeatherCursorPainter.paint(painter, blitRect);
    }

    mWidthCursorPainter.paint(painter, blitRect);

    painter.restore();
}
