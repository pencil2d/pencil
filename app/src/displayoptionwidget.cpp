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

#include "displayoptionwidget.h"
#include "ui_displayoption.h"

#include <QToolButton>
#include <QGridLayout>
#include <QSettings>

#include "preferencemanager.h"
#include "viewmanager.h"
#include "overlaymanager.h"
#include "layermanager.h"
#include "editor.h"
#include "util.h"
#include "movemode.h"

#include "flowlayout.h"


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
    anglePreferences();
    makeConnections();

    FlowLayout* layout = new FlowLayout;
    layout->addWidget(ui->mirrorButton);
    layout->addWidget(ui->mirrorVButton);
    layout->addWidget(ui->thinLinesButton);
    layout->addWidget(ui->outLinesButton);
    layout->addWidget(ui->overlayCenterButton);
    layout->addWidget(ui->overlayThirdsButton);
    layout->addWidget(ui->overlayGoldenRatioButton);
    layout->addWidget(ui->overlaySafeAreaButton);
    layout->addWidget(ui->overlayPerspective1Button);
    layout->addWidget(ui->overlayPerspective2Button);
    layout->addWidget(ui->overlayPerspective3Button);
    layout->addWidget(ui->overlayChooseAngle);

    delete ui->scrollAreaWidgetContents->layout();
    ui->scrollAreaWidgetContents->setLayout(layout);

    prepareOverlayManager();

#ifdef __APPLE__
    // Mac only style. ToolButtons are naturally borderless on Win/Linux.
    QString stylesheet =
        "QToolButton { border: 0px; } "
        "QToolButton:pressed{ border: 1px solid #FFADAD; border-radius: 2px; background-color: #D5D5D5; }"
        "QToolButton:checked{ border: 1px solid #ADADAD; border-radius: 2px; background-color: #D5D5D5; }";

    ui->mirrorButton->setStyleSheet(stylesheet);
    ui->mirrorVButton->setStyleSheet(stylesheet);
    ui->thinLinesButton->setStyleSheet(stylesheet);
    ui->outLinesButton->setStyleSheet(stylesheet);
    ui->overlayCenterButton->setStyleSheet(stylesheet);
    ui->overlayThirdsButton->setStyleSheet(stylesheet);
    ui->overlayGoldenRatioButton->setStyleSheet(stylesheet);
    ui->overlaySafeAreaButton->setStyleSheet(stylesheet);
    ui->overlayPerspective1Button->setStyleSheet(stylesheet);
    ui->overlayPerspective2Button->setStyleSheet(stylesheet);
    ui->overlayPerspective3Button->setStyleSheet(stylesheet);
    ui->overlayChooseAngle->setStyleSheet(stylesheet);
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
    connect(ui->overlayPerspective1Button, &QToolButton::clicked, this, &DisplayOptionWidget::toggleOverlayPerspective1);
    connect(ui->overlayPerspective2Button, &QToolButton::clicked, this, &DisplayOptionWidget::toggleOverlayPerspective2);
    connect(ui->overlayPerspective3Button, &QToolButton::clicked, this, &DisplayOptionWidget::toggleOverlayPerspective3);
    connect(ui->thinLinesButton, &QToolButton::clicked, this, &DisplayOptionWidget::toggleThinLines);
    connect(ui->outLinesButton, &QToolButton::clicked, this, &DisplayOptionWidget::toggleOutlines);

    connect(editor()->preference(), &PreferenceManager::optionChanged, this, &DisplayOptionWidget::updateUI);
    connect(editor()->view(), &ViewManager::viewFlipped, this, &DisplayOptionWidget::updateUI);
}

void DisplayOptionWidget::prepareOverlayManager()
{
    if (ui->overlayPerspective1Button->isChecked())
    {
        editor()->overlays()->setOnePointPerspectiveEnabled(true);
        editor()->overlays()->updatePerspective(1);
    }
    if (ui->overlayPerspective2Button->isChecked())
    {
        editor()->overlays()->setTwoPointPerspectiveEnabled(true);
        editor()->overlays()->updatePerspective(2);
    }
    if (ui->overlayPerspective3Button->isChecked())
    {
        editor()->overlays()->setThreePointPerspectiveEnabled(true);
        editor()->overlays()->updatePerspective(3);
    }
    editor()->overlays()->updatePerspOverlayActiveList();
}

