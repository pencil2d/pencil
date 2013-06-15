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

#include <QDockWidget>
#include <QListWidget>
#include <QListWidgetItem>
#include <QToolButton>
#include "toolset.h"

class Object;
class Editor;


class Palette : public QDockWidget
{
    Q_OBJECT

public:
    Palette(Editor*);
    int currentColourNumber() { return listOfColours->currentRow(); }

protected:
    Editor* editor;
    //Object* object;

    QListWidget* listOfColours;
    QToolButton* addButton;
    QToolButton* removeButton;
    QSlider* sliderRed;
    QSlider* sliderGreen;
    QSlider* sliderBlue;
    QSlider* sliderAlpha;
    QToolButton* colourSwatch;

public slots:
    void updateList();
    void updateSwatch(QColor);
    void selectColour(int i) { listOfColours->setCurrentRow(i); }
    void setColour(QColor);
    void setColour(int, int, int, int);

private slots:
    void colourSwatchClicked();
    void colorListItemChanged(QListWidgetItem*, QListWidgetItem*);
    void selectAndApplyColour(QListWidgetItem*);
    void colorSliderMoved();
    void colourSliderValueChange();
    void changeColourName(QListWidgetItem*);
    void clickAddColorButton();
    void clickRemoveColorButton();
    void closeIfDocked(bool);
};

#endif

