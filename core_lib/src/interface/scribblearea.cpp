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

#include "scribblearea.h"

#include <cmath>
#include <QGuiApplication>
#include <QMessageBox>
#include <QPixmapCache>
#include <QTimer>

#include "pointerevent.h"
#include "beziercurve.h"
#include "object.h"
#include "editor.h"
#include "layerbitmap.h"
#include "layervector.h"
#include "layercamera.h"
#include "bitmapimage.h"
#include "vectorimage.h"
#include "blitrect.h"
#include "tile.h"

#include "onionskinpainteroptions.h"

#include "colormanager.h"
#include "toolmanager.h"
#include "layermanager.h"
#include "playbackmanager.h"
#include "viewmanager.h"
#include "selectionmanager.h"
#include "overlaymanager.h"

ScribbleArea::ScribbleArea(QWidget* parent) : QWidget(parent),
    mCanvasPainter(mCanvas),
    mCameraPainter(mCanvas)
{
    setObjectName("ScribbleArea");

    // Qt::WA_StaticContents ensure that the widget contents are rooted to the top-left corner
    // and don't change when the widget is resized.
    setAttribute(Qt::WA_StaticContents);
}

ScribbleArea::~ScribbleArea()
{
}

bool ScribbleArea::init()
{
    mPrefs = mEditor->preference();
    mDoubleClickTimer = new QTimer(this);
    mMouseFilterTimer = new QTimer(this);

    connect(mPrefs, &PreferenceManager::optionChanged, this, &ScribbleArea::settingUpdated);
    connect(mEditor->tools(), &ToolManager::toolPropertyChanged, this, &ScribbleArea::onToolPropertyUpdated);
    connect(mEditor->tools(), &ToolManager::toolChanged, this, &ScribbleArea::onToolChanged);

    connect(mDoubleClickTimer, &QTimer::timeout, this, &ScribbleArea::handleDoubleClick);
    connect(mMouseFilterTimer, &QTimer::timeout, this, &ScribbleArea::tabletReleaseEventFired);

    connect(mEditor->select(), &SelectionManager::selectionChanged, this, &ScribbleArea::onSelectionChanged);
    connect(mEditor->select(), &SelectionManager::needDeleteSelection, this, &ScribbleArea::deleteSelection);

    connect(&mTiledBuffer, &TiledBuffer::tileUpdated, this, &ScribbleArea::onTileUpdated);
    connect(&mTiledBuffer, &TiledBuffer::tileCreated, this, &ScribbleArea::onTileCreated);

    mDoubleClickTimer->setInterval(50);
    mMouseFilterTimer->setInterval(50);

    const int curveSmoothingLevel = mPrefs->getInt(SETTING::CURVE_SMOOTHING);
    mCurveSmoothingLevel = curveSmoothingLevel / 20.0; // default value is 1.0

    mMakeInvisible = false;

    mLayerVisibility = static_cast<LayerVisibility>(mPrefs->getInt(SETTING::LAYER_VISIBILITY));

    mDeltaFactor = mEditor->preference()->isOn(SETTING::INVERT_SCROLL_ZOOM_DIRECTION) ? -1 : 1;

    setMouseTracking(true); // reacts to mouse move events, even if the button is not pressed
#if QT_VERSION >= QT_VERSION_CHECK(5, 9, 0)
    setTabletTracking(true); // tablet tracking first added in 5.9
#endif

    setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding));

    QPixmapCache::setCacheLimit(100 * 1024); // unit is kb, so it's 100MB cache
    mPixmapCacheKeys.clear();

    return true;
}

void ScribbleArea::settingUpdated(SETTING setting)
{
    switch (setting)
    {
    case SETTING::CURVE_SMOOTHING:
        setCurveSmoothing(mPrefs->getInt(SETTING::CURVE_SMOOTHING));
        break;
    case SETTING::TOOL_CURSOR:
        updateToolCursor();
        break;
    case SETTING::ONION_PREV_FRAMES_NUM:
    case SETTING::ONION_NEXT_FRAMES_NUM:
    case SETTING::ONION_MIN_OPACITY:
    case SETTING::ONION_MAX_OPACITY:
        invalidateAllCache();
        break;
    case SETTING::ANTIALIAS:
    case SETTING::GRID:
    case SETTING::GRID_SIZE_W:
    case SETTING::GRID_SIZE_H:
    case SETTING::OVERLAY_CENTER:
    case SETTING::OVERLAY_THIRDS:
    case SETTING::OVERLAY_GOLDEN:
    case SETTING::OVERLAY_SAFE:
    case SETTING::OVERLAY_PERSPECTIVE1:
    case SETTING::OVERLAY_PERSPECTIVE2:
    case SETTING::OVERLAY_PERSPECTIVE3:
    case SETTING::ACTION_SAFE_ON:
    case SETTING::ACTION_SAFE:
    case SETTING::TITLE_SAFE_ON:
    case SETTING::TITLE_SAFE:
    case SETTING::OVERLAY_SAFE_HELPER_TEXT_ON:
    case SETTING::PREV_ONION:
    case SETTING::NEXT_ONION:
    case SETTING::ONION_BLUE:
    case SETTING::ONION_RED:
    case SETTING::INVISIBLE_LINES:
    case SETTING::OUTLINES:
    case SETTING::ONION_TYPE:
    case SETTING::ONION_WHILE_PLAYBACK:
        invalidateAllCache();
        break;
    case SETTING::ONION_MUTLIPLE_LAYERS:
        invalidateAllCache();
        break;
    case SETTING::LAYER_VISIBILITY_THRESHOLD:
    case SETTING::LAYER_VISIBILITY:
        setLayerVisibility(static_cast<LayerVisibility>(mPrefs->getInt(SETTING::LAYER_VISIBILITY)));
        break;
    case SETTING::INVERT_SCROLL_ZOOM_DIRECTION:
        mDeltaFactor = mEditor->preference()->isOn(SETTING::INVERT_SCROLL_ZOOM_DIRECTION) ? -1 : 1;
        break;
    default:
        break;
    }

}

void ScribbleArea::updateToolCursor()
{
    setCursor(currentTool()->cursor());
}

void ScribbleArea::setCurveSmoothing(int newSmoothingLevel)
{
    mCurveSmoothingLevel = newSmoothingLevel / 20.0;
    invalidatePainterCaches();
}

void ScribbleArea::setEffect(SETTING e, bool isOn)
{
    mPrefs->set(e, isOn);
    invalidatePainterCaches();
}

/************************************************************************************/
// update methods

void ScribbleArea::onTileUpdated(TiledBuffer* tiledBuffer, Tile* tile)
{
    Q_UNUSED(tiledBuffer);
    const QRectF& mappedRect = mEditor->view()->getView().mapRect(QRectF(tile->bounds()));
    update(mappedRect.toAlignedRect());
}

void ScribbleArea::onTileCreated(TiledBuffer* tiledBuffer, Tile* tile)
{
    Q_UNUSED(tiledBuffer)
    const QRectF& mappedRect = mEditor->view()->getView().mapRect(QRectF(tile->bounds()));
    update(mappedRect.toAlignedRect());
}

