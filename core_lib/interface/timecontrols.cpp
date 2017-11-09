/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2012-2017 Matthew Chiawen Chang

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


TimeControls::TimeControls(TimeLine *parent ) : QToolBar( parent )
{
    QSettings settings( PENCIL2D, PENCIL2D );

    mFpsBox = new QSpinBox();
    mFpsBox->setFont( QFont("Helvetica", 10) );
    mFpsBox->setFixedHeight(24);
    mFpsBox->setValue(settings.value("fps").toInt());
    mFpsBox->setMinimum(1);
    mFpsBox->setMaximum(90);
    mFpsBox->setSuffix(" fps");
    mFpsBox->setToolTip( tr("Frames per second") );
    mFpsBox->setFocusPolicy(Qt::WheelFocus);

    mLoopStartSpinBox = new QSpinBox();
    mLoopStartSpinBox->setFont( QFont("Helvetica", 10) );
    mLoopStartSpinBox->setFixedHeight(24);
    mLoopStartSpinBox->setValue(settings.value("loopStart").toInt());
    mLoopStartSpinBox->setMinimum(1);
    mLoopStartSpinBox->setMaximum(parent->getFrameLength() - 1);
    mLoopStartSpinBox->setToolTip(tr("Start of playback loop"));
    mLoopStartSpinBox->setFocusPolicy(Qt::WheelFocus);

    mLoopEndSpinBox= new QSpinBox();
    mLoopEndSpinBox->setFont( QFont("Helvetica", 10) );
    mLoopEndSpinBox->setFixedHeight(24);
    mLoopEndSpinBox->setValue( settings.value( "loopEnd" ).toInt() );
    mLoopEndSpinBox->setMinimum(mLoopStartSpinBox->value() + 1);
    mLoopEndSpinBox->setMaximum(parent->getFrameLength());
    mLoopEndSpinBox->setToolTip(tr("End of playback loop"));
    mLoopEndSpinBox->setFocusPolicy(Qt::WheelFocus);

    mPlaybackRangeCheckBox = new QCheckBox( tr("Range") );
    mPlaybackRangeCheckBox->setFont( QFont("Helvetica", 10) );
    mPlaybackRangeCheckBox->setFixedHeight(24);
    mPlaybackRangeCheckBox->setToolTip(tr("Playback range"));

    mPlayButton = new QPushButton( this );
    mLoopButton = new QPushButton();
    mSoundButton = new QPushButton();
    mJumpToEndButton= new QPushButton();
    mJumpToStartButton= new QPushButton();

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

    auto spinBoxValueChanged = static_cast< void ( QSpinBox::* )( int ) >( &QSpinBox::valueChanged );
    connect( mLoopStartSpinBox, spinBoxValueChanged, this, &TimeControls::preLoopStartClick );
    connect( mLoopEndSpinBox, spinBoxValueChanged, this, &TimeControls::loopEndClick );

    connect( mPlaybackRangeCheckBox, &QCheckBox::toggled, mLoopStartSpinBox, &QSpinBox::setEnabled );
    connect( mPlaybackRangeCheckBox, &QCheckBox::toggled, mLoopEndSpinBox, &QSpinBox::setEnabled );

    connect( mSoundButton, &QPushButton::clicked, this, &TimeControls::soundClick );
    connect( mSoundButton, &QPushButton::clicked, this, &TimeControls::updateSoundIcon );
    connect( mFpsBox, spinBoxValueChanged, this, &TimeControls::fpsClick );
}

void TimeControls::initUI()
{
    updateUI();
}

void TimeControls::updateUI()
{
    PlaybackManager* playback = mEditor->playback();

    mPlaybackRangeCheckBox->setChecked( playback->isRangedPlaybackOn() ); // don't block this signal

    SignalBlocker b1( mLoopStartSpinBox );
    mLoopStartSpinBox->setValue( playback->markInFrame() );

    SignalBlocker b2( mLoopEndSpinBox );
    mLoopEndSpinBox->setValue( playback->markOutFrame() );

    SignalBlocker b3( mFpsBox );
    mFpsBox->setValue( playback->fps() );

    SignalBlocker b4( mLoopButton );
    mLoopButton->setChecked( playback->isLooping() );
}

void TimeControls::setFps( int value )
{
    SignalBlocker blocker( mFpsBox );
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

void TimeControls::setCore( Editor* editor )
{
    Q_ASSERT( editor != nullptr );
    mEditor = editor;
}

void TimeControls::makeConnections()
{
    connect( mPlayButton,        &QPushButton::clicked, this, &TimeControls::playButtonClicked );
    connect( mJumpToEndButton,   &QPushButton::clicked, this, &TimeControls::jumpToEndButtonClicked );
    connect( mJumpToStartButton, &QPushButton::clicked, this, &TimeControls::jumpToStartButtonClicked );
    connect( mLoopButton,        &QPushButton::clicked, this, &TimeControls::loopButtonClicked );
    connect( mPlaybackRangeCheckBox, &QCheckBox::clicked, this, &TimeControls::playbackRangeClicked );
}

void TimeControls::playButtonClicked()
{
    if ( mEditor->playback()->isPlaying() )
    {
        mEditor->playback()->stop();
    }
    else
    {
        mEditor->playback()->play();
    }
    updatePlayState();
}

void TimeControls::updatePlayState()
{
    if( mEditor->playback()->isPlaying() )
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
    if ( mPlaybackRangeCheckBox->isChecked() )
    {
        mEditor->scrubTo( mLoopStartSpinBox->value() );
        mEditor->playback()->setCheckForSoundsHalfway( true );
    }
    else
    {
        mEditor->scrubTo( mEditor->layers()->firstKeyFrameIndex() );
    }
    mEditor->playback()->stopSounds();
}

void TimeControls::jumpToEndButtonClicked()
{
    if ( mPlaybackRangeCheckBox->isChecked() )
    {
        mEditor->scrubTo( mLoopEndSpinBox->value() );
    }
    else
    {
        mEditor->scrubTo( mEditor->layers()->lastKeyFrameIndex() );
    }
}

void TimeControls::loopButtonClicked( bool bChecked )
{
    mEditor->playback()->setLooping( bChecked );
}

void TimeControls::playbackRangeClicked( bool bChecked )
{
    mEditor->playback()->enableRangedPlayback( bChecked );
    emit rangeStateChange();
}

void TimeControls::preLoopStartClick(int i) {
    if( i >= mLoopEndSpinBox->value() )
    {
        mLoopEndSpinBox->setValue( i + 1 );
    }
    mLoopEndSpinBox->setMinimum( i + 1 );

    emit loopStartClick(i);
}

void TimeControls::updateSoundIcon(bool soundEnabled)
{
    if(soundEnabled)
    {
        mSoundButton->setIcon( QIcon(":icons/controls/sound.png") );
    }
    else
    {
        mSoundButton->setIcon( QIcon(":icons/controls/sound-disabled.png") );
    }
}

void TimeControls::updateLength(int frameLength) {
    mLoopStartSpinBox->setMaximum(frameLength - 1);
    mLoopEndSpinBox->setMaximum(frameLength);
}
