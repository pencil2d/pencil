/*

Pencil2D - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2012-2020 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/
#ifndef POPUPCOLORPALETTEWIDGET_H
#define POPUPCOLORPALETTEWIDGET_H

#include <QWidget>

class ColorBox;
class ScribbleArea;
class QPushButton;


class PopupColorPaletteWidget : public QWidget
{
    Q_OBJECT

public:
    PopupColorPaletteWidget( ScribbleArea *parent = 0 );
    QPushButton* closeButton;
    ColorBox* getColorBox() { return mColorBox; }
    void popup();

private slots:
    void onColorChanged(const QColor& color);

protected:
    ScribbleArea* mContainer;
    ColorBox* mColorBox = nullptr;
    void keyPressEvent(QKeyEvent *event);
};

#endif // POPUPCOLORPALETTEWIDGET_H