void ScribbleArea::updateFrame()
{
    if (currentTool()->type() == POLYLINE) { update();}
    else if (currentTool()->isActive() && currentTool()->isDrawingTool()) {
        return;
    }

    update();
}

void ScribbleArea::invalidateCacheForDirtyFrames()
{
    Layer* currentLayer = mEditor->layers()->currentLayer();
    for (int pos : currentLayer->dirtyFrames()) {

        invalidateCacheForFrame(pos);
        invalidateOnionSkinsCacheAround(pos);
    }
    currentLayer->clearDirtyFrames();
}

void ScribbleArea::invalidateOnionSkinsCacheAround(int frameNumber)
{
    if (frameNumber < 0) { return; }

    bool isOnionAbsolute = mPrefs->getString(SETTING::ONION_TYPE) == "absolute";
    Layer *layer = mEditor->layers()->currentLayer(0);

    // The current layer can be null if updateFrame is triggered when creating a new project
    if (!layer) return;

    if (mPrefs->isOn(SETTING::PREV_ONION))
    {
        int onionFrameNumber = frameNumber;
        if (isOnionAbsolute)
        {
            onionFrameNumber = layer->getPreviousFrameNumber(onionFrameNumber + 1, true);
        }

        for(int i = 1; i <= mPrefs->getInt(SETTING::ONION_PREV_FRAMES_NUM); i++)
        {
            onionFrameNumber = layer->getPreviousFrameNumber(onionFrameNumber, isOnionAbsolute);
            if (onionFrameNumber < 0) break;

            invalidateCacheForFrame(onionFrameNumber);
        }
    }

    if (mPrefs->isOn(SETTING::NEXT_ONION))
    {
        int onionFrameNumber = frameNumber;

        for(int i = 1; i <= mPrefs->getInt(SETTING::ONION_NEXT_FRAMES_NUM); i++)
        {
            onionFrameNumber = layer->getNextFrameNumber(onionFrameNumber, isOnionAbsolute);
            if (onionFrameNumber < 0) break;

            invalidateCacheForFrame(onionFrameNumber);
        }
    }
}

void ScribbleArea::invalidateAllCache()
{
    if (currentTool()->type() == POLYLINE) { update(); }
    else if (currentTool()->isDrawingTool() && currentTool()->isActive()) { return; }

    QPixmapCache::clear();
    mPixmapCacheKeys.clear();
    invalidatePainterCaches();
    mEditor->layers()->currentLayer()->clearDirtyFrames();

    updateFrame();
}

void ScribbleArea::invalidateCacheForFrame(int frameNumber)
{
    auto cacheKeyIter = mPixmapCacheKeys.find(static_cast<unsigned int>(frameNumber));
    if (cacheKeyIter != mPixmapCacheKeys.end())
    {
        QPixmapCache::remove(cacheKeyIter.value());
        unsigned int key = cacheKeyIter.key();
        mPixmapCacheKeys.remove(key);
    }
}

void ScribbleArea::invalidatePainterCaches()
{
    mCameraPainter.resetCache();
    mCanvasPainter.resetLayerCache();
    updateFrame();
}

void ScribbleArea::onToolPropertyUpdated(ToolType, ToolPropertyType type)
{
    switch (type)
    {
    case ToolPropertyType::CAMERAPATH:
        onFrameModified(mEditor->currentFrame());
        break;
    default:
        break;
    }
}

void ScribbleArea::onToolChanged(ToolType)
{
    int frame = mEditor->currentFrame();
    prepOverlays(frame);
    prepCameraPainter(frame);
    invalidateCacheForFrame(frame);
    updateFrame();
}


void ScribbleArea::onPlayStateChanged()
{
    int currentFrame = mEditor->currentFrame();
    if (mPrefs->isOn(SETTING::PREV_ONION) ||
        mPrefs->isOn(SETTING::NEXT_ONION)) {
        invalidatePainterCaches();
    }

    prepOverlays(currentFrame);
    prepCameraPainter(currentFrame);
    invalidateCacheForFrame(currentFrame);
    updateFrame();
}

void ScribbleArea::onScrubbed(int frameNumber)
{
    Q_UNUSED(frameNumber)
    invalidatePainterCaches();
    updateFrame();
}

void ScribbleArea::onFramesModified()
{
    invalidateCacheForDirtyFrames();
    if (mPrefs->isOn(SETTING::PREV_ONION) || mPrefs->isOn(SETTING::NEXT_ONION)) {
        invalidatePainterCaches();
    }
    updateFrame();
}

void ScribbleArea::onFrameModified(int frameNumber)
{
    if (mPrefs->isOn(SETTING::PREV_ONION) || mPrefs->isOn(SETTING::NEXT_ONION)) {
        invalidateOnionSkinsCacheAround(frameNumber);
        invalidatePainterCaches();
    }
    invalidateCacheForFrame(frameNumber);
    updateFrame();
}

void ScribbleArea::onViewChanged()
{
    invalidateAllCache();
}

void ScribbleArea::onLayerChanged()
{
    invalidateAllCache();
}

void ScribbleArea::onSelectionChanged()
{
    int currentFrame = mEditor->currentFrame();
    invalidateCacheForFrame(currentFrame);
    updateFrame();
}

void ScribbleArea::onOnionSkinTypeChanged()
{
    invalidateAllCache();
}

void ScribbleArea::onObjectLoaded()
{
    invalidateAllCache();
}

bool ScribbleArea::event(QEvent *event)
{
    bool processed = false;
    if (event->type() == QEvent::WindowDeactivate)
    {
        editor()->tools()->clearTemporaryTool();
        processed = true;
    } else if (event->type() == QEvent::Enter)
    {
        processed = currentTool()->enterEvent(static_cast<QEnterEvent*>(event)) || processed;
    } else if (event->type() == QEvent::Leave)
    {
        processed = currentTool()->leaveEvent(event) || processed;
    }

    return QWidget::event(event) || processed;
}

/************************************************************************/
/* key event handlers                                                   */
/************************************************************************/

void ScribbleArea::keyPressEvent(QKeyEvent *event)
{
    // Don't handle this event on auto repeat
    if (event->isAutoRepeat()) { return; }

    if (isPointerInUse()) { return; } // prevents shortcuts calls while drawing

    if (currentTool()->keyPressEvent(event))
    {
        return; // has been handled by tool
    }

    // --- fixed control key shortcuts ---
    if (event->modifiers() == (Qt::ControlModifier | Qt::ShiftModifier) &&
        editor()->tools()->setTemporaryTool(ERASER, {}, event->modifiers()))
    {
        return;
    }

    // ---- fixed normal keys ----

    auto selectMan = mEditor->select();
    bool isSomethingSelected = selectMan->somethingSelected();
    if (isSomethingSelected)
    {
        keyEventForSelection(event);
    }
    else
    {
        keyEvent(event);
    }
}

