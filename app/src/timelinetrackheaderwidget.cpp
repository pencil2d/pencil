/*

Pencil2D - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2008-2009 Mj Mendoza IV
Copyright (C) 2012-2020 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/

#include "timelinetrackheaderwidget.h"

#include "editor.h"
#include "timeline.h"

#include "preferencemanager.h"
#include "layermanager.h"
#include "layer.h"
#include "playbackmanager.h"

#include <QMouseEvent>
#include <QApplication>
#include <QDebug>

TimeLineTrackHeaderWidget::TimeLineTrackHeaderWidget(TimeLine* timeLine, Editor* editor)
    : QWidget(timeLine),
      mEditor(editor),
      mTimeLine(timeLine)
{

    mFrameSize = mEditor->preference()->getInt(SETTING::FRAME_SIZE);
    mShortScrub = mEditor->preference()->isOn(SETTING::SHORT_SCRUB);

    connect(mEditor->preference(), &PreferenceManager::optionChanged, this, &TimeLineTrackHeaderWidget::onSettingChanged);
}

void TimeLineTrackHeaderWidget::paintEvent(QPaintEvent*)
{
    QPainter painter(this);

    const QPalette palette = QApplication::palette();
    paintBackground(painter, palette);
    if (!mEditor->playback()->isPlaying()) {
        paintOnionSkin(painter);
    }
    paintTicks(painter, palette);
    paintSplitter(painter, palette);

    paintScrubber(painter, palette);
}

void TimeLineTrackHeaderWidget::onSettingChanged(SETTING setting)
{
    switch (setting) {
        case SETTING::FRAME_SIZE:
            mFrameSize = mEditor->preference()->getInt(setting);
            break;
        case SETTING::SHORT_SCRUB:
            mShortScrub = mEditor->preference()->isOn(setting);
            break;
        default:
            break;
    }

    update();
}

void TimeLineTrackHeaderWidget::paintBackground(QPainter& painter, const QPalette& palette) const
{
    painter.setPen(Qt::NoPen);
    painter.setBrush(palette.color(QPalette::Base));
    painter.drawRect(QRect(0, 0, width() - 1, 0));
}

void TimeLineTrackHeaderWidget::paintSplitter(QPainter& painter, const QPalette& palette) const
{
    // --- draw bottom line splitter for track bar
    QPen pen;
    pen.setBrush(palette.color(QPalette::Mid));
    pen.setWidthF(1);
    painter.setPen(pen);
    painter.drawLine(0, rect().bottom(), width() - 1, rect().bottom());
}

void TimeLineTrackHeaderWidget::paintOnionSkin(QPainter& painter) const
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
            int frameX = getFrameX(onionFrameNumber);
            onionRect.setTopLeft(QPoint(frameX, 0));
            onionRect.setBottomRight(QPoint(frameX + mFrameSize, height()));
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
            int frameX = getFrameX(onionFrameNumber);
            onionRect.setTopLeft(QPoint(frameX, 0));
            onionRect.setBottomRight(QPoint(frameX + mFrameSize, height()));
            painter.drawRect(onionRect);

            onionFrameNumber = layer->getNextFrameNumber(onionFrameNumber, isAbsolute);
            onionPosition++;
        }
    }
}

void TimeLineTrackHeaderWidget::paintScrubber(QPainter &painter, const QPalette &palette) const
{
    int currentFrame = mEditor->currentFrame();
    // --- draw the position of the current frame
    if (currentFrame > mScrollOffsetX)
    {
        QColor scrubColor = palette.color(QPalette::Highlight);
        scrubColor.setAlpha(160);
        painter.setBrush(scrubColor);
        painter.setPen(Qt::NoPen);

        int currentFrameX = getFrameX(currentFrame);
        QRect scrubRect;
        scrubRect.setTopLeft(QPoint(currentFrameX, 0));
        scrubRect.setBottomRight(QPoint(currentFrameX + mFrameSize, height()));
        painter.save();
        painter.drawRect(scrubRect);
        painter.restore();

        painter.setPen(palette.color(QPalette::HighlightedText));
        int incr = (currentFrame < 10) ? 4 : 0;
        painter.drawText(QPoint(currentFrameX + incr, 15),
                         QString::number(currentFrame));
    }
}

void TimeLineTrackHeaderWidget::paintTicks(QPainter& painter, const QPalette& palette) const
{
    painter.setPen(palette.color(QPalette::Text));
    painter.setBrush(palette.brush(QPalette::Text));
    int fps = mEditor->playback()->fps();
    for (int i = mScrollOffsetX; i < mScrollOffsetX + (width()) / mFrameSize; i++)
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

void TimeLineTrackHeaderWidget::mousePressEvent(QMouseEvent *event)
{
    QWidget::mousePressEvent(event);
    if (event->buttons() & Qt::LeftButton) {
        mEditor->scrubTo(getFrameNumber(event->pos().x()));
    }
}

void TimeLineTrackHeaderWidget::mouseMoveEvent(QMouseEvent* event)
{
    QWidget::mouseMoveEvent(event);
    if (event->buttons() & Qt::LeftButton) {
        if (mEditor->currentFrame() > 0) {
            mTimeLine->scrubbing = true;
        }
        if (mTimeLine->scrubbing)
        {
            if (mEditor->playback()->isPlaying())
            {
                mEditor->playback()->stop();
            }
            mEditor->scrubTo(getFrameNumber(event->pos().x()));
        }
    }
}

void TimeLineTrackHeaderWidget::mouseDoubleClickEvent(QMouseEvent* event)
{
    QWidget::mouseDoubleClickEvent(event);
    if (event->buttons() & Qt::LeftButton) {
        // -- short scrub --
        mEditor->preference()->set(SETTING::SHORT_SCRUB, !mShortScrub);
    }
}

void TimeLineTrackHeaderWidget::mouseReleaseEvent(QMouseEvent* event)
{
    QWidget::mouseReleaseEvent(event);
    mTimeLine->scrubbing = false;
}

int TimeLineTrackHeaderWidget::getFrameNumber(int x) const
{
    return mScrollOffsetX + 1 + (x) / mFrameSize;
}

int TimeLineTrackHeaderWidget::getFrameX(int frameNumber) const
{
    return ((frameNumber - mScrollOffsetX) * mFrameSize) - mFrameSize;
}

void TimeLineTrackHeaderWidget::onHScrollChange(int x)
{
    mScrollOffsetX = x;
    update();
}


