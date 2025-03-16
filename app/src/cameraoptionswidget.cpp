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
    QWidget(parent),
    ui(new Ui::CameraOptionsWidget), mEditor(editor)
{
    ui->setupUi(this);

    auto toolMan = mEditor->tools();
    mCameraTool = static_cast<CameraTool*>(toolMan->getTool(CAMERA));

    connect(ui->showCameraPathCheckBox, &QCheckBox::clicked, toolMan, [=](bool enabled) {
        mCameraTool->setCameraPathON(enabled);
    });

    // TODO: should this be a tool property or an action with data?
    connect(ui->pathColorComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), toolMan, [=](int value) {
        mCameraTool->setPathDotColorType(static_cast<DotColorType>(value));
    });

    connect(ui->btnResetPath, &QPushButton::clicked, toolMan, [=]() {
        mCameraTool->performAction(CAMERA_PATH_RESET);
    });

    connect(ui->resetAllButton, &QPushButton::clicked, toolMan, [=] {
        mCameraTool->performAction(CAMERA_RESET_FIELD);
    });
    connect(ui->resetTranslationButton, &QPushButton::clicked, toolMan, [=] {
        mCameraTool->performAction(CAMERA_RESET_TRANSLATION);
    });
    connect(ui->resetRotationButton, &QPushButton::clicked, toolMan, [=] {
        mCameraTool->performAction(CAMERA_RESET_ROTATION);
    });
    connect(ui->resetScaleButton, &QPushButton::clicked, toolMan, [=] {
        mCameraTool->performAction(CAMERA_RESET_SCALING);
    });

    connect(toolMan, &ToolManager::toolPropertyChanged, this, &CameraOptionsWidget::onToolPropertyChanged);

    connect(mEditor->layers(), &LayerManager::currentLayerChanged, this, &CameraOptionsWidget::updateUI);
    connect(mEditor->tools(), &ToolManager::toolChanged, this, &CameraOptionsWidget::updateUI);
}

CameraOptionsWidget::~CameraOptionsWidget()
{
    delete ui;
}

void CameraOptionsWidget::updateUI()
{

    Q_ASSERT(mCameraTool->type() == CAMERA);

    const CameraSettings* p = static_cast<const CameraSettings*>(mCameraTool->settings());

    setShowCameraPath(p->showPath());
    setPathDotColorType(p->dotColorType());
}

void CameraOptionsWidget::onToolPropertyChanged(ToolType, ToolPropertyType ePropertyType)
{
    const CameraSettings* p = static_cast<const CameraSettings*>(mCameraTool->settings());

    switch (ePropertyType)
    {
    case CAMERA_SHOWPATH_CHECKED: { setShowCameraPath(p->showPath()); break; }
    default:
        break;
    }
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
