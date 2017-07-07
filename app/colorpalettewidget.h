/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2013-2017 Matt Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

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
class QActionGroup;
class QMenu;

namespace Ui
{
class ColorPalette;
}

class PreferenceManager;
enum class SETTING;

class ColorPaletteWidget : public BaseDockWidget
{
    Q_OBJECT

public:
    explicit ColorPaletteWidget( QWidget* parent );

    void init(PreferenceManager * prefs);
    void initUI() override;
    void updateUI() override;

    int currentColourNumber();

public slots:
    void selectColorNumber(int);
    void setColor(QColor);
    void refreshColorList();

protected:
    void resizeEvent(QResizeEvent *event) override;

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
    void palettePreferences();
    void setListMode();
    void setGridMode();
    void setSwatchSizeSmall();
    void setSwatchSizeMedium();
    void setSwatchSizeLarge();
    void updateGridUI();
    void settingUpdated(SETTING setting);

private:
    Ui::ColorPalette* ui = nullptr;
    QActionGroup *layoutModes;
    QAction *listMode;
    QAction *gridMode;
    QActionGroup *iconSizes;
    QAction *smallSwatch;
    QAction *mediumSwatch;
    QAction *largeSwatch;
    QAction *separator;
    QSize iconSize;
    QMenu *toolMenu;
    int stepper;
    PreferenceManager * mPrefs = nullptr;

    void loadBackgroundStyle();
};

#endif

