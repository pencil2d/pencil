/*

Pencil2D - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2012-2020 Matthew Chiawen Chang
Copyright (C) 2024-2099 Oliver S. Larsen

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/
#include "toolboxwidget.h"
#include "ui_toolboxwidget.h"

#include <QScrollBar>
#include <QResizeEvent>
#include <QDebug>
#include <QButtonGroup>

#include "layermanager.h"
#include "toolmanager.h"
#include "editor.h"
#include "pencilsettings.h"

// ----------------------------------------------------------------------------------
QString GetToolTips(QString strCommandName)
{
    strCommandName = QString("shortcuts/") + strCommandName;
    QKeySequence keySequence(pencilSettings().value(strCommandName).toString());
    return QString("<b>%1</b>").arg(keySequence.toString()); // don't tr() this string.
}

ToolBoxWidget::ToolBoxWidget(QWidget* parent)
    : QWidget(parent), ui(new Ui::ToolBoxWidget)
{
    ui->setupUi(this);
}

ToolBoxWidget::~ToolBoxWidget()
{
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
    ui->smudgeButton->setStyleSheet(sStyle);
#endif

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
    ui->smudgeButton->setWhatsThis( tr( "Smudge Tool (%1)" )
        .arg( GetToolTips( CMD_TOOL_SMUDGE ) ) );

    connect(ui->pencilButton, &QToolButton::clicked, this, &ToolBoxWidget::pencilOn);
    connect(ui->eraserButton, &QToolButton::clicked, this, &ToolBoxWidget::eraserOn);
    connect(ui->selectButton, &QToolButton::clicked, this, &ToolBoxWidget::selectOn);
    connect(ui->moveButton, &QToolButton::clicked, this, &ToolBoxWidget::moveOn);
    connect(ui->penButton, &QToolButton::clicked, this, &ToolBoxWidget::penOn);
    connect(ui->handButton, &QToolButton::clicked, this, &ToolBoxWidget::handOn);
    connect(ui->polylineButton, &QToolButton::clicked, this, &ToolBoxWidget::polylineOn);
    connect(ui->bucketButton, &QToolButton::clicked, this, &ToolBoxWidget::bucketOn);
    connect(ui->eyedropperButton, &QToolButton::clicked, this, &ToolBoxWidget::eyedropperOn);
    connect(ui->brushButton, &QToolButton::clicked, this, &ToolBoxWidget::brushOn);
    connect(ui->smudgeButton, &QToolButton::clicked, this, &ToolBoxWidget::smudgeOn);

    mFlowlayout = new ToolBoxLayout(nullptr, 3,3,3);

    mFlowlayout->addWidget(ui->pencilButton);
    mFlowlayout->addWidget(ui->eraserButton);
    mFlowlayout->addWidget(ui->selectButton);
    mFlowlayout->addWidget(ui->moveButton);
    mFlowlayout->addWidget(ui->penButton);
    mFlowlayout->addWidget(ui->handButton);
    mFlowlayout->addWidget(ui->polylineButton);
    mFlowlayout->addWidget(ui->bucketButton);
    mFlowlayout->addWidget(ui->eyedropperButton);
    mFlowlayout->addWidget(ui->brushButton);
    mFlowlayout->addWidget(ui->smudgeButton);

    delete ui->scrollAreaWidgetContents_2->layout();
    ui->scrollAreaWidgetContents_2->setLayout(mFlowlayout);

    // Important to set the proper minimumSize;
    ui->scrollArea->setMinimumSize(QSize(1,1));
    setMinimumSize(mFlowlayout->minimumSize());

    QButtonGroup* buttonGroup = new QButtonGroup(this);
    buttonGroup->addButton(ui->pencilButton);
    buttonGroup->addButton(ui->eraserButton);
    buttonGroup->addButton(ui->selectButton);
    buttonGroup->addButton(ui->moveButton);
    buttonGroup->addButton(ui->penButton);
    buttonGroup->addButton(ui->handButton);
    buttonGroup->addButton(ui->polylineButton);
    buttonGroup->addButton(ui->bucketButton);
    buttonGroup->addButton(ui->eyedropperButton);
    buttonGroup->addButton(ui->brushButton);
    buttonGroup->addButton(ui->smudgeButton);
}

int ToolBoxWidget::getMinHeightForWidth(int width) const
{
    return mFlowlayout->heightForWidth(width);
}

QSize ToolBoxWidget::sizeHint() const
{
    return minimumSizeHint();
}

QSize ToolBoxWidget::minimumSizeHint() const
{
    int minWidth = mFlowlayout->minimumSize().width();
    return QSize(minWidth, getMinHeightForWidth(width()));
}

void ToolBoxWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);

    updateLayoutAlignment();
}

void ToolBoxWidget::updateLayoutAlignment()
{
    mFlowlayout->invalidate();
    if (mFlowlayout->rows() > 1) {
        mFlowlayout->setAlignment(Qt::AlignJustify);
    } else {
        mFlowlayout->setAlignment(Qt::AlignHCenter);
    }

    mFlowlayout->activate();
}

void ToolBoxWidget::updateUI()
{
}

void ToolBoxWidget::setActiveTool(ToolType toolType)
{
    switch (toolType) {
    case ToolType::BRUSH:
        brushOn();
        break;
    case ToolType::PEN:
        penOn();
        break;
    case ToolType::PENCIL:
        pencilOn();
        break;
    case ToolType::SELECT:
        selectOn();
        break;
    case ToolType::HAND:
        handOn();
        break;
    case ToolType::MOVE:
    case ToolType::CAMERA:
        moveOn();
        break;
    case ToolType::ERASER:
        eraserOn();
        break;
    case ToolType::POLYLINE:
        polylineOn();
        break;
    case ToolType::SMUDGE:
        smudgeOn();
        break;
    case ToolType::BUCKET:
        bucketOn();
        break;
    case ToolType::EYEDROPPER:
        eyedropperOn();
        break;
    default:
        break;
    }
}

void ToolBoxWidget::pencilOn()
{
    toolOn(PENCIL, ui->pencilButton);
}

void ToolBoxWidget::eraserOn()
{
    toolOn(ERASER, ui->eraserButton);
}

void ToolBoxWidget::selectOn()
{
    toolOn(SELECT, ui->selectButton);
}

void ToolBoxWidget::moveOn()
{
    if (mEditor->layers()->currentLayer()->type() == Layer::CAMERA) {
        toolOn(CAMERA, ui->moveButton);
    } else {
        toolOn(MOVE, ui->moveButton);
    }
}

void ToolBoxWidget::penOn()
{
    toolOn(PEN, ui->penButton);
}

void ToolBoxWidget::handOn()
{
    toolOn(HAND, ui->handButton);
}

void ToolBoxWidget::polylineOn()
{
    toolOn(POLYLINE, ui->polylineButton);
}

void ToolBoxWidget::bucketOn()
{
    toolOn(BUCKET, ui->bucketButton);
}

void ToolBoxWidget::eyedropperOn()
{
    toolOn(EYEDROPPER, ui->eyedropperButton);
}

void ToolBoxWidget::brushOn()
{
    toolOn(BRUSH, ui->brushButton);
}

void ToolBoxWidget::smudgeOn()
{
    toolOn(SMUDGE, ui->smudgeButton);
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

void ToolBoxWidget::toolOn(ToolType toolType, QToolButton* toolButton)
{
    if (mEditor->tools()->currentTool()->type() == toolType) {
        // Prevent un-checking the current tool and do nothing
        toolButton->setChecked(true);
        return;
    }
    if (!mEditor->tools()->leavingThisTool())
    {
        toolButton->setChecked(false);
        return;
    }
    mEditor->tools()->setCurrentTool(toolType);
}
