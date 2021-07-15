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

#include "timelinepage.h"

#include "pencildef.h"
#include "preferencemanager.h"

#include "ui_timelinepage.h"

TimelinePage::TimelinePage()
    : ui(new Ui::TimelinePage)
{
    ui->setupUi(this);

    ui->timelineLength->setMaximum(MaxFramesBound);

    auto spinBoxValueChange = static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged);
    auto sliderChanged = static_cast<void(QSlider::*)(int)>(&QSlider::valueChanged);
    auto comboChanged = static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged);
    connect(ui->timelineLength, spinBoxValueChange, this, &TimelinePage::timelineLengthChanged);
    connect(ui->scrubBox, &QCheckBox::stateChanged, this, &TimelinePage::scrubChanged);
    connect(ui->radioButtonAddNewKey, &QRadioButton::toggled, this, &TimelinePage::drawEmptyKeyRadioButtonToggled);
    connect(ui->radioButtonDuplicate, &QRadioButton::toggled, this, &TimelinePage::drawEmptyKeyRadioButtonToggled);
    connect(ui->radioButtonDrawOnPrev, &QRadioButton::toggled, this, &TimelinePage::drawEmptyKeyRadioButtonToggled);
    connect(ui->flipRollMsecsSlider, sliderChanged, this, &TimelinePage::flipRollMsecSliderChanged);
    connect(ui->flipRollMsecsSpinBox, spinBoxValueChange, this, &TimelinePage::flipRollMsecSpinboxChanged);
    connect(ui->flipRollNumDrawingsSlider, sliderChanged, this, &TimelinePage::flipRollNumDrawingsSliderChanged);
    connect(ui->flipRollNumDrawingsSpinBox, spinBoxValueChange, this, &TimelinePage::flipRollNumDrawingsSpinboxChanged);
    connect(ui->flipInBtwnMsecSlider, sliderChanged, this, &TimelinePage::flipInbetweenMsecSliderChanged);
    connect(ui->flipInBtwnMsecSpinBox, spinBoxValueChange, this, &TimelinePage::flipInbetweenMsecSpinboxChanged);
    connect(ui->soundScrubSlider, sliderChanged, this, &TimelinePage::soundScrubMsecSliderChanged);
    connect(ui->soundScrubSpinBox, spinBoxValueChange, this, &TimelinePage::soundScrubMsecSpinboxChanged);
    connect(ui->layerVisibilityComboBox, comboChanged, this, &TimelinePage::layerVisibilityChanged);
    connect(ui->visibilitySlider, &QSlider::valueChanged, this, &TimelinePage::layerVisibilityThresholdChanged);
    connect(ui->visibilitySpinbox, spinBoxValueChange, this, &TimelinePage::layerVisibilityThresholdChanged);
    ui->visibilitySpinbox->setSuffix("%");
}

TimelinePage::~TimelinePage()
{
    delete ui;
}

void TimelinePage::updateValues()
{
    QSignalBlocker b1(ui->scrubBox);
    ui->scrubBox->setChecked(mManager->isOn(SETTING::SHORT_SCRUB));

    QSignalBlocker b3(ui->timelineLength);
    ui->timelineLength->setValue(mManager->getInt(SETTING::TIMELINE_SIZE));
    if (mManager->getString(SETTING::TIMELINE_SIZE).toInt() <= 0)
        ui->timelineLength->setValue(240);

    QSignalBlocker b4(ui->radioButtonAddNewKey);
    QSignalBlocker b5(ui->radioButtonDuplicate);
    QSignalBlocker b6(ui->radioButtonDrawOnPrev);
    int action = mManager->getInt(SETTING::DRAW_ON_EMPTY_FRAME_ACTION);
    switch (action)
    {
    case CREATE_NEW_KEY:
        ui->radioButtonAddNewKey->setChecked(true);
        break;
    case DUPLICATE_PREVIOUS_KEY:
        ui->radioButtonDuplicate->setChecked(true);
        break;
    case KEEP_DRAWING_ON_PREVIOUS_KEY:
        ui->radioButtonDrawOnPrev->setChecked(true);
        break;
    default:
        break;
    }

    // to secure that you have a relevant minimum setting for sound scrub
    int fps = mManager->getInt(SETTING::FPS);
    int minMsec = 1000 / fps;
    if (minMsec > 100) { minMsec = 100; }
    ui->soundScrubSpinBox->setMinimum(minMsec);
    ui->soundScrubSlider->setMinimum(minMsec);

    ui->flipRollMsecsSlider->setValue(mManager->getInt(SETTING::FLIP_ROLL_MSEC));
    ui->flipRollNumDrawingsSlider->setValue(mManager->getInt(SETTING::FLIP_ROLL_DRAWINGS));
    ui->flipInBtwnMsecSlider->setValue(mManager->getInt(SETTING::FLIP_INBETWEEN_MSEC));
    ui->flipRollMsecsSpinBox->setValue(mManager->getInt(SETTING::FLIP_ROLL_MSEC));
    ui->flipRollNumDrawingsSpinBox->setValue(mManager->getInt(SETTING::FLIP_ROLL_DRAWINGS));
    ui->flipInBtwnMsecSpinBox->setValue(mManager->getInt(SETTING::FLIP_INBETWEEN_MSEC));
    ui->soundScrubSpinBox->setValue(mManager->getInt(SETTING::SOUND_SCRUB_MSEC));
    ui->soundScrubSlider->setValue(mManager->getInt(SETTING::SOUND_SCRUB_MSEC));

    int convertedVisibilityThreshold = static_cast<int>(mManager->getFloat(SETTING::LAYER_VISIBILITY_THRESHOLD)*100);

    ui->visibilitySlider->setValue(convertedVisibilityThreshold);
    ui->visibilitySpinbox->setValue(convertedVisibilityThreshold);

    int visibilityType = mManager->getInt(SETTING::LAYER_VISIBILITY);
    ui->layerVisibilityComboBox->setCurrentIndex(visibilityType);
    layerVisibilityChanged(visibilityType);
}

