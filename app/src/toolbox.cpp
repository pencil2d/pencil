/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2013-2018 Matt Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/

#include "toolbox.h"
#include "ui_toolboxwidget.h"

#include <QToolButton>
#include <QKeySequence>
#include <QButtonGroup>

#include "flowlayout.h"
#include "spinslider.h"
#include "editor.h"
#include "toolmanager.h"
#include "layermanager.h"
#include "pencilsettings.h"

// ----------------------------------------------------------------------------------
QString GetToolTips(QString strCommandName)
{
    strCommandName = QString("shortcuts/") + strCommandName;
    QKeySequence keySequence(pencilSettings().value(strCommandName).toString());
    return QString("<b>%1</b>").arg(keySequence.toString()); // don't tr() this string.
}

ToolBoxWidget::ToolBoxWidget(QWidget* parent) :
    BaseDockWidget(parent),
    ui(new Ui::ToolBoxWidget)
{
    ui->setupUi(this);
}

ToolBoxWidget::~ToolBoxWidget()
{
    QSettings settings(PENCIL2D, PENCIL2D);
    settings.setValue("ToolBoxGeom", this->saveGeometry());
    delete ui;
}

void ToolBoxWidget::initUI()
{
#ifdef __APPLE__
    // Only Mac needs this. ToolButton is naturally borderless on Win/Linux.
    QString sStyle =
        "QToolButton { border: 0px; }"
        "QToolButton:pressed { border: 1px solid #ADADAD; border-radius: 2px; background-color: #D5D5D5; }"
        "QToolButton:checked { border: 1px solid #ADADAD; border-radius: 2px; background-color: #D5D5D5; }";
    ui->pencilButton->setStyleSheet(sStyle);
    ui->selectButton->setStyleSheet(sStyle);
    ui->moveButton->setStyleSheet(sStyle);
    ui->handButton->setStyleSheet(sStyle);
    ui->penButton->setStyleSheet(sStyle);
    ui->eraserButton->setStyleSheet(sStyle);
    ui->polylineButton->setStyleSheet(sStyle);
    ui->bucketButton->setStyleSheet(sStyle);
    ui->brushButton->setStyleSheet(sStyle);
    ui->eyedropperButton->setStyleSheet(sStyle);
    ui->clearButton->setStyleSheet(sStyle);
    ui->smudgeButton->setStyleSheet(sStyle);
#endif

    QButtonGroup* buttonGroup = new QButtonGroup(this);
    buttonGroup->setExclusive(true);

    ui->pencilButton->setToolTip( tr( "Pencil Tool (%1): Sketch with pencil" )
        .arg( GetToolTips( CMD_TOOL_PENCIL ) ) );
    ui->selectButton->setToolTip( tr( "Select Tool (%1): Select an object" )
        .arg( GetToolTips( CMD_TOOL_SELECT ) ) );
    ui->moveButton->setToolTip( tr( "Move Tool (%1): Move an object" )
        .arg( GetToolTips( CMD_TOOL_MOVE ) ) );
    ui->handButton->setToolTip( tr( "Hand Tool (%1): Move the canvas" )
        .arg( GetToolTips( CMD_TOOL_HAND ) ) );
    ui->penButton->setToolTip( tr( "Pen Tool (%1): Sketch with pen" )
        .arg( GetToolTips( CMD_TOOL_PEN ) ) );
    ui->eraserButton->setToolTip( tr( "Eraser Tool (%1): Erase" )
        .arg( GetToolTips( CMD_TOOL_ERASER ) ) );
    ui->polylineButton->setToolTip( tr( "Polyline Tool (%1): Create line/curves" )
        .arg( GetToolTips( CMD_TOOL_POLYLINE ) ) );
    ui->bucketButton->setToolTip( tr( "Paint Bucket Tool (%1): Fill selected area with a color" )
        .arg( GetToolTips( CMD_TOOL_BUCKET ) ) );
    ui->brushButton->setToolTip( tr( "Brush Tool (%1): Paint smooth stroke with a brush" )
        .arg( GetToolTips( CMD_TOOL_BRUSH ) ) );
    ui->eyedropperButton->setToolTip( tr( "Eyedropper Tool (%1): "
            "Set color from the stage<br>[ALT] for instant access" )
        .arg( GetToolTips( CMD_TOOL_EYEDROPPER ) ) );
    ui->clearButton->setToolTip( tr( "Clear Frame (%1): Erases content of selected frame" )
        .arg( GetToolTips( CMD_CLEAR_FRAME ) ) );
    ui->smudgeButton->setToolTip( tr( "Smudge Tool (%1):<br>Edit polyline/curves<br>"
            "Liquify bitmap pixels<br> (%1)+[Alt]: Smooth" )
        .arg( GetToolTips( CMD_TOOL_SMUDGE ) ) );

    ui->pencilButton->setWhatsThis( tr( "Pencil Tool (%1)" )
        .arg( GetToolTips( CMD_TOOL_PENCIL ) ) );
    ui->selectButton->setWhatsThis( tr( "Select Tool (%1)" )
        .arg( GetToolTips( CMD_TOOL_SELECT ) ) );
    ui->moveButton->setWhatsThis( tr( "Move Tool (%1)" )
        .arg( GetToolTips( CMD_TOOL_MOVE ) ) );
    ui->handButton->setWhatsThis( tr( "Hand Tool (%1)" )
        .arg( GetToolTips( CMD_TOOL_HAND ) ) );
    ui->penButton->setWhatsThis( tr( "Pen Tool (%1)" )
        .arg( GetToolTips( CMD_TOOL_PEN ) ) );
    ui->eraserButton->setWhatsThis( tr( "Eraser Tool (%1)" )
        .arg( GetToolTips( CMD_TOOL_ERASER ) ) );
    ui->polylineButton->setWhatsThis( tr( "Polyline Tool (%1)" )
        .arg( GetToolTips( CMD_TOOL_POLYLINE ) ) );
    ui->bucketButton->setWhatsThis( tr( "Paint Bucket Tool (%1)" )
        .arg( GetToolTips( CMD_TOOL_BUCKET ) ) );
    ui->brushButton->setWhatsThis( tr( "Brush Tool (%1)" )
        .arg( GetToolTips( CMD_TOOL_BRUSH ) ) );
    ui->eyedropperButton->setWhatsThis( tr( "Eyedropper Tool (%1)" )
        .arg( GetToolTips( CMD_TOOL_EYEDROPPER ) ) );
    ui->clearButton->setWhatsThis( tr( "Clear Tool (%1)" )
        .arg( GetToolTips( CMD_CLEAR_FRAME ) ) );
    ui->smudgeButton->setWhatsThis( tr( "Smudge Tool (%1)" )
        .arg( GetToolTips( CMD_TOOL_SMUDGE ) ) );

    mToolButtons.insert(CLEAR, ui->clearButton);
    mToolButtons.insert(PENCIL, ui->pencilButton);
    mToolButtons.insert(ERASER, ui->eraserButton);
    mToolButtons.insert(SELECT, ui->selectButton);
    mToolButtons.insert(MOVE, ui->moveButton);
    mToolButtons.insert(PEN, ui->penButton);
    mToolButtons.insert(HAND, ui->handButton);
    mToolButtons.insert(POLYLINE, ui->polylineButton);
    mToolButtons.insert(BUCKET, ui->bucketButton);
    mToolButtons.insert(EYEDROPPER, ui->eyedropperButton);
    mToolButtons.insert(BRUSH, ui->brushButton);
    mToolButtons.insert(SMUDGE, ui->smudgeButton);


    for (QToolButton* toolbutton : mToolButtons) {
        connect(toolbutton, &QToolButton::clicked, this, &ToolBoxWidget::setToolOn);
    }

    connect(editor()->tools(), &ToolManager::toolChanged, this, &ToolBoxWidget::setTool);
    connect(ui->clearButton, &QToolButton::clicked, this, &ToolBoxWidget::clearButtonClicked);

    delete ui->toolGroup->layout();
    FlowLayout* flowlayout = new FlowLayout;{}

    QMapIterator<ToolType, QToolButton*> i(mToolButtons);
    while (i.hasNext()) {
        i.next();
        flowlayout->addWidget(i.value());
        buttonGroup->addButton(i.value());
    }

    ui->toolGroup->setLayout(flowlayout);

    QSettings settings(PENCIL2D, PENCIL2D);
    restoreGeometry(settings.value("ToolBoxGeom").toByteArray());
}

