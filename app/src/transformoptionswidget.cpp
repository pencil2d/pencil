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
#include "transformoptionswidget.h"
#include "ui_transformoptionswidget.h"

#include "editor.h"
#include "toolmanager.h"
#include "transformtool.h"

TransformOptionsWidget::TransformOptionsWidget(Editor* editor, QWidget *parent) :
    BaseWidget(parent),
    ui(new Ui::TransformOptionsWidget), mEditor(editor)
{
    ui->setupUi(this);
    initUI();
}

TransformOptionsWidget::~TransformOptionsWidget()
{
    delete ui;
}

void TransformOptionsWidget::initUI()
{
    makeConnectionsFromUIToModel();
}

void TransformOptionsWidget::updateUI()
{
    BaseTool* currentTool = mEditor->tools()->currentTool();
    if (currentTool->category() != TRANSFORMTOOL) { return; }

    updatePropertyVisibility();
    updateToolConnections(currentTool);
    const TransformSettings* selectP = static_cast<const TransformSettings*>(currentTool->settings());

    if (currentTool->isPropertyEnabled(TransformSettings::SHOWSELECTIONINFO_ENABLED)) {
        setShowSelectionInfo(selectP->showSelectionInfoEnabled());
    }

    if (currentTool->isPropertyEnabled(TransformSettings::ANTI_ALIASING_ENABLED)) {
        setAntiAliasingEnabled(selectP->antiAliasingEnabled());
    }
}

void TransformOptionsWidget::updatePropertyVisibility()
{
    BaseTool* currentTool = mEditor->tools()->currentTool();
    if (currentTool->category() != TRANSFORMTOOL) { return; }

    ui->antiAliasingCheckBox->setVisible(currentTool->isPropertyEnabled(TransformSettings::ANTI_ALIASING_ENABLED));
}

void TransformOptionsWidget::updateToolConnections(BaseTool* tool)
{
    if (mTransformTool) {
        disconnect(mTransformTool, nullptr, this, nullptr);
    }

    mTransformTool = static_cast<TransformTool*>(tool);

    makeConnectionFromModelToUI(mTransformTool);
}

void TransformOptionsWidget::makeConnectionsFromUIToModel()
{
    connect(ui->showSelectionInfoCheckBox, &QCheckBox::clicked, this, [=](bool enabled) {
       mTransformTool->setShowSelectionInfo(enabled);
    });

    connect(ui->antiAliasingCheckBox, &QCheckBox::clicked, this, [=](bool enabled) {
       mTransformTool->setAntiAliasingEnabled(enabled);
    });
}

void TransformOptionsWidget::makeConnectionFromModelToUI(TransformTool* transformTool)
{
    connect(transformTool, &TransformTool::showSelectionInfoChanged, this, &TransformOptionsWidget::setShowSelectionInfo);
    connect(transformTool, &TransformTool::antiAliasingChanged, this, &TransformOptionsWidget::setAntiAliasingEnabled);
}

void TransformOptionsWidget::setShowSelectionInfo(bool enabled)
{
    QSignalBlocker b(ui->showSelectionInfoCheckBox);
    ui->showSelectionInfoCheckBox->setChecked(enabled);
}

void TransformOptionsWidget::setAntiAliasingEnabled(bool enabled)
{
    QSignalBlocker b(ui->antiAliasingCheckBox);
    ui->antiAliasingCheckBox->setChecked(enabled);
}
