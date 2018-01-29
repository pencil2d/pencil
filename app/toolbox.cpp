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

#include <cmath>

#include <QToolButton>
#include <QGridLayout>
#include <QKeySequence>

#include "spinslider.h"
#include "toolbox.h"
#include "ui_toolboxwidget.h"
#include "editor.h"
#include "toolmanager.h"
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
    settings.setValue("toolBoxDockLocation", (int)mAreaLocation);
    settings.setValue("isToolBoxFloating", isFloating());
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

    connect(ui->clearButton, &QToolButton::clicked, this, &ToolBoxWidget::clearButtonClicked);

    QSettings settings(PENCIL2D, PENCIL2D);
    this->restoreGeometry(settings.value("ToolBoxGeom").toByteArray());
    mAreaLocation = (Qt::DockWidgetArea)settings.value("toolBoxDockLocation").toInt();
    mIsFloating = settings.value("isToolBoxFloating").toBool();
}

void ToolBoxWidget::updateUI()
{
}

void ToolBoxWidget::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);

    QRect geom = geometry();
    QSize buttonSize = ui->clearButton->size(); // all buttons share same size

    if (mIsFloating)
    {
        mAreaLocation = Qt::AllDockWidgetAreas;
    }

    // disable certain areas when width is >= number of buttons.
    int NUMOFBUTTONS = 12;
    if (geometry().width() >= buttonSize.width()*NUMOFBUTTONS)
    {
        setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);
    }
    else
    { // otherwise allow all areas
        setAllowedAreas(Qt::AllDockWidgetAreas);
    }


    // Horizontal Layout
    if (geom.width() < geom.height())
    {
        if (geom.width() > buttonSize.width() * 5)
        {
            ui->gridLayout->addWidget(ui->clearButton, 0, 0);
            ui->gridLayout->addWidget(ui->moveButton, 0, 1);

            ui->gridLayout->addWidget(ui->selectButton, 0, 2);
            ui->gridLayout->addWidget(ui->brushButton, 1, 0);

            ui->gridLayout->addWidget(ui->polylineButton, 1, 1);
            ui->gridLayout->addWidget(ui->smudgeButton, 1, 2);

            ui->gridLayout->addWidget(ui->penButton, 2, 0);
            ui->gridLayout->addWidget(ui->handButton, 2, 1);

            ui->gridLayout->addWidget(ui->pencilButton, 2, 2);
            ui->gridLayout->addWidget(ui->bucketButton, 3, 0);

            ui->gridLayout->addWidget(ui->eyedropperButton, 3, 1);
            ui->gridLayout->addWidget(ui->eraserButton, 3, 2);
        }
        else if (geom.width() > buttonSize.width() * 3
                 || mAreaLocation == Qt::TopDockWidgetArea
                 || mAreaLocation == Qt::BottomDockWidgetArea )
        {
            ui->gridLayout->addWidget(ui->clearButton, 0, 0);
            ui->gridLayout->addWidget(ui->moveButton, 0, 1);

            ui->gridLayout->addWidget(ui->selectButton, 1, 0);
            ui->gridLayout->addWidget(ui->brushButton, 1, 1);

            ui->gridLayout->addWidget(ui->polylineButton, 2, 0);
            ui->gridLayout->addWidget(ui->smudgeButton, 2, 1);

            ui->gridLayout->addWidget(ui->penButton, 3, 0);
            ui->gridLayout->addWidget(ui->handButton, 3, 1);

            ui->gridLayout->addWidget(ui->pencilButton, 4, 0);
            ui->gridLayout->addWidget(ui->bucketButton, 4, 1);

            ui->gridLayout->addWidget(ui->eyedropperButton, 5, 0);
            ui->gridLayout->addWidget(ui->eraserButton, 5, 1);

        }
        else if (mAreaLocation != Qt::TopDockWidgetArea
                 || mAreaLocation != Qt::BottomDockWidgetArea
                 || mIsFloating)
        {
            ui->gridLayout->addWidget(ui->clearButton, 0, 0);
            ui->gridLayout->addWidget(ui->moveButton, 1, 0);
            ui->gridLayout->addWidget(ui->selectButton, 2, 0);
            ui->gridLayout->addWidget(ui->brushButton, 3, 0);
            ui->gridLayout->addWidget(ui->polylineButton, 4, 0);
            ui->gridLayout->addWidget(ui->smudgeButton, 5, 0);
            ui->gridLayout->addWidget(ui->penButton, 6, 0);
            ui->gridLayout->addWidget(ui->handButton, 7, 0);
            ui->gridLayout->addWidget(ui->pencilButton, 8, 0);
            ui->gridLayout->addWidget(ui->bucketButton, 9, 0);
            ui->gridLayout->addWidget(ui->eyedropperButton, 10, 0);
            ui->gridLayout->addWidget(ui->eraserButton, 11, 0);
        }
    }
    else
    { // Vertical
        if (geom.height() > buttonSize.height() * 5)
        {
            ui->gridLayout->addWidget(ui->clearButton, 0, 0);
            ui->gridLayout->addWidget(ui->moveButton, 1, 0);

            ui->gridLayout->addWidget(ui->selectButton, 2, 0);
            ui->gridLayout->addWidget(ui->brushButton, 0, 1);

            ui->gridLayout->addWidget(ui->polylineButton, 1, 1);
            ui->gridLayout->addWidget(ui->smudgeButton, 2, 1);

            ui->gridLayout->addWidget(ui->penButton, 0, 2);
            ui->gridLayout->addWidget(ui->handButton, 1, 2);

            ui->gridLayout->addWidget(ui->pencilButton, 2, 2);
            ui->gridLayout->addWidget(ui->bucketButton, 0, 3);

            ui->gridLayout->addWidget(ui->eyedropperButton, 1, 3);
            ui->gridLayout->addWidget(ui->eraserButton, 2, 3);
        }
        else if (geom.height() > buttonSize.height() * 3 ||
                 mAreaLocation == Qt::LeftDockWidgetArea
                 || mAreaLocation == Qt::RightDockWidgetArea)
        {
            ui->gridLayout->addWidget(ui->clearButton, 0, 0);
            ui->gridLayout->addWidget(ui->moveButton, 1, 0);

            ui->gridLayout->addWidget(ui->selectButton, 0, 1);
            ui->gridLayout->addWidget(ui->brushButton, 1, 1);

            ui->gridLayout->addWidget(ui->polylineButton, 0, 2);
            ui->gridLayout->addWidget(ui->smudgeButton, 1, 2);

            ui->gridLayout->addWidget(ui->penButton, 0, 3);
            ui->gridLayout->addWidget(ui->handButton, 1, 3);

            ui->gridLayout->addWidget(ui->pencilButton, 0, 4);
            ui->gridLayout->addWidget(ui->bucketButton, 1, 4);

            ui->gridLayout->addWidget(ui->eyedropperButton, 0, 5);
            ui->gridLayout->addWidget(ui->eraserButton, 1, 5);
        }
        else if (mAreaLocation != Qt::LeftDockWidgetArea
                 || mAreaLocation != Qt::RightDockWidgetArea
                 || mIsFloating)
        {
            ui->gridLayout->addWidget(ui->clearButton, 0, 0);
            ui->gridLayout->addWidget(ui->moveButton, 0, 1);
            ui->gridLayout->addWidget(ui->selectButton, 0, 2);
            ui->gridLayout->addWidget(ui->brushButton, 0, 3);
            ui->gridLayout->addWidget(ui->polylineButton, 0, 4);
            ui->gridLayout->addWidget(ui->smudgeButton, 0, 5);
            ui->gridLayout->addWidget(ui->penButton, 0, 6);
            ui->gridLayout->addWidget(ui->handButton, 0, 7);
            ui->gridLayout->addWidget(ui->pencilButton, 0, 8);
            ui->gridLayout->addWidget(ui->bucketButton, 0, 9);
            ui->gridLayout->addWidget(ui->eyedropperButton, 0, 10);
            ui->gridLayout->addWidget(ui->eraserButton, 0, 11);
        }
    }

    if (mHasResizedOnce)
    {
        mIsFloating = isFloating();
    }
    else
    {
        mHasResizedOnce = true;
    }
}

