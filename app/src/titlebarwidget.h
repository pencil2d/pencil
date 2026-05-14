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
class QToolButton;
class QToolBar;
class QHBoxLayout;

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

    /// Add a child widget next to the bar buttons
    void setChildWidget(QWidget* widget);

    /// Locking the title area hides the title bar buttons
    void lock(bool locked);

    /// Returns true if a custom child widget has been added, otherwise false
    bool hasChildWidget() const { return mHasChildWidget; }

signals:
    void closeButtonPressed();
    void undockButtonPressed();

private:
    void hideButtons(bool hide);
    QString flatButtonStylesheet() const;
    void showEvent(QShowEvent* event) override;
    void hideButtonsIfNeeded(int width);

    QWidget* createCustomTitleBarWidget(QWidget* parent);

    QLabel* mTitleLabel = nullptr;
    QToolButton* mCloseButton = nullptr;
    QToolButton* mDockButton = nullptr;

    QHBoxLayout* mContainerLayout = nullptr;

    bool mIsLocked = false;
    bool mIsFloating = false;
    bool mHasChildWidget = false;

    int mWidthOfFullLayout = 0;
};

#endif // TITLEBARWIDGET_H
