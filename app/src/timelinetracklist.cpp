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

#include "timelinetracklist.h"

#include <QApplication>
#include <QResizeEvent>
#include <QInputDialog>
#include <QPainter>
#include <QRegularExpression>
#include <QSettings>

#include "camerapropertiesdialog.h"
#include "editor.h"
#include "keyframe.h"
#include "layermanager.h"
#include "viewmanager.h"
#include "object.h"
#include "playbackmanager.h"
#include "preferencemanager.h"
#include "timeline.h"

#include "cameracontextmenu.h"

TimeLineTrackList::TimeLineTrackList(TimeLine* parent, Editor* editor, TIMELINE_CELL_TYPE type) : QWidget(parent)
{
    mTimeLine = parent;
    mEditor = editor;
    mPrefs = editor->preference();
    mType = type;

    mFrameLength = mPrefs->getInt(SETTING::TIMELINE_SIZE);
    mFontSize = mPrefs->getInt(SETTING::LABEL_FONT_SIZE);
    mFrameSize = mPrefs->getInt(SETTING::FRAME_SIZE);
    mbShortScrub = mPrefs->isOn(SETTING::SHORT_SCRUB);
    mDrawFrameNumber = mPrefs->isOn(SETTING::DRAW_LABEL);

    setMinimumSize(500, 4 * mLayerHeight);
    setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding));
    setAttribute(Qt::WA_OpaquePaintEvent, false);
    setMouseTracking(true);

    connect(mPrefs, &PreferenceManager::optionChanged, this, &TimeLineTrackList::loadSetting);
}

TimeLineTrackList::~TimeLineTrackList()
{
    delete mCache;
}

void TimeLineTrackList::loadSetting(SETTING setting)
{
    switch (setting)
    {
    case SETTING::TIMELINE_SIZE:
        mFrameLength = mPrefs->getInt(SETTING::TIMELINE_SIZE);
        mTimeLine->updateLength();
        break;
    case SETTING::LABEL_FONT_SIZE:
        mFontSize = mPrefs->getInt(SETTING::LABEL_FONT_SIZE);
        break;
    case SETTING::FRAME_SIZE:
        mFrameSize = mPrefs->getInt(SETTING::FRAME_SIZE);
        mTimeLine->updateLength();
        break;
    case SETTING::SHORT_SCRUB:
        mbShortScrub = mPrefs->isOn(SETTING::SHORT_SCRUB);
        break;
    case SETTING::DRAW_LABEL:
        mDrawFrameNumber = mPrefs->isOn(SETTING::DRAW_LABEL);
        break;
    default:
        break;
    }

    updateContent();
}

int TimeLineTrackList::getFrameNumber(int x) const
{
    return mFrameOffset + 1 + (x - mOffsetX) / mFrameSize;
}

int TimeLineTrackList::getFrameX(int frameNumber) const
{
    return mOffsetX + (frameNumber - mFrameOffset) * mFrameSize;
}

void TimeLineTrackList::setFrameSize(int size)
{
    mFrameSize = size;
    mPrefs->set(SETTING::FRAME_SIZE, mFrameSize);
    updateContent();
}

int TimeLineTrackList::getLayerNumber(int y) const
{
    int layerNumber = mLayerOffset + (y - mOffsetY) / mLayerHeight;

    int totalLayerCount = mEditor->object()->getLayerCount();

    // Layers numbers are displayed in descending order
    // The last row is layer 0
    if (layerNumber <= totalLayerCount)
        layerNumber = (totalLayerCount - 1) - layerNumber;
    else
        layerNumber = 0;

    if (y < mOffsetY)
    {
        layerNumber = -1;
    }

    if (layerNumber >= totalLayerCount)
    {
        layerNumber = totalLayerCount;
    }

    //If the mouse release event if fired with mouse off the frame of the application
    // mEditor->object()->getLayerCount() doesn't return the correct value.
    if (layerNumber < -1)
    {
        layerNumber = -1;
    }
    return layerNumber;
}

int TimeLineTrackList::getLayerY(int layerNumber) const
{
    return mOffsetY + (mEditor->object()->getLayerCount() - 1 - layerNumber - mLayerOffset)*mLayerHeight;
}

