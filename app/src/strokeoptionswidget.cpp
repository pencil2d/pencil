#include "strokeoptionswidget.h"
#include "ui_strokeoptionswidget.h"

#include "editor.h"
#include "pencilsettings.h"
#include "basetool.h"
#include "stroketool.h"
#include "polylinetool.h"
#include "util.h"

#include "toolmanager.h"
#include "layermanager.h"

StrokeOptionsWidget::StrokeOptionsWidget(Editor* editor, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StrokeOptionsWidget)
{
    ui->setupUi(this);

    mEditor = editor;

    initUI();

    setContentsMargins(0,0,0,0);
}

StrokeOptionsWidget::~StrokeOptionsWidget()
{
    delete ui;
}

void StrokeOptionsWidget::initUI()
{
    QSettings settings(PENCIL2D, PENCIL2D);

    ui->sizeSlider->init(tr("Width"), SpinSlider::EXPONENT, SpinSlider::INTEGER, StrokeTool::WIDTH_MIN, StrokeTool::WIDTH_MAX);
    // ui->sizeSlider->setValue(settings.value("brushWidth", "3").toDouble());
    // ui->sizeSpinBox->setValue(settings.value("brushWidth", "3").toDouble());

    ui->featherSlider->init(tr("Feather"), SpinSlider::LOG, SpinSlider::INTEGER, StrokeTool::FEATHER_MIN, StrokeTool::FEATHER_MAX);
    // ui->featherSlider->setValue(settings.value("brushFeather", "5").toDouble());
    // ui->featherSpinBox->setValue(settings.value("brushFeather", "5").toDouble());

    makeConnectionToEditor(mEditor);
}

void StrokeOptionsWidget::updateUI()
{
    BaseTool* currentTool = mEditor->tools()->currentTool();
    if (currentTool->category() != STROKETOOL) { return; }

    Q_ASSERT(currentTool);

    if (isVisible()) {
        setVisibility(currentTool);
    }

    const StrokeSettings* p = static_cast<const StrokeSettings*>(currentTool->getSettings());

    if (currentTool->isPropertyEnabled(StrokeSettings::WIDTH_VALUE))
    {
        setPenWidth(p->width());
    }
    if (currentTool->isPropertyEnabled(StrokeSettings::FEATHER_VALUE))
    {
        setPenFeather(p->feather());
    }

    if (currentTool->isPropertyEnabled(StrokeSettings::FEATHER_ON)) {
        setUseFeather(p->useFeather());
    }

    if (currentTool->isPropertyEnabled(StrokeSettings::PRESSURE_ON)) {
        setPressure(p->usePressure());
    }

    if (currentTool->isPropertyEnabled(StrokeSettings::INVISIBILITY_ON)) {
        setPenInvisibility(p->invisibility());
    }

    if (currentTool->isPropertyEnabled(StrokeSettings::ANTI_ALIASING_ON)) {
        setAA(p->useAntiAliasing());
    }

    if (currentTool->isPropertyEnabled(StrokeSettings::STABILIZATION_VALUE)) {
        setStabilizerLevel(p->stabilizerLevel());
    }

    if (currentTool->isPropertyEnabled(StrokeSettings::FILLCONTOUR_ON)) {
        setFillContour(p->useFillContour());
    }

    // if (currentTool->isPropertyEnabled(SHOWSELECTIONINFO) && currentTool->type() == SELECT) {
    //     const SelectionProperties* selectP = static_cast<const SelectionProperties*>(currentTool->getProperties());
    //     setShowSelectionInfo(selectP->showSelectionInfo());
    // }

    if (currentTool->type() == POLYLINE) {
        const PolyLineSettings* polyP = static_cast<const PolyLineSettings*>(currentTool->getSettings());
        setClosedPath(polyP->closedPath());
        setBezier(polyP->useBezier());
    }
}

