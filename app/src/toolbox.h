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
#ifndef TOOLBOXDOCKWIDGET_H
#define TOOLBOXDOCKWIDGET_H

#include "pencildef.h"
#include "basedockwidget.h"

#include "toolboxwidget.h"

class QToolButton;
class QGridLayout;
class QIcon;
class SpinSlider;
class DisplayOptionWidget;
class ToolOptionWidget;
class Editor;
class FlowLayout;

class ToolBoxDockWidget : public BaseDockWidget
{
    Q_OBJECT

public:
    ToolBoxDockWidget(QWidget* parent);
    ~ToolBoxDockWidget() override;

    void initUI() override;
    void updateUI() override;

    void setActiveTool(ToolType type);

private:
    void onLayerDidChange(int);

    ToolBoxWidget* mWidget = nullptr;
};

#endif
