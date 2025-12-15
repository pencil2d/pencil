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
#include "cameraoptionswidget.h"
#include "ui_cameraoptionswidget.h"

#include "editor.h"
#include "toolmanager.h"
#include "layermanager.h"

#include "cameratool.h"

CameraOptionsWidget::CameraOptionsWidget(Editor* editor, QWidget *parent) :
    BaseWidget(parent),
    ui(new Ui::CameraOptionsWidget), mEditor(editor)
{
    ui->setupUi(this);

    initUI();
}

CameraOptionsWidget::~CameraOptionsWidget()
{
    delete ui;
}

void CameraOptionsWidget::initUI()
{
    auto toolMan = mEditor->tools();
    mCameraTool = static_cast<CameraTool*>(toolMan->getTool(CAMERA));

    makeConnectionsFromUIToModel();
    makeConnectionsFromModelToUI();
}

void CameraOptionsWidget::updateUI()
{
    Q_ASSERT(mCameraTool->type() == CAMERA);

    const CameraSettings* p = static_cast<const CameraSettings*>(mCameraTool->settings());

    setShowCameraPath(p->showPathEnabled());
    setPathDotColorType(p->dotColorType());
}

void CameraOptionsWidget::makeConnectionsFromModelToUI()
{
    connect(mCameraTool, &CameraTool::cameraPathEnabledChanged, this, [=](bool enabled) {
       setShowCameraPath(enabled);
    });

    connect(mCameraTool, &CameraTool::pathColorChanged, this, [=](DotColorType type) {
       setPathDotColorType(type);
    });
}

void CameraOptionsWidget::makeConnectionsFromUIToModel()
{
    connect(ui->showCameraPathCheckBox, &QCheckBox::clicked, [=](bool enabled) {
        mCameraTool->setCameraPathEnabled(enabled);
    });

    connect(ui->pathColorComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=](int value) {
        mCameraTool->setPathDotColorType(static_cast<DotColorType>(value));
    });

    connect(ui->btnResetPath, &QPushButton::clicked, [=]() {
        mCameraTool->performAction(CameraTool::RESET_PATH);
    });

    connect(ui->resetAllButton, &QPushButton::clicked, [=] {
        mCameraTool->performAction(CameraTool::RESET_FIELD);
    });
    connect(ui->resetTranslationButton, &QPushButton::clicked, [=] {
        mCameraTool->performAction(CameraTool::RESET_TRANSLATION);
    });
    connect(ui->resetRotationButton, &QPushButton::clicked, [=] {
        mCameraTool->performAction(CameraTool::RESET_ROTATION);
    });
    connect(ui->resetScaleButton, &QPushButton::clicked, [=] {
        mCameraTool->performAction(CameraTool::RESET_SCALING);
    });
}

void CameraOptionsWidget::setShowCameraPath(bool showCameraPath)
{
    QSignalBlocker b(ui->showCameraPathCheckBox);
    ui->showCameraPathCheckBox->setChecked(showCameraPath);
}

void CameraOptionsWidget::setPathDotColorType(DotColorType index)
{
    QSignalBlocker b(ui->pathColorComboBox);
    ui->pathColorComboBox->setCurrentIndex(static_cast<int>(index));
}
