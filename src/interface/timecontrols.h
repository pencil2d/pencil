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
#ifndef TIMECONTROL_H
#define TIMECONTROL_H

#include <QWidget>
#include <QToolBar>
#include <QPushButton>
#include <QToolButton>

class TimeControls : public QToolBar
{
    Q_OBJECT

public:
	TimeControls(QWidget *parent = 0);

signals:
	void playClick();
	void endClick();
	void startClick();
	void loopClick(bool);
	void soundClick();
	void fpsClick(int);
    
    void loopToggled(bool);

public slots:
	//void updateLoopButton(bool);
	void updateButtons(bool);
    void toggleLoop(bool);

protected:

private:
	QPushButton* playButton;
	QPushButton* endplayButton;
	QPushButton* startplayButton;
	QPushButton* loopButton;
	QPushButton* soundButton;
};

#endif
