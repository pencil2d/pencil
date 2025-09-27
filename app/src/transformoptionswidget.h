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
#ifndef TRANSFORMOPTIONSWIDGET_H
#define TRANSFORMOPTIONSWIDGET_H

#include "basewidget.h"

#include "toolsettings.h"

class TransformTool;
class Editor;
class BaseTool;

namespace Ui {
class TransformOptionsWidget;
}

class TransformOptionsWidget : public BaseWidget
{
    Q_OBJECT

public:
    explicit TransformOptionsWidget(Editor* editor, QWidget *parent = nullptr);
    ~TransformOptionsWidget();

    void initUI() override;
    void updateUI() override;

    void makeConnectionsFromUIToModel();
    void makeConnectionFromModelToUI(TransformTool* transformTool);

private:

    void updatePropertyVisibility();

    void updateToolConnections(BaseTool* tool);
    void setShowSelectionInfo(bool enabled);
    void setAntiAliasingEnabled(bool enabled);

    Ui::TransformOptionsWidget *ui;

    TransformTool* mTransformTool = nullptr;
    Editor* mEditor = nullptr;
};

#endif // TRANSFORMOPTIONSWIDGET_H
