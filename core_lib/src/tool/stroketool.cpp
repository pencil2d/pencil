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
bool StrokeTool::msIsAdjusting = false;
bool StrokeTool::mQuickSizingEnabled = false;

StrokeTool::StrokeTool(QObject* parent) : BaseTool(parent)
{
    detectWhichOSX();
}

StrokeTool::~StrokeTool()
{
    if (mSettings) {
        // Technically this is probably not neccesary since a tool exists for the entire
        // lifetime of the program.
        delete(mSettings);
        mSettings = nullptr;
    }
}

void StrokeTool::createSettings(ToolSettings* settings)
{
    if (settings == nullptr) {
        mSettings = new StrokeSettings();
    } else {
        mSettings = static_cast<StrokeSettings*>(settings);
    }
    BaseTool::createSettings(mSettings);
}

void StrokeTool::loadSettings()
{
    mQuickSizingEnabled = mEditor->preference()->isOn(SETTING::QUICK_SIZING);
    mCanvasCursorEnabled = mEditor->preference()->isOn(SETTING::CANVAS_CURSOR);

    QSettings settings(PENCIL2D, PENCIL2D);
    QHash<int, PropertyInfo> info;
    info[StrokeSettings::WIDTH_VALUE] = { 1.0, 100.0, 24.0 };
    info[StrokeSettings::FEATHER_VALUE] = { 1.0, 99.0, 48.0 };
    info[StrokeSettings::FEATHER_ENABLED] = false;
    info[StrokeSettings::PRESSURE_ENABLED] = false;
    info[StrokeSettings::INVISIBILITY_ENABLED] = false;
    info[StrokeSettings::STABILIZATION_VALUE] = { StabilizationLevel::NONE, StabilizationLevel::STRONG, StabilizationLevel::STRONG };
    info[StrokeSettings::ANTI_ALIASING_ENABLED] = false;
    info[StrokeSettings::FILLCONTOUR_ENABLED] = false;

    mSettings->setDefaults(info);
    mSettings->load(typeName(), settings);

    /// Given the way that we update preferences currently, this connection should not be removed
    /// when the tool is not active.
    connect(mEditor->preference(), &PreferenceManager::optionChanged, this, &StrokeTool::onPreferenceChanged);
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
    if (event->eventType() == PointerEvent::Press) {
        if (mQuickSizingEnabled) {
            return startAdjusting(event->modifiers());
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

void StrokeTool::updateCanvasCursor()
{
    const qreal brushWidth = mSettings->width();
    const qreal brushFeather = mSettings->feather();

    const QPointF& cursorPos = msIsAdjusting ? mAdjustPosition : getCurrentPoint();
    const qreal cursorRad = brushWidth * 0.5;
    const QPointF& cursorOffset = QPointF(cursorPos.x() - cursorRad, cursorPos.y() - cursorRad);

    CanvasCursorPainterOptions options;
    options.widthRect = QRectF(cursorOffset, QSizeF(brushWidth, brushWidth));

    const qreal featherWidthFactor = MathUtils::normalize(brushFeather, 0.0, FEATHER_MAX);
    options.featherRect = QRectF(options.widthRect.center().x() - (cursorRad * featherWidthFactor),
                                 options.widthRect.center().y() - (cursorRad * featherWidthFactor),
                                 brushWidth * featherWidthFactor,
                                 brushWidth * featherWidthFactor);
    options.showCursor = mCanvasCursorEnabled;
    options.isAdjusting = msIsAdjusting && mQuickSizingEnabled;
    options.useFeather = mSettings->featherEnabled();

    mCanvasCursorPainter.preparePainter(options, mEditor->view()->getView());

    const QRect& dirtyRect = mCanvasCursorPainter.dirtyRect();
    const QRect& updateRect = mEditor->view()->getView().mapRect(QRectF(cursorOffset, QSizeF(brushWidth, brushWidth))).toAlignedRect();

    if (!msIsAdjusting && !mCanvasCursorEnabled) {
        if (mCanvasCursorPainter.isDirty()) {
            // Adjusted to account for some pixel bleeding outside the update rect
            mScribbleArea->update(mCanvasCursorPainter.dirtyRect().adjusted(-2, -2, 2, 2));
            mCanvasCursorPainter.clearDirty();
        }
        return;
    }

    // Adjusted to account for some pixel bleeding outside the update rect
    mScribbleArea->update(updateRect.united(dirtyRect).adjusted(-2, -2, 2, 2));
}

bool StrokeTool::startAdjusting(Qt::KeyboardModifiers modifiers)
{
    if (!mQuickSizingProperties.contains(modifiers))
    {
        return false;
    }

    const QPointF& currentPressPoint = getCurrentPressPoint();
    const QPointF& currentPoint = getCurrentPoint();
    auto propertyType = mQuickSizingProperties.value(modifiers);
    switch (propertyType) {
    case StrokeSettings::WIDTH_VALUE: {
        const qreal factor = 0.5;
        const qreal rad = mSettings->width() * factor;
        const qreal distance = QLineF(currentPressPoint - QPointF(rad, rad), currentPoint).length();
        mAdjustPosition = currentPressPoint - QPointF(distance * factor, distance * factor);
        break;
    }
    case StrokeSettings::FEATHER_VALUE: {
        const qreal factor = 0.5;
        const qreal cursorRad = mSettings->width() * factor;
        const qreal featherWidthFactor = MathUtils::normalize(mSettings->feather(), 0.0, FEATHER_MAX);
        const qreal offset = (cursorRad * featherWidthFactor) * factor;
        const qreal distance = QLineF(currentPressPoint - QPointF(offset, offset), currentPoint).length();
        mAdjustPosition = currentPressPoint - QPointF(distance, distance);
        break;
    }
    default:
        Q_UNREACHABLE();
        qWarning() << "Unhandled quick sizing property for tool" << typeName();
        return false;
    }

    msIsAdjusting = true;
    updateCanvasCursor();
    return true;
}

void StrokeTool::stopAdjusting()
{
    msIsAdjusting = false;
    mAdjustPosition = QPointF();

    updateCanvasCursor();
}

void StrokeTool::adjustCursor(Qt::KeyboardModifiers modifiers)
{
    if (!mQuickSizingProperties.contains(modifiers)) {
        return;
    }

    switch (mQuickSizingProperties.value(modifiers))
    {
    case StrokeSettings::WIDTH_VALUE: {
        // The adjusted position is based on the radius of the circle, so in order to
        // map it back to its original value, we can multiply by the factor we divided with
        const qreal newValue = QLineF(mAdjustPosition, getCurrentPoint()).length() * 2.0;
        setWidth(newValue);
        break;
    }
    case StrokeSettings::FEATHER_VALUE: {
        // The radius of the width is the max value we can get
        const qreal inputMin = 0.0;
        const qreal inputMax = mSettings->width() * 0.5;
        const qreal distance = QLineF(mAdjustPosition, getCurrentPoint()).length();
        const qreal outputMax = FEATHER_MAX;
        const qreal outputMin = 0.0;

        // We flip min and max here in order to get the inverted value for the UI
        const qreal mappedValue = MathUtils::map(distance, inputMin, inputMax, outputMax, outputMin);

        setFeather(mappedValue);
        break;
    }
    default:
        qWarning() << "Unhandled quick sizing property for tool" << typeName() << "setting: " << mQuickSizingProperties.value(modifiers);
        Q_UNREACHABLE();
    }
    updateCanvasCursor();
}

void StrokeTool::paint(QPainter& painter, const QRect& blitRect)
{
    mCanvasCursorPainter.paint(painter, blitRect);
}

void StrokeTool::setStablizationLevel(int level)
{
    mSettings->setBaseValue(StrokeSettings::STABILIZATION_VALUE, level);
    emit stabilizationLevelChanged(level);
}

void StrokeTool::setFeatherEnabled(bool enabled)
{
    mSettings->setBaseValue(StrokeSettings::FEATHER_ENABLED, enabled);
    emit featherEnabledChanged(enabled);
}

void StrokeTool::setFeather(qreal feather)
{
    mSettings->setBaseValue(StrokeSettings::FEATHER_VALUE, feather);
    emit featherChanged(mSettings->feather());
}

void StrokeTool::setWidth(qreal width)
{
    mSettings->setBaseValue(StrokeSettings::WIDTH_VALUE, width);
    emit widthChanged(mSettings->width());
}

void StrokeTool::setPressureEnabled(bool enabled)
{
    mSettings->setBaseValue(StrokeSettings::PRESSURE_ENABLED, enabled);
    emit pressureEnabledChanged(enabled);
}

void StrokeTool::setFillContourEnabled(bool enabled)
{
    mSettings->setBaseValue(StrokeSettings::FILLCONTOUR_ENABLED, enabled);
    emit fillContourEnabledChanged(enabled);
}

void StrokeTool::setAntiAliasingEnabled(bool enabled)
{
    mSettings->setBaseValue(StrokeSettings::ANTI_ALIASING_ENABLED, enabled);
    emit antiAliasingEnabledChanged(enabled);
}

void StrokeTool::setStrokeInvisibleEnabled(bool enabled)
{
    mSettings->setBaseValue(StrokeSettings::INVISIBILITY_ENABLED, enabled);
    emit invisibleStrokeEnabledChanged(enabled);
}
