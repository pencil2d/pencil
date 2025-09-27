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
#include "tooloptionwidget.h"
#include "ui_tooloptions.h"

#include <QSettings>
#include <QDebug>

#include "cameraoptionswidget.h"
#include "bucketoptionswidget.h"
#include "strokeoptionswidget.h"
#include "transformoptionswidget.h"
#include "spinslider.h"
#include "editor.h"
#include "util.h"
#include "layer.h"
#include "layermanager.h"
#include "stroketool.h"
#include "toolmanager.h"
#include "basewidget.h"


ToolOptionWidget::ToolOptionWidget(QWidget* parent) : BaseDockWidget(parent)
{
    setWindowTitle(tr("Options", "Window title of tool option panel like pen width, feather etc.."));

    QWidget* innerWidget = new QWidget;
    setWidget(innerWidget);
    ui = new Ui::ToolOptions;
    ui->setupUi(innerWidget);
}

ToolOptionWidget::~ToolOptionWidget()
{
    delete ui;
}

void ToolOptionWidget::initUI()
{
    mBucketOptionsWidget = new BucketOptionsWidget(editor(), this);
    mCameraOptionsWidget = new CameraOptionsWidget(editor(), this);

    mBucketOptionsWidget->setHidden(true);
    mCameraOptionsWidget->setHidden(true);
    
    mStrokeOptionsWidget = new StrokeOptionsWidget(editor(), this);
    mTransformOptionsWidget = new TransformOptionsWidget(editor(), this);
    ui->scrollAreaWidgetContents->layout()->addWidget(mBucketOptionsWidget);
    ui->scrollAreaWidgetContents->layout()->addWidget(mCameraOptionsWidget);
    ui->scrollAreaWidgetContents->layout()->addWidget(mStrokeOptionsWidget);
    ui->scrollAreaWidgetContents->layout()->addWidget(mTransformOptionsWidget);
    ui->scrollAreaWidgetContents->layout()->addItem(new QSpacerItem(0, 0, QSizePolicy::MinimumExpanding, QSizePolicy::Expanding));

    makeConnectionToEditor(editor());
    updateUI();
}

void ToolOptionWidget::updateUI()
{
    BaseTool* currentTool = editor()->tools()->currentTool();
    Q_ASSERT(currentTool);

    updateUIForTool(currentTool);
}

void ToolOptionWidget::makeConnectionToEditor(Editor* editor)
{
    connect(editor->tools(), &ToolManager::toolChanged, this, &ToolOptionWidget::onToolChanged);
    connect(editor->tools(), &ToolManager::toolsReset, this, &ToolOptionWidget::onToolsReset);
    connect(editor->layers(), &LayerManager::currentLayerChanged, this, &ToolOptionWidget::onLayerChanged);
}

void ToolOptionWidget::setWidgetVisibility(BaseWidget* widget, bool isVisible)
{
    widget->setVisible(isVisible);

    if (isVisible) {
        widget->updateUI();
    }
}

void ToolOptionWidget::updateUIForTool(BaseTool* tool)
{
    setWidgetVisibility(mBucketOptionsWidget, tool->type() == BUCKET);
    setWidgetVisibility(mCameraOptionsWidget, tool->type() == CAMERA);
    setWidgetVisibility(mStrokeOptionsWidget, tool->category() == STROKETOOL);
    setWidgetVisibility(mTransformOptionsWidget, tool->category() == TRANSFORMTOOL);
}

void ToolOptionWidget::onLayerChanged(int layerIndex)
{
    LayerManager* layerManager = editor()->layers();
    Layer* layer = layerManager->getLayer(layerIndex);
    if (layer->type() != layerManager->currentLayer()->type()) {
        return;
    }

    updateUIForTool(editor()->tools()->currentTool());
}

void ToolOptionWidget::onToolChanged(ToolType toolType)
{
    BaseTool* tool = editor()->tools()->getTool(toolType);
    updateUIForTool(tool);
}

void ToolOptionWidget::onToolsReset()
{
    updateUIForTool(editor()->tools()->currentTool());
}
