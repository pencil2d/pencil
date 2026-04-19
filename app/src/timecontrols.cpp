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

#include "timecontrols.h"

#include <QLabel>
#include <QSettings>
#include <QMenu>
#include <QDebug>
#include <QSpacerItem>
#include <QFontDatabase>
#include "editor.h"
#include "playbackmanager.h"
#include "layermanager.h"
#include "pencildef.h"
#include "util.h"
#include "preferencemanager.h"
#include "timeline.h"
#include "pencildef.h"

#include <QLayout>

TimeControls::TimeControls(TimeLine* parent) : QWidget(parent)
{
    mTimeline = parent;
}

void TimeControls::initUI()
{
    QSettings settings(PENCIL2D, PENCIL2D);

    QHBoxLayout* hBoxLayout = new QHBoxLayout();
    hBoxLayout->setContentsMargins(0,0,0,0);

    setLayout(hBoxLayout);

    mFpsBox = new QSpinBox(this);
    mFpsBox->setValue(settings.value("Fps").toInt());
    mFpsBox->setMinimum(1);
    mFpsBox->setMaximum(90);
    mFpsBox->setSuffix(tr(" fps"));
    mFpsBox->setToolTip(tr("Frames per second"));
    mFpsBox->setFocusPolicy(Qt::WheelFocus);

    mFps = mFpsBox->value();

    setupTimeCodeMenu();

    mLoopStartSpinBox = new QSpinBox(this);
    mLoopStartSpinBox->setValue(settings.value("loopStart").toInt());
    mLoopStartSpinBox->setMinimum(1);
    mLoopStartSpinBox->setEnabled(false);
    mLoopStartSpinBox->setToolTip(tr("Start of playback loop"));
    mLoopStartSpinBox->setFocusPolicy(Qt::WheelFocus);

    mLoopEndSpinBox = new QSpinBox(this);
    mLoopEndSpinBox->setValue(settings.value("loopEnd").toInt());
    mLoopEndSpinBox->setMinimum(2);
    mLoopEndSpinBox->setEnabled(false);
    mLoopEndSpinBox->setToolTip(tr("End of playback loop"));
    mLoopEndSpinBox->setFocusPolicy(Qt::WheelFocus);

    mPlaybackRangeCheckBox = new QCheckBox(tr("Range"));
    mPlaybackRangeCheckBox->setToolTip(tr("Playback range"));

    mPlayButton = new QToolButton(this);
    mPlayButton->setIconSize(QSize(22,22));
    mLoopButton = new QToolButton(this);
    mLoopButton->setIconSize(QSize(22,22));
    mSoundButton = new QToolButton(this);
    mSoundButton->setIconSize(QSize(22,22));
    mSoundScrubButton = new QToolButton(this);
    mSoundScrubButton->setIconSize(QSize(22,22));
    mJumpToEndButton = new QToolButton(this);
    mJumpToEndButton->setIconSize(QSize(22,22));
    mJumpToStartButton = new QToolButton(this);
    mJumpToStartButton->setIconSize(QSize(22,22));

    mLoopIcon = QIcon(":icons/themes/playful/controls/control-loop.svg");
    mSoundIcon = QIcon(":icons/themes/playful/controls/control-sound-enable.svg");
    mSoundScrubIcon = QIcon(":icons/themes/playful/controls/control-sound-scrub.svg");
    mJumpToEndIcon = QIcon(":icons/themes/playful/controls/control-play-end.svg");
    mJumpToStartIcon = QIcon(":icons/themes/playful/controls/control-play-start.svg");
    mStartIcon = QIcon(":icons/themes/playful/controls/control-play.svg");
    mStopIcon = QIcon(":icons/themes/playful/controls/control-stop.svg");
    mPlayButton->setIcon(mStartIcon);
    mLoopButton->setIcon(mLoopIcon);
    mSoundButton->setIcon(mSoundIcon);
    mSoundScrubButton->setIcon(mSoundScrubIcon);
    mJumpToEndButton->setIcon(mJumpToEndIcon);
    mJumpToStartButton->setIcon(mJumpToStartIcon);

    mPlayButton->setToolTip(tr("Play"));
    mLoopButton->setToolTip(tr("Loop"));
    mSoundButton->setToolTip(tr("Sound on/off"));
    mSoundScrubButton->setToolTip(tr("Sound scrub on/off"));
    mJumpToEndButton->setToolTip(tr("Jump to the End", "Tooltip of the jump to end button"));
    mJumpToStartButton->setToolTip(tr("Jump to the Start", "Tooltip of the jump to start button"));

    mLoopButton->setCheckable(true);
    mSoundButton->setCheckable(true);
    mSoundButton->setChecked(true);
    mSoundScrubButton->setCheckable(true);
    mSoundScrubButton->setChecked(mEditor->preference()->isOn(SETTING::SOUND_SCRUB_ACTIVE));

    layout()->addItem(new QSpacerItem(1,1, QSizePolicy::Expanding));
    layout()->addWidget(mTimeCode.timecodeLabel);
    layout()->addWidget(mTimeCode.timecodeButton);
    layout()->addWidget(mJumpToStartButton);
    layout()->addWidget(mPlayButton);
    layout()->addWidget(mJumpToEndButton);
    layout()->addWidget(mLoopButton);

    layout()->addWidget(mFpsBox);
    layout()->addWidget(mPlaybackRangeCheckBox);
    layout()->addWidget(mLoopStartSpinBox);
    layout()->addWidget(mLoopEndSpinBox);
    layout()->addWidget(mSoundButton);
    layout()->addWidget(mSoundScrubButton);
    layout()->addItem(new QSpacerItem(1,1, QSizePolicy::Expanding));

    makeConnections();

    updateUI();
}

