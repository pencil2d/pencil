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
    TimeControls(TimeLine* parent = 0);
    void initUI();
    void updateUI();

    void setFps(int value);
    void setEditor(Editor* editor);
    void updateLength(int frameLength);
    void updatePlayState();
    int getRangeLower() { return mPlaybackRangeCheckBox->isChecked() ? mLoopStartSpinBox->value() : -1; }
    int getRangeUpper() { return mPlaybackRangeCheckBox->isChecked() ? mLoopEndSpinBox->value() : -1; }

Q_SIGNALS:
    void soundClick(bool);
    void fpsClick(int);
    void playButtonTriggered();

public slots:
    void toggleLoop(bool);
    void toggleLoopControl(bool);

    /// Work-around in case the FPS spin-box "valueChanged" signal doesn't work.
    void onFpsEditingFinished();

private:
    void makeConnections();

    void playButtonClicked();
    void jumpToStartButtonClicked();
    void jumpToEndButtonClicked();
    void loopButtonClicked(bool bChecked);
    void playbackRangeClicked(bool bChecked);
    void loopStartValueChanged(int);
    void loopEndValueChanged(int);
    void updateSoundIcon(bool soundEnabled);

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
    QIcon mLoopIcon;
    QIcon mSoundIcon;
    QIcon mJumpToEndIcon;
    QIcon mJumpToStartIcon;

    TimeLine* mTimeline = nullptr;
    Editor* mEditor = nullptr;
};

#endif
