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
#include "timecodecontrolwidget.h"

#include "preferencemanager.h"
#include "pencildef.h"
#include "pencilsettings.h"

#include <QLabel>
#include <QToolButton>
#include <QHBoxLayout>
#include <QCheckBox>
#include <QComboBox>
#include <QGroupBox>

TimeCodeControlWidget::TimeCodeControlWidget(TimeCodeControls* controls, QWidget* parent)
    : QWidget(parent, Qt::Popup), mControls(controls)
{
    auto vboxLayout = new QVBoxLayout();
    vboxLayout->setContentsMargins(4,4,4,4);
    setLayout(vboxLayout);

    auto titlelabel = new QLabel(tr("Timecode controls"));

    vboxLayout->addWidget(titlelabel);

    auto showTimeCodeCheckBox = new QCheckBox(this);
    showTimeCodeCheckBox->setText(tr("Show"));
    showTimeCodeCheckBox->setChecked(controls->enabled);

    auto optionsGroupBox = new QGroupBox(this);
    optionsGroupBox->setTitle(tr("Options"));
    optionsGroupBox->setEnabled(controls->enabled);

    auto showFramesCheckBox = new QCheckBox(this);
    showFramesCheckBox->setText(tr("Frames"));
    showFramesCheckBox->setChecked(controls->showFrames);

    auto timecodeComboBox = new QComboBox(this);

    auto groupBoxVBoxLayout = new QVBoxLayout();
    groupBoxVBoxLayout->setContentsMargins(4,4,4,4);
    optionsGroupBox->setLayout(groupBoxVBoxLayout);

    groupBoxVBoxLayout->addWidget(showFramesCheckBox);
    groupBoxVBoxLayout->addWidget(timecodeComboBox);

    timecodeComboBox->addItem(tr("No Timecode"),  timecodeKindToInt(TimecodeKind::NONE));
    timecodeComboBox->addItem(tr("SMPTE Timecode"), timecodeKindToInt(TimecodeKind::SMPTE));
    timecodeComboBox->addItem(tr("SFF Timecode"), timecodeKindToInt(TimecodeKind::SFF));
    timecodeComboBox->setCurrentIndex(timecodeKindToInt(controls->kind));

    vboxLayout->addWidget(showTimeCodeCheckBox);
    vboxLayout->addWidget(optionsGroupBox);

    connect(showTimeCodeCheckBox, &QCheckBox::toggled, this, [this, optionsGroupBox](bool toggled) {
        showTimecode(toggled);
        optionsGroupBox->setEnabled(toggled);
    });

    connect(showFramesCheckBox, &QCheckBox::toggled, this, [this](bool toggled) {
        showFrames(toggled);
    });

    auto comboBoxValueChanged = static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged);
    connect(timecodeComboBox, comboBoxValueChanged, this, [this, controls](int index) {
        auto timecodeKind = controls->timecodeKindFromInt(index);

        setTimecodeTimerKind(timecodeKind);
    });
}

void TimeCodeControlWidget::showFrames(bool shown)
{
    QSettings settings(PENCIL2D, PENCIL2D);
    settings.setValue(SETTING_TIMECODE_FRAMES_ON, shown);

    mControls->showFrames = shown;
    emit timecodeUpdated();
}

int TimeCodeControlWidget::timecodeKindToInt(TimecodeKind kind) const
{
    return static_cast<int>(kind);
}

void TimeCodeControlWidget::showTimecode(bool shown)
{
    QSettings settings(PENCIL2D, PENCIL2D);
    settings.setValue(SETTING_TIMECODE_ON, shown);

    mControls->enabled = shown;
    emit timecodeUpdated();
}

void TimeCodeControlWidget::setTimecodeTimerKind(TimecodeKind kind)
{
    QSettings settings(PENCIL2D, PENCIL2D);
    settings.setValue(SETTING_TIMECODE_KIND, timecodeKindToInt(kind));
    settings.remove(SETTING_TIMECODE_TEXT);
    mControls->kind = kind;

    emit timecodeUpdated();
}
