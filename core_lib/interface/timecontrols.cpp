/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2013-2014 Matt Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation;

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/

#include <QtGui>
#include <QLabel>
#include "timecontrols.h"
#include "editor.h"
#include "playbackmanager.h"
#include "layermanager.h"


TimeControls::TimeControls(QWidget* parent) : QToolBar(parent)
{
    QSettings settings("Pencil","Pencil");

    mFpsBox = new QSpinBox();
    mFpsBox->setFont( QFont("Helvetica", 10) );
    mFpsBox->setFixedHeight(22);
    mFpsBox->setValue(settings.value("fps").toInt());
    mFpsBox->setMinimum(1);
    mFpsBox->setMaximum(50);
    mFpsBox->setToolTip("Frames per second");
    mFpsBox->setFocusPolicy(Qt::NoFocus);

    mLoopStartSpinBox = new QSpinBox();
    mLoopStartSpinBox->setFont( QFont("Helvetica", 10) );
    mLoopStartSpinBox->setFixedHeight(22);
    mLoopStartSpinBox->setValue(settings.value("loopStart").toInt());
    mLoopStartSpinBox->setMinimum(1);
    mLoopStartSpinBox->setToolTip(tr("Start of loop"));
    mLoopStartSpinBox->setFocusPolicy(Qt::NoFocus);

    mLoopEndSpinBox= new QSpinBox();
    mLoopEndSpinBox->setFont( QFont("Helvetica", 10) );
    mLoopEndSpinBox->setFixedHeight(22);
    mLoopEndSpinBox->setMinimum(2);
    mLoopEndSpinBox->setToolTip(tr("End of loop"));
    mLoopEndSpinBox->setFocusPolicy(Qt::NoFocus);
    mLoopEndSpinBox->setValue( settings.value( "loopEnd" ).toInt() );

    mPlaybackRangeCheckBox = new QCheckBox( tr("Range") );
    mPlaybackRangeCheckBox->setFont( QFont("Helvetica", 10) );
    mPlaybackRangeCheckBox->setFixedHeight(26);
    mPlaybackRangeCheckBox->setToolTip(tr("Loop control"));

    mPlayButton = new QPushButton( this );
    mLoopButton = new QPushButton();
    mSoundButton = new QPushButton();
    mJumpToEndButton= new QPushButton();
    mJumpToStartButton= new QPushButton();

    QLabel* separator = new QLabel();
    separator->setPixmap(QPixmap(":icons/controls/separator.png"));
    separator->setFixedSize(QSize(37,31));
    QLabel* spacingLabel = new QLabel("");
    spacingLabel->setIndent(6);
    QLabel* fpsLabel = new QLabel(tr("Fps: "));
    fpsLabel->setIndent(6);

    QIcon playIcon(":icons/controls/play.png");
    QIcon loopIcon(":icons/controls/loop.png");
    QIcon soundIcon(":icons/controls/sound.png");
    QIcon endplayIcon(":icons/controls/endplay.png");
    QIcon startplayIcon(":icons/controls/startplay.png");
    mPlayButton->setIcon(playIcon);
    mLoopButton->setIcon(loopIcon);
    mSoundButton->setIcon(soundIcon);
    mJumpToEndButton->setIcon(endplayIcon);
    mJumpToStartButton->setIcon(startplayIcon);

    mPlayButton->setToolTip(tr("Play"));
    mLoopButton->setToolTip(tr("Loop"));
    mSoundButton->setToolTip(tr("Sound on/off"));
    mJumpToEndButton->setToolTip(tr("End"));
    mJumpToStartButton->setToolTip(tr("Start"));

    mLoopButton->setCheckable(true);
    mSoundButton->setCheckable(true);
    mSoundButton->setChecked(true);

    addWidget(separator);
    addWidget(mJumpToStartButton);
    addWidget(mPlayButton);
    addWidget(mJumpToEndButton);
    addWidget(mLoopButton);
    addWidget(mPlaybackRangeCheckBox);
    addWidget(mLoopStartSpinBox);
    addWidget(mLoopEndSpinBox);
    addWidget(mSoundButton);
    addWidget(fpsLabel);
    addWidget(mFpsBox);

    makeConnections();

    connect(mPlaybackRangeCheckBox, &QCheckBox::toggled, this, &TimeControls::loopControlClick );//adding loopcontrol

    auto spinBoxValueChanged = static_cast< void ( QSpinBox::* )( int ) >( &QSpinBox::valueChanged );
    connect(mLoopStartSpinBox, spinBoxValueChanged, this, &TimeControls::loopStartClick );
    connect(mLoopEndSpinBox, spinBoxValueChanged, this, &TimeControls::loopEndClick );

    connect( mPlaybackRangeCheckBox, &QCheckBox::toggled, mLoopStartSpinBox, &QSpinBox::setEnabled );
    connect( mPlaybackRangeCheckBox, &QCheckBox::toggled, mLoopEndSpinBox, &QSpinBox::setEnabled );

    connect( mSoundButton, &QPushButton::clicked, this, &TimeControls::soundClick );
    connect(mFpsBox,SIGNAL(valueChanged(int)), this, SIGNAL(fpsClick(int)));

    mPlaybackRangeCheckBox->setChecked( false );
    mLoopStartSpinBox->setEnabled( false );
    mLoopEndSpinBox->setEnabled( false );
}

void TimeControls::setFps ( int value )
{
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

void TimeControls::setLoopStart ( int value )
{
    mLoopStartSpinBox->setValue(value);
}

void TimeControls::setCore( Editor* editor )
{
    Q_ASSERT( editor != nullptr );
    mEditor = editor;
}

void TimeControls::makeConnections()
{
    connect( mPlayButton,        &QPushButton::clicked, this, &TimeControls::playButtonClicked );
    connect( mJumpToEndButton,   &QPushButton::clicked, this, &TimeControls::jumpToStartButtonClicked );
    connect( mJumpToStartButton, &QPushButton::clicked, this, &TimeControls::jumpToEndButtonClicked );
    connect( mLoopButton,        &QPushButton::clicked, this, &TimeControls::loopButtonClicked );
}

void TimeControls::playButtonClicked()
{
    mEditor->playback()->play();
}

void TimeControls::jumpToStartButtonClicked()
{
    mEditor->layers()->gotoFirstKeyFrame();
}

void TimeControls::jumpToEndButtonClicked()
{
    mEditor->layers()->gotoLastKeyFrame();
}

void TimeControls::loopButtonClicked( bool bChecked )
{
    mEditor->playback()->setLooping( bChecked );
}