void StrokeOptionsWidget::makeConnectionToEditor(Editor* editor)
{
    auto toolManager = editor->tools();
    // StrokeProperties* properties = static_cast<StrokeProperties*>(toolManager->getTool(STROKETOOL)->getProperties());

    connect(ui->useBezierBox, &QCheckBox::clicked, toolManager, [=](bool enabled) {
        auto tool = static_cast<PolylineTool*>(strokeTool());
        if (tool) {
            tool->setUseBezier(enabled);
        }
    });

    // connect(ui->useClosedPathBox, &QCheckBox::clicked, toolManager, &ToolManager::setClosedPath);
    connect(ui->usePressureBox, &QCheckBox::clicked, toolManager, [=](bool enabled) {
        auto tool = strokeTool();
        if (tool) {
            tool->setPressureON(enabled);
        }
    });

    connect(ui->makeInvisibleBox, &QCheckBox::clicked, toolManager, [=](bool enabled) {
        auto tool = strokeTool();
        if (tool) {
            tool->setInvisibilityON(enabled);
        }
    });

    connect(ui->useFeatherBox, &QCheckBox::clicked, toolManager, [=](bool enabled) {
        auto tool = strokeTool();
        if (tool) {
            tool->setFeatherON(enabled);
        }
    });

    connect(ui->useAABox, &QCheckBox::clicked, toolManager, [=](bool enabled) {
        auto tool = strokeTool();
        if (tool) {
            tool->setAntiAliasingON(enabled);
        }
    });

    connect(ui->fillContourBox, &QCheckBox::clicked, toolManager, [=](bool enabled) {
        auto tool = strokeTool();
        if (tool) {
            tool->setFillContourON(enabled);
        }
    });

    connect(ui->sizeSlider, &SpinSlider::valueChanged, toolManager, [=](qreal value) {
        auto tool = strokeTool();
        if (tool) {
            tool->setWidth(value);
        }
    });

    connect(ui->featherSlider, &SpinSlider::valueChanged, toolManager, [=](qreal value) {
        auto tool = strokeTool();
        if (tool) {
            tool->setFeather(value);
        }
    });

    auto spinboxValueChanged = static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged);
    connect(ui->sizeSpinBox, spinboxValueChanged, toolManager, [=](qreal value) {
        auto tool = strokeTool();
        if (tool) {
            tool->setWidth(value);
        }
    });

    connect(ui->featherSpinBox, spinboxValueChanged, toolManager, [=](qreal value) {
        auto tool = strokeTool();
        if (tool) {
            tool->setFeather(value);
        }
    });

    clearFocusOnFinished(ui->sizeSpinBox);
    clearFocusOnFinished(ui->featherSpinBox);

    connect(ui->inpolLevelsCombo, static_cast<void (QComboBox::*)(int)>(&QComboBox::activated), toolManager, [=](int value) {
        auto tool = strokeTool();
        if (tool) {
            tool->setStablizationLevel(value);
        }
    });

    connect(toolManager, &ToolManager::toolChanged, this, &StrokeOptionsWidget::onToolChanged);
    connect(toolManager, &ToolManager::toolPropertyChanged, this, &StrokeOptionsWidget::onToolPropertyChanged);
}

void StrokeOptionsWidget::onToolPropertyChanged(ToolType, ToolPropertyType ePropertyType)
{
    if (mEditor->tools()->currentTool()->category() != STROKETOOL) {
        return;
    }

    // const Properties& p = editor()->tools()->currentTool()->properties;

    // switch (ePropertyType)
    // {
    // case WIDTH: setPenWidth(p.width()); break;
    // case FEATHER: setPenFeather(p.feather); break;
    // case USEFEATHER: setUseFeather(p.useFeather); break;
    // case PRESSURE: setPressure(p.pressure); break;
    // case INVISIBILITY: setPenInvisibility(p.invisibility); break;
    // case PRESERVEALPHA: setPreserveAlpha(p.preserveAlpha); break;
    // case VECTORMERGE: setVectorMergeEnabled(p.vectorMergeEnabled); break;
    // case ANTI_ALIASING: setAA(p.useAA); break;
    // case STABILIZATION: setStabilizerLevel(p.stabilizerLevel); break;
    // case FILLCONTOUR: setFillContour(p.useFillContour); break;
    // case SHOWSELECTIONINFO: setShowSelectionInfo(p.showSelectionInfo); break;
    // case BEZIER: setBezier(p.bezier_state); break;
    // case CLOSEDPATH: setClosedPath(p.closedPolylinePath); break;
    // case CAMERAPATH: { break; }
    // case TOLERANCE: break;
    // case USETOLERANCE: break;
    // case BUCKETFILLEXPAND: break;
    // case USEBUCKETFILLEXPAND: break;
    // case BUCKETFILLLAYERREFERENCEMODE: break;
    // case FILL_MODE: break;
    // default:
    //     Q_ASSERT(false);
    //     break;
    // }
}

void StrokeOptionsWidget::setVisibility(BaseTool* tool)
{
    ui->sizeSlider->setVisible(tool->isPropertyEnabled(StrokeSettings::WIDTH_VALUE));
    ui->sizeSpinBox->setVisible(tool->isPropertyEnabled(StrokeSettings::WIDTH_VALUE));
    ui->featherSlider->setVisible(tool->isPropertyEnabled(StrokeSettings::FEATHER_VALUE));
    ui->featherSpinBox->setVisible(tool->isPropertyEnabled(StrokeSettings::FEATHER_VALUE));
    ui->useFeatherBox->setVisible(tool->isPropertyEnabled(StrokeSettings::FEATHER_ON));
    ui->usePressureBox->setVisible(tool->isPropertyEnabled(StrokeSettings::PRESSURE_ON));
    ui->makeInvisibleBox->setVisible(tool->isPropertyEnabled(StrokeSettings::INVISIBILITY_ON));
    ui->useAABox->setVisible(tool->isPropertyEnabled(StrokeSettings::INVISIBILITY_ON));
    ui->stabilizerLabel->setVisible(tool->isPropertyEnabled(StrokeSettings::STABILIZATION_VALUE));
    ui->inpolLevelsCombo->setVisible(tool->isPropertyEnabled(StrokeSettings::STABILIZATION_VALUE));
    ui->fillContourBox->setVisible(tool->isPropertyEnabled(StrokeSettings::FILLCONTOUR_ON));
    ui->useBezierBox->setVisible(tool->isPropertyEnabled(PolyLineSettings::BEZIER_ON));
    ui->useClosedPathBox->setVisible(tool->isPropertyEnabled(PolyLineSettings::CLOSEDPATH_ON));
}

