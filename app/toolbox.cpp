/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2013-2017 Matt Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/

#include <cmath>

#include <QFrame>
#include <QToolButton>
#include <QGridLayout>
#include <QKeySequence>

#include "spinslider.h"
#include "toolbox.h"
#include "editor.h"
#include "toolmanager.h"
#include "pencilsettings.h"

// ----------------------------------------------------------------------------------
QString GetToolTips( QString strCommandName )
{
	strCommandName = QString( "shortcuts/" ) + strCommandName;
	QKeySequence keySequence( pencilSettings().value( strCommandName ).toString() );
	return QString("<b>%1</b>").arg( keySequence.toString() ); // don't tr() this string.
}

ToolBoxWidget::ToolBoxWidget( QWidget* parent ) : BaseDockWidget( parent )
{
	setWindowTitle( tr( "Tools", "Window title of tool box" ) );
}

void ToolBoxWidget::initUI()
{
	QGridLayout* layout = new QGridLayout();

    pencilButton = newToolButton( QIcon( ":icons/new/svg/pencil_detailed.svg" ),
		tr( "Pencil Tool (%1): Sketch with pencil" )
		.arg( GetToolTips( CMD_TOOL_PENCIL ) ) );
    selectButton = newToolButton( QIcon( ":icons/new/svg/selection.svg" ),
		tr( "Select Tool (%1): Select an object" )
		.arg( GetToolTips( CMD_TOOL_SELECT ) ) );
    moveButton = newToolButton( QIcon( ":icons/new/svg/arrow.svg" ),
		tr( "Move Tool (%1): Move an object" )
		.arg( GetToolTips( CMD_TOOL_MOVE ) ) );
    handButton = newToolButton( QIcon( ":icons/new/svg/hand_detailed.svg" ),
		tr( "Hand Tool (%1): Move the canvas" )
		.arg( GetToolTips( CMD_TOOL_HAND ) ) );
    penButton = newToolButton( QIcon( ":icons/new/svg/pen_detailed.svg" ),
		tr( "Pen Tool (%1): Sketch with pen" )
		.arg( GetToolTips( CMD_TOOL_PEN ) ) );
    eraserButton = newToolButton( QIcon( ":icons/new/svg/eraser_detailed.svg" ),
		tr( "Eraser Tool (%1): Erase" )
		.arg( GetToolTips( CMD_TOOL_ERASER ) ) );
    polylineButton = newToolButton( QIcon( ":icons/new/svg/line.svg" ),
		tr( "Polyline Tool (%1): Create line/curves" )
		.arg( GetToolTips( CMD_TOOL_POLYLINE ) ) );
    bucketButton = newToolButton( QIcon( ":icons/new/svg/bucket_detailed.svg" ),
		tr( "Paint Bucket Tool (%1): Fill selected area with a color" )
		.arg( GetToolTips( CMD_TOOL_BUCKET ) ) );
    colouringButton = newToolButton( QIcon( ":icons/new/svg/brush_detailed.svg" ),
		tr( "Brush Tool (%1): Paint smooth stroke with a brush" )
		.arg( GetToolTips( CMD_TOOL_BRUSH ) ) );
    eyedropperButton = newToolButton( QIcon( ":icons/new/svg/eyedropper_detailed.svg" ),
		tr( "Eyedropper Tool (%1): "
			"Set color from the stage<br>[ALT] for instant access" )
		.arg( GetToolTips( CMD_TOOL_EYEDROPPER ) ) );
    clearButton = newToolButton( QIcon( ":icons/new/svg/trash_detailed.svg" ),
		tr( "Clear Frame (%1): Erases content of selected frame" )
		.arg( GetToolTips( CMD_CLEAR_FRAME ) ) );
    smudgeButton = newToolButton( QIcon( ":icons/new/svg/smudge_detailed.svg" ),
		tr( "Smudge Tool (%1):<br>Edit polyline/curves<br>"
			"Liquify bitmap pixels<br> (%1)+[Alt]: Smooth" )
		.arg( GetToolTips( CMD_TOOL_SMUDGE ) ) );

	pencilButton->setWhatsThis( tr( "Pencil Tool (%1)" )
		.arg( GetToolTips( CMD_TOOL_PENCIL ) ) );
	selectButton->setWhatsThis( tr( "Select Tool (%1)" )
		.arg( GetToolTips( CMD_TOOL_SELECT ) ) );
	moveButton->setWhatsThis( tr( "Move Tool (%1)" )
		.arg( GetToolTips( CMD_TOOL_MOVE ) ) );
	handButton->setWhatsThis( tr( "Hand Tool (%1)" )
		.arg( GetToolTips( CMD_TOOL_HAND ) ) );
	penButton->setWhatsThis( tr( "Pen Tool (%1)" )
		.arg( GetToolTips( CMD_TOOL_PEN ) ) );
	eraserButton->setWhatsThis( tr( "Eraser Tool (%1)" )
		.arg( GetToolTips( CMD_TOOL_ERASER ) ) );
	polylineButton->setWhatsThis( tr( "Polyline Tool (%1)" )
		.arg( GetToolTips( CMD_TOOL_POLYLINE ) ) );
	bucketButton->setWhatsThis( tr( "Paint Bucket Tool (%1)" )
		.arg( GetToolTips( CMD_TOOL_BUCKET ) ) );
	colouringButton->setWhatsThis( tr( "Brush Tool (%1)" )
		.arg( GetToolTips( CMD_TOOL_BRUSH ) ) );
	eyedropperButton->setWhatsThis( tr( "Eyedropper Tool (%1)" )
		.arg( GetToolTips( CMD_TOOL_EYEDROPPER ) ) );
	clearButton->setWhatsThis( tr( "Clear Tool (%1)" )
		.arg( GetToolTips( CMD_CLEAR_FRAME ) ) );
	smudgeButton->setWhatsThis( tr( "Smudge Tool (%1)" )
		.arg( GetToolTips( CMD_TOOL_SMUDGE ) ) );

	pencilButton->setCheckable( true );
	penButton->setCheckable( true );
	polylineButton->setCheckable( true );
	bucketButton->setCheckable( true );
	colouringButton->setCheckable( true );
	smudgeButton->setCheckable( true );
	eyedropperButton->setCheckable( true );
	selectButton->setCheckable( true );
	moveButton->setCheckable( true );
	handButton->setCheckable( true );
	eraserButton->setCheckable( true );
	pencilButton->setChecked( true );

	layout->setMargin( 2 );
	layout->setSpacing( 0 );

	layout->addWidget( moveButton, 0, 0 );
	layout->setAlignment( moveButton, Qt::AlignRight );
	layout->addWidget( clearButton, 0, 1 );
	layout->setAlignment( clearButton, Qt::AlignLeft );

	layout->addWidget( selectButton, 1, 0 );
	layout->setAlignment( selectButton, Qt::AlignRight );
	layout->addWidget( colouringButton, 1, 1 );
	layout->setAlignment( colouringButton, Qt::AlignLeft );

	layout->addWidget( polylineButton, 2, 0 );
	layout->setAlignment( polylineButton, Qt::AlignRight );
	layout->addWidget( smudgeButton, 2, 1 );
	layout->setAlignment( smudgeButton, Qt::AlignLeft );

	layout->addWidget( penButton, 3, 0 );
	layout->setAlignment( penButton, Qt::AlignRight );
	layout->addWidget( handButton, 3, 1 );
	layout->setAlignment( handButton, Qt::AlignLeft );

	layout->addWidget( pencilButton, 4, 0 );
	layout->setAlignment( pencilButton, Qt::AlignRight );
	layout->addWidget( bucketButton, 4, 1 );
	layout->setAlignment( bucketButton, Qt::AlignLeft );

	layout->addWidget( eyedropperButton, 5, 0 );
	layout->setAlignment( eyedropperButton, Qt::AlignRight );
	layout->addWidget( eraserButton, 5, 1 );
	layout->setAlignment( eraserButton, Qt::AlignLeft );

	QWidget* toolGroup = new QWidget();
	setWidget( toolGroup );
	toolGroup->setLayout( layout );
	toolGroup->setMaximumHeight( 6 * 32 + 1 );

	setMaximumHeight( 200 );

	connect( pencilButton, &QToolButton::clicked, this, &ToolBoxWidget::pencilOn );
	connect( eraserButton, &QToolButton::clicked, this, &ToolBoxWidget::eraserOn );
	connect( selectButton, &QToolButton::clicked, this, &ToolBoxWidget::selectOn );
	connect( moveButton, &QToolButton::clicked, this, &ToolBoxWidget::moveOn );
	connect( penButton, &QToolButton::clicked, this, &ToolBoxWidget::penOn );
	connect( handButton, &QToolButton::clicked, this, &ToolBoxWidget::handOn );
	connect( polylineButton, &QToolButton::clicked, this, &ToolBoxWidget::polylineOn );
	connect( bucketButton, &QToolButton::clicked, this, &ToolBoxWidget::bucketOn );
	connect( eyedropperButton, &QToolButton::clicked, this, &ToolBoxWidget::eyedropperOn );
	connect( colouringButton, &QToolButton::clicked, this, &ToolBoxWidget::brushOn );
	connect( smudgeButton, &QToolButton::clicked, this, &ToolBoxWidget::smudgeOn );

	// pass to editor
	connect( clearButton, &QToolButton::clicked, this, &ToolBoxWidget::clearButtonClicked );

}

