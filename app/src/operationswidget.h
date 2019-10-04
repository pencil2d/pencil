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
#ifndef OPERATIONSWIDGET_H
#define OPERATIONSWIDGET_H

#include "pencildef.h"
#include "basedockwidget.h"

class QToolButton;
class QGridLayout;
class QIcon;
class SpinSlider;
class DisplayOptionWidget;
class ToolOptionWidget;
class Editor;

namespace Ui {
class OperationsWidget;
}

class OperationsWidget : public BaseDockWidget
{
    Q_OBJECT

public:
    OperationsWidget(QWidget* parent);
    ~OperationsWidget() override;

    void initUI() override;
    void updateUI() override;

signals:
    void newClicked();
    void openClicked();
    void saveClicked();
    void saveAsClicked();
    void undoClicked();
    void redoClicked();
    void cutClicked();
    void copyClicked();
    void pasteClicked();

private:
    Ui::OperationsWidget* ui = nullptr;
};

#endif