void TimelinePage::timelineLengthChanged(int value)
{
    mManager->set(SETTING::TIMELINE_SIZE, value);
}

void TimelinePage::fontSizeChanged(int value)
{
    mManager->set(SETTING::LABEL_FONT_SIZE, value);
}

void TimelinePage::scrubChanged(int value)
{
    mManager->set(SETTING::SHORT_SCRUB, value != Qt::Unchecked);
}

void TimelinePage::layerVisibilityChanged(int value)
{
    mManager->set(SETTING::LAYER_VISIBILITY, value);
    ui->visibilitySlider->setEnabled(value == 1);
    ui->visibilitySpinbox->setEnabled(value == 1);
}

void TimelinePage::layerVisibilityThresholdChanged(int value)
{
    float percentage = static_cast<float>(value/100.0f);
    mManager->set(SETTING::LAYER_VISIBILITY_THRESHOLD, percentage);

    QSignalBlocker b8(ui->visibilitySlider);
    ui->visibilitySlider->setValue(value);

    QSignalBlocker b9(ui->visibilitySpinbox);
    ui->visibilitySpinbox->setValue(value);
}

void TimelinePage::drawEmptyKeyRadioButtonToggled(bool)
{
    if (ui->radioButtonAddNewKey->isChecked())
    {
        mManager->set(SETTING::DRAW_ON_EMPTY_FRAME_ACTION, CREATE_NEW_KEY);
    }
    else if (ui->radioButtonDuplicate->isChecked())
    {
        mManager->set(SETTING::DRAW_ON_EMPTY_FRAME_ACTION, DUPLICATE_PREVIOUS_KEY);
    }
    else if (ui->radioButtonDrawOnPrev->isChecked())
    {
        mManager->set(SETTING::DRAW_ON_EMPTY_FRAME_ACTION, KEEP_DRAWING_ON_PREVIOUS_KEY);
    }
}

void TimelinePage::flipRollMsecSliderChanged(int value)
{
    ui->flipRollMsecsSpinBox->setValue(value);
    mManager->set(SETTING::FLIP_ROLL_MSEC, value);
}

void TimelinePage::flipRollMsecSpinboxChanged(int value)
{
    ui->flipRollMsecsSlider->setValue(value);
    mManager->set(SETTING::FLIP_ROLL_MSEC, value);
}

void TimelinePage::flipRollNumDrawingsSliderChanged(int value)
{
    ui->flipRollNumDrawingsSpinBox->setValue(value);
    mManager->set(SETTING::FLIP_ROLL_DRAWINGS, value);
}

void TimelinePage::flipRollNumDrawingsSpinboxChanged(int value)
{
    ui->flipRollNumDrawingsSlider->setValue(value);
    mManager->set(SETTING::FLIP_ROLL_DRAWINGS, value);
}

void TimelinePage::flipInbetweenMsecSliderChanged(int value)
{
    ui->flipInBtwnMsecSpinBox->setValue(value);
    mManager->set(SETTING::FLIP_INBETWEEN_MSEC, value);
}

void TimelinePage::flipInbetweenMsecSpinboxChanged(int value)
{
    ui->flipInBtwnMsecSlider->setValue(value);
    mManager->set(SETTING::FLIP_INBETWEEN_MSEC, value);
}

void TimelinePage::soundScrubActiveChanged(int i)
{
    bool b = true;
    if (i == 0)
        b = false;
    mManager->set(SETTING::SOUND_SCRUB_ACTIVE, b);
    emit soundScrubChanged(b);
}

void TimelinePage::soundScrubMsecSliderChanged(int value)
{
    ui->soundScrubSpinBox->setValue(value);
    mManager->set(SETTING::SOUND_SCRUB_MSEC, value);
    emit soundScrubMsecChanged(value);
}

void TimelinePage::soundScrubMsecSpinboxChanged(int value)
{
    ui->soundScrubSlider->setValue(value);
    mManager->set(SETTING::SOUND_SCRUB_MSEC, value);
    emit soundScrubMsecChanged(value);
}
