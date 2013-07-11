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
#include <QtGui>
#include <math.h>
#include "spinslider.h"
#include "tooloptiondockwidget.h"
#include "toolset.h"
#include "editor.h"
#include "basetool.h"

// ----------------------------------------------------------------------------------

ToolSet::ToolSet(const QString title, Editor* editor) :
    QDockWidget(title, editor)
{
    m_editor = editor;    

    QFrame* toolGroup = new QFrame();
    setWidget(toolGroup);

    QGridLayout* layout = new QGridLayout();

    newToolButton(pencilButton);
    newToolButton(selectButton);
    newToolButton(moveButton);
    newToolButton(handButton);
    newToolButton(penButton);
    newToolButton(eraserButton);
    newToolButton(polylineButton);
    newToolButton(bucketButton);
    newToolButton(colouringButton);
    newToolButton(eyedropperButton);
    newToolButton(clearButton);
    //newToolButton(magnifyButton);
    newToolButton(smudgeButton);

    pencilButton->setWhatsThis("Pencil Tool (N)");
    selectButton->setWhatsThis("Select Tool (V)");
    moveButton->setWhatsThis("Move Tool (Q)");
    handButton->setWhatsThis("Hand Tool (H)");
    penButton->setWhatsThis("Pen Tool (P)");
    eraserButton->setWhatsThis("Eraser Tool (E)");
    polylineButton->setWhatsThis("Polyline Tool (Y)");
    bucketButton->setWhatsThis("Paint Bucket Tool(K)");
    colouringButton->setWhatsThis("Brush Tool(B)");
    eyedropperButton->setWhatsThis("Eyedropper Tool (I)");
    clearButton->setWhatsThis("Clear Tool");
    //magnifyButton->setWhatsThis("Zoom Tool (Z)");
    smudgeButton->setWhatsThis("Smudge Tool (A)");

    pencilButton->setIcon(QIcon(":icons/pencil2.png"));
    pencilButton->setToolTip("Pencil Tool <b>(N)</b>: Sketch with pencil");

    selectButton->setIcon(QIcon(":icons/select.png"));
    selectButton->setToolTip("Select Tool <b>(V)</b>: Select an object");

    moveButton->setIcon(QIcon(":icons/arrow.png"));
    moveButton->setToolTip("Move Tool <b>(Q)</b>: Move an object");

    handButton->setIcon(QIcon(":icons/hand.png"));
    handButton->setToolTip("Hand Tool <b>(H)</b>: Move the canvas");

    penButton->setIcon(QIcon(":icons/pen.png"));
    penButton->setToolTip("Pen Tool <b>(P)</b>: Sketch with pen");

    eraserButton->setIcon(QIcon(":icons/eraser.png"));
    eraserButton->setToolTip("Eraser Tool <b>(E)</b>: Erase");

    polylineButton->setIcon(QIcon(":icons/polyline.png"));
    polylineButton->setToolTip("Polyline Tool <b>(Y)</b>: Create line/curves");

    bucketButton->setIcon(QIcon(":icons/bucket.png"));
    bucketButton->setToolTip("Paint Bucket Tool <b>(K)</b>: Fill selected area with a color");

    colouringButton->setIcon(QIcon(":icons/brush.png"));
    colouringButton->setToolTip("Brush Tool <b>(B)</b>: Paint smooth stroke with a brush");

    eyedropperButton->setIcon(QIcon(":icons/eyedropper.png"));
    eyedropperButton->setToolTip("Eyedropper Tool <b>(I)</b>: Set color from the stage");

    //magnifyButton->setIcon(QIcon(":icons/magnify.png"));
    //magnifyButton->setToolTip("Zoom Tool <b>(Z)</b>: Adjust the zoom level");
    //magnifyButton->setEnabled(false);

    smudgeButton->setIcon(QIcon(":icons/smudge.png"));
    smudgeButton->setToolTip("Smudge Tool <b>(A)</b>: Edit polyline/curves");
    smudgeButton->setEnabled(true);

    clearButton->setIcon(QIcon(":icons/clear.png"));
    clearButton->setToolTip("Clear Tool <b>(L)</b>: Erases content of selected frame");

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

    connect(clearButton, SIGNAL(clicked()), this, SIGNAL(clearButtonClicked()));
}

void ToolSet::newToolButton(QToolButton*& toolButton)
{
    toolButton = new QToolButton(this);
    toolButton->setAutoRaise(true);
    toolButton->setIconSize( QSize(24,24) );
    toolButton->setFixedSize(32, 32);
}

void ToolSet::pencilOn()
{
    m_editor->getScribbleArea()->setCurrentTool( PENCIL );

    // --- change properties ---
    BaseTool* pCurTool = m_editor->getScribbleArea()->currentTool();

    m_editor->setWidth(pCurTool->properties.width);
    m_editor->setFeather(pCurTool->properties.feather);
    m_editor->setFeather( -1 ); // by definition the pencil has no feather
    m_editor->setPressure(pCurTool->properties.pressure);
    m_editor->setPreserveAlpha(pCurTool->properties.preserveAlpha);
    m_editor->setFollowContour(-1);
    m_editor->setInvisibility(-1); // by definition the pencil is invisible in vector mode

    deselectAllTools();
    pencilButton->setChecked(true);
}

