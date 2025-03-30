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
    BaseWidget(parent),
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
    StrokeTool* strokeTool = static_cast<StrokeTool*>(mEditor->tools()->getTool(ToolCategory::STROKETOOL));
    const StrokeSettings* p = static_cast<const StrokeSettings*>(strokeTool->settings());

    auto widthInfo = p->getInfo(StrokeSettings::WIDTH_VALUE);
    ui->sizeSlider->init(tr("Width"), SpinSlider::EXPONENT, widthInfo.minReal(), widthInfo.maxReal());

    auto featherInfo = p->getInfo(StrokeSettings::FEATHER_VALUE);
    ui->featherSlider->init(tr("Feather"), SpinSlider::LOG, featherInfo.minReal(), featherInfo.maxReal());

    mCurrentTool = strokeTool;

    makeConnectionFromUIToModel();
}

void StrokeOptionsWidget::updateUI()
{
    StrokeTool* strokeTool = static_cast<StrokeTool*>(mEditor->tools()->getTool(ToolCategory::STROKETOOL));
    if (strokeTool == nullptr) { return; }

    Q_ASSERT(strokeTool);

    updateToolConnections(strokeTool);

    setVisibility(strokeTool);

    const StrokeSettings* p = static_cast<const StrokeSettings*>(strokeTool->settings());

    if (strokeTool->isPropertyEnabled(StrokeSettings::WIDTH_VALUE))
    {
        PropertyInfo info = p->getInfo(StrokeSettings::WIDTH_VALUE);
        QSignalBlocker b(ui->sizeSlider);
        ui->sizeSlider->setRange(info.minReal(), info.maxReal());
        QSignalBlocker b2(ui->sizeSpinBox);
        ui->sizeSpinBox->setRange(info.minReal(), info.maxReal());

        setWidthValue(info.realValue());
    }
    if (strokeTool->isPropertyEnabled(StrokeSettings::FEATHER_VALUE))
    {
        auto info = p->getInfo(StrokeSettings::FEATHER_VALUE);
        QSignalBlocker b3(ui->featherSlider);
        ui->featherSlider->setRange(info.minReal(), info.maxReal());
        QSignalBlocker b4(ui->featherSpinBox);
        ui->featherSpinBox->setRange(info.minReal(), info.maxReal());

        setFeatherValue(info.realValue());
    }

    if (strokeTool->isPropertyEnabled(StrokeSettings::FEATHER_ENABLED)) {
        setFeatherEnabled(p->featherEnabled());
    }

    if (strokeTool->isPropertyEnabled(StrokeSettings::PRESSURE_ENABLED)) {
        setPressureEnabled(p->pressureEnabled());
    }

    if (strokeTool->isPropertyEnabled(StrokeSettings::INVISIBILITY_ENABLED)) {
        setPenInvisibilityEnabled(p->invisibilityEnabled());
    }

    if (strokeTool->isPropertyEnabled(StrokeSettings::ANTI_ALIASING_ENABLED)) {
        setAntiAliasingEnabled(p->AntiAliasingEnabled());
    }

    if (strokeTool->isPropertyEnabled(StrokeSettings::STABILIZATION_VALUE)) {
        setStabilizerLevel(p->stabilizerLevel());
    }

    if (strokeTool->isPropertyEnabled(StrokeSettings::FILLCONTOUR_ENABLED)) {
        setFillContourEnabled(p->fillContourEnabled());
    }

    if (strokeTool->type() == POLYLINE) {
        const PolylineSettings* polyP = static_cast<const PolylineSettings*>(strokeTool->settings());
        setClosedPathEnabled(polyP->closedPathEnabled());
        setBezierPathEnabled(polyP->bezierPathEnabled());
    }
}

void StrokeOptionsWidget::updateToolConnections(BaseTool* tool)
{
    if (mCurrentTool) {
        disconnect(mCurrentTool, nullptr, this, nullptr);
    }

    StrokeTool* strokeTool = static_cast<StrokeTool*>(tool);
    mCurrentTool = strokeTool;

    makeConnectionFromModelToUI(strokeTool);
}

