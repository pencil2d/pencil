#include "strokeoptionswidget.h"
#include "ui_strokeoptionswidget.h"

#include "editor.h"
#include "pencilsettings.h"
#include "basetool.h"
#include "stroketool.h"
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
}

void StrokeOptionsWidget::updateUI()
{
    BaseTool* currentTool = mEditor->tools()->currentTool();
    Q_ASSERT(currentTool);

    setVisibility(currentTool);

    const StrokeSettings* p = static_cast<const StrokeSettings*>(currentTool->getProperties());

    if (currentTool->isPropertyEnabled(WIDTH))
    {
        setPenWidth(p->width());
    }
    if (currentTool->isPropertyEnabled(FEATHER))
    {
        setPenFeather(p->feather());
    }

    if (currentTool->isPropertyEnabled(USEFEATHER)) {
        setUseFeather(p->useFeather());
    }

    if (currentTool->isPropertyEnabled(PRESSURE)) {
        setPressure(p->usePressure());
    }

    if (currentTool->isPropertyEnabled(INVISIBILITY)) {
        setPenInvisibility(p->invisibility());
    }

    if (currentTool->isPropertyEnabled(ANTI_ALIASING)) {
        setAA(p->useAntiAliasing());
    }

    if (currentTool->isPropertyEnabled(STABILIZATION)) {
        setStabilizerLevel(p->stabilizerLevel());
    }

    if (currentTool->isPropertyEnabled(FILLCONTOUR)) {
        setFillContour(p->useFillContour());
    }

    // if (currentTool->isPropertyEnabled(SHOWSELECTIONINFO) && currentTool->type() == SELECT) {
    //     const SelectionProperties* selectP = static_cast<const SelectionProperties*>(currentTool->getProperties());
    //     setShowSelectionInfo(selectP->showSelectionInfo());
    // }

    if (currentTool->isPropertyEnabled(CLOSEDPATH) && currentTool->type() == POLYLINE) {
        const PolyLineSettings* polyP = static_cast<const PolyLineSettings*>(currentTool->getProperties());
        setClosedPath(polyP->closedPath());
    }
}

void StrokeOptionsWidget::makeConnectionToEditor(Editor* editor)
{
    auto toolManager = editor->tools();
    // StrokeProperties* properties = static_cast<StrokeProperties*>(toolManager->getTool(STROKETOOL)->getProperties());

    // connect(ui->useBezierBox, &QCheckBox::clicked, toolManager, [=](bool enabled) {

    //     // if (properties) {
    //     //     properties->
    //     // }
    // });

    // connect(ui->useClosedPathBox, &QCheckBox::clicked, toolManager, &ToolManager::setClosedPath);
    connect(ui->usePressureBox, &QCheckBox::clicked, toolManager, [=](bool enabled) {
        auto tool = strokeTool();
        if (tool) {
            tool->setPressureON(enabled);
        }
    });

    connect(ui->makeInvisibleBox, &QCheckBox::clicked, toolManager, [=](bool enabled) {
        // toolManager->setToolProperty(toolManager->currentTool()->type(), ToolPropertyType::INVISIBILITY, enabled);
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
        // toolManager->setToolProperty(toolManager->currentTool()->type(), ToolPropertyType::WIDTH, value);
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
        // toolManager->setToolProperty(toolManager->currentTool()->type(), ToolPropertyType::WIDTH, value);
        auto tool = strokeTool();
        if (tool) {
            tool->setWidth(value);
        }
    });

    connect(ui->featherSpinBox, spinboxValueChanged, toolManager, [=](qreal value) {
        // toolManager->setToolProperty(toolManager->currentTool()->type(), ToolPropertyType::FEATHER, value);
        auto tool = strokeTool();
        if (tool) {
            tool->setFeather(value);
        }
    });

    clearFocusOnFinished(ui->sizeSpinBox);
    clearFocusOnFinished(ui->featherSpinBox);

    connect(ui->inpolLevelsCombo, static_cast<void (QComboBox::*)(int)>(&QComboBox::activated), toolManager, [=](int value) {
        // toolManager->setToolProperty(toolManager->currentTool()->type(), ToolPropertyType::STABILIZATION, value);
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
    ui->sizeSlider->setVisible(tool->isPropertyEnabled(WIDTH));
    ui->sizeSpinBox->setVisible(tool->isPropertyEnabled(WIDTH));
    ui->featherSlider->setVisible(tool->isPropertyEnabled(FEATHER));
    ui->featherSpinBox->setVisible(tool->isPropertyEnabled(FEATHER));
    ui->useFeatherBox->setVisible(tool->isPropertyEnabled(USEFEATHER));
    ui->useBezierBox->setVisible(tool->isPropertyEnabled(BEZIER));
    ui->useClosedPathBox->setVisible(tool->isPropertyEnabled(CLOSEDPATH));
    ui->usePressureBox->setVisible(tool->isPropertyEnabled(PRESSURE));
    ui->makeInvisibleBox->setVisible(tool->isPropertyEnabled(INVISIBILITY));
    // ui->preserveAlphaBox->setVisible(tool->isPropertyEnabled(PRESERVEALPHA));
    ui->useAABox->setVisible(tool->isPropertyEnabled(ANTI_ALIASING));
    ui->stabilizerLabel->setVisible(tool->isPropertyEnabled(STABILIZATION));
    ui->inpolLevelsCombo->setVisible(tool->isPropertyEnabled(STABILIZATION));
    ui->fillContourBox->setVisible(tool->isPropertyEnabled(FILLCONTOUR));
    // ui->showInfoBox->setVisible(tool->isPropertyEnabled(SHOWSELECTIONINFO));

    auto currentLayerType = mEditor->layers()->currentLayer()->type();
    auto propertyType = mEditor->tools()->currentTool()->type();

    if (currentLayerType == Layer::VECTOR)
    {
        switch (propertyType)
        {
        case SMUDGE:
            ui->sizeSlider->setVisible(false);
            ui->sizeSpinBox->setVisible(false);
            ui->usePressureBox->setVisible(false);
            ui->featherSlider->setVisible(false);
            ui->featherSpinBox->setVisible(false);
            ui->useFeatherBox->setVisible(false);
            break;
        case PENCIL:
            ui->sizeSlider->setVisible(false);
            ui->sizeSpinBox->setVisible(false);
            ui->usePressureBox->setVisible(false);
            break;
        default:
            ui->sizeSlider->setLabel(tr("Width"));
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
            ui->sizeSpinBox->setVisible(false);
            ui->sizeSlider->setVisible(false);
            break;
        case SELECT:
        case MOVE:
            ui->sizeSlider->setVisible(false);
            ui->sizeSpinBox->setVisible(false);
            ui->usePressureBox->setVisible(false);
            ui->featherSlider->setVisible(false);
            ui->featherSpinBox->setVisible(false);
            ui->useFeatherBox->setVisible(false);
            break;
        default:
            ui->makeInvisibleBox->setVisible(false);
            break;
        }
    }
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
