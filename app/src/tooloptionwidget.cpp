/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2013-2018 Matt Chiawen Chang

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

#include "spinslider.h"
#include "editor.h"
#include "util.h"
#include "layer.h"
#include "layermanager.h"
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
}

void ToolOptionWidget::initUI()
{
    QSettings settings(PENCIL2D, PENCIL2D);

    ui->sizeSlider->init(tr("Brush"), SpinSlider::EXPONENT, SpinSlider::INTEGER, 1, 200);
    ui->sizeSlider->setValue(settings.value("brushWidth", "3").toDouble());
    ui->brushSpinBox->setValue(settings.value("brushWidth", "3").toDouble());

    ui->featherSlider->init(tr("Feather"), SpinSlider::LOG, SpinSlider::INTEGER, 2, 200);
    ui->featherSlider->setValue(settings.value("brushFeather", "5").toDouble());
    ui->featherSpinBox->setValue(settings.value("brushFeather", "5").toDouble());

    ui->toleranceSlider->init(tr("Color Tolerance"), SpinSlider::LINEAR, SpinSlider::INTEGER, 0, 100);
    ui->toleranceSlider->setValue(settings.value("Tolerance", "50").toInt());
    ui->toleranceSpinBox->setValue(settings.value("Tolerance", "50").toInt());
}

void ToolOptionWidget::updateUI()
{
    BaseTool* currentTool = editor()->tools()->currentTool();
    Q_ASSERT(currentTool);

    disableAllOptions();
    
    setVisibility(currentTool);

    const Properties& p = currentTool->properties;

    setPenWidth(p.width);
    setPenFeather(p.feather);
    setUseFeather(p.useFeather);
    setPressure(p.pressure);
    setPenInvisibility(p.invisibility);
    setPreserveAlpha(p.preserveAlpha);
    setVectorMergeEnabled(p.vectorMergeEnabled);
    setAA(p.useAA);
    setStabilizerLevel(p.stabilizerLevel);
    setTolerance(static_cast<int>(p.tolerance));
    setFillContour(p.useFillContour);
}

void ToolOptionWidget::createUI()
{} 

void ToolOptionWidget::makeConnectionToEditor(Editor* editor)
{
    auto toolManager = editor->tools();

    connect(ui->useBezierBox, &QCheckBox::clicked, toolManager, &ToolManager::setBezier);
    connect(ui->usePressureBox, &QCheckBox::clicked, toolManager, &ToolManager::setPressure);
    connect(ui->makeInvisibleBox, &QCheckBox::clicked, toolManager, &ToolManager::setInvisibility);
    connect(ui->preserveAlphaBox, &QCheckBox::clicked, toolManager, &ToolManager::setPreserveAlpha);

    connect(ui->sizeSlider, &SpinSlider::valueChanged, toolManager, &ToolManager::setWidth);
    connect(ui->featherSlider, &SpinSlider::valueChanged, toolManager, &ToolManager::setFeather);

    auto spinboxValueChanged = static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged);
    connect(ui->brushSpinBox, spinboxValueChanged, toolManager, &ToolManager::setWidth);
    connect(ui->featherSpinBox, spinboxValueChanged, toolManager, &ToolManager::setFeather);

    connect(ui->useFeatherBox, &QCheckBox::clicked, toolManager, &ToolManager::setUseFeather);

    connect(ui->vectorMergeBox, &QCheckBox::clicked, toolManager, &ToolManager::setVectorMergeEnabled);
    connect(ui->useAABox, &QCheckBox::clicked, toolManager, &ToolManager::setAA);

    connect(ui->inpolLevelsCombo, static_cast<void (QComboBox::*)(int)>(&QComboBox::activated), toolManager, &ToolManager::setStabilizerLevel);

    connect(ui->toleranceSlider, &SpinSlider::valueChanged, toolManager, &ToolManager::setTolerance);
    connect(ui->toleranceSpinBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), toolManager, &ToolManager::setTolerance);

    connect(ui->fillContourBox, &QCheckBox::clicked, toolManager, &ToolManager::setUseFillContour);

    connect(toolManager, &ToolManager::toolChanged, this, &ToolOptionWidget::onToolChanged);
    connect(toolManager, &ToolManager::toolPropertyChanged, this, &ToolOptionWidget::onToolPropertyChanged);
}

