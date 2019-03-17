/*

Pencil - Traditional Animation Software
Copyright (C) 2013-2018 Matt Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/

#include "displayoptionwidget.h"
#include "ui_displayoption.h"

#include <QToolButton>
#include <QGridLayout>

#include "preferencemanager.h"
#include "viewmanager.h"
#include "scribblearea.h"
#include "editor.h"
#include "util.h"


DisplayOptionWidget::DisplayOptionWidget(QWidget *parent) :
    BaseDockWidget(parent),
    ui(new Ui::DisplayOption)
{
    ui->setupUi(this);
}

DisplayOptionWidget::~DisplayOptionWidget()
{
    delete ui;
}

void DisplayOptionWidget::initUI()
{
    updateUI();
    makeConnections();

#ifdef __APPLE__
    // Mac only style. ToolButtons are naturally borderless on Win/Linux.
    QString stylesheet =
        "QToolButton { border: 0px; } "
        "QToolButton:pressed{ border: 1px solid #FFADAD; border-radius: 2px; background-color: #D5D5D5; }"
        "QToolButton:checked{ border: 1px solid #ADADAD; border-radius: 2px; background-color: #D5D5D5; }";
    setStyleSheet(this->styleSheet().append(stylesheet));
#endif
}

void DisplayOptionWidget::makeConnections()
{
    connect(ui->onionPrevButton, &QToolButton::clicked, this, &DisplayOptionWidget::onionPrevButtonClicked);
    connect(ui->onionNextButton, &QToolButton::clicked, this, &DisplayOptionWidget::onionNextButtonClicked);
    connect(ui->onionBlueButton, &QToolButton::clicked, this, &DisplayOptionWidget::onionBlueButtonClicked);
    connect(ui->onionRedButton, &QToolButton::clicked, this, &DisplayOptionWidget::onionRedButtonClicked);
    connect(ui->mirrorButton, &QToolButton::clicked, this, &DisplayOptionWidget::toggleMirror);
    connect(ui->mirrorVButton, &QToolButton::clicked, this, &DisplayOptionWidget::toggleMirrorV);

    PreferenceManager* prefs = editor()->preference();
    ScribbleArea* pScriArea = editor()->getScribbleArea();

    connect(ui->thinLinesButton, &QToolButton::clicked, pScriArea, &ScribbleArea::toggleThinLines);
    connect(ui->outLinesButton, &QToolButton::clicked, pScriArea, &ScribbleArea::toggleOutlines);
    connect(prefs, &PreferenceManager::optionChanged, this, &DisplayOptionWidget::updateUI);

    ViewManager* view = editor()->view();
    connect(view, &ViewManager::viewFlipped, this, &DisplayOptionWidget::updateUI);
}

void DisplayOptionWidget::updateUI()
{
    PreferenceManager* prefs = editor()->preference();

    SignalBlocker b(ui->thinLinesButton);
    ui->thinLinesButton->setChecked(prefs->isOn(SETTING::INVISIBLE_LINES));

    SignalBlocker b2(ui->outLinesButton);
    ui->outLinesButton->setChecked(prefs->isOn(SETTING::OUTLINES));

    SignalBlocker b3(ui->onionPrevButton);
    ui->onionPrevButton->setChecked(prefs->isOn(SETTING::PREV_ONION));

    SignalBlocker b4(ui->onionNextButton);
    ui->onionNextButton->setChecked(prefs->isOn(SETTING::NEXT_ONION));

    SignalBlocker b5(ui->onionBlueButton);
    ui->onionBlueButton->setChecked(prefs->isOn(SETTING::ONION_BLUE));

    SignalBlocker b6(ui->onionRedButton);
    ui->onionRedButton->setChecked(prefs->isOn(SETTING::ONION_RED));

    ViewManager* view = editor()->view();

    SignalBlocker b7(ui->mirrorButton);
    ui->mirrorButton->setChecked(view->isFlipHorizontal());

    SignalBlocker b8(ui->mirrorVButton);
    ui->mirrorVButton->setChecked(view->isFlipVertical());
}


void DisplayOptionWidget::onionPrevButtonClicked(bool isOn)
{
    PreferenceManager* prefs = editor()->preference();
    prefs->set(SETTING::PREV_ONION, isOn);
}

void DisplayOptionWidget::onionNextButtonClicked(bool isOn)
{
    PreferenceManager* prefs = editor()->preference();
    prefs->set(SETTING::NEXT_ONION, isOn);
}

void DisplayOptionWidget::onionBlueButtonClicked(bool isOn)
{
    PreferenceManager* prefs = editor()->preference();
    prefs->set(SETTING::ONION_BLUE, isOn);
}

void DisplayOptionWidget::onionRedButtonClicked(bool isOn)
{
    PreferenceManager* prefs = editor()->preference();
    prefs->set(SETTING::ONION_RED, isOn);
}

void DisplayOptionWidget::toggleMirror(bool isOn)
{
    editor()->view()->flipHorizontal(isOn);
}

void DisplayOptionWidget::toggleMirrorV(bool isOn)
{
    editor()->view()->flipVertical(isOn);
}
