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

#include <QtGui>
#include <QGridLayout>

#include "spinslider.h"
#include "tooloptiondockwidget.h"
#include "toolbox.h"
#include "editor.h"
#include "toolmanager.h"

// ----------------------------------------------------------------------------------

ToolBoxWidget::ToolBoxWidget(const QString title, QWidget* pParent ) :
    QDockWidget(title, pParent)
{
    QFrame* toolGroup = new QFrame();
    setWidget(toolGroup);

    QGridLayout* layout = new QGridLayout();

    pencilButton = newToolButton(QIcon("://icons/pencil2.png"), tr("Pencil Tool <b>(N)</b>: Sketch with pencil"));
    selectButton = newToolButton(QIcon(":icons/select.png"), tr("Select Tool <b>(V)</b>: Select an object"));
    moveButton = newToolButton(QIcon(":icons/arrow.png"), tr("Move Tool <b>(Q)</b>: Move an object"));
    handButton = newToolButton(QIcon(":icons/hand.png"), tr("Hand Tool <b>(H)</b>: Move the canvas"));
    penButton = newToolButton(QIcon(":icons/pen.png"), tr("Pen Tool <b>(P)</b>: Sketch with pen"));
    eraserButton = newToolButton(QIcon(":icons/eraser.png"), tr("Eraser Tool <b>(E)</b>: Erase"));
    polylineButton = newToolButton(QIcon(":icons/polyline.png"), tr("Polyline Tool <b>(Y)</b>: Create line/curves"));
    bucketButton = newToolButton(QIcon(":icons/bucket.png"), tr("Paint Bucket Tool <b>(K)</b>: Fill selected area with a color"));
    colouringButton = newToolButton(QIcon(":icons/brush.png"), tr("Brush Tool <b>(B)</b>: Paint smooth stroke with a brush"));
    eyedropperButton = newToolButton(QIcon(":icons/eyedropper.png"), tr("Eyedropper Tool <b>(I)</b>: Set color from the stage<br><b>[ALT]</b> for instant access"));
    clearButton = newToolButton(QIcon(":icons/clear.png"), tr("Clear Frame <b>(Ctrl+D)</b>: Erases content of selected frame"));
    smudgeButton = newToolButton(QIcon(":icons/smudge.png"), tr("Smudge Tool <b>(A)</b>:<br>Edit polyline/curves<br>Liquify bitmap pixels<br><b>+[Alt]</b>: Smooth"));

    pencilButton->setWhatsThis(tr("Pencil Tool (N)"));
    selectButton->setWhatsThis(tr("Select Tool (V)"));
    moveButton->setWhatsThis(tr("Move Tool (Q)"));
    handButton->setWhatsThis(tr("Hand Tool (H)"));
    penButton->setWhatsThis(tr("Pen Tool (P)"));
    eraserButton->setWhatsThis(tr("Eraser Tool (E)"));
    polylineButton->setWhatsThis(tr("Polyline Tool (Y)"));
    bucketButton->setWhatsThis(tr("Paint Bucket Tool(K)"));
    colouringButton->setWhatsThis(tr("Brush Tool(B)"));
    eyedropperButton->setWhatsThis(tr("Eyedropper Tool (I)"));
    clearButton->setWhatsThis(tr("Clear Tool"));
    smudgeButton->setWhatsThis(tr("Smudge Tool (A)"));

    pencilButton->setCheckable(true);
    penButton->setCheckable(true);
    polylineButton->setCheckable(true);
    bucketButton->setCheckable(true);
    colouringButton->setCheckable(true);
    smudgeButton->setCheckable(true);
    eyedropperButton->setCheckable(true);
    selectButton->setCheckable(true);
    moveButton->setCheckable(true);
    handButton->setCheckable(true);
    eraserButton->setCheckable(true);
    pencilButton->setChecked(true);

    layout->setMargin(2);
    layout->setSpacing(0);

    layout->addWidget(moveButton,0,0);
    layout->setAlignment(moveButton, Qt::AlignRight);
    layout->addWidget(clearButton,0,1);
    layout->setAlignment(clearButton, Qt::AlignLeft);

    layout->addWidget(selectButton,1,0);
    layout->setAlignment(selectButton, Qt::AlignRight);
    layout->addWidget(colouringButton,1,1);
    layout->setAlignment(colouringButton, Qt::AlignLeft);

    layout->addWidget(polylineButton,2,0);
    layout->setAlignment(polylineButton, Qt::AlignRight);
    layout->addWidget(smudgeButton,2,1);
    layout->setAlignment(smudgeButton, Qt::AlignLeft);

    layout->addWidget(penButton,3,0);
    layout->setAlignment(penButton, Qt::AlignRight);
    layout->addWidget(handButton,3,1);
    layout->setAlignment(handButton, Qt::AlignLeft);

    layout->addWidget(pencilButton,4,0);
    layout->setAlignment(pencilButton, Qt::AlignRight);
    layout->addWidget(bucketButton,4,1);
    layout->setAlignment(bucketButton, Qt::AlignLeft);

    layout->addWidget(eyedropperButton,5,0);
    layout->setAlignment(eyedropperButton, Qt::AlignRight);
    layout->addWidget(eraserButton,5,1);
    layout->setAlignment(eraserButton, Qt::AlignLeft);

    toolGroup->setLayout(layout);
    toolGroup->setMaximumHeight(6*32+1);

    this->setMaximumHeight(200);

    connect(pencilButton, SIGNAL(clicked()), this, SLOT(pencilOn()));
    connect(eraserButton, SIGNAL(clicked()), this, SLOT(eraserOn()));
    connect(selectButton, SIGNAL(clicked()), this, SLOT(selectOn()));
    connect(moveButton, SIGNAL(clicked()), this, SLOT(moveOn()));
    connect(penButton, SIGNAL(clicked()), this, SLOT(penOn()));
    connect(handButton, SIGNAL(clicked()), this, SLOT(handOn()));
    connect(polylineButton, SIGNAL(clicked()), this, SLOT(polylineOn()));
    connect(bucketButton, SIGNAL(clicked()), this, SLOT(bucketOn()));
    connect(eyedropperButton, SIGNAL(clicked()), this, SLOT(eyedropperOn()));
    connect(colouringButton, SIGNAL(clicked()), this, SLOT(brushOn()));
    connect(smudgeButton, SIGNAL(clicked()), this, SLOT(smudgeOn()));

    // pass to editor
    connect(clearButton, SIGNAL(clicked()), this, SIGNAL(clearButtonClicked()));
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
    m_editor->toolManager()->setCurrentTool( PENCIL );

    deselectAllTools();
    pencilButton->setChecked(true);
}

