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
#ifndef CAMERAOPTIONSWIDGET_H
#define CAMERAOPTIONSWIDGET_H

#include "basewidget.h"

#include "pencildef.h"
#include "camera.h"

class Editor;
class CameraTool;

namespace Ui {
class CameraOptionsWidget;
}

class CameraOptionsWidget : public BaseWidget
{
    Q_OBJECT
public:
    explicit CameraOptionsWidget(Editor* editor, QWidget *parent = nullptr);
    ~CameraOptionsWidget();

    void initUI() override;
    void updateUI() override;

    void setShowCameraPath(bool showCameraPath);
    void setPathDotColorType(DotColorType index);

    void makeConnectionsFromModelToUI();
    void makeConnectionsFromUIToModel();

private:
    Ui::CameraOptionsWidget *ui;
    Editor* mEditor = nullptr;

    CameraTool* mCameraTool = nullptr;
};

#endif // CAMERAOPTIONSWIDGET_H
