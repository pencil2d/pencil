/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation;

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/

#include <cmath>

#include <QFrame>
#include <QToolButton>
#include <QGridLayout>

#include "spinslider.h"
#include "toolbox.h"
#include "editor.h"
#include "toolmanager.h"
#include "pencilsettings.h"

// ----------------------------------------------------------------------------------

ToolBoxWidget::ToolBoxWidget(const QString title, QWidget* pParent ) :
    BaseDockWidget( title, pParent, Qt::Tool )
{
    QGridLayout* layout = new QGridLayout();

    auto cmdKeySeq = []( QString strCommandName ) -> QKeySequence
    {
        strCommandName = QString( "shortcuts/" ) + strCommandName;
        QKeySequence keySequence( pencilSettings().value( strCommandName ).toString() );
        return keySequence;
    };

    pencilButton = newToolButton( QIcon( ":icons/pencil2.png" ),
                                  tr( "Pencil Tool <b> (%1) </b>: Sketch with pencil" )
                                  .arg( cmdKeySeq( CMD_TOOL_PENCIL ).toString() ));
    selectButton = newToolButton( QIcon( ":icons/select.png" ),
                                  tr( "Select Tool <b>(%1)</b>: Select an object" )
                                  .arg( cmdKeySeq( CMD_TOOL_SELECT ).toString() ));
    moveButton = newToolButton( QIcon( ":icons/arrow.png" ),
                                tr( "Move Tool <b>(%1)</b>: Move an object" )
                                .arg( cmdKeySeq( CMD_TOOL_MOVE ).toString() ));
    handButton = newToolButton( QIcon( ":icons/hand.png" ),
                                tr( "Hand Tool <b>(%1)</b>: Move the canvas" )
                                .arg( cmdKeySeq( CMD_TOOL_HAND ).toString() ));
    penButton = newToolButton( QIcon( ":icons/pen.png" ),
                               tr( "Pen Tool <b>(%1)</b>: Sketch with pen" )
                               .arg( cmdKeySeq( CMD_TOOL_PEN ).toString() ));
    eraserButton = newToolButton( QIcon( ":icons/eraser.png" ),
                                  tr( "Eraser Tool <b>(%1)</b>: Erase" )
                                  .arg( cmdKeySeq( CMD_TOOL_ERASER ).toString() ));
    polylineButton = newToolButton( QIcon( ":icons/polyline.png" ),
                                    tr( "Polyline Tool <b>(%1)</b>: Create line/curves" )
                                    .arg( cmdKeySeq( CMD_TOOL_POLYLINE ).toString() ));
    bucketButton = newToolButton( QIcon( ":icons/bucket.png" ),
                                  tr( "Paint Bucket Tool <b>(%1)</b>: Fill selected area with a color" )
                                  .arg( cmdKeySeq( CMD_TOOL_BUCKET ).toString() ));
    colouringButton = newToolButton( QIcon( ":icons/brush.png" ),
                                     tr( "Brush Tool <b>(%1)</b>: Paint smooth stroke with a brush" )
                                     .arg( cmdKeySeq( CMD_TOOL_BRUSH ).toString() ));
    eyedropperButton = newToolButton( QIcon( ":icons/eyedropper.png" ),
                                      tr( "Eyedropper Tool <b>(%1)</b>: "
                                          "Set color from the stage<br><b>[ALT]</b> for instant access" )
                                      .arg( cmdKeySeq( CMD_TOOL_EYEDROPPER ).toString() ));
    clearButton = newToolButton( QIcon( ":icons/clear.png" ),
                                 tr( "Clear Frame <b>(%1)</b>: Erases content of selected frame" )
                                 .arg( cmdKeySeq( CMD_CLEAR_FRAME ).toString() ));
    smudgeButton = newToolButton( QIcon( ":icons/smudge.png" ),
                                  tr( "Smudge Tool <b>(%1)</b>:<br>Edit polyline/curves<br>"
                                      "Liquify bitmap pixels<br> <b>(%1)+[Alt]</b>: Smooth" )
                                  .arg( cmdKeySeq( CMD_TOOL_SMUDGE ).toString() ));

    pencilButton->setWhatsThis( tr( "Pencil Tool (%1)" )
                                .arg( cmdKeySeq( CMD_TOOL_PENCIL ).toString() ));
    selectButton->setWhatsThis( tr( "Select Tool (%1)" )
                                .arg( cmdKeySeq( CMD_TOOL_SELECT ).toString() ));
    moveButton->setWhatsThis( tr( "Move Tool (%1)" )
                              .arg( cmdKeySeq( CMD_TOOL_MOVE ).toString() ));
    handButton->setWhatsThis( tr( "Hand Tool (%1)" )
                              .arg( cmdKeySeq( CMD_TOOL_HAND ).toString() ));
    penButton->setWhatsThis( tr( "Pen Tool (%1)" )
                             .arg( cmdKeySeq( CMD_TOOL_PEN ).toString() ));
    eraserButton->setWhatsThis( tr( "Eraser Tool (%1)" )
                                .arg( cmdKeySeq( CMD_TOOL_ERASER ).toString() ));
    polylineButton->setWhatsThis( tr( "Polyline Tool (%1)" )
                                  .arg( cmdKeySeq( CMD_TOOL_POLYLINE ).toString() ));
    bucketButton->setWhatsThis( tr( "Paint Bucket Tool (%1)" )
                                .arg( cmdKeySeq( CMD_TOOL_BUCKET ).toString() ));
    colouringButton->setWhatsThis( tr( "Brush Tool (%1)" )
                                   .arg( cmdKeySeq( CMD_TOOL_BRUSH ).toString() ));
    eyedropperButton->setWhatsThis( tr( "Eyedropper Tool (%1)" )
                                    .arg( cmdKeySeq( CMD_TOOL_EYEDROPPER ).toString() ));
    clearButton->setWhatsThis( tr( "Clear Tool (%1)" )
                               .arg( cmdKeySeq( CMD_CLEAR_FRAME ).toString() ));
    smudgeButton->setWhatsThis( tr( "Smudge Tool (%1)" )
                                .arg( cmdKeySeq( CMD_TOOL_SMUDGE ).toString() ));

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

    QFrame* toolGroup = new QFrame();
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


void ToolBoxWidget::initUI()
{

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
