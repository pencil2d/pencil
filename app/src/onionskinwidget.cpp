/*

Pencil2D - Traditional Animation Software
Copyright (C) 2012-2020 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/

#include "onionskinwidget.h"
#include "ui_onionskin.h"

#include "preferencemanager.h"
#include "editor.h"
#include "flowlayout.h"
#include "util.h"

OnionSkinWidget::OnionSkinWidget(QWidget *parent) :
    BaseDockWidget(parent),
    ui(new Ui::OnionSkin)
{
    ui->setupUi(this);

    clearFocusOnFinished(ui->onionPrevFramesNumBox);
    clearFocusOnFinished(ui->onionNextFramesNumBox);
    clearFocusOnFinished(ui->onionMinOpacityBox);
    clearFocusOnFinished(ui->onionMaxOpacityBox);
}

OnionSkinWidget::~OnionSkinWidget()
{
    delete ui;
}

void OnionSkinWidget::initUI()
{
    updateUI();
    makeConnections();

    // Change the horizontal layout in the Distributed Opacity group box to a
    // flow layout to reduce the minimum width
    FlowLayout *opacityLayout = new FlowLayout;
    opacityLayout->setAlignment(Qt::AlignHCenter);
    opacityLayout->setContentsMargins(0, 6, 0, 6);
    ui->opacityGroup->layout()->removeWidget(ui->minOpacityGroup);
    ui->opacityGroup->layout()->removeWidget(ui->maxOpacityGroup);
    opacityLayout->addWidget(ui->minOpacityGroup);
    opacityLayout->addWidget(ui->maxOpacityGroup);
    delete ui->opacityGroup->layout();
    ui->opacityGroup->setLayout(opacityLayout);

#ifdef __APPLE__
    // Mac only style. ToolButtons are naturally borderless on Win/Linux.
    QString stylesheet =
        "QToolButton { border: 0px; } "
        "QToolButton:pressed{ border: 1px solid #FFADAD; border-radius: 2px; background-color: #D5D5D5; }"
        "QToolButton:checked{ border: 1px solid #ADADAD; border-radius: 2px; background-color: #D5D5D5; }";
    setStyleSheet(this->styleSheet().append(stylesheet));
#endif
}

void OnionSkinWidget::makeConnections()
{
    auto spinBoxChanged = static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged);
    connect(ui->onionMaxOpacityBox, spinBoxChanged, this, &OnionSkinWidget::onionMaxOpacityChange);
    connect(ui->onionMinOpacityBox, spinBoxChanged, this, &OnionSkinWidget::onionMinOpacityChange);
    connect(ui->onionPrevFramesNumBox, spinBoxChanged, this, &OnionSkinWidget::onionPrevFramesNumChange);
    connect(ui->onionNextFramesNumBox, spinBoxChanged, this, &OnionSkinWidget::onionNextFramesNumChange);

    connect(ui->onionPrevButton, &QToolButton::clicked, this, &OnionSkinWidget::onionPrevButtonClicked);
    connect(ui->onionNextButton, &QToolButton::clicked, this, &OnionSkinWidget::onionNextButtonClicked);
    connect(ui->onionBlueButton, &QToolButton::clicked, this, &OnionSkinWidget::onionBlueButtonClicked);
    connect(ui->onionRedButton, &QToolButton::clicked, this, &OnionSkinWidget::onionRedButtonClicked);

    connect(ui->onionSkinMode, &QCheckBox::stateChanged, this, &OnionSkinWidget::onionSkinModeChange);
    connect(ui->onionWhilePlayback, &QCheckBox::stateChanged, this, &OnionSkinWidget::playbackStateChanged);

    PreferenceManager* prefs = editor()->preference();
    connect(prefs, &PreferenceManager::optionChanged, this, &OnionSkinWidget::updateUI);

}

void OnionSkinWidget::updateUI()
{
    PreferenceManager* prefs = editor()->preference();

    QSignalBlocker b1(ui->onionPrevButton);
    ui->onionPrevButton->setChecked(prefs->isOn(SETTING::PREV_ONION));

    QSignalBlocker b2(ui->onionNextButton);
    ui->onionNextButton->setChecked(prefs->isOn(SETTING::NEXT_ONION));

    QSignalBlocker b3(ui->onionBlueButton);
    ui->onionBlueButton->setChecked(prefs->isOn(SETTING::ONION_BLUE));

    ui->onionRedButton->setEnabled(ui->onionPrevButton->isChecked());
    ui->onionBlueButton->setEnabled(ui->onionNextButton->isChecked());

    QSignalBlocker b4(ui->onionRedButton);
    ui->onionRedButton->setChecked(prefs->isOn(SETTING::ONION_RED));

    ui->onionMaxOpacityBox->setValue(prefs->getInt(SETTING::ONION_MAX_OPACITY));
    ui->onionMinOpacityBox->setValue(prefs->getInt(SETTING::ONION_MIN_OPACITY));
    ui->onionPrevFramesNumBox->setValue(prefs->getInt(SETTING::ONION_PREV_FRAMES_NUM));
    ui->onionNextFramesNumBox->setValue(prefs->getInt(SETTING::ONION_NEXT_FRAMES_NUM));

    QSignalBlocker b5(ui->onionSkinMode);
    ui->onionSkinMode->setChecked(prefs->getString(SETTING::ONION_TYPE) == "absolute");

    QSignalBlocker b6(ui->onionWhilePlayback);
    ui->onionWhilePlayback->setChecked(prefs->getInt(SETTING::ONION_WHILE_PLAYBACK));

}

void OnionSkinWidget::onionPrevButtonClicked(bool isOn)
{
    PreferenceManager* prefs = editor()->preference();
    prefs->set(SETTING::PREV_ONION, isOn);
}

void OnionSkinWidget::onionNextButtonClicked(bool isOn)
{
    PreferenceManager* prefs = editor()->preference();
    prefs->set(SETTING::NEXT_ONION, isOn);
}

void OnionSkinWidget::onionRedButtonClicked(bool isOn)
{
    PreferenceManager* prefs = editor()->preference();
    prefs->set(SETTING::ONION_RED, isOn);
}

void OnionSkinWidget::onionBlueButtonClicked(bool isOn)
{
    PreferenceManager* prefs = editor()->preference();
    prefs->set(SETTING::ONION_BLUE, isOn);
}

void OnionSkinWidget::onionMaxOpacityChange(int value)
{
    PreferenceManager* prefs = editor()->preference();
    prefs->set(SETTING::ONION_MAX_OPACITY, value);
}

void OnionSkinWidget::onionMinOpacityChange(int value)
{
    PreferenceManager* prefs = editor()->preference();
    prefs->set(SETTING::ONION_MIN_OPACITY, value);
}

void OnionSkinWidget::onionPrevFramesNumChange(int value)
{
    PreferenceManager* prefs = editor()->preference();
    prefs->set(SETTING::ONION_PREV_FRAMES_NUM, value);
}

void OnionSkinWidget::onionNextFramesNumChange(int value)
{
    PreferenceManager* prefs = editor()->preference();
    prefs->set(SETTING::ONION_NEXT_FRAMES_NUM, value);
}

void OnionSkinWidget::onionSkinModeChange(int value)
{
    PreferenceManager* prefs = editor()->preference();
    if (value == Qt::Checked)
    {
        prefs->set(SETTING::ONION_TYPE, QString("absolute"));
    }
    else
    {
        prefs->set(SETTING::ONION_TYPE, QString("relative"));
    }
}

void OnionSkinWidget::playbackStateChanged(int value)
{
    PreferenceManager* prefs = editor()->preference();
    prefs->set(SETTING::ONION_WHILE_PLAYBACK, value);
}