void TimeLineTrackList::updateFrame(int frameNumber)
{
    int x = getFrameX(frameNumber);
    update(x - mFrameSize, 0, mFrameSize + 1, height());
}

void TimeLineTrackList::updateContent()
{
    mRedrawContent = true;
    update();
}

bool TimeLineTrackList::didDetachLayer() const {
    return abs(mMouseMoveY) > mLayerDetachThreshold;
}

void TimeLineTrackList::showCameraMenu(QPoint pos)
{
    int frameNumber = getFrameNumber(pos.x());

    const Layer* curLayer = mEditor->layers()->currentLayer();
    Q_ASSERT(curLayer);

    // only show menu if on camera layer and key exists
    if (curLayer->type() != Layer::CAMERA || !curLayer->keyExists(frameNumber))
    {
        return;
    }

    mHighlightFrameEnabled = true;
    mHighlightedFrame = frameNumber;

    CameraContextMenu menu(frameNumber, static_cast<const LayerCamera*>(curLayer));

    menu.connect(&menu, &CameraContextMenu::aboutToHide, this, [=] {
        mHighlightFrameEnabled = false;
        mHighlightedFrame = -1;
        update();

        KeyFrame* key = curLayer->getKeyFrameAt(frameNumber);
        if (key->isModified()) {
            emit mEditor->frameModified(frameNumber);
        }
    });

    // Update needs to happen before executing menu, otherwise paint event might be postponed
    update();

    menu.exec(mapToGlobal(pos));
}

void TimeLineTrackList::drawContent()
{
    if (mCache == nullptr)
    {
        mCache = new QPixmap(size());
        if (mCache->isNull())
        {
            // fail to create cache
            return;
        }
    }

    QPainter painter(mCache);

    // grey background of the view
    const QPalette palette = QApplication::palette();
    painter.setPen(Qt::NoPen);
    painter.setBrush(palette.color(QPalette::Base));
    painter.drawRect(QRect(0, 0, width(), height()));

    const int widgetWidth = width();

    // Draw non-current layers
    const Object* object = mEditor->object();
    Q_ASSERT(object != nullptr);
    // for (int i = 0; i < object->getLayerCount(); i++)
    // {
    //     if (i == mEditor->layers()->currentLayerIndex())
    //     {
    //         continue;
    //     }
    //     const Layer* layeri = object->getLayer(i);

    //     if (layeri != nullptr)
    //     {
    //         const int layerY = getLayerY(i);
    //         switch (mType)
    //         {
    //         case TIMELINE_CELL_TYPE::Tracks:
    //             paintTrack(painter, layeri, mOffsetX,
    //                        layerY, widgetWidth - mOffsetX,
    //                        mLayerHeight, false, mFrameSize);
    //             break;

    //         case TIMELINE_CELL_TYPE::Layers:
    //             break;
    //         }
    //     }
    // }

    // Draw current layer
    const Layer* currentLayer = mEditor->layers()->currentLayer();
    if (didDetachLayer())
    {
        int layerYMouseMove = mMouseMoveY;
        if (mType == TIMELINE_CELL_TYPE::Tracks)
        {
            paintTrack(painter, currentLayer,
                       mOffsetX, layerYMouseMove,
                       widgetWidth - mOffsetX, mLayerHeight,
                       true, mFrameSize);
        }
    }
    // else
    // {
    //     if (mType == TIMELINE_CELL_TYPE::Tracks)
    //     {
    //         paintTrack(painter,
    //                    currentLayer,
    //                    mOffsetX,
    //                    getLayerY(mEditor->layers()->currentLayerIndex()),
    //                    widgetWidth - mOffsetX,
    //                    mLayerHeight,
    //                    true,
    //                    mFrameSize);
    //     }
    // }

    // --- draw track bar background
    painter.setPen(Qt::NoPen);
    painter.setBrush(palette.color(QPalette::Base));
    painter.drawRect(QRect(0, 0, width() - 1, mOffsetY - 1));

    // --- draw bottom line splitter for track bar
    painter.setPen(palette.color(QPalette::Mid));
    painter.drawLine(0, mOffsetY - 2, width() - 1, mOffsetY - 2);

    if (mType == TIMELINE_CELL_TYPE::Tracks)
    {
        paintTicks(painter, palette);

        for (int i = 0; i < object->getLayerCount(); i++) {
            paintSelectedFrames(painter, object->getLayer(i), i);
        }
    }
    mRedrawContent = false;
}

