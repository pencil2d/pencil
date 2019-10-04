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

#include "operationswidget.h"
#include "ui_operationswidget.h"

#include <cmath>

#include <QToolButton>
#include <QGridLayout>
#include <QKeySequence>
#include <QAction>

#include "flowlayout.h"
#include "editor.h"
#include "toolmanager.h"
#include "pencilsettings.h"

namespace {
    QString GetToolTips(QString strCommandName)
    {
        strCommandName = QString("shortcuts/") + strCommandName;
        QKeySequence keySequence(pencilSettings().value(strCommandName).toString());
        return QString("<b>%1</b>").arg(keySequence.toString()); // don't tr() this string.
    }
}


OperationsWidget::OperationsWidget(QWidget* parent) :
    BaseDockWidget(parent),
    ui(new Ui::OperationsWidget)
{
    ui->setupUi(this);
}

OperationsWidget::~OperationsWidget()
{
    QSettings settings(PENCIL2D, PENCIL2D);
    settings.setValue("OperationsBarGeom", this->saveGeometry());
    delete ui;
}

void OperationsWidget::initUI()
{
#ifdef __APPLE__
    // Only Mac needs this. ToolButton is naturally borderless on Win/Linux.
    QString sStyle =
        "QToolButton { border: 0px; }"
        "QToolButton:pressed { border: 1px solid #ADADAD; border-radius: 2px; background-color: #D5D5D5; }"
    ui->newButton->setStyleSheet(sStyle);
    ui->openButton->setStyleSheet(sStyle);
    ui->saveButton->setStyleSheet(sStyle);
    ui->saveAsButton->setStyleSheet(sStyle);
    ui->undoButton->setStyleSheet(sStyle);
    ui->redoButton->setStyleSheet(sStyle);
    ui->cutButton->setStyleSheet(sStyle);
    ui->copyButton->setStyleSheet(sStyle);
    ui->pasteButton->setStyleSheet(sStyle);
#endif

    ui->newButton->setToolTip(tr("New File (%1)")
        .arg(GetToolTips(CMD_NEW_FILE)));
    ui->openButton->setToolTip(tr("Open File (%1)")
        .arg(GetToolTips(CMD_OPEN_FILE)));
    ui->saveButton->setToolTip(tr("Save (%1)")
        .arg(GetToolTips(CMD_SAVE_FILE)));
    ui->saveAsButton->setToolTip(tr("Save As.. (%1)")
        .arg(GetToolTips(CMD_SAVE_AS)));
    ui->undoButton->setToolTip(tr("Undo (%1)")
        .arg(GetToolTips(CMD_UNDO)));
    ui->redoButton->setToolTip(tr("Redo (%1)")
        .arg(GetToolTips(CMD_REDO)));
    ui->cutButton->setToolTip(tr("Cut (%1)")
        .arg(GetToolTips(CMD_CUT)));
    ui->copyButton->setToolTip(tr("Copy (%1)")
        .arg(GetToolTips(CMD_COPY)));
    ui->pasteButton->setToolTip(tr("Paste (%1)")
        .arg(GetToolTips(CMD_PASTE)));

    ui->newButton->setWhatsThis(tr("New File (%1)")
        .arg(GetToolTips(CMD_NEW_FILE)));
    ui->openButton->setWhatsThis(tr("Open File (%1)")
        .arg(GetToolTips(CMD_OPEN_FILE)));
    ui->saveButton->setWhatsThis(tr("Save (%1)")
        .arg(GetToolTips(CMD_SAVE_FILE)));
    ui->saveAsButton->setWhatsThis(tr("Save As.. (%1)")
        .arg(GetToolTips(CMD_SAVE_AS)));
    ui->undoButton->setWhatsThis(tr("Undo (%1)")
        .arg(GetToolTips(CMD_UNDO)));
    ui->redoButton->setWhatsThis(tr("Redo (%1)")
        .arg(GetToolTips(CMD_REDO)));
    ui->cutButton->setWhatsThis(tr("Cut (%1)")
        .arg(GetToolTips(CMD_CUT)));
    ui->copyButton->setWhatsThis(tr("Copy (%1)")
        .arg(GetToolTips(CMD_COPY)));
    ui->pasteButton->setWhatsThis(tr("Paste (%1)")
        .arg(GetToolTips(CMD_PASTE)));

    connect(ui->newButton, &QToolButton::clicked, this, &OperationsWidget::newClicked);
    connect(ui->openButton, &QToolButton::clicked, this, &OperationsWidget::openClicked);
    connect(ui->saveButton, &QToolButton::clicked, this, &OperationsWidget::saveClicked);
    connect(ui->saveAsButton, &QToolButton::clicked, this, &OperationsWidget::saveAsClicked);
    connect(ui->undoButton, &QToolButton::clicked, this, &OperationsWidget::undoClicked);
    connect(ui->redoButton, &QToolButton::clicked, this, &OperationsWidget::redoClicked);
    connect(ui->cutButton, &QToolButton::clicked, this, &OperationsWidget::cutClicked);
    connect(ui->copyButton, &QToolButton::clicked, this, &OperationsWidget::copyClicked);
    connect(ui->pasteButton, &QToolButton::clicked, this, &OperationsWidget::pasteClicked);

    delete ui->toolGroup->layout();
    FlowLayout* flowlayout = new FlowLayout;

    flowlayout->addWidget(ui->newButton);
    flowlayout->addWidget(ui->openButton);
    flowlayout->addWidget(ui->saveButton);
    flowlayout->addWidget(ui->saveAsButton);
    flowlayout->addWidget(ui->undoButton);
    flowlayout->addWidget(ui->redoButton);
    flowlayout->addWidget(ui->cutButton);
    flowlayout->addWidget(ui->copyButton);
    flowlayout->addWidget(ui->pasteButton);
    
    ui->toolGroup->setLayout(flowlayout);

    QSettings settings(PENCIL2D, PENCIL2D);
    restoreGeometry(settings.value("OperationsBarGeom").toByteArray());
}

void OperationsWidget::updateUI()
{
}
