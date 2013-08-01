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
class ColorBox;


class Palette : public QDockWidget
{
    Q_OBJECT

public:
    Palette(Editor*);
    int currentColourNumber();

protected:
    Editor* editor;
    ColorBox* m_colorBox;
    QListWidget* m_colorListView;
    QToolButton* addButton;
    QToolButton* removeButton;    
signals:
    //void colorChanged(QColor);

public slots:    
    void selectColorListRow(int);
    void setColour(QColor);
    
    void refreshColorList();

private slots:    
    void updateItemColor(int, QColor);    
    void colourSwatchClicked();
    void colorListCurrentItemChanged(QListWidgetItem*, QListWidgetItem*);
    void clickColorListItem(QListWidgetItem*);
    void colorWheelChanged(QColor);
    void changeColourName(QListWidgetItem*);
    void clickAddColorButton();
    void clickRemoveColorButton();
private:

};

#endif