void ToolSet::eraserOn()
{
    m_editor->getScribbleArea()->setCurrentTool( ERASER );
    BaseTool* pCurrentTool = m_editor->getScribbleArea()->currentTool();

    // --- change properties ---
    m_editor->setWidth(pCurrentTool->properties.width);
    m_editor->setFeather(pCurrentTool->properties.feather);
    m_editor->setPressure(pCurrentTool->properties.pressure);
    m_editor->setPreserveAlpha(0);
    m_editor->setInvisibility(0);

    m_editor->setFeather(-1);
    m_editor->setPreserveAlpha(-1);
    m_editor->setFollowContour(-1);
    m_editor->setInvisibility(-1);

    deselectAllTools();
    eraserButton->setChecked(true);
}

void ToolSet::selectOn()
{
    m_editor->getScribbleArea()->setCurrentTool( SELECT );

    // --- change properties ---
    m_editor->setWidth(-1);
    m_editor->setFeather(-1);
    m_editor->setPressure(-1);
    m_editor->setInvisibility(-1);
    m_editor->setPreserveAlpha(-1);
    m_editor->setFollowContour(-1);

    deselectAllTools();
    selectButton->setChecked(true);
}

void ToolSet::moveOn()
{
    m_editor->getScribbleArea()->setCurrentTool( MOVE );
    // --- change properties ---
    m_editor->setWidth(-1);
    m_editor->setFeather(-1);
    m_editor->setPressure(-1);
    m_editor->setInvisibility(-1);
    m_editor->setPreserveAlpha(-1);
    m_editor->setFollowContour(-1);

    deselectAllTools();
    moveButton->setChecked(true);
}

void ToolSet::penOn()
{
    m_editor->getScribbleArea()->setCurrentTool( PEN );

    BaseTool* pCurrentTool = m_editor->getScribbleArea()->currentTool();

    // --- change properties ---
    m_editor->setToolProperties( pCurrentTool->properties );
    m_editor->setWidth(2);

    deselectAllTools();
    penButton->setChecked(true);
}

void ToolSet::handOn()
{
    BaseTool* pCurrentTool = m_editor->getScribbleArea()->currentTool();
    if (pCurrentTool->type() == HAND)
    {
        m_editor->getScribbleArea()->resetView();
    }

     m_editor->getScribbleArea()->setCurrentTool( HAND );
    // --- change properties ---
    m_editor->setWidth(-1);
    m_editor->setFeather(-1);
    m_editor->setPressure(-1);
    m_editor->setInvisibility(-1);
    m_editor->setPreserveAlpha(-1);
    m_editor->setFollowContour(-1);

    deselectAllTools();
    handButton->setChecked(true);
}

void ToolSet::polylineOn()
{
     m_editor->getScribbleArea()->setCurrentTool( POLYLINE );
    // --- change properties ---

     BaseTool* pCurrentTool = m_editor->getScribbleArea()->currentTool();

     m_editor->setWidth(pCurrentTool->properties.width);
     m_editor->setFeather(-1);
     m_editor->setPressure(pCurrentTool->properties.pressure);
     m_editor->setInvisibility(pCurrentTool->properties.invisibility);
     m_editor->setPreserveAlpha(pCurrentTool->properties.preserveAlpha);
     m_editor->setFollowContour(-1);

     deselectAllTools();
     polylineButton->setChecked(true);
}

void ToolSet::bucketOn()
{
     m_editor->getScribbleArea()->setCurrentTool( BUCKET );
     BaseTool* pCurrentTool = m_editor->getScribbleArea()->currentTool();

    // --- change properties ---

    m_editor->setWidth(-1);
    m_editor->setFeather(pCurrentTool->properties.feather);
    m_editor->setFeather(-1);
    m_editor->setPressure(0);
    m_editor->setPressure(-1); // disable the button
    m_editor->setInvisibility(0);
    m_editor->setInvisibility(-1); // disable the button
    m_editor->setPreserveAlpha(0);
    m_editor->setPreserveAlpha(-1); // disable the button
    m_editor->setFollowContour(-1);

    deselectAllTools();
    bucketButton->setChecked(true);
}

void ToolSet::eyedropperOn()
{
     m_editor->getScribbleArea()->setCurrentTool( EYEDROPPER );
    // --- change properties ---
    m_editor->setWidth(-1);
    m_editor->setFeather(-1);
    m_editor->setPressure(-1);
    m_editor->setInvisibility(0);
    m_editor->setInvisibility(-1);
    m_editor->setPreserveAlpha(0);
    m_editor->setPreserveAlpha(-1);
    m_editor->setFollowContour(-1);

    deselectAllTools();
    eyedropperButton->setChecked(true);
}

void ToolSet::brushOn()
{
    m_editor->getScribbleArea()->setCurrentTool( BRUSH );
    BaseTool* pCurrentTool = m_editor->getScribbleArea()->currentTool();
    // --- change properties ---

    m_editor->setToolProperties(pCurrentTool->properties);
    m_editor->setInvisibility(-1);
    //m_pEditor->setFollowContour(followContour); //TODO: clarily what is this?

    deselectAllTools();
    colouringButton->setChecked(true);
}

void ToolSet::smudgeOn()
{
     m_editor->getScribbleArea()->setCurrentTool( EDIT );
    // --- change properties ---
    m_editor->setWidth(-1);
    m_editor->setFeather(-1);
    m_editor->setPressure(-1);
    m_editor->setInvisibility(0);
    m_editor->setInvisibility(-1);
    m_editor->setPreserveAlpha(0);
    m_editor->setPreserveAlpha(-1);
    m_editor->setFollowContour(-1);

    deselectAllTools();
    smudgeButton->setChecked(true);
}

void ToolSet::deselectAllTools()
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

void ToolSet::setCurrentTool( ToolType toolType )
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