void ScribbleArea::keyEventForSelection(QKeyEvent* event)
{
    auto selectMan = mEditor->select();
    switch (event->key())
    {
    case Qt::Key_Right:
        selectMan->translate(QPointF(1, 0));
        selectMan->calculateSelectionTransformation();
        emit mEditor->frameModified(mEditor->currentFrame());
        return;
    case Qt::Key_Left:
        selectMan->translate(QPointF(-1, 0));
        selectMan->calculateSelectionTransformation();
        emit mEditor->frameModified(mEditor->currentFrame());
        return;
    case Qt::Key_Up:
        selectMan->translate(QPointF(0, -1));
        selectMan->calculateSelectionTransformation();
        emit mEditor->frameModified(mEditor->currentFrame());
        return;
    case Qt::Key_Down:
        selectMan->translate(QPointF(0, 1));
        selectMan->calculateSelectionTransformation();
        emit mEditor->frameModified(mEditor->currentFrame());
        return;
    case Qt::Key_Return:
        applyTransformedSelection();
        mEditor->deselectAll();
        return;
    case Qt::Key_Escape:
        cancelTransformedSelection();
        mEditor->deselectAll();
        return;
    case Qt::Key_Backspace:
        deleteSelection();
        mEditor->deselectAll();
        return;
    case Qt::Key_Space:
        if (editor()->tools()->setTemporaryTool(HAND, Qt::Key_Space, Qt::NoModifier))
        {
            return;
        }
        break;
    default:
        break;
    }
    event->ignore();
}

void ScribbleArea::keyEvent(QKeyEvent* event)
{
    switch (event->key())
    {
    case Qt::Key_Right:
        mEditor->scrubForward();
        break;
    case Qt::Key_Left:
        mEditor->scrubBackward();
        break;
    case Qt::Key_Up:
        mEditor->layers()->gotoNextLayer();
        break;
    case Qt::Key_Down:
        mEditor->layers()->gotoPreviouslayer();
        break;
    case Qt::Key_Space:
        if(editor()->tools()->setTemporaryTool(HAND, Qt::Key_Space, Qt::NoModifier))
        {
            return;
        }
        break;
    default:
        break;
    }
    event->ignore();
}

void ScribbleArea::keyReleaseEvent(QKeyEvent *event)
{
    // Don't handle this event on auto repeat
    //
    if (event->isAutoRepeat()) {
        return;
    }

    if (event->key() == 0)
    {
        editor()->tools()->tryClearTemporaryTool(Qt::Key_unknown);
    }
    else
    {
        editor()->tools()->tryClearTemporaryTool(static_cast<Qt::Key>(event->key()));
    }

    if (isPointerInUse()) { return; }

    if (currentTool()->keyReleaseEvent(event))
    {
        // has been handled by tool
        return;
    }
}

/************************************************************************************/
// mouse and tablet event handlers
void ScribbleArea::wheelEvent(QWheelEvent* event)
{
    // Don't change view if the tool is in use
    if (isPointerInUse()) return;

    static const bool isX11 = QGuiApplication::platformName() == "xcb";
    const QPoint pixels = event->pixelDelta();
    const QPoint angle = event->angleDelta();
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
    const QPointF offset = mEditor->view()->mapScreenToCanvas(event->position());
#else
    const QPointF offset = mEditor->view()->mapScreenToCanvas(event->posF());
#endif

    const qreal currentScale = mEditor->view()->scaling();

    // From the pixelDelta documentation: On X11 this value is driver-specific and unreliable, use angleDelta() instead
    int delta = 0;
    if (!isX11 && !pixels.isNull())
    {
        delta = pixels.y();
    }
    else if (!angle.isNull()) // Wheel based delta
    {
        delta = angle.y();
    }

    if (delta != 0) {
        const qreal newScale = currentScale * std::pow(100, (delta * mDeltaFactor) / (12.0 * 120));
        mEditor->view()->scaleAtOffset(newScale, offset);
    }
    event->accept();
}

void ScribbleArea::tabletEvent(QTabletEvent *e)
{
    PointerEvent event(e, mEditor->view()->mapScreenToCanvas(e->posF()));

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    if (event.pointerType() == QPointingDevice::PointerType::Eraser)
#else
    if (event.pointerType() == QTabletEvent::Eraser)
#endif
    {
        editor()->tools()->tabletSwitchToEraser();
    }
    else
    {
        editor()->tools()->tabletRestorePrevTool();
    }

    if (event.eventType() == PointerEvent::Press)
    {
        event.accept();
        if (mIsFirstClick)
        {
            mIsFirstClick = false;
            mDoubleClickTimer->start();
            pointerPressEvent(&event);
        }
        else
        {
            qreal distance = QLineF(e->posF(), mTabletPressPos).length();
            mTabletPressPos = e->posF();

            if (mDoubleClickMillis <= DOUBLE_CLICK_THRESHOLD && distance < 5.0) {
                currentTool()->pointerDoubleClickEvent(&event);
            }
            else
            {
                // in case we handled the event as double click but really should have handled it as single click.
                pointerPressEvent(&event);
            }
        }
        mTabletInUse = event.isAccepted();
    }
    else if (event.eventType() == PointerEvent::Move)
    {
        if (!(event.buttons() & (Qt::LeftButton | Qt::RightButton)) || mTabletInUse)
        {
            pointerMoveEvent(&event);
        }
    }
    else if (event.eventType() == PointerEvent::Release)
    {
        mTabletReleaseMillisAgo = 0;
        mMouseFilterTimer->start();
        if (mTabletInUse)
        {
            pointerReleaseEvent(&event);
            mTabletInUse = false;
        }
    }

#if defined Q_OS_LINUX
    // Generate mouse events on linux to work around bug where the
    // widget will not receive mouseEnter/mouseLeave
    // events and the cursor will not update correctly.
    // See https://codereview.qt-project.org/c/qt/qtbase/+/255384
    // Scribblearea should not do anything with the mouse event when mTabletInUse is true.
    event.ignore();
#else
    // Always accept so that mouse events are not generated (theoretically)
    // Unfortunately Windows sometimes generates the events anyway
    // As long as mTabletInUse is true, mouse events *should* be ignored even when
    // the are generated
    event.accept();
#endif
}

void ScribbleArea::pointerPressEvent(PointerEvent* event)
{
    bool isCameraLayer = mEditor->layers()->currentLayer()->type() == Layer::CAMERA;
    if ((currentTool()->type() != HAND || isCameraLayer) && (event->button() != Qt::RightButton) && (event->button() != Qt::MiddleButton || isCameraLayer))
    {
        Layer* layer = mEditor->layers()->currentLayer();
        if (!layer->visible())
        {
            event->ignore();
            // This needs to be async so that mTabletInUse is set to false before
            // further events are created (modal dialogs do not currently block tablet events)
            QTimer::singleShot(0, this, &ScribbleArea::showLayerNotVisibleWarning);
            return;
        }
    }

    if (event->buttons() & (Qt::MiddleButton | Qt::RightButton) &&
        editor()->tools()->setTemporaryTool(HAND, event->buttons()))
    {
        currentTool()->pointerPressEvent(event);
    } else if (event->button() == Qt::LeftButton)
    {
        currentTool()->pointerPressEvent(event);
    }
}