void DisplayOptionWidget::clearPreviousAngle(int angle)
{
    switch (angle)
    {
    case  2: ui->action2_degrees->setChecked(false); break;
    case  3: ui->action3_degrees->setChecked(false); break;
    case  5: ui->action5_degrees->setChecked(false); break;
    case  7: ui->action7_5_degrees->setChecked(false); break;
    case 10: ui->action10_degrees->setChecked(false); break;
    case 15: ui->action15_degrees->setChecked(false); break;
    case 20: ui->action20_degrees->setChecked(false); break;
    case 30: ui->action30_degrees->setChecked(false); break;
    default: ui->action15_degrees->setChecked(false);
    }
}

void DisplayOptionWidget::changeAngle(int angle)
{
    clearPreviousAngle(mOverlayAngle);
    mOverlayAngle = angle;
    editor()->preference()->set(SETTING::OVERLAY_ANGLE, angle);
    emit editor()->view()->viewChanged();
}

void DisplayOptionWidget::updateUI()
{
    PreferenceManager* prefs = editor()->preference();

    bool canEnableVectorButtons = editor()->layers()->currentLayer()->type() == Layer::VECTOR;
    ui->thinLinesButton->setEnabled(canEnableVectorButtons);
    ui->outLinesButton->setEnabled(canEnableVectorButtons);

    QSignalBlocker b1(ui->thinLinesButton);
    ui->thinLinesButton->setChecked(prefs->isOn(SETTING::INVISIBLE_LINES));

    QSignalBlocker b2(ui->outLinesButton);
    ui->outLinesButton->setChecked(prefs->isOn(SETTING::OUTLINES));

    QSignalBlocker b9(ui->overlayCenterButton);
    ui->overlayCenterButton->setChecked(prefs->isOn(SETTING::OVERLAY_CENTER));

    QSignalBlocker b10(ui->overlayThirdsButton);
    ui->overlayThirdsButton->setChecked(prefs->isOn(SETTING::OVERLAY_THIRDS));

    QSignalBlocker b11(ui->overlayGoldenRatioButton);
    ui->overlayGoldenRatioButton->setChecked(prefs->isOn(SETTING::OVERLAY_GOLDEN));

    QSignalBlocker b12(ui->overlaySafeAreaButton);
    ui->overlaySafeAreaButton->setChecked(prefs->isOn(SETTING::OVERLAY_SAFE));

    QSignalBlocker b13(ui->overlayPerspective1Button);
    ui->overlayPerspective1Button->setChecked(prefs->isOn(SETTING::OVERLAY_PERSPECTIVE1));

    QSignalBlocker b14(ui->overlayPerspective2Button);
    ui->overlayPerspective2Button->setChecked(prefs->isOn(SETTING::OVERLAY_PERSPECTIVE2));

    QSignalBlocker b15(ui->overlayPerspective3Button);
    ui->overlayPerspective3Button->setChecked(prefs->isOn(SETTING::OVERLAY_PERSPECTIVE3));

    bool enableSafeArea = (prefs->isOn(SETTING::ACTION_SAFE_ON) || prefs->isOn(SETTING::TITLE_SAFE_ON));
    ui->overlaySafeAreaButton->setEnabled(enableSafeArea);

    const ViewManager* view = editor()->view();

    QSignalBlocker b3(ui->mirrorButton);
    ui->mirrorButton->setChecked(view->isFlipHorizontal());

    QSignalBlocker b4(ui->mirrorVButton);
    ui->mirrorVButton->setChecked(view->isFlipVertical());
}

int DisplayOptionWidget::getMinHeightForWidth(int width)
{
    return ui->innerWidget->layout()->heightForWidth(width);
}

void DisplayOptionWidget::toggleThinLines(bool isOn)
{
    editor()->preference()->set(SETTING::INVISIBLE_LINES, isOn);
}

void DisplayOptionWidget::toggleOutlines(bool isOn)
{
    editor()->preference()->set(SETTING::OUTLINES, isOn);
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
    editor()->overlays()->setCenterEnabled(isOn);
    editor()->preference()->set(SETTING::OVERLAY_CENTER, isOn);
    emit editor()->view()->viewChanged();
}

void DisplayOptionWidget::toggleOverlayThirds(bool isOn)
{
    editor()->overlays()->setThirdsEnabled(isOn);
    editor()->preference()->set(SETTING::OVERLAY_THIRDS, isOn);
    emit editor()->view()->viewChanged();
}

void DisplayOptionWidget::toggleOverlayGoldenRatio(bool isOn)
{
    editor()->overlays()->setGoldenRatioEnabled(isOn);
    editor()->preference()->set(SETTING::OVERLAY_GOLDEN, isOn);
    emit editor()->view()->viewChanged();
}

void DisplayOptionWidget::toggleOverlaySafeAreas(bool isOn)
{
    editor()->overlays()->setSafeAreasEnabled(isOn);
    editor()->preference()->set(SETTING::OVERLAY_SAFE, isOn);
    emit editor()->view()->viewChanged();
}

