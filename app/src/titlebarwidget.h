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
#ifndef TITLEBARWIDGET_H
#define TITLEBARWIDGET_H

#include <QWidget>

#include "appearance.h"

class QLabel;
class QHBoxLayout;
class QMenu;
class QToolButton;

class TitleBarWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TitleBarWidget(QWidget* parent = nullptr);
    ~TitleBarWidget();

    void resizeEvent(QResizeEvent* resizeEvent) override;
    void setTitle(const QString& title);
    void paintEvent(QPaintEvent*) override;

    void setIsFloating(bool floating) { mIsFloating = floating; }

signals:
    void closeButtonPressed();
    void undockButtonPressed();

private:
    QString flatButtonStylesheet() const;
    void showEvent(QShowEvent* event) override;
    void hideButtons(bool hide);
    void hideButtonsIfNeeded(int width);

    QWidget* createNormalTitleBarWidget(QWidget* parent);

    QWidget* mNormalTitleBarWidget = nullptr;
    QLayout* mContainerLayout = nullptr;

    QLabel* mTitleLabel = nullptr;
    QToolButton* mCloseButton = nullptr;
    QToolButton* mDockButton = nullptr;

    QMenu* mMenu = nullptr;
    QAction* mDockAction = nullptr;
    bool mIsFloating = false;

    int mWidthOfFullLayout = 0;
};

#endif // TITLEBARWIDGET_H