void StrokeOptionsWidget::onToolChanged(ToolType)
{
    updateUI();
}

void StrokeOptionsWidget::setPenWidth(qreal width)
{
    QSignalBlocker b(ui->sizeSlider);
    ui->sizeSlider->setEnabled(true);
    ui->sizeSlider->setValue(width);

    QSignalBlocker b2(ui->sizeSpinBox);
    ui->sizeSpinBox->setEnabled(true);
    ui->sizeSpinBox->setValue(width);
}

void StrokeOptionsWidget::setPenFeather(qreal featherValue)
{
    QSignalBlocker b(ui->featherSlider);
    ui->featherSlider->setEnabled(true);
    ui->featherSlider->setValue(featherValue);

    QSignalBlocker b2(ui->featherSpinBox);
    ui->featherSpinBox->setEnabled(true);
    ui->featherSpinBox->setValue(featherValue);
}

void StrokeOptionsWidget::setUseFeather(bool useFeather)
{
    QSignalBlocker b(ui->useFeatherBox);
    ui->useFeatherBox->setEnabled(true);
    ui->useFeatherBox->setChecked(useFeather);
}

void StrokeOptionsWidget::setPenInvisibility(int x)
{
    QSignalBlocker b(ui->makeInvisibleBox);
    ui->makeInvisibleBox->setEnabled(true);
    ui->makeInvisibleBox->setChecked(x > 0);
}

void StrokeOptionsWidget::setPressure(int x)
{
    QSignalBlocker b(ui->usePressureBox);
    ui->usePressureBox->setEnabled(true);
    ui->usePressureBox->setChecked(x > 0);
}

// void StrokeOptionsWidget::setPreserveAlpha(int x)
// {
//     QSignalBlocker b(ui->preserveAlphaBox);
//     ui->preserveAlphaBox->setEnabled(true);
//     ui->preserveAlphaBox->setChecked(x > 0);
// }

// void StrokeOptionsWidget::setVectorMergeEnabled(int x)
// {
//     QSignalBlocker b(ui->vectorMergeBox);
//     ui->vectorMergeBox->setEnabled(true);
//     ui->vectorMergeBox->setChecked(x > 0);
// }

void StrokeOptionsWidget::setAA(int x)
{
    QSignalBlocker b(ui->useAABox);
    ui->useAABox->setEnabled(true);
    ui->useAABox->setVisible(false);

    auto layerType = mEditor->layers()->currentLayer()->type();

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

void StrokeOptionsWidget::setStabilizerLevel(int x)
{
    ui->inpolLevelsCombo->setCurrentIndex(qBound(0, x, ui->inpolLevelsCombo->count() - 1));
}

void StrokeOptionsWidget::setFillContour(int useFill)
{
    QSignalBlocker b(ui->fillContourBox);
    ui->fillContourBox->setEnabled(true);
    ui->fillContourBox->setChecked(useFill > 0);
}

void StrokeOptionsWidget::setBezier(bool useBezier)
{
    QSignalBlocker b(ui->useBezierBox);
    ui->useBezierBox->setChecked(useBezier);
}

void StrokeOptionsWidget::setClosedPath(bool useClosedPath)
{
    QSignalBlocker b(ui->useClosedPathBox);
    ui->useClosedPathBox->setChecked(useClosedPath);
}

// void StrokeOptionsWidget::setShowSelectionInfo(bool showSelectionInfo)
// {
    // QSignalBlocker b(ui->showInfoBox);
    // ui->showInfoBox->setChecked(showSelectionInfo);
// }

void StrokeOptionsWidget::disableAllOptions()
{
    ui->sizeSlider->hide();
    ui->sizeSpinBox->hide();
    ui->featherSlider->hide();
    ui->featherSpinBox->hide();
    ui->useFeatherBox->hide();
    ui->useBezierBox->hide();
    ui->useClosedPathBox->hide();
    ui->usePressureBox->hide();
    ui->makeInvisibleBox->hide();
    ui->useAABox->hide();
    ui->inpolLevelsCombo->hide();
    ui->fillContourBox->hide();
    ui->stabilizerLabel->hide();
}

StrokeTool* StrokeOptionsWidget::strokeTool()
{
    return static_cast<StrokeTool*>(mEditor->tools()->getTool(STROKETOOL));
}
