/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2012-2018 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/

#include "timecontrols.h"

#include <QtGui>
#include <QLabel>

#include "editor.h"
#include "playbackmanager.h"
#include "layermanager.h"
#include "pencildef.h"
#include "util.h"
#include "preferencemanager.h"
#include "timeline.h"


TimeControls::TimeControls(TimeLine* parent) : QToolBar(parent)
{
    mTimeline = parent;
}

void TimeControls::initUI()
{
    QSettings settings(PENCIL2D, PENCIL2D);

    mFpsBox = new QSpinBox(this);
    mFpsBox->setFixedHeight(24);
    mFpsBox->setValue(settings.value("fps").toInt());
    mFpsBox->setMinimum(1);
    mFpsBox->setMaximum(90);
    mFpsBox->setSuffix(" fps");
    mFpsBox->setToolTip(tr("Frames per second"));
    mFpsBox->setFocusPolicy(Qt::WheelFocus);

    mLoopStartSpinBox = new QSpinBox(this);
    mLoopStartSpinBox->setFixedHeight(24);
    mLoopStartSpinBox->setValue(settings.value("loopStart").toInt());
    mLoopStartSpinBox->setMinimum(1);
    mLoopStartSpinBox->setToolTip(tr("Start of playback loop"));
    mLoopStartSpinBox->setFocusPolicy(Qt::WheelFocus);

    mLoopEndSpinBox = new QSpinBox(this);
    mLoopEndSpinBox->setFixedHeight(24);
    mLoopEndSpinBox->setValue(settings.value("loopEnd").toInt());
    mLoopEndSpinBox->setMinimum(2);
    mLoopEndSpinBox->setToolTip(tr("End of playback loop"));
    mLoopEndSpinBox->setFocusPolicy(Qt::WheelFocus);

    mPlaybackRangeCheckBox = new QCheckBox(tr("Range"));
    mPlaybackRangeCheckBox->setFixedHeight(24);
    mPlaybackRangeCheckBox->setToolTip(tr("Playback range"));

    mPlayButton = new QPushButton(this);
    mLoopButton = new QPushButton(this);
    mSoundButton = new QPushButton(this);
    mJumpToEndButton = new QPushButton(this);
    mJumpToStartButton = new QPushButton(this);

    mLoopIcon = QIcon(":icons/controls/loop.png");
    mSoundIcon = QIcon(":icons/controls/sound.png");
    mJumpToEndIcon = QIcon(":icons/controls/endplay.png");
    mJumpToStartIcon = QIcon(":icons/controls/startplay.png");
    mStartIcon = QIcon(":icons/controls/play.png");
    mStopIcon = QIcon(":icons/controls/stop.png");
    mPlayButton->setIcon(mStartIcon);
    mLoopButton->setIcon(mLoopIcon);
    mSoundButton->setIcon(mSoundIcon);
    mJumpToEndButton->setIcon(mJumpToEndIcon);
    mJumpToStartButton->setIcon(mJumpToStartIcon);

    mPlayButton->setToolTip(tr("Play"));
    mLoopButton->setToolTip(tr("Loop"));
    mSoundButton->setToolTip(tr("Sound on/off"));
    mJumpToEndButton->setToolTip(tr("End"));
    mJumpToStartButton->setToolTip(tr("Start"));

    mLoopButton->setCheckable(true);
    mSoundButton->setCheckable(true);
    mSoundButton->setChecked(true);

    addWidget(mJumpToStartButton);
    addWidget(mPlayButton);
    addWidget(mJumpToEndButton);
    addWidget(mLoopButton);
    addWidget(mPlaybackRangeCheckBox);
    addWidget(mLoopStartSpinBox);
    addWidget(mLoopEndSpinBox);
    addWidget(mSoundButton);
    addWidget(mFpsBox);

    makeConnections();

    updateUI();
}

void TimeControls::updateUI()
{
    PlaybackManager* playback = mEditor->playback();

    mPlaybackRangeCheckBox->setChecked(playback->isRangedPlaybackOn()); // don't block this signal since it enables start/end range spinboxes.

    SignalBlocker b1(mLoopStartSpinBox);
    mLoopStartSpinBox->setValue(playback->markInFrame());

    SignalBlocker b2(mLoopEndSpinBox);
    mLoopEndSpinBox->setValue(playback->markOutFrame());

    SignalBlocker b3(mFpsBox);
    mFpsBox->setValue(playback->fps());

    SignalBlocker b4(mLoopButton);
    mLoopButton->setChecked(playback->isLooping());
}

void TimeControls::setFps(int value)
{
    SignalBlocker blocker(mFpsBox);
    mFpsBox->setValue(value);
}

