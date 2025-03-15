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
#include "spinslider.h"
#include "editor.h"
#include "util.h"
#include "layer.h"
#include "layermanager.h"
#include "stroketool.h"
#include "toolmanager.h"


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
    mStrokeOptionsWidget = new StrokeOptionsWidget(editor(), this);
    ui->scrollAreaWidgetContents->layout()->addWidget(mBucketOptionsWidget);
    ui->scrollAreaWidgetContents->layout()->addWidget(mCameraOptionsWidget);
    ui->scrollAreaWidgetContents->layout()->addWidget(mStrokeOptionsWidget);
    ui->scrollAreaWidgetContents->layout()->addItem(new QSpacerItem(0, 0, QSizePolicy::MinimumExpanding, QSizePolicy::Expanding));

    makeConnectionToEditor(editor());
    updateUI();
}

void ToolOptionWidget::updateUI()
{
    BaseTool* currentTool = editor()->tools()->currentTool();
    Q_ASSERT(currentTool);

    setVisibility(currentTool);
}

void ToolOptionWidget::makeConnectionToEditor(Editor* editor)
{
    connect(editor->tools(), &ToolManager::toolChanged, this, &ToolOptionWidget::onToolChanged);
    connect(editor->layers(), &LayerManager::currentLayerChanged, this, &ToolOptionWidget::onLayerChanged);
}

void ToolOptionWidget::setVisibility(BaseTool* tool)
{
    Q_ASSERT(mBucketOptionsWidget);
    Q_ASSERT(mCameraOptionsWidget);

    mBucketOptionsWidget->setVisible(tool->type() == BUCKET);
    mBucketOptionsWidget->updateUI();
    mCameraOptionsWidget->setVisible(tool->type() == CAMERA);
    mCameraOptionsWidget->updateUI();
    mStrokeOptionsWidget->setVisible(tool->category() == STROKETOOL);
    mStrokeOptionsWidget->updateUI();
}

void ToolOptionWidget::onLayerChanged(int layerIndex)
{
    LayerManager* layerManager = editor()->layers();
    Layer* layer = layerManager->getLayer(layerIndex);
    if (layer->type() != layerManager->currentLayer()->type()) {
        return;
    }

    setVisibility(editor()->tools()->currentTool());
}

void ToolOptionWidget::onToolChanged(ToolType toolType)
{
    BaseTool* tool = editor()->tools()->getTool(toolType);
    setVisibility(tool);
}

// void ToolOptionWidget::setPenWidth(qreal width)
// {
//     QSignalBlocker b(ui->sizeSlider);
//     ui->sizeSlider->setEnabled(true);
//     ui->sizeSlider->setValue(width);

//     QSignalBlocker b2(ui->brushSpinBox);
//     ui->brushSpinBox->setEnabled(true);
//     ui->brushSpinBox->setValue(width);
// }

// void ToolOptionWidget::setPenFeather(qreal featherValue)
// {
//     QSignalBlocker b(ui->featherSlider);
//     ui->featherSlider->setEnabled(true);
//     ui->featherSlider->setValue(featherValue);

//     QSignalBlocker b2(ui->featherSpinBox);
//     ui->featherSpinBox->setEnabled(true);
//     ui->featherSpinBox->setValue(featherValue);
// }

// void ToolOptionWidget::setUseFeather(bool useFeather)
// {
//     QSignalBlocker b(ui->useFeatherBox);
//     ui->useFeatherBox->setEnabled(true);
//     ui->useFeatherBox->setChecked(useFeather);
// }

// void ToolOptionWidget::setPenInvisibility(int x)
// {
//     QSignalBlocker b(ui->makeInvisibleBox);
//     ui->makeInvisibleBox->setEnabled(true);
//     ui->makeInvisibleBox->setChecked(x > 0);
// }

// void ToolOptionWidget::setPressure(int x)
// {
//     QSignalBlocker b(ui->usePressureBox);
//     ui->usePressureBox->setEnabled(true);
//     ui->usePressureBox->setChecked(x > 0);
// }

// void ToolOptionWidget::setPreserveAlpha(int x)
// {
//     QSignalBlocker b(ui->preserveAlphaBox);
//     ui->preserveAlphaBox->setEnabled(true);
//     ui->preserveAlphaBox->setChecked(x > 0);
// }

// void ToolOptionWidget::setVectorMergeEnabled(int x)
// {
//     QSignalBlocker b(ui->vectorMergeBox);
//     ui->vectorMergeBox->setEnabled(true);
//     ui->vectorMergeBox->setChecked(x > 0);
// }

// void ToolOptionWidget::setAA(int x)
// {
//     QSignalBlocker b(ui->useAABox);
//     ui->useAABox->setEnabled(true);
//     ui->useAABox->setVisible(false);

//     auto layerType = editor()->layers()->currentLayer()->type();

//     if (layerType == Layer::BITMAP)
//     {
//         if (x == -1)
//         {
//             ui->useAABox->setEnabled(false);
//             ui->useAABox->setVisible(false);
//         }
//         else
//         {
//             ui->useAABox->setVisible(true);
//         }
//         ui->useAABox->setChecked(x > 0);
//     }
// }

// void ToolOptionWidget::setStabilizerLevel(int x)
// {
//     ui->inpolLevelsCombo->setCurrentIndex(qBound(0, x, ui->inpolLevelsCombo->count() - 1));
// }

// void ToolOptionWidget::setFillContour(int useFill)
// {
//     QSignalBlocker b(ui->fillContourBox);
//     ui->fillContourBox->setEnabled(true);
//     ui->fillContourBox->setChecked(useFill > 0);
// }

// void ToolOptionWidget::setBezier(bool useBezier)
// {
//     QSignalBlocker b(ui->useBezierBox);
//     ui->useBezierBox->setChecked(useBezier);
// }

// void ToolOptionWidget::setClosedPath(bool useClosedPath)
// {
//     QSignalBlocker b(ui->useClosedPathBox);
//     ui->useClosedPathBox->setChecked(useClosedPath);
// }

// void ToolOptionWidget::setShowSelectionInfo(bool showSelectionInfo)
// {
//     QSignalBlocker b(ui->showInfoBox);
//     ui->showInfoBox->setChecked(showSelectionInfo);
// }