void ToolBoxWidget::eraserOn()
{
    m_editor->toolManager()->setCurrentTool( ERASER );

    deselectAllTools();
    eraserButton->setChecked(true);
}

void ToolBoxWidget::selectOn()
{
    m_editor->toolManager()->setCurrentTool( SELECT );

    deselectAllTools();
    selectButton->setChecked(true);
}

void ToolBoxWidget::moveOn()
{
    m_editor->toolManager()->setCurrentTool( MOVE );

    deselectAllTools();
    moveButton->setChecked(true);
}

void ToolBoxWidget::penOn()
{
    m_editor->toolManager()->setCurrentTool( PEN );

    deselectAllTools();
    penButton->setChecked(true);
}

void ToolBoxWidget::handOn()
{
    m_editor->toolManager()->setCurrentTool( HAND );
    
    deselectAllTools();
    handButton->setChecked(true);
}

void ToolBoxWidget::polylineOn()
{
    m_editor->toolManager()->setCurrentTool( POLYLINE );

    deselectAllTools();
    polylineButton->setChecked(true);
}

void ToolBoxWidget::bucketOn()
{
    m_editor->toolManager()->setCurrentTool( BUCKET );

    deselectAllTools();
    bucketButton->setChecked(true);
}

void ToolBoxWidget::eyedropperOn()
{
    m_editor->toolManager()->setCurrentTool( EYEDROPPER );

    deselectAllTools();
    eyedropperButton->setChecked(true);
}

void ToolBoxWidget::brushOn()
{
    m_editor->toolManager()->setCurrentTool( BRUSH );
  
    deselectAllTools();
    colouringButton->setChecked(true);
}

void ToolBoxWidget::smudgeOn()
{
    m_editor->toolManager()->setCurrentTool( SMUDGE );

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

void ToolBoxWidget::setCurrentTool( ToolType toolType )
{
    switch(toolType)
    {
    case PENCIL:
        emit pencilOn();
        break;
    case ERASER:
        emit eraserOn();
        break;
    case SELECT:
        emit selectOn();
        break;
    case MOVE:
        emit moveOn();
        break;
    case HAND:
        emit handOn();
        break;
    case SMUDGE:
        emit smudgeOn();
        break;
    case PEN:
        emit penOn();
        break;
    case POLYLINE:
        emit polylineOn();
        break;
    case BUCKET:
        emit bucketOn();
        break;
    case EYEDROPPER:
        emit eyedropperOn();
        break;
    case BRUSH:
        emit brushOn();
        break;
    default:
        break;
    }
}