void ScribbleArea::pointerMoveEvent(PointerEvent* event)
{
    currentTool()->pointerMoveEvent(event);
}

void ScribbleArea::pointerReleaseEvent(PointerEvent* event)
{
    currentTool()->pointerReleaseEvent(event);

    editor()->tools()->tryClearTemporaryTool(event->button());
}

void ScribbleArea::handleDoubleClick()
{
    mDoubleClickMillis += 100;

    if (mDoubleClickMillis >= DOUBLE_CLICK_THRESHOLD)
    {
        mDoubleClickMillis = 0;
        mIsFirstClick = true;
        mDoubleClickTimer->stop();
    }
}

void ScribbleArea::tabletReleaseEventFired()
{
    // Under certain circumstances, a mouse press event will fire after a tablet release event.
    // This causes unexpected behaviors for some tools, e.g., the bucket tool.
    // The problem only seems to occur on Windows and only when tapping.
    // Prior to this fix, the event queue would look like this:
    // e.g.: TabletPress -> TabletRelease -> MousePress
    // The following will filter mouse events created after a tablet release event.
    mTabletReleaseMillisAgo += 50;

    if (mTabletReleaseMillisAgo >= MOUSE_FILTER_THRESHOLD) {
        mTabletReleaseMillisAgo = 0;
        mMouseFilterTimer->stop();
    }
}

void ScribbleArea::mousePressEvent(QMouseEvent* e)
{
    if (mTabletInUse || (mMouseFilterTimer->isActive() && mTabletReleaseMillisAgo < MOUSE_FILTER_THRESHOLD))
    {
        e->ignore();
        return;
    }

    PointerEvent event(e, mEditor->view()->mapScreenToCanvas(e->localPos()));
    pointerPressEvent(&event);
    mMouseInUse = event.isAccepted();
}

void ScribbleArea::mouseMoveEvent(QMouseEvent* e)
{
    if (mTabletInUse || (mMouseFilterTimer->isActive() && mTabletReleaseMillisAgo < MOUSE_FILTER_THRESHOLD)) { e->ignore(); return; }

    PointerEvent event(e, mEditor->view()->mapScreenToCanvas(e->localPos()));
    pointerMoveEvent(&event);
}

void ScribbleArea::mouseReleaseEvent(QMouseEvent* e)
{
    if (mTabletInUse || (mMouseFilterTimer->isActive() && mTabletReleaseMillisAgo < MOUSE_FILTER_THRESHOLD)) { e->ignore(); return; }

    PointerEvent event(e, mEditor->view()->mapScreenToCanvas(e->localPos()));
    pointerReleaseEvent(&event);
    mMouseInUse = (e->buttons() & Qt::RightButton) || (e->buttons() & Qt::LeftButton);
}

void ScribbleArea::mouseDoubleClickEvent(QMouseEvent* e)
{
    if (mTabletInUse) { e->ignore(); return; }

    PointerEvent event(e, mEditor->view()->mapScreenToCanvas(e->localPos()));
    currentTool()->pointerDoubleClickEvent(&event);
}

void ScribbleArea::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    mDevicePixelRatio = devicePixelRatioF();
    mCanvas = QPixmap(QSizeF(size() * mDevicePixelRatio).toSize());
    mCanvas.setDevicePixelRatio(mDevicePixelRatio);
    mEditor->view()->setCanvasSize(size());

    invalidateCacheForFrame(mEditor->currentFrame());
    invalidatePainterCaches();
    mCanvasPainter.reset();
    mCameraPainter.reset();
}

void ScribbleArea::showLayerNotVisibleWarning()
{
    QMessageBox::warning(this, tr("Warning"),
                         tr("You are trying to modify a hidden layer! Please select another layer (or make the current layer visible)."),
                         QMessageBox::Ok,
                         QMessageBox::Ok);
}

void ScribbleArea::paintBitmapBuffer()
{
    LayerBitmap* layer = static_cast<LayerBitmap*>(mEditor->layers()->currentLayer());
    Q_ASSERT(layer);
    Q_ASSERT(layer->type() == Layer::BITMAP);

    int frameNumber = mEditor->currentFrame();

    // If there is no keyframe at or before the current position,
    // just return (since we have nothing to paint on).
    if (layer->getLastKeyFrameAtPosition(frameNumber) == nullptr)
    {
        updateFrame();
        return;
    }

    BitmapImage* targetImage = currentBitmapImage(layer);
    if (targetImage != nullptr)
    {
        QPainter::CompositionMode cm = QPainter::CompositionMode_SourceOver;
        switch (currentTool()->type())
        {
        case ERASER:
            cm = QPainter::CompositionMode_DestinationOut;
            break;
        case BRUSH:
        case PEN:
        case PENCIL:
            if (currentTool()->properties.preserveAlpha)
            {
                cm = QPainter::CompositionMode_SourceOver;
            }
            break;
        default: //nothing
            break;
        }
        targetImage->paste(&mTiledBuffer, cm);
    }

    QRect rect = mEditor->view()->mapCanvasToScreen(mTiledBuffer.bounds()).toRect();

    update(rect);

    layer->setModified(frameNumber, true);
    mTiledBuffer.clear();
}

void ScribbleArea::clearDrawingBuffer()
{
    mTiledBuffer.clear();
}

void ScribbleArea::handleDrawingOnEmptyFrame()
{
    auto layer = mEditor->layers()->currentLayer();

    if (!layer || !layer->isPaintable())
    {
        return;
    }

    if (currentTool()->type() == ERASER) {
        return;
    }

    int frameNumber = mEditor->currentFrame();
    if (layer->getKeyFrameAt(frameNumber)) { return; }

    // Drawing on an empty frame; take action based on preference.
    int action = mPrefs->getInt(SETTING::DRAW_ON_EMPTY_FRAME_ACTION);
    auto previousKeyFrame = layer->getLastKeyFrameAtPosition(frameNumber);
    switch (action)
    {
    case KEEP_DRAWING_ON_PREVIOUS_KEY:
    {
        if (previousKeyFrame == nullptr) {
            mEditor->addNewKey();
        } else {
            onFrameModified(previousKeyFrame->pos());
        }
        break;
    }
    case DUPLICATE_PREVIOUS_KEY:
    {
        if (previousKeyFrame != nullptr) {
            KeyFrame* dupKey = previousKeyFrame->clone();
            layer->addKeyFrame(frameNumber, dupKey);
            mEditor->scrubTo(frameNumber);
            break;
        }
    }
    // if the previous keyframe doesn't exist,
    // an empty keyframe needs to be created, so
    // fallthrough
    case CREATE_NEW_KEY:
        mEditor->addNewKey();

        // Refresh canvas
        drawCanvas(frameNumber, mCanvas.rect());
        break;
    default:
        break;
    }
}

