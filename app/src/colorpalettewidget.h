/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2013-2018 Matt Chiawen Chang

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
class QString;

namespace Ui
{
    class ColorPalette;
}


class ColorPaletteWidget : public BaseDockWidget
{
    Q_OBJECT

public:

    explicit ColorPaletteWidget(QWidget* parent);
    ~ColorPaletteWidget();

    void initUI() override;
    void updateUI() override;

    int currentColourNumber();

    void selectColorNumber(int);
    void setColor(QColor, int);
    void refreshColorList();

    void showContextMenu(const QPoint&);

signals:
    void colorChanged(QColor);
    void colorNumberChanged(int);

protected:
    void resizeEvent(QResizeEvent* event) override;

private slots:
    void clickColorListItem(QListWidgetItem*);
    void changeColourName(QListWidgetItem*);
    void onItemChanged(QListWidgetItem* item);
    void clickAddColorButton();
    void clickColorDialogButton();
    void clickRemoveColorButton();
    void palettePreferences();
    void setListMode();
    void setGridMode();
    void setSwatchSizeSmall();
    void setSwatchSizeMedium();
    void setSwatchSizeLarge();
    void addItem();
    void replaceItem();
    void removeItem();
    void showPaletteReminder();

    bool showPaletteWarning();

private:
    void updateItemColor(int, QColor);
    void updateGridUI();

    QString getDefaultColorName(QColor c);

    Ui::ColorPalette* ui = nullptr;
    QActionGroup* mLayoutModeActionGroup = nullptr;
    QAction* mListModeAction = nullptr;
    QAction* mGridModeAction = nullptr;
    QActionGroup* mIconSizesActionGroup = nullptr;
    QAction* mSmallSwatchAction = nullptr;
    QAction* mMediumSwatchAction = nullptr;
    QAction* mLargeSwatchAction = nullptr;
    QAction* mSeparator = nullptr;

    QSize mIconSize{ 34, 34 };
    QMenu* mToolMenu = nullptr;
    int stepper = 0;

    QString buttonStylesheet;

    bool mIsColorDialog = false;
    bool mMultipleSelected = false;

};

#endif

