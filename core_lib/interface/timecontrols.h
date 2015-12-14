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
#ifndef TIMECONTROL_H
#define TIMECONTROL_H

#include <QWidget>
#include <QToolBar>
#include <QPushButton>
#include <QToolButton>
#include <QSpinBox>
#include <QCheckBox>


class Editor;


class TimeControls : public QToolBar
{
    Q_OBJECT

public:
    TimeControls( QWidget* parent = 0 );
    void initUI();
    
    void setFps ( int value );
    void setCore( Editor* editor );

Q_SIGNALS:
    void soundClick( bool );
    void fpsClick(int);

    void loopStartClick(int);
    void loopEndClick(int);

public slots:
    void toggleLoop(bool);
    void toggleLoopControl(bool);

private:
    void makeConnections();
    void playButtonClicked();
    void jumpToStartButtonClicked();
    void jumpToEndButtonClicked();
    void loopButtonClicked( bool bChecked );
    void playbackRangeClicked( bool bChecked );

private:
    QPushButton* mPlayButton = nullptr;
    QPushButton* mJumpToEndButton = nullptr;
    QPushButton* mJumpToStartButton = nullptr;
    QPushButton* mLoopButton = nullptr;
    QPushButton* mSoundButton = nullptr;
    QSpinBox*    mFpsBox = nullptr;
    QCheckBox*   mPlaybackRangeCheckBox = nullptr;
    QSpinBox*    mLoopStartSpinBox = nullptr;
    QSpinBox*    mLoopEndSpinBox = nullptr;

    Editor* mEditor = nullptr;
};

#endif