void TimeLineTrackList::paintTicks(QPainter& painter, const QPalette& palette) const
{
    painter.setPen(palette.color(QPalette::Text));
    painter.setBrush(palette.brush(QPalette::Text));
    int fps = mEditor->playback()->fps();
    for (int i = mFrameOffset; i < mFrameOffset + (width() - mOffsetX) / mFrameSize; i++)
    {
        // line x pos + some offset
        const int lineX = getFrameX(i) + 1;
        if (i + 1 >= mTimeLine->getRangeLower() && i < mTimeLine->getRangeUpper())
        {
            painter.setPen(Qt::NoPen);
            painter.setBrush(palette.color(QPalette::Highlight));

            painter.drawRect(lineX, 1, mFrameSize + 1, 2);

            painter.setPen(palette.color(QPalette::Text));
            painter.setBrush(palette.brush(QPalette::Text));
        }

        // Draw large tick at fps mark
        if (i % fps == 0 || i % fps == fps / 2)
        {
            painter.drawLine(lineX, 1, lineX, 5);
        }
        else // draw small tick
        {
            painter.drawLine(lineX, 1, lineX, 3);
        }
        if (i == 0 || i % fps == fps - 1)
        {
            int incr = (i < 9) ? 4 : 0; // poor man’s text centering
            painter.drawText(QPoint(lineX + incr, 15), QString::number(i + 1));
        }
    }
}