void ToolOptionWidget::onToolPropertyChanged(ToolType, ToolPropertyType ePropertyType)
{
    const Properties& p = editor()->tools()->currentTool()->properties;

    switch (ePropertyType)
    {
    case WIDTH: setPenWidth(p.width); break;
    case FEATHER: setPenFeather(p.feather); break;
    case USEFEATHER: setUseFeather(p.useFeather); break;
    case PRESSURE: setPressure(p.pressure); break;
    case INVISIBILITY: setPenInvisibility(p.invisibility); break;
    case PRESERVEALPHA: setPreserveAlpha(p.preserveAlpha); break;
    case VECTORMERGE: setVectorMergeEnabled(p.vectorMergeEnabled); break;
    case ANTI_ALIASING: setAA(p.useAA); break;
    case STABILIZATION: setStabilizerLevel(p.stabilizerLevel); break;
    case TOLERANCE: setTolerance(static_cast<int>(p.tolerance)); break;
    case FILLCONTOUR: setFillContour(p.useFillContour); break;
    case BEZIER: setBezier(p.bezier_state); break;
    default:
        Q_ASSERT(false);
        break;
    }
}

void ToolOptionWidget::setVisibility(BaseTool* tool)
{
    ui->sizeSlider->setVisible(tool->isPropertyEnabled(WIDTH));
    ui->brushSpinBox->setVisible(tool->isPropertyEnabled(WIDTH));
    ui->featherSlider->setVisible(tool->isPropertyEnabled(FEATHER));
    ui->useFeatherBox->setVisible(tool->isPropertyEnabled(FEATHER));
    ui->featherSpinBox->setVisible(tool->isPropertyEnabled(FEATHER));
    ui->useBezierBox->setVisible(tool->isPropertyEnabled(BEZIER));
    ui->usePressureBox->setVisible(tool->isPropertyEnabled(PRESSURE));
    ui->makeInvisibleBox->setVisible(tool->isPropertyEnabled(INVISIBILITY));
    ui->preserveAlphaBox->setVisible(tool->isPropertyEnabled(PRESERVEALPHA));
    ui->useAABox->setVisible(tool->isPropertyEnabled(ANTI_ALIASING));
    ui->stabilizerLabel->setVisible(tool->isPropertyEnabled(STABILIZATION));
    ui->inpolLevelsCombo->setVisible(tool->isPropertyEnabled(STABILIZATION));
    ui->toleranceSlider->setVisible(tool->isPropertyEnabled(TOLERANCE));
    ui->toleranceSpinBox->setVisible(tool->isPropertyEnabled(TOLERANCE));
    ui->fillContourBox->setVisible(tool->isPropertyEnabled(FILLCONTOUR));

    auto currentLayerType = editor()->layers()->currentLayer()->type();
    auto propertyType = editor()->tools()->currentTool()->type();

    if (currentLayerType == Layer::VECTOR)
    {
        switch (propertyType)
        {
        case SMUDGE:
            ui->sizeSlider->setVisible(false);
            ui->brushSpinBox->setVisible(false);
            ui->usePressureBox->setVisible(false);
            ui->featherSlider->setVisible(false);
            ui->featherSpinBox->setVisible(false);
            ui->useFeatherBox->setVisible(false);
            break;
        case PENCIL:
            ui->sizeSlider->setVisible(false);
            ui->brushSpinBox->setVisible(false);
            ui->usePressureBox->setVisible(false);
            break;
        case BUCKET:
            ui->sizeSlider->setLabel(tr("Stroke Thickness"));
            ui->toleranceSlider->setVisible(false);
            ui->toleranceSpinBox->setVisible(false);
            break;
        default:
            ui->sizeSlider->setLabel(tr("Width"));
            ui->toleranceSlider->setVisible(false);
            ui->toleranceSpinBox->setVisible(false);
            ui->useAABox->setVisible(false);
            break;
        }
    }
    else
    {
        switch (propertyType)
        {
        case PENCIL:
            ui->fillContourBox->setVisible(false);
            break;
        case BUCKET:
            ui->brushSpinBox->setVisible(false);
            ui->sizeSlider->setVisible(false);
            break;
        default:
            ui->makeInvisibleBox->setVisible(false);
            break;
        }
    }
}

