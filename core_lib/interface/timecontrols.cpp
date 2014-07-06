/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon

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


TimeControls::TimeControls(QWidget* parent) : QToolBar(parent)
{
    QSettings settings("Pencil","Pencil");

    //QFrame* frame = new QFrame();

    fpsBox = new QSpinBox();
    //fpsBox->setFixedWidth(50);
    fpsBox->setFont( QFont("Helvetica", 10) );
    fpsBox->setFixedHeight(22);
    fpsBox->setValue(settings.value("fps").toInt());
    fpsBox->setMinimum(1);
    fpsBox->setMaximum(50);
    fpsBox->setToolTip("Frames per second");
    fpsBox->setFocusPolicy(Qt::NoFocus);

    m_pLoopStartSpinBox = new QSpinBox();
    m_pLoopStartSpinBox->setFont( QFont("Helvetica", 10) );
    m_pLoopStartSpinBox->setFixedHeight(22);
    m_pLoopStartSpinBox->setValue(settings.value("loopStart").toInt());
    m_pLoopStartSpinBox->setMinimum(1);
    m_pLoopStartSpinBox->setToolTip(tr("Start of loop"));
    m_pLoopStartSpinBox->setFocusPolicy(Qt::NoFocus);

    m_pLoopEndSpinBox= new QSpinBox();
    m_pLoopEndSpinBox->setFont( QFont("Helvetica", 10) );
    m_pLoopEndSpinBox->setFixedHeight(22);
    m_pLoopEndSpinBox->setMinimum(2);
    m_pLoopEndSpinBox->setToolTip(tr("End of loop"));
    m_pLoopEndSpinBox->setFocusPolicy(Qt::NoFocus);
    m_pLoopEndSpinBox->setValue( settings.value( "loopEnd" ).toInt() );

    m_pPlaybackRangeCheckBox = new QCheckBox( tr("Range") );
    m_pPlaybackRangeCheckBox->setFont( QFont("Helvetica", 10) );
    m_pPlaybackRangeCheckBox->setFixedHeight(26);
    m_pPlaybackRangeCheckBox->setToolTip(tr("Loop control"));

    QPushButton* playButton = new QPushButton( this );
    loopButton = new QPushButton();
    soundButton = new QPushButton();
    m_pGotoEndButton= new QPushButton();
    m_pGotoStartButton= new QPushButton();
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
#ifdef Q_WS_MAC
    /*loopButton->setFixedSize( QSize(35,30) );
    loopButton->setIconSize( QSize(35,30) );
    loopIcon.addFile (":icons/controls/loopOn.png", QSize(35,20), QIcon::Normal, QIcon::On );
    loopIcon.addFile (":icons/controls/loopOff.png", QSize(35,20), QIcon::Normal, QIcon::Off);
    loopIcon.addFile (":icons/controls/loopOn.png", QSize(35,20), QIcon::Disabled, QIcon::On );
    loopIcon.addFile (":icons/controls/loopOff.png", QSize(35,20), QIcon::Disabled, QIcon::Off);
    loopIcon.addFile (":icons/controls/loopOn.png", QSize(35,20), QIcon::Active, QIcon::On );
    loopIcon.addFile (":icons/controls/loopOff.png", QSize(35,20), QIcon::Active, QIcon::Off);*/
#endif
    playButton->setIcon(playIcon);
    loopButton->setIcon(loopIcon);
    soundButton->setIcon(soundIcon);
    m_pGotoEndButton->setIcon(endplayIcon);
    m_pGotoStartButton->setIcon(startplayIcon);

    playButton->setToolTip(tr("Play"));
    loopButton->setToolTip(tr("Loop"));
    soundButton->setToolTip(tr("Sound on/off"));
    m_pGotoEndButton->setToolTip(tr("End"));
    m_pGotoStartButton->setToolTip(tr("Start"));

    loopButton->setCheckable(true);
    soundButton->setCheckable(true);
    soundButton->setChecked(true);

    addWidget(separator);
    addWidget(m_pGotoStartButton);
    addWidget(playButton);
    addWidget(m_pGotoEndButton);
    addWidget(loopButton);
    addWidget(m_pPlaybackRangeCheckBox);
    addWidget(m_pLoopStartSpinBox);
    addWidget(m_pLoopEndSpinBox);
    addWidget(soundButton);
    addWidget(fpsLabel);
    addWidget(fpsBox);

    connect(playButton, SIGNAL(clicked()), this, SIGNAL(playClick()));
    connect(m_pGotoEndButton, SIGNAL(clicked()), this, SIGNAL(clickGotoEndButton()));
    connect(m_pGotoStartButton, SIGNAL(clicked()), this, SIGNAL(clickGotoStartButton()));
    connect(loopButton, SIGNAL(clicked(bool)), this, SIGNAL(loopClick(bool)));

    connect(m_pPlaybackRangeCheckBox, &QCheckBox::toggled, this, &TimeControls::loopControlClick );//adding loopcontrol

    auto spinBoxValueChanged = static_cast< void ( QSpinBox::* )( int ) >( &QSpinBox::valueChanged );
    connect(m_pLoopStartSpinBox, spinBoxValueChanged, this, &TimeControls::loopStartClick );
    connect(m_pLoopEndSpinBox, spinBoxValueChanged, this, &TimeControls::loopEndClick );

    connect( m_pPlaybackRangeCheckBox, &QCheckBox::toggled, m_pLoopStartSpinBox, &QSpinBox::setEnabled );
    connect( m_pPlaybackRangeCheckBox, &QCheckBox::toggled, m_pLoopEndSpinBox, &QSpinBox::setEnabled );

    connect( soundButton, &QPushButton::clicked, this, &TimeControls::soundClick );
    connect(fpsBox,SIGNAL(valueChanged(int)), this, SIGNAL(fpsClick(int)));

    m_pPlaybackRangeCheckBox->setChecked( false );
    m_pLoopStartSpinBox->setEnabled( false );
    m_pLoopEndSpinBox->setEnabled( false );
}

void TimeControls::updateButtons(bool floating)
{
    Q_UNUSED(floating);
    /*if (floating) {
    	loopButton->setFixedSize(QSize(45,22));
    	soundButton->setFixedSize(QSize(45,22));
    } else {
    	loopButton->setFixedSize(QSize(26,17));
    	soundButton->setFixedSize(QSize(26,17));
    }*/
}

void TimeControls::setFps ( int value )
{
    fpsBox->setValue(value);
}

void TimeControls::toggleLoop(bool checked)
{
    loopButton->setChecked(checked);
}

void TimeControls::toggleLoopControl(bool checked)
{
    m_pPlaybackRangeCheckBox->setChecked(checked);
}

void TimeControls::setLoopStart ( int value )
{
    m_pLoopStartSpinBox->setValue(value);
}
