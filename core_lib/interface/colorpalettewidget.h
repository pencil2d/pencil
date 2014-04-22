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

#ifndef COLOR_PALETTE_WIDGET_H
#define COLOR_PALETTE_WIDGET_H

#include "basedockwidget.h"

class QToolButton;
class QListWidget;
class QListWidgetItem;
class Object;
class Editor;
class ColorBox;

namespace Ui
{
class ColorPalette;
}


class ColorPaletteWidget : public BaseDockWidget
{
    Q_OBJECT

public:
    ColorPaletteWidget( QWidget* pParent );
    void initUI() override;
    void updateUI() override;

    int currentColourNumber();

public slots:
    void selectColorNumber(int);
    void setColor(QColor);
    void refreshColorList();

signals:
    void colorChanged(QColor);
    void colorNumberChanged(int);

private slots:
    void updateItemColor(int, QColor);
    void colorListCurrentItemChanged(QListWidgetItem*, QListWidgetItem*);
    void clickColorListItem(QListWidgetItem*);
    void changeColourName(QListWidgetItem*);
    void clickAddColorButton();
    void clickRemoveColorButton();

private:
    Ui::ColorPalette* ui;
};

#endif