void ToolOptionWidget::onToolChanged(ToolType)
{
    updateUI();
}

void ToolOptionWidget::setPenWidth(qreal width)
{
    SignalBlocker b(ui->sizeSlider);
    ui->sizeSlider->setEnabled(true);
    ui->sizeSlider->setValue(width);

    SignalBlocker b2(ui->brushSpinBox);
    ui->brushSpinBox->setEnabled(true);
    ui->brushSpinBox->setValue(width);
}

void ToolOptionWidget::setPenFeather(qreal featherValue)
{
    SignalBlocker b(ui->featherSlider);
    ui->featherSlider->setEnabled(true);
    ui->featherSlider->setValue(featherValue);

    SignalBlocker b2(ui->featherSpinBox);
    ui->featherSpinBox->setEnabled(true);
    ui->featherSpinBox->setValue(featherValue);
}

void ToolOptionWidget::setUseFeather(bool useFeather)
{
    SignalBlocker b(ui->useFeatherBox);
    ui->useFeatherBox->setEnabled(true);
    ui->useFeatherBox->setChecked(useFeather);
}

void ToolOptionWidget::setPenInvisibility(int x)
{
    SignalBlocker b(ui->makeInvisibleBox);
    ui->makeInvisibleBox->setEnabled(true);
    ui->makeInvisibleBox->setChecked(x > 0);
}

void ToolOptionWidget::setPressure(int x)
{
    SignalBlocker b(ui->usePressureBox);
    ui->usePressureBox->setEnabled(true);
    ui->usePressureBox->setChecked(x > 0);
}

void ToolOptionWidget::setPreserveAlpha(int x)
{
    SignalBlocker b(ui->preserveAlphaBox);
    ui->preserveAlphaBox->setEnabled(true);
    ui->preserveAlphaBox->setChecked(x > 0);
}

void ToolOptionWidget::setVectorMergeEnabled(int x)
{
    SignalBlocker b(ui->vectorMergeBox);
    ui->vectorMergeBox->setEnabled(true);
    ui->vectorMergeBox->setChecked(x > 0);
}

void ToolOptionWidget::setAA(int x)
{
    SignalBlocker b(ui->useAABox);
    ui->useAABox->setEnabled(true);
    ui->useAABox->setVisible(false);

    auto layerType = editor()->layers()->currentLayer()->type();

    if (layerType == Layer::BITMAP)
    {
        if (x == -1)
        {
            ui->useAABox->setEnabled(false);
            ui->useAABox->setVisible(false);
        }
        else
        {
            ui->useAABox->setVisible(true);
        }
        ui->useAABox->setChecked(x > 0);
    }
}

void ToolOptionWidget::setStabilizerLevel(int x)
{
    ui->inpolLevelsCombo->setCurrentIndex(qBound(0, x, ui->inpolLevelsCombo->count() - 1));
}

void ToolOptionWidget::setTolerance(int tolerance)
{
    SignalBlocker b(ui->toleranceSlider);
    ui->toleranceSlider->setEnabled(true);
    ui->toleranceSlider->setValue(tolerance);

    SignalBlocker b2(ui->toleranceSpinBox);
    ui->toleranceSpinBox->setEnabled(true);
    ui->toleranceSpinBox->setValue(tolerance);
}

void ToolOptionWidget::setFillContour(int useFill)
{
    SignalBlocker b(ui->fillContourBox);
    ui->fillContourBox->setEnabled(true);
    ui->fillContourBox->setChecked(useFill > 0);
}

void ToolOptionWidget::setBezier(bool useBezier)
{
    SignalBlocker b(ui->useBezierBox);
    ui->useBezierBox->setChecked(useBezier);
}

void ToolOptionWidget::disableAllOptions()
{
    ui->sizeSlider->hide();
    ui->brushSpinBox->hide();
    ui->featherSlider->hide();
    ui->featherSpinBox->hide();
    ui->useFeatherBox->hide();
    ui->useBezierBox->hide();
    ui->usePressureBox->hide();
    ui->makeInvisibleBox->hide();
    ui->preserveAlphaBox->hide();
    ui->vectorMergeBox->hide();
    ui->useAABox->hide();
    ui->inpolLevelsCombo->hide();
    ui->toleranceSlider->hide();
    ui->toleranceSpinBox->hide();
    ui->fillContourBox->hide();
}
