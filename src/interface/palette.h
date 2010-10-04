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
#ifndef PALETTE_H
#define PALETTE_H

//#include <QSize>
#include <QDockWidget>
#include <QListWidget>
#include <QListWidgetItem>
#include <QToolButton>
//#include <QString>
#include "toolset.h"

class Editor;

class Palette : public QDockWidget
{
	Q_OBJECT
			
  public:
		Palette(Editor*);
		int currentColour() { return listOfColours->currentRow(); }
	
  protected:
		//QSize sizeHint();
		
		Editor* editor;
		QListWidget* listOfColours;
		QToolButton *addButton, *removeButton;
		QSlider *sliderRed, *sliderGreen, *sliderBlue, *sliderAlpha;
		QToolButton *colourSwatch;
	
	public slots:
		void updateList();
		void updateSwatch(QColor);
		void selectColour(int i) { listOfColours->setCurrentRow(i); }
		void setColour(QColor);
		void setColour(int, int, int, int);
		
  private slots:
		void colourSwatchClicked();
		void selectColour(QListWidgetItem*, QListWidgetItem*);
		void selectAndApplyColour(QListWidgetItem*);
		void updateColour();
		void changeColour();
		void changeColour(QListWidgetItem*);
		void changeColourName(QListWidgetItem*);
		void addClick();
		void rmClick();
		void closeIfDocked(bool);
};

#endif