void ToolBoxWidget::getDockLocation(Qt::DockWidgetArea area)
{
    mAreaLocation = area;
}

void ToolBoxWidget::pencilOn()
{
    editor()->tools()->setCurrentTool(PENCIL);

    deselectAllTools();
    ui->pencilButton->setChecked(true);
}

void ToolBoxWidget::eraserOn()
{
    editor()->tools()->setCurrentTool(ERASER);

    deselectAllTools();
    ui->eraserButton->setChecked(true);
}

void ToolBoxWidget::selectOn()
{
    editor()->tools()->setCurrentTool(SELECT);

    deselectAllTools();
    ui->selectButton->setChecked(true);
}

void ToolBoxWidget::moveOn()
{
    editor()->tools()->setCurrentTool(MOVE);

    deselectAllTools();
    ui->moveButton->setChecked(true);
}

void ToolBoxWidget::penOn()
{
    editor()->tools()->setCurrentTool(PEN);

    deselectAllTools();
    ui->penButton->setChecked(true);
}

void ToolBoxWidget::handOn()
{
    editor()->tools()->setCurrentTool( HAND );

    deselectAllTools();
    ui->handButton->setChecked(true);
}

void ToolBoxWidget::polylineOn()
{
    editor()->tools()->setCurrentTool(POLYLINE);

    deselectAllTools();
    ui->polylineButton->setChecked(true);
}

void ToolBoxWidget::bucketOn()
{
    editor()->tools()->setCurrentTool(BUCKET);

    deselectAllTools();
    ui->bucketButton->setChecked(true);
}

void ToolBoxWidget::eyedropperOn()
{
    editor()->tools()->setCurrentTool(EYEDROPPER);

    deselectAllTools();
    ui->eyedropperButton->setChecked(true);
}

void ToolBoxWidget::brushOn()
{
    editor()->tools()->setCurrentTool( BRUSH );

    deselectAllTools();
    ui->brushButton->setChecked(true);
}

void ToolBoxWidget::smudgeOn()
{
    editor()->tools()->setCurrentTool(SMUDGE);

    deselectAllTools();
    ui->smudgeButton->setChecked(true);
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
