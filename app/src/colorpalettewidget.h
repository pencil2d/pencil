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
    ~ColorPaletteWidget() override;

    void initUI() override;
    void updateUI() override;
    void setCore(Editor* editor);

    int currentColorNumber();

    void selectColorNumber(int) const;
    void refreshColorList();

    void adjustSwatches();

    void showContextMenu(const QPoint&);

signals:
    void colorNumberChanged(int);

protected:
    void resizeEvent(QResizeEvent* event) override;

private slots:
    void clickColorListItem(QListWidgetItem*);
    void changeColorName(QListWidgetItem*);
    void onItemChanged(QListWidgetItem* item);
    void onRowsMoved(const QModelIndex &parent, int start, int end, const QModelIndex &destination, int row);
    void clickAddColorButton();
    void clickColorDialogButton();
    void clickRemoveColorButton();
    void palettePreferences();
    void setListMode();
    void setGridMode();
    void setSwatchSizeSmall();
    void setSwatchSizeMedium();
    void setSwatchSizeLarge();
    void fitSwatchSize();
    void addItem();
    void replaceItem();
    void removeItem();
    void showPaletteReminder();

    bool showPaletteWarning();

private:
    void updateItemColor(int, QColor);
    void updateGridUI();
    void addSwatch(int colorIndex) const;

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
    bool mFitSwatches = false;

    const int MIN_ICON_SIZE = 19;
    const int MEDIUM_ICON_SIZE = 26;
    const int MAX_ICON_SIZE = 36;

    Editor* mEditor = nullptr;
    Object* mObject = nullptr;
};

#endif