void TimeControls::updateUI()
{
    PlaybackManager* playback = mEditor->playback();

    mPlaybackRangeCheckBox->setChecked(playback->isRangedPlaybackOn()); // don't block this signal since it enables start/end range spinboxes.

    QSignalBlocker b1(mLoopStartSpinBox);
    mLoopStartSpinBox->setValue(playback->markInFrame());

    QSignalBlocker b2(mLoopEndSpinBox);
    mLoopEndSpinBox->setValue(playback->markOutFrame());

    QSignalBlocker b3(mFpsBox);
    mFpsBox->setValue(playback->fps());

    QSignalBlocker b4(mLoopButton);
    mLoopButton->setChecked(playback->isLooping());
}

void TimeControls::setEditor(Editor* editor)
{
    Q_ASSERT(editor != nullptr);
    mEditor = editor;
}

void TimeControls::setupTimeCodeMenu()
{
    QMenu* timeSelectMenu = new QMenu(tr("Display timecode", "Timeline menu for choose a timecode"), this);

    mTimeCode.timecodeButton = new QToolButton(this);
    mTimeCode.timecodeButton->setIconSize(QSize(22,22));
    mTimeCode.timecodeButton->setIcon(QIcon(":/icons/themes/playful/controls/control-timecode.svg"));

    timeSelectMenu->addAction(mTimeCode.hideAction = new QAction(tr("Hide"), this));
    timeSelectMenu->addSeparator();
    timeSelectMenu->addAction(mTimeCode.framesAction = new QAction(tr("Frames"), this));
    timeSelectMenu->addSeparator();
    timeSelectMenu->addAction(mTimeCode.smpteAction = new QAction(tr("SMPTE Timecode"), this));
    timeSelectMenu->addAction(mTimeCode.sffAction = new QAction(tr("SFF Timecode"), this));
    mTimeCode.timecodeButton->setMenu(timeSelectMenu);
    mTimeCode.timecodeButton->setPopupMode(QToolButton::InstantPopup);
    mTimeCode.timecodeButton->setStyleSheet("::menu-indicator{ image: none; }");
    mTimeCode.timecodeKind = timecodeKindFromPreference();
    mTimeCode.timecodeLabel = new QLabel(this);
    QFont font = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    // The default mono font is smaller, so we restore the font here
    font.setPointSize(mTimeCode.timecodeLabel->font().pointSize());
    mTimeCode.timecodeLabel->setFont(font);
    mTimeCode.timecodeLabel->setContentsMargins(2, 0, 0, 0);
    mTimeCode.timecodeLabel->setAlignment(Qt::AlignTrailing | Qt::AlignVCenter);
    mTimeCode.timecodeLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    updateTimecodeLabel(mEditor->currentFrame());
    updateTimecodeToolTip(mTimeCode.timecodeKind);
}

void TimeControls::updateTimecodeToolTip(TimecodeKind kind)
{
    switch (kind)
    {
    case TimecodeKind::NOTEXT:
        mTimeCode.timecodeLabel->setToolTip("");
        break;
    case TimecodeKind::FRAMES:
        mTimeCode.timecodeLabel->setToolTip(tr("Actual frame number"));
        break;
    case TimecodeKind::SMPTE:
        mTimeCode.timecodeLabel->setToolTip(tr("Timecode format MM:SS:FF"));
        break;
    case TimecodeKind::SFF:
        mTimeCode.timecodeLabel->setToolTip(tr("Timecode format S:FF"));
        break;
    default:
        mTimeCode.timecodeLabel->setToolTip("");
    }
}

TimecodeKind TimeControls::timecodeKindFromPreference() const
{
    int timecodePreference = mEditor->preference()->getInt(SETTING::TIMECODE_TEXT);

    switch (timecodePreference)
    {
        case 0:
            return TimecodeKind::NOTEXT;
        case 1:
            return TimecodeKind::FRAMES;
        case 2:
            return TimecodeKind::SMPTE;
        case 3:
            return TimecodeKind::SFF;
        default:
            return TimecodeKind::NOTEXT;
    }
}

