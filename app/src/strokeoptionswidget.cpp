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
    BaseTool* currentTool = mEditor->tools()->currentTool();
    const StrokeSettings* p = static_cast<const StrokeSettings*>(currentTool->settings());

    auto widthInfo = p->getInfo(StrokeSettings::WIDTH_VALUE);
    ui->sizeSlider->init(tr("Width"), SpinSlider::EXPONENT, widthInfo.getMinReal(), widthInfo.getMaxReal());
    ui->featherSlider->init(tr("Feather"), SpinSlider::LOG, widthInfo.getMinReal(), widthInfo.getMaxReal());

    makeConnectionFromUIToModel();
}

void StrokeOptionsWidget::updateUI()
{
    BaseTool* currentTool = mEditor->tools()->currentTool();
    if (currentTool->category() != STROKETOOL) { return; }

    Q_ASSERT(currentTool);

    updateToolConnections(currentTool);

    setVisibility(currentTool);

    const StrokeSettings* p = static_cast<const StrokeSettings*>(currentTool->settings());

    if (currentTool->isPropertyEnabled(StrokeSettings::WIDTH_VALUE))
    {
        PropertyInfo info = p->getInfo(StrokeSettings::WIDTH_VALUE);
        QSignalBlocker b(ui->sizeSlider);
        ui->sizeSlider->setRange(info.getMinReal(), info.getMaxReal());
        QSignalBlocker b2(ui->sizeSpinBox);
        ui->sizeSpinBox->setRange(info.getMinReal(), info.getMaxReal());

        setPenWidth(p->width());
    }
    if (currentTool->isPropertyEnabled(StrokeSettings::FEATHER_VALUE))
    {
        auto info = p->getInfo(StrokeSettings::FEATHER_VALUE);
        QSignalBlocker b3(ui->featherSlider);
        ui->featherSlider->setRange(info.getMinReal(), info.getMaxReal());
        QSignalBlocker b4(ui->featherSpinBox);
        ui->featherSpinBox->setRange(info.getMinReal(), info.getMaxReal());

        setPenFeather(p->feather());
    }

    if (currentTool->isPropertyEnabled(StrokeSettings::FEATHER_ENABLED)) {
        setFeatherEnabled(p->featherEnabled());
    }

    if (currentTool->isPropertyEnabled(StrokeSettings::PRESSURE_ENABLED)) {
        setPressure(p->pressureEnabled());
    }

    if (currentTool->isPropertyEnabled(StrokeSettings::INVISIBILITY_ENABLED)) {
        setPenInvisibility(p->invisibilityEnabled());
    }

    if (currentTool->isPropertyEnabled(StrokeSettings::ANTI_ALIASING_ENABLED)) {
        setAA(p->AntiAliasingEnabled());
    }

    if (currentTool->isPropertyEnabled(StrokeSettings::STABILIZATION_VALUE)) {
        setStabilizerLevel(p->stabilizerLevel());
    }

    if (currentTool->isPropertyEnabled(StrokeSettings::FILLCONTOUR_ENABLED)) {
        setFillContour(p->fillContourEnabled());
    }

    if (currentTool->type() == POLYLINE) {
        const PolylineSettings* polyP = static_cast<const PolylineSettings*>(currentTool->settings());
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
    connect(strokeTool, &StrokeTool::widthChanged, this, &StrokeOptionsWidget::setPenWidth);
    connect(strokeTool, &StrokeTool::featherChanged, this, &StrokeOptionsWidget::setPenFeather);
    connect(strokeTool, &StrokeTool::featherEnabledChanged, this, &StrokeOptionsWidget::setFeatherEnabled);
    connect(strokeTool, &StrokeTool::pressureEnabledChanged, this, &StrokeOptionsWidget::setPressure);
    connect(strokeTool, &StrokeTool::stabilizationLevelChanged, this, &StrokeOptionsWidget::setStabilizerLevel);
    connect(strokeTool, &StrokeTool::antiAliasingEnabledChanged, this, &StrokeOptionsWidget::setAA);
    connect(strokeTool, &StrokeTool::fillContourEnabledChanged, this, &StrokeOptionsWidget::setFillContour);
    connect(strokeTool, &StrokeTool::InvisibleStrokeEnabledChanged, this, &StrokeOptionsWidget::setPenInvisibility);

    if (strokeTool->type() == POLYLINE) {
        PolylineTool* polyline = static_cast<PolylineTool*>(strokeTool);
        connect(polyline, &PolylineTool::useBezierChanged, this, &StrokeOptionsWidget::setBezierPathEnabled);
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

void StrokeOptionsWidget::setPenWidth(qreal width)
{
    QSignalBlocker b(ui->sizeSlider);
    ui->sizeSlider->setValue(width);

    QSignalBlocker b2(ui->sizeSpinBox);
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

void StrokeOptionsWidget::setFeatherEnabled(bool useFeather)
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
    QSignalBlocker b(ui->inpolLevelsCombo);
    ui->inpolLevelsCombo->setCurrentIndex(qBound(0, x, ui->inpolLevelsCombo->count() - 1));
}

void StrokeOptionsWidget::setFillContour(int useFill)
{
    QSignalBlocker b(ui->fillContourBox);
    ui->fillContourBox->setEnabled(true);
    ui->fillContourBox->setChecked(useFill > 0);
}

void StrokeOptionsWidget::setBezierPathEnabled(bool useBezier)
{
    QSignalBlocker b(ui->useBezierBox);
    ui->useBezierBox->setChecked(useBezier);
}

void StrokeOptionsWidget::setClosedPathEnabled(bool useClosedPath)
{
    QSignalBlocker b(ui->useClosedPathBox);
    ui->useClosedPathBox->setChecked(useClosedPath);
}

StrokeTool* StrokeOptionsWidget::strokeTool()
{
    return static_cast<StrokeTool*>(mEditor->tools()->getTool(STROKETOOL));
}