void ToolBoxWidget::updateUI()
{
}

QToolButton* ToolBoxWidget::newToolButton(const QIcon& icon, QString strToolTip)
{
    QToolButton* toolButton = new QToolButton(this);
    toolButton->setAutoRaise(true);
    toolButton->setIconSize( QSize(24,24) );
    toolButton->setFixedSize(32, 32);
    toolButton->setIcon(icon);
    toolButton->setToolTip(strToolTip);

    return toolButton;
}

void ToolBoxWidget::pencilOn()
{
    editor()->tools()->setCurrentTool( PENCIL );

    deselectAllTools();
    pencilButton->setChecked(true);
}

void ToolBoxWidget::eraserOn()
{
    editor()->tools()->setCurrentTool( ERASER );

    deselectAllTools();
    eraserButton->setChecked(true);
}

void ToolBoxWidget::selectOn()
{
    editor()->tools()->setCurrentTool( SELECT );

    deselectAllTools();
    selectButton->setChecked(true);
}

void ToolBoxWidget::moveOn()
{
    editor()->tools()->setCurrentTool( MOVE );

    deselectAllTools();
    moveButton->setChecked(true);
}

void ToolBoxWidget::penOn()
{
    editor()->tools()->setCurrentTool( PEN );

    deselectAllTools();
    penButton->setChecked(true);
}

