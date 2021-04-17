#include "bucketoptionswidget.h"
#include "ui_bucketoptionswidget.h"

#include <QDebug>

#include "spinslider.h"
#include "pencilsettings.h"

#include "editor.h"
#include "toolmanager.h"
#include "util.h"

BucketOptionsWidget::BucketOptionsWidget(Editor* editor, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BucketOptionsWidget),
    mEditor(editor)
{
    ui->setupUi(this);

    ui->colorToleranceSlider->init(tr("Color tolerance"), SpinSlider::GROWTH_TYPE::LINEAR, SpinSlider::VALUE_TYPE::INTEGER, 0, MAX_COLOR_TOLERANCE);
    ui->expandSlider->init(tr("Expand fill"), SpinSlider::GROWTH_TYPE::LINEAR, SpinSlider::VALUE_TYPE::INTEGER, 0, MAX_EXPAND);

    QSettings settings(PENCIL2D, PENCIL2D);

    ui->colorToleranceCheckbox->setChecked(settings.value(SETTING_BUCKET_TOLERANCE_ON, true).toBool());
    ui->colorToleranceSpinbox->setMaximum(MAX_COLOR_TOLERANCE);

    ui->expandCheckbox->setChecked(settings.value(SETTING_BUCKET_FILL_EXPAND_ON, true).toBool());
    ui->expandSpinBox->setMaximum(MAX_EXPAND);

    connect(ui->colorToleranceSlider, &SpinSlider::valueChanged, mEditor->tools(), &ToolManager::setTolerance);
    connect(ui->colorToleranceSpinbox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), mEditor->tools(), &ToolManager::setTolerance);
    connect(ui->colorToleranceCheckbox, &QCheckBox::toggled, mEditor->tools(), &ToolManager::setBucketColorToleranceEnabled);

    connect(ui->expandSlider, &SpinSlider::valueChanged, mEditor->tools(), &ToolManager::setBucketFillExpand);
    connect(ui->expandSpinBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), mEditor->tools(), &ToolManager::setBucketFillExpand);
    connect(ui->expandCheckbox, &QCheckBox::toggled, mEditor->tools(), &ToolManager::setBucketFillExpandEnabled);

    connect(mEditor->tools(), &ToolManager::toolPropertyChanged, this, &BucketOptionsWidget::onPropertyChanged);

    ui->expandSlider->setValue(settings.value(SETTING_BUCKET_FILL_EXPAND, 2).toInt());
    ui->expandSpinBox->setValue(settings.value(SETTING_BUCKET_FILL_EXPAND, 2).toInt());
    ui->colorToleranceSlider->setValue(settings.value(SETTING_BUCKET_TOLERANCE, 50).toInt());
    ui->colorToleranceSpinbox->setValue(settings.value(SETTING_BUCKET_TOLERANCE, 50).toInt());

    clearFocusOnFinished(ui->colorToleranceSpinbox);
    clearFocusOnFinished(ui->expandSpinBox);
}

void BucketOptionsWidget::onPropertyChanged(ToolType, ToolPropertyType propertyType)
{
    const Properties& p = mEditor->tools()->currentTool()->properties;
    switch (propertyType)
    {
    case ToolPropertyType::TOLERANCE: {
         setColorTolerance(static_cast<int>(p.tolerance)); break;
    }
    case ToolPropertyType::USETOLERANCE: {
         setColorToleranceEnabled(static_cast<int>(p.toleranceEnabled)); break;
    }
    case ToolPropertyType::BUCKETFILLEXPAND: {
         setFillExpand(static_cast<int>(p.bucketFillExpand)); break;
    }
    case ToolPropertyType::USEBUCKETFILLEXPAND:
        setFillExpandEnabled(static_cast<int>(p.bucketFillExpandEnabled)); break;
    default:
        break;
    }
}


BucketOptionsWidget::~BucketOptionsWidget()
{
    delete ui;
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

void BucketOptionsWidget::setFillExpandEnabled(bool enabled)
{
    QSignalBlocker b(ui->expandCheckbox);
    ui->expandCheckbox->setChecked(enabled);
}

void BucketOptionsWidget::setFillExpand(int fillExpandValue)
{
    QSignalBlocker b(ui->expandSlider);
    ui->expandSlider->setValue(fillExpandValue);

    QSignalBlocker b2(ui->expandSpinBox);
    ui->expandSpinBox->setValue(fillExpandValue);
}