void TimeLineTrackList::paintTrack(QPainter& painter, const Layer* layer,
                       int x, int y, int width, int height,
                       bool selected, int frameSize) const
{
    const QPalette palette = QApplication::palette();
    QColor col;
    // Color each track according to the layer type
    if (layer->type() == Layer::BITMAP) col = QColor(51, 155, 252);
    if (layer->type() == Layer::VECTOR) col = QColor(70, 205, 123);
    if (layer->type() == Layer::SOUND) col = QColor(255, 141, 112);
    if (layer->type() == Layer::CAMERA) col = QColor(253, 202, 92);
    // Dim invisible layers
    if (!layer->visible()) col.setAlpha(64);

    painter.save();
    painter.setBrush(col);
    painter.setPen(QPen(QBrush(palette.color(QPalette::Mid)), 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter.drawRect(x, y, width, height);

    if (!layer->visible())
    {
        painter.restore();
        return;
    }

    // Changes the appearance if selected
    if (selected)
    {
        paintSelection(painter, x, y, width, height);
    }
    else
    {
        painter.save();
        QLinearGradient linearGrad(QPointF(0, y), QPointF(0, y + height));
        linearGrad.setColorAt(0, QColor(255,255,255,150));
        linearGrad.setColorAt(1, QColor(0,0,0,0));
        painter.setCompositionMode(QPainter::CompositionMode_Overlay);
        painter.setBrush(linearGrad);
        painter.drawRect(x, y, width, height);
        painter.restore();
    }

    paintFrames(painter, col, layer, y, height, selected, frameSize);

    painter.restore();
}

void TimeLineTrackList::paintFrames(QPainter& painter, QColor trackCol, const Layer* layer, int y, int height, bool selected, int frameSize) const
{
    painter.setPen(QPen(QBrush(QColor(40, 40, 40)), 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

    int recTop = y + 1;
    int standardWidth = frameSize - 2;

    int recHeight = height - 4;

    layer->foreachKeyFrame([&](KeyFrame* key)
    {
        int framePos = key->pos();
        int recWidth = standardWidth;
        int recLeft = getFrameX(framePos) - recWidth;

        if (key->length() > 1)
        {
            // This is especially for sound clips.
            // Sound clips are the only type of KeyFrame with variable frame length.
            recWidth = frameSize * key->length();
        }

        // Paint the frame border
        painter.setPen(QPen(QBrush(QColor(40, 40, 40)), 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

        // Paint the frame contents
        if (selected)
        {
            if (key->isSelected()) {
                painter.setBrush(QColor(60, 60, 60));
            }
            else
            {
                painter.setBrush(QColor(trackCol.red(), trackCol.green(), trackCol.blue(), 150));
            }
        }

        if (!key->isSelected()) {
            painter.drawRect(recLeft, recTop, recWidth, recHeight);
        }
    });
}

void TimeLineTrackList::paintCurrentFrameBorder(QPainter &painter, int recLeft, int recTop, int recWidth, int recHeight) const
{
    painter.save();
    painter.setBrush(Qt::NoBrush);
    painter.setPen(Qt::white);
    painter.drawRect(recLeft, recTop, recWidth, recHeight);
    painter.restore();
}

void TimeLineTrackList::paintFrameCursorOnCurrentLayer(QPainter &painter, int recTop, int recWidth, int recHeight) const
{
    int recLeft = getFrameX(mFramePosMoveX) - recWidth;

    painter.save();
    const QPalette palette = QApplication::palette();
    // Don't fill
    painter.setBrush(Qt::NoBrush);
    // paint border
    QColor penColor = palette.color(QPalette::WindowText);
    penColor.setAlpha(127);
    painter.setPen(penColor);
    painter.drawRect(recLeft, recTop, recWidth, recHeight);
    painter.restore();
}

void TimeLineTrackList::paintHighlightedFrame(QPainter& painter, int framePos, int recTop, int recWidth, int recHeight) const
{
    int recLeft = getFrameX(framePos) - recWidth;

    painter.save();
    const QPalette palette = QApplication::palette();
    painter.setBrush(palette.color(QPalette::Window));
    painter.setPen(palette.color(QPalette::WindowText));

    // Draw a rect slighly smaller than the frame
    painter.drawRect(recLeft+1, recTop+1, recWidth-1, recHeight-1);
    painter.restore();
}

void TimeLineTrackList::paintSelectedFrames(QPainter& painter, const Layer* layer, const int layerIndex) const
{
    int mouseX = mMouseMoveX;
    int posUnderCursor = getFrameNumber(mMousePressX);
    int standardWidth = mFrameSize - 2;
    int recWidth = standardWidth;
    int recHeight = mLayerHeight - 4;
    int recTop = getLayerY(layerIndex) + 1;

    painter.save();
    for (int framePos : layer->getSelectedFramesByPos()) {

        KeyFrame* key = layer->getKeyFrameAt(framePos);
        if (key->length() > 1)
        {
            // This is a special case for sound clip.
            // Sound clip is the only type of KeyFrame that has variable frame length.
            recWidth = mFrameSize * key->length();
        }

        painter.setBrush(QColor(60, 60, 60));
        painter.setPen(QPen(QBrush(QColor(40, 40, 40)), 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

        int frameX = getFrameX(framePos);
        if (mMovingFrames) {
            int offset = (framePos - posUnderCursor) + mFrameOffset;
            int newFrameX = getFrameX(getFrameNumber(getFrameX(offset)+mouseX)) - standardWidth;
            // Paint as frames are hovering
            painter.drawRect(newFrameX, recTop-4, recWidth, recHeight);

        } else {
            int currentFrameX = frameX - standardWidth;
            painter.drawRect(currentFrameX, recTop, recWidth, recHeight);
        }
    }
    painter.restore();
}

void TimeLineTrackList::paintSelection(QPainter& painter, int x, int y, int width, int height) const
{
    QLinearGradient linearGrad(QPointF(0, y), QPointF(0, y + height));
    linearGrad.setColorAt(0, QColor(0, 0, 0, 255));
    linearGrad.setColorAt(1, QColor(255, 255, 255, 0));
    painter.save();
    painter.setCompositionMode(QPainter::CompositionMode_Overlay);
    painter.setBrush(linearGrad);
    painter.setPen(Qt::NoPen);
    painter.drawRect(x, y, width, height - 1);
    painter.restore();
}

void TimeLineTrackList::paintOnionSkin(QPainter& painter) const
{
    Layer* layer = mEditor->layers()->currentLayer();
    if (layer == nullptr) { return; }

    int frameNumber = mEditor->currentFrame();

    int prevOnionSkinCount = mEditor->preference()->getInt(SETTING::ONION_PREV_FRAMES_NUM);
    int nextOnionSkinCount = mEditor->preference()->getInt(SETTING::ONION_NEXT_FRAMES_NUM);

    bool isAbsolute = (mEditor->preference()->getString(SETTING::ONION_TYPE) == "absolute");

    if (mEditor->preference()->isOn(SETTING::PREV_ONION) && prevOnionSkinCount > 0)
    {
        int onionFrameNumber = frameNumber;
        if (isAbsolute)
        {
            onionFrameNumber = layer->getPreviousFrameNumber(onionFrameNumber+1, true);
        }
        onionFrameNumber = layer->getPreviousFrameNumber(onionFrameNumber, isAbsolute);
        int onionPosition = 0;

        while (onionPosition < prevOnionSkinCount && onionFrameNumber > 0)
        {
            painter.setBrush(QColor(128, 128, 128, 128));
            painter.setPen(Qt::NoPen);
            QRect onionRect;
            onionRect.setTopLeft(QPoint(getFrameX(onionFrameNumber - 1), 0));
            onionRect.setBottomRight(QPoint(getFrameX(onionFrameNumber), height()));
            onionRect.setBottomRight(QPoint(getFrameX(onionFrameNumber), 19));
            painter.drawRect(onionRect);

            onionFrameNumber = layer->getPreviousFrameNumber(onionFrameNumber, isAbsolute);
            onionPosition++;
        }
    }

    if (mEditor->preference()->isOn(SETTING::NEXT_ONION) && nextOnionSkinCount > 0) {

        int onionFrameNumber = layer->getNextFrameNumber(frameNumber, isAbsolute);
        int onionPosition = 0;

        while (onionPosition < nextOnionSkinCount && onionFrameNumber > 0)
        {
            painter.setBrush(QColor(128, 128, 128, 128));
            painter.setPen(Qt::NoPen);
            QRect onionRect;
            onionRect.setTopLeft(QPoint(getFrameX(onionFrameNumber - 1), 0));
            onionRect.setBottomRight(QPoint(getFrameX(onionFrameNumber), height()));
            onionRect.setBottomRight(QPoint(getFrameX(onionFrameNumber), 19));
            painter.drawRect(onionRect);

            onionFrameNumber = layer->getNextFrameNumber(onionFrameNumber, isAbsolute);
            onionPosition++;
        }
    }
}

void TimeLineTrackList::paintEvent(QPaintEvent*)
{
    const QPalette palette = QApplication::palette();
    QPainter painter(this);

    bool isPlaying = mEditor->playback()->isPlaying();
    if (mCache == nullptr || mRedrawContent || trackScrubber())
    {
        drawContent();
    }
    if (mCache)
    {
        painter.drawPixmap(QPoint(0, 0), *mCache);
    }

    if (mType == TIMELINE_CELL_TYPE::Tracks)
    {
        if (!isPlaying) {
            paintOnionSkin(painter);
        }

        int currentFrame = mEditor->currentFrame();
        Layer* currentLayer = mEditor->layers()->currentLayer();
        KeyFrame* keyFrame = currentLayer->getKeyFrameWhichCovers(currentFrame);
        if (keyFrame != nullptr && !keyFrame->isSelected())
        {
            int recWidth = keyFrame->length() == 1 ? mFrameSize - 2 : mFrameSize * keyFrame->length();
            int recLeft = getFrameX(keyFrame->pos()) - (mFrameSize - 2);
            paintCurrentFrameBorder(painter, recLeft, getLayerY(mEditor->currentLayerIndex()) + 1, recWidth, mLayerHeight - 4);
        }

        if (!mMovingFrames && mLayerPosMoveY != -1 && mLayerPosMoveY == mEditor->currentLayerIndex())
        {
            // This is terrible but well...
            int recTop = getLayerY(mLayerPosMoveY) + 1;
            int standardWidth = mFrameSize - 2;
            int recHeight = mLayerHeight - 4;

            if (mHighlightFrameEnabled)
            {
                paintHighlightedFrame(painter, mHighlightedFrame, recTop, standardWidth, recHeight);
            }
            if (currentLayer->visible())
            {
                paintFrameCursorOnCurrentLayer(painter, recTop, standardWidth, recHeight);
            }
        }

        // --- draw the position of the current frame
        if (currentFrame > mFrameOffset)
        {
            QColor scrubColor = palette.color(QPalette::Highlight);
            scrubColor.setAlpha(160);
            painter.setBrush(scrubColor);
            painter.setPen(Qt::NoPen);

            int currentFrameStartX = getFrameX(currentFrame - 1) + 1;
            int currentFrameEndX = getFrameX(currentFrame);
            QRect scrubRect;
            scrubRect.setTopLeft(QPoint(currentFrameStartX, 0));
            scrubRect.setBottomRight(QPoint(currentFrameEndX, height()));
            if (mbShortScrub)
            {
                scrubRect.setBottomRight(QPoint(currentFrameEndX, 19));
            }
            painter.save();

            bool mouseUnderScrubber = currentFrame == mFramePosMoveX;
            if (mouseUnderScrubber) {
                QRect smallScrub = QRect(QPoint(currentFrameStartX, 0), QPoint(currentFrameEndX,19));
                QPen pen = scrubColor;
                pen.setWidth(2);
                painter.setPen(pen);
                painter.drawRect(smallScrub);
                painter.setBrush(Qt::NoBrush);
            }
            painter.drawRect(scrubRect);
            painter.restore();

            painter.setPen(palette.color(QPalette::HighlightedText));
            int incr = (currentFrame < 10) ? 4 : 0;
            painter.drawText(QPoint(currentFrameStartX + incr, 15),
                             QString::number(currentFrame));
        }
    }
}

void TimeLineTrackList::resizeEvent(QResizeEvent* event)
{
    clearCache();
    updateContent();
    event->accept();
    emit lengthChanged(getFrameLength());
}

bool TimeLineTrackList::event(QEvent* event)
{
    if (event->type() == QEvent::Leave) {
        onDidLeaveWidget();
    }

    return QWidget::event(event);
}

void TimeLineTrackList::mousePressEvent(QMouseEvent* event)
{
    int frameNumber = getFrameNumber(event->pos().x());
    int layerNumber = getLayerNumber(event->pos().y());
    mCurrentLayerNumber = layerNumber;

    mMousePressX = event->pos().x();
    mFromLayer = mToLayer = layerNumber;

    mStartY = event->pos().y();
    mStartLayerNumber = layerNumber;
    mEndY = event->pos().y();

    mStartFrameNumber = frameNumber;
    mLastFrameNumber = mStartFrameNumber;

    mCanMoveFrame = false;
    mMovingFrames = false;

    mCanBoxSelect = false;
    mBoxSelecting = false;

    mClickSelecting = false;

    primaryButton = event->button();

    switch (mType)
    {
    case TIMELINE_CELL_TYPE::Layers:
        break;
    case TIMELINE_CELL_TYPE::Tracks:
        if (event->button() == Qt::MiddleButton)
        {
            mLastFrameNumber = getFrameNumber(event->pos().x());
        }
        else
        {
            if (frameNumber == mEditor->currentFrame() && mStartY < 20)
            {
                if (mEditor->playback()->isPlaying())
                {
                    mEditor->playback()->stop();
                }
                mTimeLine->scrubbing = true;
            }
            else
            {
                if ((layerNumber != -1) && layerNumber < mEditor->object()->getLayerCount())
                {
                    int previousLayerNumber = mEditor->layers()->currentLayerIndex();

                    if (previousLayerNumber != layerNumber)
                    {
                        Layer *previousLayer = mEditor->object()->getLayer(previousLayerNumber);
                        previousLayer->deselectAll();
                        emit mEditor->selectedFramesChanged();
                        mEditor->layers()->setCurrentLayer(layerNumber);
                    }

                    Layer *currentLayer = mEditor->object()->getLayer(layerNumber);

                    // Check if we are using the alt key
                    if (event->modifiers() == Qt::AltModifier)
                    {
                        // If it is the case, we select everything that is after the selected frame
                        mClickSelecting = true;
                        mCanMoveFrame = true;

                        currentLayer->selectAllFramesAfter(frameNumber);
                        emit mEditor->selectedFramesChanged();
                    }
                    // Check if we are clicking on a non selected frame
                    else if (!currentLayer->isFrameSelected(frameNumber))
                    {
                        // If it is the case, we select it if it is the left button...
                        mCanBoxSelect = true;
                        mClickSelecting = true;
                        if (event->button() == Qt::LeftButton)
                        {

                            if (event->modifiers() == Qt::ControlModifier)
                            {
                                // Add/remove from already selected
                                currentLayer->toggleFrameSelected(frameNumber, true);
                                emit mEditor->selectedFramesChanged();
                            }
                            else if (event->modifiers() == Qt::ShiftModifier)
                            {
                                // Select a range from the last selected
                                currentLayer->extendSelectionTo(frameNumber);
                                emit mEditor->selectedFramesChanged();
                            }
                            else
                            {
                                // Only select if left button clicked
                                currentLayer->toggleFrameSelected(frameNumber, false);
                                emit mEditor->selectedFramesChanged();
                            }
                        }

                        // ... or we show the camera context menu, if it is the right button
                        if (event->button() == Qt::RightButton)
                        {
                            showCameraMenu(event->pos());
                        }

                    }
                    else
                    {
                        // If selected they can also be interpolated
                        if (event->button() == Qt::RightButton)
                        {
                            showCameraMenu(event->pos());
                        }
                        // We clicked on a selected frame, we can move it
                        mCanMoveFrame = true;
                    }

                    if (currentLayer->hasAnySelectedFrames()) {
                        emit selectionChanged();
                    }

                    mTimeLine->updateContent();
                }
                else
                {
                    if (frameNumber > 0)
                    {
                        if (mEditor->playback()->isPlaying())
                        {
                            mEditor->playback()->stop();
                        }
                        if (mEditor->playback()->getSoundScrubActive() && mLastScrubFrame != frameNumber)
                        {
                            mEditor->playback()->playScrub(frameNumber);
                            mLastScrubFrame = frameNumber;
                        }

                        mEditor->scrubTo(frameNumber);

                        mTimeLine->scrubbing = true;
                        qDebug("Scrub to %d frame", frameNumber);
                    }
                }
            }
        }
        break;
    }
}

void TimeLineTrackList::mouseMoveEvent(QMouseEvent* event)
{
    mMouseMoveX = event->pos().x();
    mFramePosMoveX = getFrameNumber(mMouseMoveX);
    mLayerPosMoveY = getLayerNumber(event->pos().y());

    if (mType == TIMELINE_CELL_TYPE::Tracks)
    {
        if (primaryButton == Qt::MiddleButton)
        {
            mFrameOffset = qMin(qMax(0, mFrameLength - width() / getFrameSize()), qMax(0, mFrameOffset + mLastFrameNumber - mFramePosMoveX));
            update();
            emit offsetChanged(mFrameOffset);
        }
        else
        {
            if (mTimeLine->scrubbing)
            {
                if (mEditor->playback()->getSoundScrubActive() && mLastScrubFrame != mFramePosMoveX)
                {
                    mEditor->playback()->playScrub(mFramePosMoveX);
                    mLastScrubFrame = mFramePosMoveX;
                }
                mEditor->scrubTo(mFramePosMoveX);
            }
            else
            {
                if (event->buttons() & Qt::LeftButton) {
                    if (mStartLayerNumber != -1 && mStartLayerNumber < mEditor->object()->getLayerCount())
                    {
                        Layer *currentLayer = mEditor->object()->getLayer(mStartLayerNumber);

                        // Check if the frame we clicked was selected
                        if (mCanMoveFrame) {

                            // If it is the case, we move the selected frames in the layer
                            mMovingFrames = true;
                        }
                        else if (mCanBoxSelect)
                        {
                            // Otherwise, we do a box select
                            mBoxSelecting = true;

                            currentLayer->deselectAll();
                            currentLayer->setFrameSelected(mStartFrameNumber, true);
                            currentLayer->extendSelectionTo(mFramePosMoveX);
                            emit mEditor->selectedFramesChanged();
                        }
                        mLastFrameNumber = mFramePosMoveX;
                        updateContent();
                    }
                }
                update();
            }
        }
    }
}

void TimeLineTrackList::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() != primaryButton) return;

    int frameNumber = getFrameNumber(event->pos().x());
    if (frameNumber < 1) frameNumber = 1;
    int layerNumber = getLayerNumber(event->pos().y());

    if (mType == TIMELINE_CELL_TYPE::Tracks && mCurrentLayerNumber != -1 && primaryButton != Qt::MiddleButton)
    {
        // We should affect the current layer based on what's selected, not where the mouse currently is.
        Layer* currentLayer = mEditor->layers()->getLayer(mCurrentLayerNumber);
        Q_ASSERT(currentLayer);

        if (mMovingFrames)
        {
            int posUnderCursor = getFrameNumber(mMousePressX);
            int offset = frameNumber - posUnderCursor;

            currentLayer->moveSelectedFrames(offset);

            mEditor->layers()->notifyAnimationLengthChanged();
            emit mEditor->framesModified();
            updateContent();
        }
        else if (!mTimeLine->scrubbing && !mMovingFrames && !mClickSelecting && !mBoxSelecting)
        {
            // De-selecting if we didn't move, scrub nor select anything
            bool multipleSelection = (event->modifiers() == Qt::ControlModifier);

            // Add/remove from already selected
            currentLayer->toggleFrameSelected(frameNumber, multipleSelection);
            emit mEditor->selectedFramesChanged();
            updateContent();
        }
    }

    primaryButton = Qt::NoButton;
    mEndY = mStartY;
    mTimeLine->scrubbing = false;
    mMovingFrames = false;
}

void TimeLineTrackList::mouseDoubleClickEvent(QMouseEvent* event)
{
    int frameNumber = getFrameNumber(event->pos().x());
    int layerNumber = getLayerNumber(event->pos().y());

    // -- short scrub --
    if (event->pos().y() < 20 && (mType != TIMELINE_CELL_TYPE::Layers || event->pos().x() >= 15))
    {
        mPrefs->set(SETTING::SHORT_SCRUB, !mbShortScrub);
    }

    // -- layer --
    Layer* layer = mEditor->object()->getLayer(layerNumber);
    if (layer && event->buttons() & Qt::LeftButton)
    {
        if (mType == TIMELINE_CELL_TYPE::Tracks && (layerNumber != -1) && (frameNumber > 0) && layerNumber < mEditor->object()->getLayerCount())
        {
            if (!layer->keyExistsWhichCovers(frameNumber))
            {
                mEditor->scrubTo(frameNumber);
                emit insertNewKeyFrame();
            }

            // The release event will toggle the frame on again, so we make sure it gets
            // deselected now instead.
            layer->setFrameSelected(frameNumber, true);
        }
    }
    QWidget::mouseDoubleClickEvent(event);
}

void TimeLineTrackList::hScrollChange(int x)
{
    mFrameOffset = x;
    updateContent();
}

void TimeLineTrackList::vScrollChange(int x)
{
    mLayerOffset = x;
    mScrollingVertically = true;
    updateContent();
}

void TimeLineTrackList::onScrollingVerticallyStopped()
{
    mScrollingVertically = false;
}

void TimeLineTrackList::setMouseMoveY(int x)
{
    mMouseMoveY = x;
    updateContent();
}

bool TimeLineTrackList::trackScrubber()
{
    if (mType != TIMELINE_CELL_TYPE::Tracks ||
        (mPrevFrame == mEditor->currentFrame() && !mEditor->playback()->isPlaying()))
    {
        return false;
    }
    mPrevFrame = mEditor->currentFrame();

    if (mEditor->currentFrame() <= mFrameOffset)
    {
        // Move the timeline back if the scrubber is offscreen to the left
        mFrameOffset = mEditor->currentFrame() - 1;
        emit offsetChanged(mFrameOffset);
        return true;
    }
    else if (width() < (mEditor->currentFrame() - mFrameOffset + 1) * mFrameSize)
    {
        // Move timeline forward if the scrubber is offscreen to the right
        if (mEditor->playback()->isPlaying())
            mFrameOffset = mFrameOffset + ((mEditor->currentFrame() - mFrameOffset) / 2);
        else
            mFrameOffset = mEditor->currentFrame() - width() / mFrameSize;
        emit offsetChanged(mFrameOffset);
        return true;
    }
    return false;
}

void TimeLineTrackList::onDidLeaveWidget()
{
    // Reset last known frame pos to avoid wrong UI states when leaving the widget
    mFramePosMoveX = 0;
    update();
}
