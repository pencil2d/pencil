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

    ui->colorToleranceSlider->init(tr("Color tolerance"), SpinSlider::GROWTH_TYPE::LINEAR, SpinSlider::VALUE_TYPE::INTEGER, 0, MAX_COLOR_TOLERANCE);
    ui->expandSlider->init(tr("Expand fill"), SpinSlider::GROWTH_TYPE::LINEAR, SpinSlider::VALUE_TYPE::INTEGER, 0, MAX_EXPAND);
    ui->strokeThicknessSlider->init(tr("Stroke thickness"), SpinSlider::GROWTH_TYPE::LOG, SpinSlider::VALUE_TYPE::FLOAT, 1, MAX_STROKE_THICKNESS);

    QSettings settings(PENCIL2D, PENCIL2D);

    ui->colorToleranceCheckbox->setChecked(settings.value(SETTING_BUCKET_TOLERANCE_ON, true).toBool());
    ui->expandCheckbox->setChecked(settings.value(SETTING_BUCKET_FILL_EXPAND_ON, true).toBool());

    ui->expandSpinBox->setMaximum(MAX_EXPAND);
    ui->strokeThicknessSpinBox->setMaximum(MAX_STROKE_THICKNESS);
    ui->colorToleranceSpinbox->setMaximum(MAX_COLOR_TOLERANCE);
    ui->strokeThicknessSpinBox->setMinimum(1);

    ui->referenceLayerComboBox->addItem(tr("Current layer", "Reference Layer Options"), 0);
    ui->referenceLayerComboBox->addItem(tr("All layers", "Reference Layer Options"), 1);
    ui->referenceLayerComboBox->setToolTip(tr("Refers to the layer that used to flood fill from"));

    ui->blendModeComboBox->addItem(tr("Overlay", "Blend Mode dropdown option"), 0);
    ui->blendModeComboBox->addItem(tr("Replace", "Blend Mode dropdown option"), 1);
    ui->blendModeComboBox->addItem(tr("Behind",  "Blend Mode dropdown option"), 2);
    ui->blendModeComboBox->setToolTip(tr("Defines how the fill will behave when the new color is not opaque"));

    mBucketTool = static_cast<BucketTool*>(mEditor->tools()->getTool(BUCKET));

    connect(ui->colorToleranceSlider, &SpinSlider::valueChanged, mEditor->tools(), [=](int value) {
        mBucketTool->setTolerance(value);
    });
    connect(ui->colorToleranceSpinbox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), mEditor->tools(), [=](int value) {
        mBucketTool->setTolerance(value);
    });

    connect(ui->colorToleranceCheckbox, &QCheckBox::toggled, mEditor->tools(), [=](bool enabled) {
        mBucketTool->setToleranceON(enabled);
    });

    connect(ui->expandSlider, &SpinSlider::valueChanged, mEditor->tools(), [=](int value) {
        mBucketTool->setFillExpand(value);
    });

    connect(ui->expandSpinBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), mEditor->tools(), [=](int value) {
        mBucketTool->setFillExpand(value);
    });

    connect(ui->expandCheckbox, &QCheckBox::toggled, mEditor->tools(), [=](bool enabled) {
        mBucketTool->setFillExpandON(enabled);
    });

    connect(ui->referenceLayerComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), mEditor->tools(), [=](int value) {
        mBucketTool->setFillReferenceMode(value);
    });

    connect(ui->blendModeComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), mEditor->tools(), [=](int value) {
        mBucketTool->setFillMode(value);
    });

    connect(ui->strokeThicknessSlider, &SpinSlider::valueChanged, mEditor->tools(), [=](qreal value) {
        mBucketTool->setStrokeThickness(value);
    });

    connect(ui->strokeThicknessSpinBox, static_cast<void (QDoubleSpinBox::*)(qreal)>(&QDoubleSpinBox::valueChanged), mEditor->tools(), [=](qreal value) {
        mBucketTool->setStrokeThickness(value);
    });

    connect(mEditor->tools(), &ToolManager::toolPropertyChanged, this, &BucketOptionsWidget::onPropertyChanged);
    connect(mEditor->layers(), &LayerManager::currentLayerChanged, this, &BucketOptionsWidget::onLayerChanged);

    const BucketSettings* properties = static_cast<const BucketSettings*>(mEditor->tools()->getTool(BUCKET)->settings());

    ui->expandSlider->setValue(properties->fillExpandAmount());
    ui->expandSpinBox->setValue(properties->fillExpandAmount());
    ui->colorToleranceSlider->setValue(properties->tolerance());
    ui->colorToleranceSpinbox->setValue(properties->tolerance());
    ui->referenceLayerComboBox->setCurrentIndex(properties->fillReferenceMode());
    ui->blendModeComboBox->setCurrentIndex(properties->fillMode());

    clearFocusOnFinished(ui->colorToleranceSpinbox);
    clearFocusOnFinished(ui->expandSpinBox);

    updatePropertyVisibility();
}

