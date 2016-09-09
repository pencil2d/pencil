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
class PreferenceManager;
class TimeLine;

class TimeControls : public QToolBar
{
    Q_OBJECT

public:
    TimeControls(TimeLine* parent = 0 );
    void initUI();
    
    void setFps ( int value );
    void setCore( Editor* editor );
    void updateLength(int frameLength);
    void updatePlayState();
    int getRangeLower() { return mPlaybackRangeCheckBox->isChecked() ? mLoopStartSpinBox->value() : -1; }
    int getRangeUpper() { return mPlaybackRangeCheckBox->isChecked() ? mLoopEndSpinBox->value() : -1; }

Q_SIGNALS:
    void soundClick( bool );
    void fpsClick(int);

    void loopStartClick(int);
    void loopEndClick(int);
    void rangeStateChange();

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
    void preLoopStartClick(int);

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

    QIcon mStartIcon;
    QIcon mStopIcon;

    Editor* mEditor = nullptr;
};

#endif
