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
#include "bucketoptionswidget.h"
#include "ui_bucketoptionswidget.h"

#include <QDebug>

#include "spinslider.h"

#include "layer.h"
#include "editor.h"
#include "layermanager.h"
#include "toolmanager.h"
#include "util.h"

#include "stroketool.h"
#include "buckettool.h"

BucketOptionsWidget::BucketOptionsWidget(Editor* editor, QWidget* parent) :
    BaseWidget(parent),
    ui(new Ui::BucketOptionsWidget),
    mEditor(editor)
{
    ui->setupUi(this);

    initUI();
}

BucketOptionsWidget::~BucketOptionsWidget()
{
    delete ui;
}

void BucketOptionsWidget::initUI()
{
    mBucketTool = static_cast<BucketTool*>(mEditor->tools()->getTool(BUCKET));

    BucketToolProperties properties = mBucketTool->settings();

    auto toleranceInfo = properties.getInfo(BucketToolProperties::COLORTOLERANCE_VALUE);
    auto expandInfo = properties.getInfo(BucketToolProperties::FILLEXPAND_VALUE);
    auto thicknessInfo = properties.getInfo(BucketToolProperties::FILLTHICKNESS_VALUE);

    ui->colorToleranceSlider->init(tr("Color tolerance"), SpinSlider::GROWTH_TYPE::LINEAR, toleranceInfo.minInt(), toleranceInfo.maxInt());
    ui->expandSlider->init(tr("Expand fill"), SpinSlider::GROWTH_TYPE::LINEAR, expandInfo.minInt(), expandInfo.maxInt());
    ui->strokeThicknessSlider->init(tr("Stroke thickness"), SpinSlider::GROWTH_TYPE::LOG, thicknessInfo.minReal(), thicknessInfo.maxReal());

    ui->expandSpinBox->setMaximum(expandInfo.maxInt());
    ui->strokeThicknessSpinBox->setMaximum(thicknessInfo.maxReal());
    ui->strokeThicknessSpinBox->setMinimum(thicknessInfo.minReal());
    ui->colorToleranceSpinbox->setMaximum(toleranceInfo.maxInt());

    ui->referenceLayerComboBox->addItem(tr("Current layer", "Reference Layer Options"), 0);
    ui->referenceLayerComboBox->addItem(tr("All layers", "Reference Layer Options"), 1);
    ui->referenceLayerComboBox->setToolTip(tr("Refers to the layer that used to flood fill from"));

    ui->blendModeComboBox->addItem(tr("Overlay", "Blend Mode dropdown option"), 0);
    ui->blendModeComboBox->addItem(tr("Replace", "Blend Mode dropdown option"), 1);
    ui->blendModeComboBox->addItem(tr("Behind",  "Blend Mode dropdown option"), 2);
    ui->blendModeComboBox->setToolTip(tr("Defines how the fill will behave when the new color is not opaque"));

    makeConnectionsFromUIToModel();
    makeConnectionsFromModelToUI();

    clearFocusOnFinished(ui->colorToleranceSpinbox);
    clearFocusOnFinished(ui->expandSpinBox);

    updatePropertyVisibility();
}

void BucketOptionsWidget::updateUI()
{
    updatePropertyVisibility();

    BucketToolProperties properties = mBucketTool->settings();

    if (mBucketTool->isPropertyEnabled(BucketToolProperties::FILLTHICKNESS_VALUE)) {
        mBucketTool->setStrokeThickness(properties.fillThickness());
    }

    if (mBucketTool->isPropertyEnabled(BucketToolProperties::FILLEXPAND_ENABLED)) {
        mBucketTool->setFillExpandEnabled(properties.fillExpandEnabled());
    }

    if (mBucketTool->isPropertyEnabled(BucketToolProperties::FILLEXPAND_VALUE)) {
        mBucketTool->setFillExpand(properties.fillExpandAmount());
    }

    if (mBucketTool->isPropertyEnabled(BucketToolProperties::FILLLAYERREFERENCEMODE_VALUE)) {
        mBucketTool->setFillReferenceMode(properties.fillReferenceMode());
    }

    if (mBucketTool->isPropertyEnabled(BucketToolProperties::FILLMODE_VALUE)) {
        mBucketTool->setFillMode(properties.fillMode());
    }

    if (mBucketTool->isPropertyEnabled(BucketToolProperties::COLORTOLERANCE_VALUE)) {
        mBucketTool->setColorTolerance(properties.tolerance());
    }

    if (mBucketTool->isPropertyEnabled(BucketToolProperties::COLORTOLERANCE_ENABLED)) {
        mBucketTool->setColorToleranceEnabled(properties.colorToleranceEnabled());
    }
}

void BucketOptionsWidget::makeConnectionsFromModelToUI()
{
    connect(mBucketTool, &BucketTool::toleranceChanged, this, [=](int value) {
       setColorTolerance(value);
    });

    connect(mBucketTool, &BucketTool::toleranceEnabledChanged, this, [=](bool enabled) {
       setColorToleranceEnabled(enabled);
    });

    connect(mBucketTool, &BucketTool::fillExpandChanged, this, [=](int value) {
       setFillExpand(value);
    });

    connect(mBucketTool, &BucketTool::fillExpandEnabledChanged, this, [=](bool enabled) {
       setFillExpandEnabled(enabled);
    });

    connect(mBucketTool, &BucketTool::fillReferenceModeChanged, this, [=](int value) {
       setFillReferenceMode(value);
    });

    connect(mBucketTool, &BucketTool::fillModeChanged, this, [=](int value) {
       setFillMode(value);
    });

    connect(mBucketTool, &BucketTool::strokeThicknessChanged, this, [=](qreal value) {
       setStrokeWidth(value);
    });
}