void ScribbleArea::paintEvent(QPaintEvent* event)
{
    int currentFrame = mEditor->currentFrame();
    if (!currentTool()->isActive())
    {
        // --- we retrieve the canvas from the cache; we create it if it doesn't exist
        const int frameNumber = mEditor->layers()->lastFrameAtFrame(currentFrame);

        if (frameNumber < 0)
        {
            drawCanvas(currentFrame, event->rect());
        }
        else
        {
            auto cacheKeyIter = mPixmapCacheKeys.find(static_cast<unsigned>(frameNumber));

            if (cacheKeyIter == mPixmapCacheKeys.end() || !QPixmapCache::find(cacheKeyIter.value(), &mCanvas))
            {
                drawCanvas(currentFrame, event->rect());
                mPixmapCacheKeys[static_cast<unsigned>(currentFrame)] = QPixmapCache::insert(mCanvas);
                //qDebug() << "Repaint canvas!";
            }
            else
            {
                // Simply use the cached canvas from PixmapCache
            }
        }
    }
    else
    {
        prepCanvas(currentFrame);
        prepCameraPainter(currentFrame);
        prepOverlays(currentFrame);

        mCanvasPainter.paintCached(event->rect());
        mCameraPainter.paintCached(event->rect());
    }

    if (currentTool()->type() == MOVE)
    {
        Layer* layer = mEditor->layers()->currentLayer();
        Q_CHECK_PTR(layer);
        if (layer->type() == Layer::VECTOR)
        {
            VectorImage* vectorImage = currentVectorImage(layer);
            if (vectorImage != nullptr)
            {
                vectorImage->setModified(true);
            }
        }
    }

    QPainter painter(this);

    // paints the canvas
    painter.setWorldMatrixEnabled(false);

    // In other places we use the blitRect to paint the buffer pixmap, however
    // the main pixmap which needs to be scaled accordingly to DPI, which is not accounted for when using the event rect
    // instead we can set a clipRect to avoid the area being updated needlessly
    painter.setClipRect(event->rect());
    painter.drawPixmap(QPointF(), mCanvas);

    currentTool()->paint(painter, event->rect());

    if (!editor()->playback()->isPlaying())    // we don't need to display the following when the animation is playing
    {
        Layer* layer = mEditor->layers()->currentLayer();
        if (layer->type() == Layer::VECTOR)
        {
            VectorImage* vectorImage = currentVectorImage(layer);
            if (vectorImage != nullptr)
            {
                switch (currentTool()->type())
                {
                case SMUDGE:
                case HAND:
                {
                    auto selectMan = mEditor->select();
                    painter.save();
                    painter.setWorldMatrixEnabled(false);
                    painter.setRenderHint(QPainter::Antialiasing, false);
                    // ----- paints the edited elements
                    QPen pen2(Qt::black, 0.5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
                    painter.setPen(pen2);
                    QColor color;
                    // ------------ vertices of the edited curves
                    color = QColor(200, 200, 200);
                    painter.setBrush(color);
                    VectorSelection vectorSelection = selectMan->vectorSelection;
                    for (int k = 0; k < vectorSelection.curve.size(); k++)
                    {
                        int curveNumber = vectorSelection.curve.at(k);

                        for (int vertexNumber = -1; vertexNumber < vectorImage->getCurveSize(curveNumber); vertexNumber++)
                        {
                            QPointF vertexPoint = vectorImage->getVertex(curveNumber, vertexNumber);
                            QRectF rectangle(mEditor->view()->mapCanvasToScreen(vertexPoint) - QPointF(3.0, 3.0), QSizeF(7, 7));
                            if (rect().contains(mEditor->view()->mapCanvasToScreen(vertexPoint).toPoint()))
                            {
                                painter.drawRect(rectangle);
                            }
                        }
                    }
                    // ------------ selected vertices of the edited curves
                    color = QColor(100, 100, 255);
                    painter.setBrush(color);
                    for (int k = 0; k < vectorSelection.vertex.size(); k++)
                    {
                        VertexRef vertexRef = vectorSelection.vertex.at(k);
                        QPointF vertexPoint = vectorImage->getVertex(vertexRef);
                        QRectF rectangle0 = QRectF(mEditor->view()->mapCanvasToScreen(vertexPoint) - QPointF(3.0, 3.0), QSizeF(7, 7));
                        painter.drawRect(rectangle0);
                    }
                    // ----- paints the closest vertices
                    color = QColor(255, 0, 0);
                    painter.setBrush(color);
                    QList<VertexRef> closestVertices = selectMan->closestVertices();
                    if (vectorSelection.curve.size() > 0)
                    {
                        for (int k = 0; k < closestVertices.size(); k++)
                        {
                            VertexRef vertexRef = closestVertices.at(k);
                            QPointF vertexPoint = vectorImage->getVertex(vertexRef);

                            QRectF rectangle = QRectF(mEditor->view()->mapCanvasToScreen(vertexPoint) - QPointF(3.0, 3.0), QSizeF(7, 7));
                            painter.drawRect(rectangle);
                        }
                    }
                    painter.restore();
                    break;
                }
                default:
                {
                    break;
                }
                } // end switch
            }
        }

        mOverlayPainter.paint(painter, rect());

        // paints the selection outline
        if (mEditor->select()->somethingSelected())
        {
            paintSelectionVisuals(painter);
        }
    }

    // outlines the frame of the viewport
#ifdef _DEBUG
    painter.setWorldMatrixEnabled(false);
    painter.setPen(QPen(Qt::gray, 2));
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(QRect(0, 0, width(), height()));
#endif

    event->accept();
}

void ScribbleArea::paintSelectionVisuals(QPainter &painter)
{
    Object* object = mEditor->object();

    auto selectMan = mEditor->select();

    QRectF currentSelectionRect = selectMan->mySelectionRect();

    if (currentSelectionRect.isEmpty()) { return; }

    TransformParameters params = { currentSelectionRect, editor()->view()->getView(), selectMan->selectionTransform() };
    mSelectionPainter.paint(painter, object, mEditor->currentLayerIndex(), currentTool(), params);
    emit selectionUpdated();
}

BitmapImage* ScribbleArea::currentBitmapImage(Layer* layer) const
{
    Q_ASSERT(layer->type() == Layer::BITMAP);
    auto bitmapLayer = static_cast<LayerBitmap*>(layer);
    return bitmapLayer->getLastBitmapImageAtFrame(mEditor->currentFrame());
}

VectorImage* ScribbleArea::currentVectorImage(Layer* layer) const
{
    Q_ASSERT(layer->type() == Layer::VECTOR);
    auto vectorLayer = static_cast<LayerVector*>(layer);
    return vectorLayer->getLastVectorImageAtFrame(mEditor->currentFrame(), 0);
}

void ScribbleArea::prepCameraPainter(int frame)
{
    Object* object = mEditor->object();

    mCameraPainter.preparePainter(object,
                                  mEditor->currentLayerIndex(),
                                  frame,
                                  mEditor->view()->getView(),
                                  mEditor->playback()->isPlaying(),
                                  mLayerVisibility,
                                  mPrefs->getFloat(SETTING::LAYER_VISIBILITY_THRESHOLD),
                                  mEditor->view()->getScaleInversed());

    OnionSkinPainterOptions onionSkinOptions;
    onionSkinOptions.enabledWhilePlaying = mPrefs->getInt(SETTING::ONION_WHILE_PLAYBACK);
    onionSkinOptions.isPlaying = mEditor->playback()->isPlaying();
    onionSkinOptions.isAbsolute = (mPrefs->getString(SETTING::ONION_TYPE) == "absolute");
    onionSkinOptions.skinPrevFrames = mPrefs->isOn(SETTING::PREV_ONION);
    onionSkinOptions.skinNextFrames = mPrefs->isOn(SETTING::NEXT_ONION);
    onionSkinOptions.colorizePrevFrames = mPrefs->isOn(SETTING::ONION_RED);
    onionSkinOptions.colorizeNextFrames = mPrefs->isOn(SETTING::ONION_BLUE);
    onionSkinOptions.framesToSkinPrev = mPrefs->getInt(SETTING::ONION_PREV_FRAMES_NUM);
    onionSkinOptions.framesToSkinNext = mPrefs->getInt(SETTING::ONION_NEXT_FRAMES_NUM);
    onionSkinOptions.maxOpacity = mPrefs->getInt(SETTING::ONION_MAX_OPACITY);
    onionSkinOptions.minOpacity = mPrefs->getInt(SETTING::ONION_MIN_OPACITY);

    mCameraPainter.setOnionSkinPainterOptions(onionSkinOptions);
}

void ScribbleArea::prepCanvas(int frame)
{
    Object* object = mEditor->object();

    CanvasPainterOptions o;
    o.bOnionSkinMultiLayer = mPrefs->isOn(SETTING::ONION_MUTLIPLE_LAYERS);
    o.bAntiAlias = mPrefs->isOn(SETTING::ANTIALIAS);
    o.bThinLines = mPrefs->isOn(SETTING::INVISIBLE_LINES);
    o.bOutlines = mPrefs->isOn(SETTING::OUTLINES);
    o.eLayerVisibility = mLayerVisibility;
    o.fLayerVisibilityThreshold = mPrefs->getFloat(SETTING::LAYER_VISIBILITY_THRESHOLD);
    o.scaling = mEditor->view()->scaling();
    o.cmBufferBlendMode = mEditor->tools()->currentTool()->type() == ToolType::ERASER ? QPainter::CompositionMode_DestinationOut : QPainter::CompositionMode_SourceOver;

    OnionSkinPainterOptions onionSkinOptions;
    onionSkinOptions.enabledWhilePlaying = mPrefs->getInt(SETTING::ONION_WHILE_PLAYBACK);
    onionSkinOptions.isPlaying = mEditor->playback()->isPlaying();
    onionSkinOptions.isAbsolute = (mPrefs->getString(SETTING::ONION_TYPE) == "absolute");
    onionSkinOptions.skinPrevFrames = mPrefs->isOn(SETTING::PREV_ONION);
    onionSkinOptions.skinNextFrames = mPrefs->isOn(SETTING::NEXT_ONION);
    onionSkinOptions.colorizePrevFrames = mPrefs->isOn(SETTING::ONION_RED);
    onionSkinOptions.colorizeNextFrames = mPrefs->isOn(SETTING::ONION_BLUE);
    onionSkinOptions.framesToSkinPrev = mPrefs->getInt(SETTING::ONION_PREV_FRAMES_NUM);
    onionSkinOptions.framesToSkinNext = mPrefs->getInt(SETTING::ONION_NEXT_FRAMES_NUM);
    onionSkinOptions.maxOpacity = mPrefs->getInt(SETTING::ONION_MAX_OPACITY);
    onionSkinOptions.minOpacity = mPrefs->getInt(SETTING::ONION_MIN_OPACITY);

    mCanvasPainter.setOnionSkinOptions(onionSkinOptions);
    mCanvasPainter.setOptions(o);

    ViewManager* vm = mEditor->view();
    SelectionManager* sm = mEditor->select();
    mCanvasPainter.setViewTransform(vm->getView(), vm->getViewInverse());
    mCanvasPainter.setTransformedSelection(sm->mySelectionRect().toRect(), sm->selectionTransform());

    mCanvasPainter.setPaintSettings(object, mEditor->layers()->currentLayerIndex(), frame, &mTiledBuffer);
}

void ScribbleArea::drawCanvas(int frame, QRect rect)
{
    prepCanvas(frame);
    prepCameraPainter(frame);
    prepOverlays(frame);
    mCanvasPainter.paint(rect);
    mCameraPainter.paint(rect);
}

void ScribbleArea::setGaussianGradient(QGradient &gradient, QColor color, qreal opacity, qreal offset)
{
    if (offset < 0) { offset = 0; }
    if (offset > 100) { offset = 100; }

    int r = color.red();
    int g = color.green();
    int b = color.blue();
    qreal a = color.alphaF();

    int mainColorAlpha = qRound(a * 255 * opacity);

    // the more feather (offset), the more softness (opacity)
    int alphaAdded = qRound((mainColorAlpha * offset) / 100);

    gradient.setColorAt(0.0, QColor(r, g, b, mainColorAlpha - alphaAdded));
    gradient.setColorAt(1.0, QColor(r, g, b, 0));
    gradient.setColorAt(1.0 - (offset / 100.0), QColor(r, g, b, mainColorAlpha - alphaAdded));
}

void ScribbleArea::drawPath(QPainterPath path, QPen pen, QBrush brush, QPainter::CompositionMode cm)
{
    mTiledBuffer.drawPath(mEditor->view()->mapScreenToCanvas(path), pen, brush, cm, mPrefs->isOn(SETTING::ANTIALIAS));
}

void ScribbleArea::drawPen(QPointF thePoint, qreal brushWidth, QColor fillColor, bool useAA)
{
    // We use Source as opposed to SourceOver here to avoid the dabs being added on top of each other
    mTiledBuffer.drawBrush(thePoint, brushWidth, Qt::NoPen, QBrush(fillColor, Qt::SolidPattern), QPainter::CompositionMode_Source, useAA);
}

void ScribbleArea::drawPencil(QPointF thePoint, qreal brushWidth, qreal fixedBrushFeather, QColor fillColor, qreal opacity)
{
    drawBrush(thePoint, brushWidth, fixedBrushFeather, fillColor, QPainter::CompositionMode_SourceOver, opacity, true);
}

void ScribbleArea::drawBrush(QPointF thePoint, qreal brushWidth, qreal mOffset, QColor fillColor, QPainter::CompositionMode compMode, qreal opacity, bool usingFeather, bool useAA)
{
    QBrush brush;
    if (usingFeather)
    {
        QRadialGradient radialGrad(thePoint, 0.5 * brushWidth);
        setGaussianGradient(radialGrad, fillColor, opacity, mOffset);
        brush = radialGrad;
    }
    else
    {
        brush = QBrush(fillColor, Qt::SolidPattern);
    }
    mTiledBuffer.drawBrush(thePoint, brushWidth, Qt::NoPen, brush, compMode, useAA);
}

void ScribbleArea::drawPolyline(QPainterPath path, QPen pen, bool useAA)
{
    BlitRect blitRect;

    // In order to clear what was previously dirty, we need to include the previous buffer bound
    // this ensures that we won't see stroke artifacts
    blitRect.extend(mEditor->view()->mapCanvasToScreen(mTiledBuffer.bounds()).toRect());

    QRect updateRect = mEditor->view()->mapCanvasToScreen(path.boundingRect()).toRect();
    // Now extend with the new path bounds mapped to the local coordinate
    blitRect.extend(updateRect);

    mTiledBuffer.clear();
    mTiledBuffer.drawPath(path, pen, Qt::NoBrush, QPainter::CompositionMode_SourceOver, useAA);

    // And update only the affected area
    update(blitRect.adjusted(-1, -1, 1, 1));
}

void ScribbleArea::endStroke()
{
    if (mEditor->layers()->currentLayer()->type() == Layer::BITMAP) {
        paintBitmapBuffer();
    }

    onFrameModified(mEditor->currentFrame());
}

void ScribbleArea::flipSelection(bool flipVertical)
{
    mEditor->select()->flipSelection(flipVertical);
}

void ScribbleArea::prepOverlays(int frame)
{
    OverlayPainterOptions o;

    o.bGrid = mPrefs->isOn(SETTING::GRID);
    o.nGridSizeW = mPrefs->getInt(SETTING::GRID_SIZE_W);
    o.nGridSizeH = mPrefs->getInt(SETTING::GRID_SIZE_H);
    o.bCenter = mPrefs->isOn(SETTING::OVERLAY_CENTER);
    o.bThirds = mPrefs->isOn(SETTING::OVERLAY_THIRDS);
    o.bGoldenRatio = mPrefs->isOn(SETTING::OVERLAY_GOLDEN);
    o.bSafeArea = mPrefs->isOn(SETTING::OVERLAY_SAFE);
    o.bPerspective1 = mPrefs->isOn(SETTING::OVERLAY_PERSPECTIVE1);
    o.bPerspective2 = mPrefs->isOn(SETTING::OVERLAY_PERSPECTIVE2);
    o.bPerspective3 = mPrefs->isOn(SETTING::OVERLAY_PERSPECTIVE3);
    o.nOverlayAngle = mPrefs->getInt(SETTING::OVERLAY_ANGLE);
    o.bActionSafe = mPrefs->isOn(SETTING::ACTION_SAFE_ON);
    o.nActionSafe = mPrefs->getInt(SETTING::ACTION_SAFE);
    o.bShowSafeAreaHelperText = mPrefs->isOn(SETTING::OVERLAY_SAFE_HELPER_TEXT_ON);
    o.bTitleSafe = mPrefs->isOn(SETTING::TITLE_SAFE_ON);
    o.nTitleSafe = mPrefs->getInt(SETTING::TITLE_SAFE);
    o.nOverlayAngle = mPrefs->getInt(SETTING::OVERLAY_ANGLE);
    o.bShowHandle = mEditor->tools()->currentTool()->type() == MOVE && mEditor->layers()->currentLayer()->type() != Layer::CAMERA;

    o.mSinglePerspPoint = mEditor->overlays()->getSinglePerspectivePoint();
    o.mLeftPerspPoint = mEditor->overlays()->getLeftPerspectivePoint();
    o.mRightPerspPoint = mEditor->overlays()->getRightPerspectivePoint();
    o.mMiddlePerspPoint = mEditor->overlays()->getMiddlePerspectivePoint();

    o.nFrameIndex = frame;

    mOverlayPainter.setOptions(o);
    mOverlayPainter.preparePainter(mEditor->layers()->getCameraLayerBelow(mEditor->currentLayerIndex()), palette());

    ViewManager* vm = mEditor->view();
    mOverlayPainter.setViewTransform(vm->getView());
}

void ScribbleArea::blurBrush(BitmapImage *bmiSource_, QPointF srcPoint_, QPointF thePoint_, qreal brushWidth_, qreal mOffset_, qreal opacity_)
{
    QRadialGradient radialGrad(thePoint_, 0.5 * brushWidth_);
    setGaussianGradient(radialGrad, QColor(255, 255, 255, 127), opacity_, mOffset_);

    QRectF srcRect(srcPoint_.x() - 0.5 * brushWidth_, srcPoint_.y() - 0.5 * brushWidth_, brushWidth_, brushWidth_);
    QRectF trgRect(thePoint_.x() - 0.5 * brushWidth_, thePoint_.y() - 0.5 * brushWidth_, brushWidth_, brushWidth_);

    BitmapImage bmiSrcClip = bmiSource_->copy(srcRect.toAlignedRect());
    BitmapImage bmiTmpClip = bmiSrcClip; // TODO: find a shorter way

    bmiTmpClip.drawRect(srcRect, Qt::NoPen, radialGrad, QPainter::CompositionMode_Source, true);
    bmiSrcClip.bounds().moveTo(trgRect.topLeft().toPoint());
    bmiTmpClip.paste(&bmiSrcClip, QPainter::CompositionMode_SourceIn);
    mTiledBuffer.drawImage(*bmiTmpClip.image(), bmiTmpClip.bounds(), QPainter::CompositionMode_SourceOver, mPrefs->isOn(SETTING::ANTIALIAS));
}

void ScribbleArea::liquifyBrush(BitmapImage *bmiSource_, QPointF srcPoint_, QPointF thePoint_, qreal brushWidth_, qreal mOffset_, qreal opacity_)
{
    QPointF delta = (thePoint_ - srcPoint_); // increment vector
    QRectF trgRect(thePoint_.x() - 0.5 * brushWidth_, thePoint_.y() - 0.5 * brushWidth_, brushWidth_, brushWidth_);

    QRadialGradient radialGrad(thePoint_, 0.5 * brushWidth_);
    setGaussianGradient(radialGrad, QColor(255, 255, 255, 255), opacity_, mOffset_);

    // Create gradient brush
    BitmapImage bmiTmpClip;
    bmiTmpClip.drawRect(trgRect, Qt::NoPen, radialGrad, QPainter::CompositionMode_Source, mPrefs->isOn(SETTING::ANTIALIAS));

    // Slide texture/pixels of the source image
    qreal factor, factorGrad;

    for (int yb = bmiTmpClip.top(); yb < bmiTmpClip.bottom(); yb++)
    {
        for (int xb = bmiTmpClip.left(); xb < bmiTmpClip.right(); xb++)
        {
            QColor color;
            color.setRgba(bmiTmpClip.pixel(xb, yb));
            factorGrad = color.alphaF(); // any from r g b a is ok

            int xa = xb - factorGrad * delta.x();
            int ya = yb - factorGrad * delta.y();

            color.setRgba(bmiSource_->pixel(xa, ya));
            factor = color.alphaF();

            if (factor > 0.0)
            {
                color.setRed(color.red() / factor);
                color.setGreen(color.green() / factor);
                color.setBlue(color.blue() / factor);
                color.setAlpha(255); // Premultiplied color

                color.setRed(color.red()*factorGrad);
                color.setGreen(color.green()*factorGrad);
                color.setBlue(color.blue()*factorGrad);
                color.setAlpha(255 * factorGrad); // Premultiplied color

                bmiTmpClip.setPixel(xb, yb, color.rgba());
            }
            else
            {
                bmiTmpClip.setPixel(xb, yb, qRgba(255, 255, 255, 0));
            }
        }
    }
    mTiledBuffer.drawImage(*bmiTmpClip.image(), bmiTmpClip.bounds(), QPainter::CompositionMode_SourceOver, mPrefs->isOn(SETTING::ANTIALIAS));
}

/************************************************************************************/
// view handling

QPointF ScribbleArea::getCentralPoint()
{
    return mEditor->view()->mapScreenToCanvas(QPointF(width() / 2, height() / 2));
}

void ScribbleArea::applyTransformedSelection()
{
    mCanvasPainter.ignoreTransformedSelection();

    Layer* layer = mEditor->layers()->currentLayer();

    bool useAA = mEditor->tools()->currentTool()->properties.useAA;

    if (layer == nullptr) { return; }

    auto selectMan = mEditor->select();
    if (selectMan->somethingSelected())
    {
        if (selectMan->mySelectionRect().isEmpty() || selectMan->selectionTransform().isIdentity()) { return; }

        if (layer->type() == Layer::BITMAP)
        {
            handleDrawingOnEmptyFrame();
            BitmapImage* bitmapImage = currentBitmapImage(layer);
            if (bitmapImage == nullptr) { return; }
            BitmapImage transformedImage = bitmapImage->transformed(selectMan->mySelectionRect().toRect(), selectMan->selectionTransform(), useAA);


            bitmapImage->clear(selectMan->mySelectionRect());
            bitmapImage->paste(&transformedImage, QPainter::CompositionMode_SourceOver);
        }
        else if (layer->type() == Layer::VECTOR)
        {
            // Unfortunately this doesn't work right currently so vector transforms
            // will always be applied on the previous keyframe when on an empty frame
            //handleDrawingOnEmptyFrame();
            VectorImage* vectorImage = currentVectorImage(layer);
            if (vectorImage == nullptr) { return; }

            vectorImage->applySelectionTransformation();
        }

        mEditor->setModified(mEditor->layers()->currentLayerIndex(), mEditor->currentFrame());
    }

    updateFrame();
}

void ScribbleArea::cancelTransformedSelection()
{
    mCanvasPainter.ignoreTransformedSelection();

    auto selectMan = mEditor->select();
    if (selectMan->somethingSelected())
    {
        Layer* layer = mEditor->layers()->currentLayer();
        if (layer == nullptr) { return; }

        if (layer->type() == Layer::VECTOR)
        {
            VectorImage* vectorImage = currentVectorImage(layer);
            if (vectorImage != nullptr)
            {
                vectorImage->setSelectionTransformation(QTransform());
            }
        }

        mEditor->select()->setSelection(selectMan->mySelectionRect(), false);

        selectMan->resetSelectionProperties();
        mOriginalPolygonF = QPolygonF();

        mEditor->setModified(mEditor->layers()->currentLayerIndex(), mEditor->currentFrame());
        updateFrame();
    }
}

void ScribbleArea::toggleThinLines()
{
    bool previousValue = mPrefs->isOn(SETTING::INVISIBLE_LINES);
    setEffect(SETTING::INVISIBLE_LINES, !previousValue);
}

void ScribbleArea::setLayerVisibility(LayerVisibility visibility)
{
    mLayerVisibility = visibility;
    mPrefs->set(SETTING::LAYER_VISIBILITY, static_cast<int>(mLayerVisibility));

    invalidateAllCache();
}

void ScribbleArea::increaseLayerVisibilityIndex()
{
    ++mLayerVisibility;
    mPrefs->set(SETTING::LAYER_VISIBILITY, static_cast<int>(mLayerVisibility));

    invalidateAllCache();
}

void ScribbleArea::decreaseLayerVisibilityIndex()
{
    --mLayerVisibility;
    mPrefs->set(SETTING::LAYER_VISIBILITY, static_cast<int>(mLayerVisibility));

    invalidateAllCache();
}

/************************************************************************************/
// tool handling

BaseTool* ScribbleArea::currentTool() const
{
    return editor()->tools()->currentTool();
}

void ScribbleArea::deleteSelection()
{
    auto selectMan = mEditor->select();
    if (selectMan->somethingSelected())      // there is something selected
    {
        Layer* layer = mEditor->layers()->currentLayer();
        if (layer == nullptr) { return; }

        handleDrawingOnEmptyFrame();

        mEditor->backup(tr("Delete Selection", "Undo Step: clear the selection area."));

        selectMan->clearCurves();
        if (layer->type() == Layer::VECTOR)
        {
            VectorImage* vectorImage = currentVectorImage(layer);
            Q_CHECK_PTR(vectorImage);
            vectorImage->deleteSelection();
        }
        else if (layer->type() == Layer::BITMAP)
        {
            BitmapImage* bitmapImage = currentBitmapImage(layer);
            Q_CHECK_PTR(bitmapImage);
            bitmapImage->clear(selectMan->mySelectionRect());
        }
        mEditor->setModified(mEditor->currentLayerIndex(), mEditor->currentFrame());
    }
}

void ScribbleArea::clearImage()
{
    Layer* layer = mEditor->layers()->currentLayer();
    if (layer == nullptr) { return; }

    if (layer->type() == Layer::VECTOR)
    {
        mEditor->backup(tr("Clear Image", "Undo step text"));

        VectorImage* vectorImage = currentVectorImage(layer);
        if (vectorImage != nullptr)
        {
            vectorImage->clear();
        }
        mEditor->select()->clearCurves();
        mEditor->select()->clearVertices();
    }
    else if (layer->type() == Layer::BITMAP)
    {
        mEditor->backup(tr("Clear Image", "Undo step text"));

        BitmapImage* bitmapImage = currentBitmapImage(layer);
        if (bitmapImage == nullptr) return;
        bitmapImage->clear();
    }
    else
    {
        return; // skip updates when nothing changes
    }
    mEditor->setModified(mEditor->layers()->currentLayerIndex(), mEditor->currentFrame());
}

void ScribbleArea::paletteColorChanged(QColor color)
{
    Q_UNUSED(color)

    for (int i = 0; i < mEditor->layers()->count(); i++)
    {
        Layer* layer = mEditor->layers()->getLayer(i);
        if (layer->type() == Layer::VECTOR)
        {
            VectorImage* vectorImage = static_cast<LayerVector*>(layer)->getVectorImageAtFrame(mEditor->currentFrame());
            if (vectorImage != nullptr)
            {
                vectorImage->modification();
            }
        }
    }

    invalidateAllCache();
}
