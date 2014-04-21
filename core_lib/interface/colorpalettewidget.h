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

#include <QDockWidget>
#include <QToolButton>


class QListWidget;
class QListWidgetItem;
class Object;
class Editor;
class ColorBox;


class ColorPaletteWidget : public QDockWidget
{
    Q_OBJECT

public:
    ColorPaletteWidget( QWidget* );
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
    void colourSwatchClicked();
    void colorListCurrentItemChanged(QListWidgetItem*, QListWidgetItem*);
    void clickColorListItem(QListWidgetItem*);
    void changeColourName(QListWidgetItem*);
    void clickAddColorButton();
    void clickRemoveColorButton();

private:
    Editor* m_pEditor;
    QListWidget* m_colorListView;
    QToolButton* m_addButton;
    QToolButton* m_removeButton;
};

#endif