void StrokeOptionsWidget::makeConnectionFromModelToUI(StrokeTool* strokeTool)
{
    connect(strokeTool, &StrokeTool::widthChanged, this, &StrokeOptionsWidget::setWidthValue);
    connect(strokeTool, &StrokeTool::featherChanged, this, &StrokeOptionsWidget::setFeatherValue);
    connect(strokeTool, &StrokeTool::featherEnabledChanged, this, &StrokeOptionsWidget::setFeatherEnabled);
    connect(strokeTool, &StrokeTool::pressureEnabledChanged, this, &StrokeOptionsWidget::setPressureEnabled);
    connect(strokeTool, &StrokeTool::stabilizationLevelChanged, this, &StrokeOptionsWidget::setStabilizerLevel);
    connect(strokeTool, &StrokeTool::antiAliasingEnabledChanged, this, &StrokeOptionsWidget::setAntiAliasingEnabled);
    connect(strokeTool, &StrokeTool::fillContourEnabledChanged, this, &StrokeOptionsWidget::setFillContourEnabled);
    connect(strokeTool, &StrokeTool::InvisibleStrokeEnabledChanged, this, &StrokeOptionsWidget::setPenInvisibilityEnabled);

    if (strokeTool->type() == POLYLINE) {
        PolylineTool* polyline = static_cast<PolylineTool*>(strokeTool);
        connect(polyline, &PolylineTool::bezierPathEnabledChanged, this, &StrokeOptionsWidget::setBezierPathEnabled);
        connect(polyline, &PolylineTool::closePathChanged, this, &StrokeOptionsWidget::setClosedPathEnabled);
    }
}

void StrokeOptionsWidget::makeConnectionFromUIToModel()
{
    auto spinboxValueChanged = static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged);

    connect(ui->useBezierBox, &QCheckBox::clicked, [=](bool enabled) {
        PolylineTool* tool = static_cast<PolylineTool*>(mCurrentTool);
        tool->setUseBezier(enabled);
    });

    connect(ui->useClosedPathBox, &QCheckBox::clicked, [=](bool enabled) {
        PolylineTool* tool = static_cast<PolylineTool*>(mCurrentTool);
        tool->setClosePath(enabled);
    });
    connect(ui->usePressureBox, &QCheckBox::clicked, [=](bool enabled) {
        mCurrentTool->setPressureEnabled(enabled);
    });

    connect(ui->makeInvisibleBox, &QCheckBox::clicked, [=](bool enabled) {
        mCurrentTool->setStrokeInvisibleEnabled(enabled);
    });

    connect(ui->useFeatherBox, &QCheckBox::clicked, [=](bool enabled) {
        mCurrentTool->setFeatherEnabled(enabled);
    });

    connect(ui->useAABox, &QCheckBox::clicked, [=](bool enabled) {
        mCurrentTool->setAntiAliasingEnabled(enabled);
    });

    connect(ui->fillContourBox, &QCheckBox::clicked, [=](bool enabled) {
        mCurrentTool->setFillContourEnabled(enabled);
    });

    connect(ui->sizeSlider, &SpinSlider::valueChanged, [=](qreal value) {
        mCurrentTool->setWidth(value);
    });

    connect(ui->sizeSpinBox, spinboxValueChanged, [=](qreal value) {
        mCurrentTool->setWidth(value);
    });

    connect(ui->featherSlider, &SpinSlider::valueChanged, [=](qreal value) {
        mCurrentTool->setFeather(value);
    });

    connect(ui->featherSpinBox, spinboxValueChanged, [=](qreal value) {
        mCurrentTool->setFeather(value);
    });

    clearFocusOnFinished(ui->sizeSpinBox);
    clearFocusOnFinished(ui->featherSpinBox);

    connect(ui->inpolLevelsCombo, static_cast<void (QComboBox::*)(int)>(&QComboBox::activated), [=](int value) {
        mCurrentTool->setStablizationLevel(value);
    });
}

