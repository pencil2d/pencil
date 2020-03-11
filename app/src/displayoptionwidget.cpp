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
    connect(ui->mirrorButton, &QToolButton::clicked, this, &DisplayOptionWidget::toggleMirror);
    connect(ui->mirrorVButton, &QToolButton::clicked, this, &DisplayOptionWidget::toggleMirrorV);
    connect(ui->overlayCenterButton, &QToolButton::clicked, this, &DisplayOptionWidget::toggleOverlayCenter);
    connect(ui->overlayThirdsButton, &QToolButton::clicked, this, &DisplayOptionWidget::toggleOverlayThirds);
    connect(ui->overlayGoldenRatioButton, &QToolButton::clicked, this, &DisplayOptionWidget::toggleOverlayGoldenRatio);
    connect(ui->overlaySafeAreaButton, &QToolButton::clicked, this, &DisplayOptionWidget::toggleOverlaySafeAreas);

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

    SignalBlocker b1(ui->thinLinesButton);
    ui->thinLinesButton->setChecked(prefs->isOn(SETTING::INVISIBLE_LINES));

    SignalBlocker b2(ui->outLinesButton);
    ui->outLinesButton->setChecked(prefs->isOn(SETTING::OUTLINES));

    SignalBlocker b9(ui->overlayCenterButton);
    ui->overlayCenterButton->setChecked(prefs->isOn(SETTING::OVERLAY_CENTER));

    SignalBlocker b10(ui->overlayThirdsButton);
    ui->overlayThirdsButton->setChecked(prefs->isOn(SETTING::OVERLAY_THIRDS));

    SignalBlocker b11(ui->overlayGoldenRatioButton);
    ui->overlayGoldenRatioButton->setChecked(prefs->isOn(SETTING::OVERLAY_GOLDEN));

    SignalBlocker b12(ui->overlaySafeAreaButton);
    ui->overlaySafeAreaButton->setChecked(prefs->isOn(SETTING::OVERLAY_SAFE));

    if (prefs->isOn(SETTING::ACTION_SAFE_ON) || prefs->isOn(SETTING::TITLE_SAFE_ON))
    {
        ui->overlaySafeAreaButton->setEnabled(true);
    } else {
        ui->overlaySafeAreaButton->setEnabled(false);
    }

    ViewManager* view = editor()->view();

    SignalBlocker b3(ui->mirrorButton);
    ui->mirrorButton->setChecked(view->isFlipHorizontal());

    SignalBlocker b4(ui->mirrorVButton);
    ui->mirrorVButton->setChecked(view->isFlipVertical());
}

void DisplayOptionWidget::toggleMirror(bool isOn)
{
    editor()->view()->flipHorizontal(isOn);
}

void DisplayOptionWidget::toggleMirrorV(bool isOn)
{
    editor()->view()->flipVertical(isOn);
}

void DisplayOptionWidget::toggleOverlayCenter(bool isOn)
{
    editor()->view()->setOverlayCenter(isOn);
    editor()->preference()->set(SETTING::OVERLAY_CENTER, isOn);
}

void DisplayOptionWidget::toggleOverlayThirds(bool isOn)
{
    editor()->view()->setOverlayThirds(isOn);
    editor()->preference()->set(SETTING::OVERLAY_THIRDS, isOn);
}

void DisplayOptionWidget::toggleOverlayGoldenRatio(bool isOn)
{
    editor()->view()->setOverlayGoldenRatio(isOn);
    editor()->preference()->set(SETTING::OVERLAY_GOLDEN, isOn);
}

void DisplayOptionWidget::toggleOverlaySafeAreas(bool isOn)
{
    editor()->view()->setOverlaySafeAreas(isOn);
    editor()->preference()->set(SETTING::OVERLAY_SAFE, isOn);
}
