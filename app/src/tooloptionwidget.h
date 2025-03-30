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
#ifndef TOOLOPTIONDOCKWIDGET_H
#define TOOLOPTIONDOCKWIDGET_H

#include "basedockwidget.h"
#include "pencildef.h"

namespace Ui
{
    class ToolOptions;
}

class Editor;
class BaseTool;
class BucketOptionsWidget;
class CameraOptionsWidget;
class StrokeOptionsWidget;
class TransformOptionsWidget;

class ToolOptionWidget : public BaseDockWidget
{
    Q_OBJECT
public:
    explicit ToolOptionWidget(QWidget* parent);
    virtual ~ToolOptionWidget() override;

    void initUI() override;
    void updateUI() override;

    void makeConnectionToEditor(Editor* editor);

public slots:
    void onToolsReset();
    void onToolChanged(ToolType);
    void onLayerChanged(int index);

private:
    void setWidgetVisibility(BaseTool*);

private:
    Ui::ToolOptions* ui = nullptr;

    BucketOptionsWidget* mBucketOptionsWidget = nullptr;
    CameraOptionsWidget* mCameraOptionsWidget = nullptr;
    StrokeOptionsWidget* mStrokeOptionsWidget = nullptr;
    TransformOptionsWidget* mTransformOptionsWidget = nullptr;
};

#endif // TOOLOPTIONDOCKWIDGET_H