void ToolBoxWidget::updateUI()
{
    if (editor()->layers()->currentLayer() != nullptr) {
        mLayerType = editor()->layers()->currentLayer()->type();
    }

    if (mLayerType == Layer::CAMERA)
    {
        ui->moveButton->setEnabled(true);
        ui->handButton->setEnabled(true);

        ui->clearButton->setEnabled(false);
        ui->pencilButton->setEnabled(false);
        ui->eraserButton->setEnabled(false);
        ui->selectButton->setEnabled(false);
        ui->penButton->setEnabled(false);
        ui->polylineButton->setEnabled(false);
        ui->bucketButton->setEnabled(false);
        ui->eyedropperButton->setEnabled(false);
        ui->brushButton->setEnabled(false);
        ui->smudgeButton->setEnabled(false);
    } else if (mLayerType == Layer::SOUND) {
        ui->clearButton->setEnabled(false);
        ui->pencilButton->setEnabled(false);
        ui->eraserButton->setEnabled(false);
        ui->selectButton->setEnabled(false);
        ui->moveButton->setEnabled(false);
        ui->penButton->setEnabled(false);
        ui->handButton->setEnabled(false);
        ui->polylineButton->setEnabled(false);
        ui->bucketButton->setEnabled(false);
        ui->eyedropperButton->setEnabled(false);
        ui->brushButton->setEnabled(false);
        ui->smudgeButton->setEnabled(false);
    } else {
        ui->clearButton->setEnabled(true);
        ui->pencilButton->setEnabled(true);
        ui->eraserButton->setEnabled(true);
        ui->selectButton->setEnabled(true);
        ui->moveButton->setEnabled(true);
        ui->penButton->setEnabled(true);
        ui->handButton->setEnabled(true);
        ui->polylineButton->setEnabled(true);
        ui->bucketButton->setEnabled(true);
        ui->eyedropperButton->setEnabled(true);
        ui->brushButton->setEnabled(true);
        ui->smudgeButton->setEnabled(true);
    }
}