void TimeControls::toggleLoop(bool checked)
{
    mLoopButton->setChecked(checked);
}

void TimeControls::toggleLoopControl(bool checked)
{
    mPlaybackRangeCheckBox->setChecked(checked);
}

void TimeControls::setEditor(Editor* editor)
{
    Q_ASSERT(editor != nullptr);
    mEditor = editor;
}

void TimeControls::makeConnections()
{
    connect(mPlayButton, &QPushButton::clicked, this, &TimeControls::playButtonClicked);
    connect(mJumpToEndButton, &QPushButton::clicked, this, &TimeControls::jumpToEndButtonClicked);
    connect(mJumpToStartButton, &QPushButton::clicked, this, &TimeControls::jumpToStartButtonClicked);
    connect(mLoopButton, &QPushButton::clicked, this, &TimeControls::loopButtonClicked);
    connect(mPlaybackRangeCheckBox, &QCheckBox::clicked, this, &TimeControls::playbackRangeClicked);

    auto spinBoxValueChanged = static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged);
    connect(mLoopStartSpinBox, spinBoxValueChanged, this, &TimeControls::loopStartValueChanged);
    connect(mLoopEndSpinBox, spinBoxValueChanged, this, &TimeControls::loopEndValueChanged);

    connect(mPlaybackRangeCheckBox, &QCheckBox::toggled, mLoopStartSpinBox, &QSpinBox::setEnabled);
    connect(mPlaybackRangeCheckBox, &QCheckBox::toggled, mLoopEndSpinBox, &QSpinBox::setEnabled);

    connect(mSoundButton, &QPushButton::clicked, this, &TimeControls::soundClick);
    connect(mSoundButton, &QPushButton::clicked, this, &TimeControls::updateSoundIcon);
    auto connection = connect(mFpsBox, spinBoxValueChanged, this, &TimeControls::fpsClick);
    if(!connection)
    {
        // Use "editingFinished" if the "spinBoxValueChanged" signal doesn't work...
        connect(mFpsBox, &QSpinBox::editingFinished, this, &TimeControls::onFpsEditingFinished);
    }
}

void TimeControls::playButtonClicked()
{
    emit playButtonTriggered();
}

void TimeControls::updatePlayState()
{
    if (mEditor->playback()->isPlaying())
    {
        mPlayButton->setIcon(mStopIcon);
        mPlayButton->setToolTip(tr("Stop"));
    }
    else
    {
        mPlayButton->setIcon(mStartIcon);
        mPlayButton->setToolTip(tr("Start"));
    }
}

void TimeControls::jumpToStartButtonClicked()
{
    if (mPlaybackRangeCheckBox->isChecked())
    {
        mEditor->scrubTo(mLoopStartSpinBox->value());
        mEditor->playback()->setCheckForSoundsHalfway(true);
    }
    else
    {
        mEditor->scrubTo(mEditor->layers()->firstKeyFrameIndex());
    }
    mEditor->playback()->stopSounds();
}

void TimeControls::jumpToEndButtonClicked()
{
    if (mPlaybackRangeCheckBox->isChecked())
    {
        mEditor->scrubTo(mLoopEndSpinBox->value());
    }
    else
    {
        mEditor->scrubTo(mEditor->layers()->lastKeyFrameIndex());
    }
}

void TimeControls::loopButtonClicked(bool bChecked)
{
    mEditor->playback()->setLooping(bChecked);
}

void TimeControls::playbackRangeClicked(bool bChecked)
{
    mEditor->playback()->enableRangedPlayback(bChecked);
    mTimeline->updateLength();
}

void TimeControls::loopStartValueChanged(int i)
{
    if (i >= mLoopEndSpinBox->value())
    {
        mLoopEndSpinBox->setValue(i + 1);
    }
    mLoopEndSpinBox->setMinimum(i + 1);

    mEditor->playback()->setRangedStartFrame(i);
    mTimeline->updateLength();
}

void TimeControls::loopEndValueChanged(int i)
{
    mEditor->playback()->setRangedEndFrame(i);
    mTimeline->updateLength();
}

void TimeControls::updateSoundIcon(bool soundEnabled)
{
    if (soundEnabled)
    {
        mSoundButton->setIcon(QIcon(":icons/controls/sound.png"));
    }
    else
    {
        mSoundButton->setIcon(QIcon(":icons/controls/sound-disabled.png"));
    }
}

void TimeControls::onFpsEditingFinished()
{
    emit fpsClick(mFpsBox->value());
}

void TimeControls::updateLength(int frameLength)
{
    mLoopStartSpinBox->setMaximum(frameLength - 1);
    mLoopEndSpinBox->setMaximum(frameLength);
}