void BucketOptionsWidget::makeConnectionsFromUIToModel()
{
    connect(ui->colorToleranceSlider, &SpinSlider::valueChanged, [=](int value) {
        mBucketTool->setColorTolerance(value);
    });
    connect(ui->colorToleranceSpinbox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=](int value) {
        mBucketTool->setColorTolerance(value);
    });

    connect(ui->colorToleranceCheckbox, &QCheckBox::toggled, [=](bool enabled) {
        mBucketTool->setColorToleranceEnabled(enabled);
    });

    connect(ui->expandSlider, &SpinSlider::valueChanged, [=](int value) {
        mBucketTool->setFillExpand(value);
    });

    connect(ui->expandSpinBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=](int value) {
        mBucketTool->setFillExpand(value);
    });

    connect(ui->expandCheckbox, &QCheckBox::toggled, [=](bool enabled) {
        mBucketTool->setFillExpandEnabled(enabled);
    });

    connect(ui->referenceLayerComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=](int value) {
        mBucketTool->setFillReferenceMode(value);
    });

    connect(ui->blendModeComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=](int value) {
        mBucketTool->setFillMode(value);
    });

    connect(ui->strokeThicknessSlider, &SpinSlider::valueChanged, [=](qreal value) {
        mBucketTool->setStrokeThickness(value);
    });

    connect(ui->strokeThicknessSpinBox, static_cast<void (QDoubleSpinBox::*)(qreal)>(&QDoubleSpinBox::valueChanged), [=](qreal value) {
        mBucketTool->setStrokeThickness(value);
    });
}

void BucketOptionsWidget::updatePropertyVisibility()
{
    ui->strokeThicknessSlider->setVisible(mBucketTool->isPropertyEnabled(BucketToolProperties::FILLTHICKNESS_VALUE));
    ui->strokeThicknessSpinBox->setVisible(mBucketTool->isPropertyEnabled(BucketToolProperties::FILLTHICKNESS_VALUE));
    ui->colorToleranceCheckbox->setVisible(mBucketTool->isPropertyEnabled(BucketToolProperties::COLORTOLERANCE_ENABLED));
    ui->colorToleranceSlider->setVisible(mBucketTool->isPropertyEnabled(BucketToolProperties::COLORTOLERANCE_VALUE));
    ui->colorToleranceSpinbox->setVisible(mBucketTool->isPropertyEnabled(BucketToolProperties::COLORTOLERANCE_VALUE));
    ui->expandCheckbox->setVisible(mBucketTool->isPropertyEnabled(BucketToolProperties::FILLEXPAND_ENABLED));
    ui->expandSlider->setVisible(mBucketTool->isPropertyEnabled(BucketToolProperties::FILLEXPAND_VALUE));
    ui->expandSpinBox->setVisible(mBucketTool->isPropertyEnabled(BucketToolProperties::FILLEXPAND_VALUE));
    ui->referenceLayerComboBox->setVisible(mBucketTool->isPropertyEnabled(BucketToolProperties::FILLLAYERREFERENCEMODE_VALUE));
    ui->referenceLayerDescLabel->setVisible(mBucketTool->isPropertyEnabled(BucketToolProperties::FILLLAYERREFERENCEMODE_VALUE));
    ui->blendModeComboBox->setVisible(mBucketTool->isPropertyEnabled(BucketToolProperties::FILLMODE_VALUE));
    ui->blendModeLabel->setVisible(mBucketTool->isPropertyEnabled(BucketToolProperties::FILLMODE_VALUE));
}

void BucketOptionsWidget::onLayerChanged(int)
{
    updatePropertyVisibility();
}

void BucketOptionsWidget::setColorTolerance(int tolerance)
{
    QSignalBlocker b(ui->colorToleranceSlider);
    ui->colorToleranceSlider->setValue(tolerance);

    QSignalBlocker b2(ui->colorToleranceSpinbox);
    ui->colorToleranceSpinbox->setValue(tolerance);
}

void BucketOptionsWidget::setColorToleranceEnabled(bool enabled)
{
    QSignalBlocker b(ui->colorToleranceCheckbox);
    ui->colorToleranceCheckbox->setChecked(enabled);
}

void BucketOptionsWidget::setFillMode(int mode)
{
    QSignalBlocker b(ui->blendModeComboBox);
    ui->blendModeComboBox->setCurrentIndex(mode);
}

void BucketOptionsWidget::setFillExpandEnabled(bool enabled)
{
    QSignalBlocker b(ui->expandCheckbox);
    ui->expandCheckbox->setChecked(enabled);
}

void BucketOptionsWidget::setFillExpand(int value)
{
    QSignalBlocker b(ui->expandSlider);
    ui->expandSlider->setValue(value);

    QSignalBlocker b2(ui->expandSpinBox);
    ui->expandSpinBox->setValue(value);
}

void BucketOptionsWidget::setFillReferenceMode(int referenceMode)
{
    QSignalBlocker b(ui->referenceLayerComboBox);
    ui->referenceLayerComboBox->setCurrentIndex(referenceMode);
}

void BucketOptionsWidget::setStrokeWidth(qreal value)
{
    QSignalBlocker b(ui->strokeThicknessSlider);
    ui->strokeThicknessSlider->setValue(value);

    QSignalBlocker b2(ui->strokeThicknessSpinBox);
    ui->strokeThicknessSpinBox->setValue(value);
}