void ToolBoxWidget::deselectAllTools()
{
    ui->pencilButton->setChecked(false);
    ui->eraserButton->setChecked(false);
    ui->selectButton->setChecked(false);
    ui->moveButton->setChecked(false);
    ui->handButton->setChecked(false);
    ui->penButton->setChecked(false);
    ui->polylineButton->setChecked(false);
    ui->bucketButton->setChecked(false);
    ui->eyedropperButton->setChecked(false);
    ui->brushButton->setChecked(false);
    ui->smudgeButton->setChecked(false);
}

void ToolBoxWidget::setToolOn()
{
    QObject* object = sender();
    QToolButton* toolButton = static_cast<QToolButton*>(object);

    if (!leavingTool(toolButton)) { return; }
    toolButton->setChecked(true);

    for (QToolButton* toolButton : mToolButtons) {
        if (object == toolButton) {

            ToolType toolType = mToolButtons.key(toolButton);
            editor()->tools()->setCurrentTool(toolType, mLayerType);
            saveToolUsed(toolType);
            break;
        }
    }

}

void ToolBoxWidget::saveToolUsed(ToolType toolType)
{
    QSettings settings (PENCIL2D, PENCIL2D);
    if (mLayerType == Layer::VECTOR) {
        settings.setValue(SETTING_TOOL_VECTOR_LASTUSED, static_cast<int>(toolType));
    } else if (mLayerType == Layer::BITMAP) {
        settings.setValue(SETTING_TOOL_BITMAP_LASTUSED, static_cast<int>(toolType));
    } else {
        settings.setValue(SETTING_TOOL_CAMERA_LASTUSED, static_cast<int>(toolType));
    }
}

void ToolBoxWidget::setTool(ToolType toolType)
{
    QToolButton* button = mToolButtons[toolType];
    QSignalBlocker b1(button);
    mToolButtons[toolType]->setChecked(true);
}

bool ToolBoxWidget::leavingTool(QToolButton* toolButton)
{
    if (!editor()->tools()->leavingThisTool())
    {
        if (toolButton->isChecked()) {
            toolButton->setChecked(false);
        }
        return false;
    }
    return true;
}