void StrokeOptionsWidget::setVisibility(BaseTool* tool)
{
    ui->sizeSlider->setVisible(tool->isPropertyEnabled(StrokeSettings::WIDTH_VALUE));
    ui->sizeSpinBox->setVisible(tool->isPropertyEnabled(StrokeSettings::WIDTH_VALUE));
    ui->featherSlider->setVisible(tool->isPropertyEnabled(StrokeSettings::FEATHER_VALUE));
    ui->featherSpinBox->setVisible(tool->isPropertyEnabled(StrokeSettings::FEATHER_VALUE));
    ui->useFeatherBox->setVisible(tool->isPropertyEnabled(StrokeSettings::FEATHER_ENABLED));
    ui->usePressureBox->setVisible(tool->isPropertyEnabled(StrokeSettings::PRESSURE_ENABLED));
    ui->makeInvisibleBox->setVisible(tool->isPropertyEnabled(StrokeSettings::INVISIBILITY_ENABLED));
    ui->useAABox->setVisible(tool->isPropertyEnabled(StrokeSettings::INVISIBILITY_ENABLED));
    ui->stabilizerLabel->setVisible(tool->isPropertyEnabled(StrokeSettings::STABILIZATION_VALUE));
    ui->inpolLevelsCombo->setVisible(tool->isPropertyEnabled(StrokeSettings::STABILIZATION_VALUE));
    ui->fillContourBox->setVisible(tool->isPropertyEnabled(StrokeSettings::FILLCONTOUR_ENABLED));
    ui->useBezierBox->setVisible(tool->isPropertyEnabled(PolylineSettings::BEZIERPATH_ENABLED));
    ui->useClosedPathBox->setVisible(tool->isPropertyEnabled(PolylineSettings::CLOSEDPATH_ENABLED));
}

void StrokeOptionsWidget::setWidthValue(qreal width)
{
    QSignalBlocker b(ui->sizeSlider);
    ui->sizeSlider->setValue(width);

    QSignalBlocker b2(ui->sizeSpinBox);
    ui->sizeSpinBox->setValue(width);
}

void StrokeOptionsWidget::setFeatherValue(qreal featherValue)
{
    QSignalBlocker b(ui->featherSlider);
    ui->featherSlider->setValue(featherValue);

    QSignalBlocker b2(ui->featherSpinBox);
    ui->featherSpinBox->setValue(featherValue);
}

void StrokeOptionsWidget::setFeatherEnabled(bool isOn)
{
    QSignalBlocker b(ui->useFeatherBox);
    ui->useFeatherBox->setChecked(isOn);
}

void StrokeOptionsWidget::setPenInvisibilityEnabled(bool isOn)
{
    QSignalBlocker b(ui->makeInvisibleBox);
    ui->makeInvisibleBox->setChecked(isOn);
}

void StrokeOptionsWidget::setPressureEnabled(bool isOn)
{
    QSignalBlocker b(ui->usePressureBox);
    ui->usePressureBox->setChecked(isOn);
}

void StrokeOptionsWidget::setAntiAliasingEnabled(bool isOn)
{
    QSignalBlocker b(ui->useAABox);
    ui->useAABox->setEnabled(isOn);
}

void StrokeOptionsWidget::setStabilizerLevel(int level)
{
    QSignalBlocker b(ui->inpolLevelsCombo);
    ui->inpolLevelsCombo->setCurrentIndex(level);
}

void StrokeOptionsWidget::setFillContourEnabled(bool isOn)
{
    QSignalBlocker b(ui->fillContourBox);
    ui->fillContourBox->setChecked(isOn);
}

void StrokeOptionsWidget::setBezierPathEnabled(bool isOn)
{
    QSignalBlocker b(ui->useBezierBox);
    ui->useBezierBox->setChecked(isOn);
}

void StrokeOptionsWidget::setClosedPathEnabled(bool isOn)
{
    QSignalBlocker b(ui->useClosedPathBox);
    ui->useClosedPathBox->setChecked(isOn);
}

StrokeTool* StrokeOptionsWidget::strokeTool()
{
    return static_cast<StrokeTool*>(mEditor->tools()->getTool(STROKETOOL));
}