void ToolBoxWidget::handOn()
{
    editor()->tools()->setCurrentTool( HAND );
    
    deselectAllTools();
    handButton->setChecked(true);
}

void ToolBoxWidget::polylineOn()
{
    editor()->tools()->setCurrentTool( POLYLINE );

    deselectAllTools();
    polylineButton->setChecked(true);
}

void ToolBoxWidget::bucketOn()
{
    editor()->tools()->setCurrentTool( BUCKET );

    deselectAllTools();
    bucketButton->setChecked(true);
}

void ToolBoxWidget::eyedropperOn()
{
    editor()->tools()->setCurrentTool( EYEDROPPER );

    deselectAllTools();
    eyedropperButton->setChecked(true);
}

void ToolBoxWidget::brushOn()
{
    editor()->tools()->setCurrentTool( BRUSH );
  
    deselectAllTools();
    colouringButton->setChecked(true);
}

void ToolBoxWidget::smudgeOn()
{
    editor()->tools()->setCurrentTool( SMUDGE );

    deselectAllTools();
    smudgeButton->setChecked(true);
}

void ToolBoxWidget::deselectAllTools()
{
    pencilButton->setChecked(false);
    eraserButton->setChecked(false);
    selectButton->setChecked(false);
    moveButton->setChecked(false);
    handButton->setChecked(false);
    penButton->setChecked(false);
    polylineButton->setChecked(false);
    bucketButton->setChecked(false);
    eyedropperButton->setChecked(false);
    colouringButton->setChecked(false);
    smudgeButton->setChecked(false);
}
