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
#include "pencilsettings.h"

#include "layer.h"
#include "editor.h"
#include "layermanager.h"
#include "toolmanager.h"
#include "util.h"

#include "stroketool.h"
#include "buckettool.h"

BucketOptionsWidget::BucketOptionsWidget(Editor* editor, QWidget* parent) :
    QWidget(parent),
    ui(new Ui::BucketOptionsWidget),
    mEditor(editor)
{
    ui->setupUi(this);

    mBucketTool = static_cast<BucketTool*>(mEditor->tools()->getTool(BUCKET));
    mSettings = static_cast<const BucketSettings*>(mBucketTool->settings());

    auto toleranceInfo = mSettings->getInfo(BucketSettings::COLORTOLERANCE_VALUE);
    auto expandInfo = mSettings->getInfo(BucketSettings::FILLEXPAND_VALUE);
    auto thicknessInfo = mSettings->getInfo(BucketSettings::FILLTHICKNESS_VALUE);

    ui->colorToleranceSlider->init(tr("Color tolerance"), SpinSlider::GROWTH_TYPE::LINEAR, toleranceInfo.getMinInt(), toleranceInfo.getMaxInt());
    ui->expandSlider->init(tr("Expand fill"), SpinSlider::GROWTH_TYPE::LINEAR, expandInfo.getMinInt(), expandInfo.getMaxInt());
    ui->strokeThicknessSlider->init(tr("Stroke thickness"), SpinSlider::GROWTH_TYPE::LOG, thicknessInfo.getMinReal(), thicknessInfo.getMaxReal());

    ui->expandSpinBox->setMaximum(expandInfo.getMaxInt());
    ui->strokeThicknessSpinBox->setMaximum(thicknessInfo.getMaxReal());
    ui->strokeThicknessSpinBox->setMinimum(thicknessInfo.getMinReal());
    ui->colorToleranceSpinbox->setMaximum(toleranceInfo.getMaxInt());

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

BucketOptionsWidget::~BucketOptionsWidget()
{
    delete ui;
}

void BucketOptionsWidget::makeConnectionsFromModelToUI()
{
    connect(mBucketTool, &BucketTool::toleranceChanged, this, [=](int value) {
       setColorTolerance(value);
    });

    connect(mBucketTool, &BucketTool::toleranceONChanged, this, [=](bool enabled) {
       setColorToleranceEnabled(enabled);
    });

    connect(mBucketTool, &BucketTool::fillExpandChanged, this, [=](int value) {
       setFillExpand(value);
    });

    connect(mBucketTool, &BucketTool::fillExpandONChanged, this, [=](bool enabled) {
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
        mBucketTool->setTolerance(value);
    });
    connect(ui->colorToleranceSpinbox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=](int value) {
        mBucketTool->setTolerance(value);
    });

    connect(ui->colorToleranceCheckbox, &QCheckBox::toggled, [=](bool enabled) {
        mBucketTool->setToleranceON(enabled);
    });

    connect(ui->expandSlider, &SpinSlider::valueChanged, [=](int value) {
        mBucketTool->setFillExpand(value);
    });

    connect(ui->expandSpinBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=](int value) {
        mBucketTool->setFillExpand(value);
    });

    connect(ui->expandCheckbox, &QCheckBox::toggled, [=](bool enabled) {
        mBucketTool->setFillExpandON(enabled);
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

void BucketOptionsWidget::updateUI()
{
    if (!isVisible()) {
        return;
    }

    updatePropertyVisibility();

    if (mBucketTool->isPropertyEnabled(BucketSettings::FILLTHICKNESS_VALUE)) {
        mBucketTool->setStrokeThickness(mSettings->fillThickness());
    }

    if (mBucketTool->isPropertyEnabled(BucketSettings::FILLEXPAND_ENABLED)) {
        mBucketTool->setFillExpandON(mSettings->fillExpandEnabled());
    }

    if (mBucketTool->isPropertyEnabled(BucketSettings::FILLEXPAND_VALUE)) {
        mBucketTool->setFillExpand(mSettings->fillExpandAmount());
    }

    if (mBucketTool->isPropertyEnabled(BucketSettings::FILLLAYERREFERENCEMODE_VALUE)) {
        mBucketTool->setFillReferenceMode(mSettings->fillReferenceMode());
    }

    if (mBucketTool->isPropertyEnabled(BucketSettings::FILLMODE_VALUE)) {
        mBucketTool->setFillMode(mSettings->fillMode());
    }

    if (mBucketTool->isPropertyEnabled(BucketSettings::COLORTOLERANCE_VALUE)) {
        mBucketTool->setTolerance(mSettings->tolerance());
    }

    if (mBucketTool->isPropertyEnabled(BucketSettings::COLORTOLERANCE_ENABLED)) {
        mBucketTool->setToleranceON(mSettings->toleranceEnabled());
    }
}

void BucketOptionsWidget::updatePropertyVisibility()
{
    ui->strokeThicknessSlider->setVisible(mBucketTool->isPropertyEnabled(BucketSettings::FILLTHICKNESS_VALUE));
    ui->strokeThicknessSpinBox->setVisible(mBucketTool->isPropertyEnabled(BucketSettings::FILLTHICKNESS_VALUE));
    ui->colorToleranceCheckbox->setVisible(mBucketTool->isPropertyEnabled(BucketSettings::COLORTOLERANCE_ENABLED));
    ui->colorToleranceSlider->setVisible(mBucketTool->isPropertyEnabled(BucketSettings::COLORTOLERANCE_VALUE));
    ui->colorToleranceSpinbox->setVisible(mBucketTool->isPropertyEnabled(BucketSettings::COLORTOLERANCE_VALUE));
    ui->expandCheckbox->setVisible(mBucketTool->isPropertyEnabled(BucketSettings::FILLEXPAND_ENABLED));
    ui->expandSlider->setVisible(mBucketTool->isPropertyEnabled(BucketSettings::FILLEXPAND_VALUE));
    ui->expandSpinBox->setVisible(mBucketTool->isPropertyEnabled(BucketSettings::FILLEXPAND_VALUE));
    ui->referenceLayerComboBox->setVisible(mBucketTool->isPropertyEnabled(BucketSettings::FILLLAYERREFERENCEMODE_VALUE));
    ui->referenceLayerDescLabel->setVisible(mBucketTool->isPropertyEnabled(BucketSettings::FILLLAYERREFERENCEMODE_VALUE));
    ui->blendModeComboBox->setVisible(mBucketTool->isPropertyEnabled(BucketSettings::FILLMODE_VALUE));
    ui->blendModeLabel->setVisible(mBucketTool->isPropertyEnabled(BucketSettings::FILLMODE_VALUE));
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