void DisplayOptionWidget::toggleOverlayPerspective1(bool isOn)
{
    editor()->overlays()->setOnePointPerspectiveEnabled(isOn);
    editor()->preference()->set(SETTING::OVERLAY_PERSPECTIVE1, isOn);
    emit editor()->view()->viewChanged();
}

void DisplayOptionWidget::toggleOverlayPerspective2(bool isOn)
{
    editor()->overlays()->setTwoPointPerspectiveEnabled(isOn);
    editor()->preference()->set(SETTING::OVERLAY_PERSPECTIVE2, isOn);
    emit editor()->view()->viewChanged();
}

void DisplayOptionWidget::toggleOverlayPerspective3(bool isOn)
{
    editor()->overlays()->setThreePointPerspectiveEnabled(isOn);
    editor()->preference()->set(SETTING::OVERLAY_PERSPECTIVE3, isOn);
    emit editor()->view()->viewChanged();
}

void DisplayOptionWidget::anglePreferences()
{
    buttonStylesheet = "::menu-indicator{ image: none; }"
        "QToolButton { border: 0px; }"
        "QToolButton:pressed { border: 1px solid #ADADAD; border-radius: 2px; background-color: #D5D5D5; }"
        "QToolButton:checked { border: 1px solid #ADADAD; border-radius: 2px; background-color: #D5D5D5; }";

    ui->overlayChooseAngle->setArrowType(Qt::ArrowType::NoArrow);
    ui->overlayChooseAngle->setStyleSheet(buttonStylesheet);

    ui->overlayChooseAngle->addAction(ui->action2_degrees);
    ui->overlayChooseAngle->addAction(ui->action3_degrees);
    ui->overlayChooseAngle->addAction(ui->action5_degrees);
    ui->overlayChooseAngle->addAction(ui->action7_5_degrees);
    ui->overlayChooseAngle->addAction(ui->action10_degrees);
    ui->overlayChooseAngle->addAction(ui->action15_degrees);
    ui->overlayChooseAngle->addAction(ui->action20_degrees);
    ui->overlayChooseAngle->addAction(ui->action30_degrees);

    QSettings settings(PENCIL2D, PENCIL2D);
    mOverlayAngle = settings.value("OverlayAngle").toInt();

    switch (mOverlayAngle)
    {
    case  2:  ui->action2_degrees->setChecked(true); break;
    case  3:  ui->action3_degrees->setChecked(true); break;
    case  5:  ui->action5_degrees->setChecked(true); break;
    case  7:  ui->action7_5_degrees->setChecked(true); break;
    case 10: ui->action10_degrees->setChecked(true); break;
    case 15: ui->action15_degrees->setChecked(true); break;
    case 20: ui->action20_degrees->setChecked(true); break;
    case 30: ui->action30_degrees->setChecked(true); break;
    default: ui->action15_degrees->setChecked(true);
    }

    connect(ui->action2_degrees, &QAction::triggered, this,  &DisplayOptionWidget::angle2degrees);
    connect(ui->action3_degrees, &QAction::triggered, this,  &DisplayOptionWidget::angle3degrees);
    connect(ui->action5_degrees, &QAction::triggered, this,  &DisplayOptionWidget::angle5degrees);
    connect(ui->action7_5_degrees, &QAction::triggered, this,  &DisplayOptionWidget::angle7_5degrees);
    connect(ui->action10_degrees, &QAction::triggered, this, &DisplayOptionWidget::angle10degrees);
    connect(ui->action15_degrees, &QAction::triggered, this, &DisplayOptionWidget::angle15degrees);
    connect(ui->action20_degrees, &QAction::triggered, this, &DisplayOptionWidget::angle20degrees);
    connect(ui->action30_degrees, &QAction::triggered, this, &DisplayOptionWidget::angle30degrees);
}

void DisplayOptionWidget::angle2degrees(bool)
{
    changeAngle(2);
}

void DisplayOptionWidget::angle3degrees(bool)
{
    changeAngle(3);
}

void DisplayOptionWidget::angle5degrees(bool)
{
    changeAngle(5);
}

void DisplayOptionWidget::angle7_5degrees(bool)
{
    changeAngle(7);
}

void DisplayOptionWidget::angle10degrees(bool)
{
    changeAngle(10);
}

void DisplayOptionWidget::angle15degrees(bool)
{
    changeAngle(15);
}

void DisplayOptionWidget::angle20degrees(bool)
{
    changeAngle(20);
}

void DisplayOptionWidget::angle30degrees(bool)
{
    changeAngle(30);
}