int TimeControls::timecodeKindToInt(TimecodeKind kind) const
{
    return static_cast<int>(kind);
}

void TimeControls::setFps(int value)
{
    QSignalBlocker blocker(mFpsBox);
    mFpsBox->setValue(value);
    mFps = value;
    updateTimecodeLabel(mEditor->currentFrame());
}

void TimeControls::setLoop(bool checked)
{
    mLoopButton->setChecked(checked);
}

void TimeControls::setRangeState(bool checked)
{
    mPlaybackRangeCheckBox->setChecked(checked);
    mTimeline->updateLength();
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
    clearFocusOnFinished(mLoopStartSpinBox);
    connect(mLoopEndSpinBox, spinBoxValueChanged, this, &TimeControls::loopEndValueChanged);
    clearFocusOnFinished(mLoopEndSpinBox);

    connect(mPlaybackRangeCheckBox, &QCheckBox::toggled, mLoopStartSpinBox, &QSpinBox::setEnabled);
    connect(mPlaybackRangeCheckBox, &QCheckBox::toggled, mLoopEndSpinBox, &QSpinBox::setEnabled);

    connect(mSoundButton, &QPushButton::clicked, this, &TimeControls::soundToggled);

    connect(mSoundScrubButton, &QPushButton::clicked, this, &TimeControls::soundScrubToggled);
    connect(mSoundScrubButton, &QPushButton::clicked, this, &TimeControls::updateSoundScrubIcon);

    connect(mFpsBox, spinBoxValueChanged, this, &TimeControls::fpsChanged);
    connect(mFpsBox, &QSpinBox::editingFinished, this, &TimeControls::onFpsEditingFinished);

    connect(mFpsBox, spinBoxValueChanged, this, &TimeControls::setFps);
    connect(mEditor, &Editor::fpsChanged, this, &TimeControls::setFps);
    connect(mTimeCode.hideAction, &QAction::triggered, this, [this]() {
        setTimecode(TimecodeKind::NOTEXT);
    });
    connect(mTimeCode.framesAction, &QAction::triggered, this, [this]() {
        setTimecode(TimecodeKind::FRAMES);
    });
    connect(mTimeCode.smpteAction, &QAction::triggered, this, [this]() {
        setTimecode(TimecodeKind::SMPTE);
    });
    connect(mTimeCode.sffAction, &QAction::triggered, this, [this]() {
        setTimecode(TimecodeKind::SFF);
    });
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
        mPlayButton->setToolTip(tr("Play"));
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

void TimeControls::updateSoundScrubIcon(bool soundScrubEnabled)
{
    mEditor->playback()->setSoundScrubActive(soundScrubEnabled);
    mEditor->preference()->set(SETTING::SOUND_SCRUB_ACTIVE, soundScrubEnabled);
}

void TimeControls::setTimecode(TimecodeKind kind)
{
    QSettings settings(PENCIL2D, PENCIL2D);
    settings.setValue(SETTING_TIMECODE_TEXT, timecodeKindToInt(kind));
    mTimeCode.timecodeKind = kind;
    updateTimecodeLabel(mEditor->currentFrame());
    updateTimecodeToolTip(kind);
}

void TimeControls::onFpsEditingFinished()
{
    mFpsBox->clearFocus();
    emit fpsChanged(mFpsBox->value());
    mFps = mFpsBox->value();
}

void TimeControls::updateTimecodeLabel(int frame)
{
    mTimeCode.timecodeLabel->setVisible(true);

    switch (mTimeCode.timecodeKind)
    {
    case TimecodeKind::SMPTE:
        mTimeCode.timecodeLabel->setText(QString("%1:%2:%3")
                                .arg(frame / (60 * mFps) % 60, 2, 10, QLatin1Char('0'))
                                .arg(frame / mFps % 60, 2, 10, QLatin1Char('0'))
                                .arg(frame % mFps, 2, 10, QLatin1Char('0')));
        break;
    case TimecodeKind::SFF:
        mTimeCode.timecodeLabel->setText(QString("%1:%2")
                                .arg(frame / mFps)
                                .arg(frame % mFps, 2, 10, QLatin1Char('0')));
        break;
    case TimecodeKind::FRAMES:
        mTimeCode.timecodeLabel->setText(QString::number(frame).rightJustified(4, '0'));
        break;
    case TimecodeKind::NOTEXT:
    default:
        mTimeCode.timecodeLabel->setVisible(false);
        break;
    }
}

void TimeControls::updateLength(int frameLength)
{
    mLoopStartSpinBox->setMaximum(frameLength - 1);
    mLoopEndSpinBox->setMaximum(frameLength);
}