void BucketOptionsWidget::updateUI()
{
    if (isVisible()) {
        qDebug() << "update property visibilities";
        updatePropertyVisibility();
    }
}

BucketOptionsWidget::~BucketOptionsWidget()
{
    delete ui;
}

void BucketOptionsWidget::updatePropertyVisibility()
{
    ui->strokeThicknessSlider->setVisible(mBucketTool->isPropertyEnabled(BucketSettings::FILLTHICKNESS_VALUE));
    ui->strokeThicknessSpinBox->setVisible(mBucketTool->isPropertyEnabled(BucketSettings::FILLTHICKNESS_VALUE));
    ui->colorToleranceCheckbox->setVisible(mBucketTool->isPropertyEnabled(BucketSettings::TOLERANCE_ON));
    ui->colorToleranceSlider->setVisible(mBucketTool->isPropertyEnabled(BucketSettings::TOLERANCE_VALUE));
    ui->colorToleranceSpinbox->setVisible(mBucketTool->isPropertyEnabled(BucketSettings::TOLERANCE_VALUE));
    ui->expandCheckbox->setVisible(mBucketTool->isPropertyEnabled(BucketSettings::FILLEXPAND_ON));
    ui->expandSlider->setVisible(mBucketTool->isPropertyEnabled(BucketSettings::FILLEXPAND_VALUE));
    ui->expandSpinBox->setVisible(mBucketTool->isPropertyEnabled(BucketSettings::FILLEXPAND_VALUE));
    ui->referenceLayerComboBox->setVisible(mBucketTool->isPropertyEnabled(BucketSettings::FILLLAYERREFERENCEMODE_VALUE));
    ui->referenceLayerDescLabel->setVisible(mBucketTool->isPropertyEnabled(BucketSettings::FILLLAYERREFERENCEMODE_VALUE));
    ui->blendModeComboBox->setVisible(mBucketTool->isPropertyEnabled(BucketSettings::FILLMODE_VALUE));
    ui->blendModeLabel->setVisible(mBucketTool->isPropertyEnabled(BucketSettings::FILLMODE_VALUE));
}

void BucketOptionsWidget::onPropertyChanged(ToolType, ToolPropertyType propertyType)
{
    // const BucketSettings* p = static_cast<const BucketSettings*>(mEditor->tools()->getTool(BUCKET)->getSettings());
    // switch (propertyType)
    // {
    // case ToolPropertyType::TOLERANCE:
    //      setColorTolerance(p->tolerance()); break;
    // case ToolPropertyType::USETOLERANCE:
    //      setColorToleranceEnabled(p->useTolerance()); break;
    // case ToolPropertyType::WIDTH:
    //      setStrokeWidth(p->fillThickness()); break;
    // case ToolPropertyType::BUCKETFILLEXPAND:
    //      setFillExpand(p->fillExpandAmount()); break;
    // case ToolPropertyType::USEBUCKETFILLEXPAND:
    //     setFillExpandEnabled(p->useFillExpand()); break;
    // case ToolPropertyType::BUCKETFILLLAYERREFERENCEMODE:
    //     setFillReferenceMode(p->fillReferenceMode()); break;
    // case ToolPropertyType::FILL_MODE:
    //     setFillMode(p->fillMode()); break;
    // default:
    //     break;
    // }
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
