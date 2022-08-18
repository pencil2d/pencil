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
#ifndef TOOLBOXWIDGET_H
#define TOOLBOXWIDGET_H

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
class ToolBoxWidget;
}

class ToolBoxWidget : public BaseDockWidget
{
    Q_OBJECT

public:
    ToolBoxWidget(QWidget* parent);
    ~ToolBoxWidget() override;

    void initUI() override;
    void updateUI() override;

public slots:
    void onToolSetActive(ToolType toolType);
    void pencilOn();
    void eraserOn();
    void selectOn();
    void moveOn();
    void penOn();
    void handOn();
    void polylineOn();
    void bucketOn();
    void eyedropperOn();
    void brushOn();
    void smudgeOn();

protected:
    int getMinHeightForWidth(int width) override;

signals:
    void clearButtonClicked();

private:
    void deselectAllTools();
    bool toolOn(ToolType toolType, QToolButton* toolButton);

    Ui::ToolBoxWidget* ui = nullptr;
};

#endif
