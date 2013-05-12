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



// ----------------------------------------------------------------------------------

ToolSet::ToolSet()
{
    drawPalette = new QDockWidget(tr("Tools"));

    QFrame* drawGroup = new QFrame();
    drawPalette->setWidget(drawGroup);
    QGridLayout* drawLay = new QGridLayout();

    optionPalette = new ToolOptionDockWidget(this);

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
    newToolButton(magnifyButton);
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
    magnifyButton->setWhatsThis("Zoom Tool (Z)");
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

    magnifyButton->setIcon(QIcon(":icons/magnify.png"));
    magnifyButton->setToolTip("Zoom Tool <b>(Z)</b>: Adjust the zoom level");
    magnifyButton->setEnabled(false);

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

    drawLay->setMargin(2);
    drawLay->setSpacing(0);

    drawLay->addWidget(moveButton,0,0);
    drawLay->setAlignment(moveButton, Qt::AlignRight);
    drawLay->addWidget(clearButton,0,1);
    drawLay->setAlignment(clearButton, Qt::AlignLeft);

    drawLay->addWidget(selectButton,1,0);
    drawLay->setAlignment(selectButton, Qt::AlignRight);
    drawLay->addWidget(colouringButton,1,1);
    drawLay->setAlignment(colouringButton, Qt::AlignLeft);

    drawLay->addWidget(polylineButton,2,0);
    drawLay->setAlignment(polylineButton, Qt::AlignRight);
    drawLay->addWidget(smudgeButton,2,1);
    drawLay->setAlignment(smudgeButton, Qt::AlignLeft);

    drawLay->addWidget(penButton,3,0);
    drawLay->setAlignment(penButton, Qt::AlignRight);
    drawLay->addWidget(handButton,3,1);
    drawLay->setAlignment(handButton, Qt::AlignLeft);

    drawLay->addWidget(pencilButton,4,0);
    drawLay->setAlignment(pencilButton, Qt::AlignRight);
    drawLay->addWidget(bucketButton,4,1);
    drawLay->setAlignment(bucketButton, Qt::AlignLeft);

    drawLay->addWidget(eyedropperButton,5,0);
    drawLay->setAlignment(eyedropperButton, Qt::AlignRight);
    drawLay->addWidget(eraserButton,5,1);
    drawLay->setAlignment(eraserButton, Qt::AlignLeft);

    drawGroup->setLayout(drawLay);  
    drawGroup->setMaximumHeight(6*32+1);
    drawPalette->setMaximumHeight(200);

    connect(pencilButton, SIGNAL(clicked()), this, SIGNAL(pencilClick()));
    connect(selectButton, SIGNAL(clicked()), this, SIGNAL(selectClick()));
    connect(moveButton, SIGNAL(clicked()), this, SIGNAL(moveClick()));
    connect(handButton, SIGNAL(clicked()), this, SIGNAL(handClick()));
    connect(eraserButton, SIGNAL(clicked()), this, SIGNAL(eraserClick()));
    connect(penButton, SIGNAL(clicked()), this, SIGNAL(penClick()));
    connect(polylineButton, SIGNAL(clicked()), this, SIGNAL(polylineClick()));
    connect(bucketButton, SIGNAL(clicked()), this, SIGNAL(bucketClick()));
    connect(eyedropperButton, SIGNAL(clicked()), this, SIGNAL(eyedropperClick()));
    connect(colouringButton, SIGNAL(clicked()), this, SIGNAL(colouringClick()));
    connect(smudgeButton, SIGNAL(clicked()), this, SIGNAL(smudgeClick()));
    connect(clearButton, SIGNAL(clicked()), this, SIGNAL(clearClick()));

    connect(pencilButton, SIGNAL(clicked()), this, SLOT(changePencilButton()));
    connect(selectButton, SIGNAL(clicked()), this, SLOT(changeSelectButton()));
    connect(moveButton, SIGNAL(clicked()), this, SLOT(changeMoveButton()));
    connect(handButton, SIGNAL(clicked()), this, SLOT(changeHandButton()));
    connect(eraserButton, SIGNAL(clicked()), this, SLOT(changeEraserButton()));
    connect(penButton, SIGNAL(clicked()), this, SLOT(changePenButton()));
    connect(polylineButton, SIGNAL(clicked()), this, SLOT(changePolylineButton()));
    connect(bucketButton, SIGNAL(clicked()), this, SLOT(changeBucketButton()));
    connect(eyedropperButton, SIGNAL(clicked()), this, SLOT(changeEyedropperButton()));
    connect(colouringButton, SIGNAL(clicked()), this, SLOT(changeColouringButton()));
    connect(smudgeButton, SIGNAL(clicked()), this, SLOT(changeSmudgeButton()));
}

void ToolSet::newToolButton(QToolButton*& toolButton)
{
    toolButton = new QToolButton(this);
    toolButton->setAutoRaise(true);
    toolButton->setIconSize( QSize(24,24) );
    toolButton->setFixedSize(32,32);
}

void ToolSet::setWidth(qreal x)
{
    if(x < 0)
    {
        optionPalette->sizeSlider->setEnabled(false);
    }
    else
    {
        optionPalette->sizeSlider->setEnabled(true);
        optionPalette->sizeSlider->setValue(x);
    }
}

void ToolSet::setFeather(qreal x)
{
    if(x < 0)
    {
        optionPalette->featherSlider->setEnabled(false);
    }
    else
    {
        optionPalette->featherSlider->setEnabled(true);
        optionPalette->featherSlider->setValue(x);
    }
}

void ToolSet::setOpacity(qreal x)
{
    if(x < 0)
    {
        //optionPalette->opacitySlider->setEnabled(false);
    }
    else
    {
        //optionPalette->opacitySlider->setEnabled(true);
        //optionPalette->opacitySlider->setValue(x);
    }
}

void ToolSet::setPressure(int x)   // x = -1, 0, 1
{
    if(x<0)
    {
        optionPalette->usePressureBox->setEnabled(false);
    }
    else
    {
        optionPalette->usePressureBox->setEnabled(true);
        optionPalette->usePressureBox->setChecked(x>0);
    }
}

void ToolSet::setInvisibility(int x)   // x = -1, 0, 1
{
    if(x<0)
    {
        optionPalette->makeInvisibleBox->setEnabled(false);
    }
    else
    {
        optionPalette->makeInvisibleBox->setEnabled(true);
        optionPalette->makeInvisibleBox->setChecked(x>0);
    }
}

void ToolSet::setPreserveAlpha(int x)   // x = -1, 0, 1
{
    qDebug() << "Setting - Preserve Alpha=" << x;
    if ( x < 0 )
    {
        optionPalette->preserveAlphaBox->setEnabled(false);
    }
    else
    {
        optionPalette->preserveAlphaBox->setEnabled(true);
        optionPalette->preserveAlphaBox->setChecked(x > 0);
    }
}

void ToolSet::setFollowContour(int x)   // x = -1, 0, 1
{
    if (x < 0)
    {
        optionPalette->followContourBox->setEnabled(false);
    }
    else
    {
        optionPalette->followContourBox->setEnabled(true);
        optionPalette->followContourBox->setChecked(x>0);
    }
}

void ToolSet::setColour(QColor x)
{
    QPixmap colourSwatch(30,30);
    colourSwatch.fill(x);
    optionPalette->choseColour->setIcon(colourSwatch);
}

void ToolSet::changePencilButton()
{
    deselectAllTools();
    pencilButton->setChecked(true);
}

void ToolSet::changeEraserButton()
{
    deselectAllTools();
    eraserButton->setChecked(true);
}

void ToolSet::changeSelectButton()
{
    deselectAllTools();
    selectButton->setChecked(true);
}

void ToolSet::changeMoveButton()
{
    deselectAllTools();
    moveButton->setChecked(true);
}

void ToolSet::changeHandButton()
{
    deselectAllTools();
    handButton->setChecked(true);
}

void ToolSet::changePenButton()
{
    deselectAllTools();
    penButton->setChecked(true);
}

void ToolSet::changePolylineButton()
{
    deselectAllTools();
    polylineButton->setChecked(true);
}

void ToolSet::changeBucketButton()
{
    deselectAllTools();
    bucketButton->setChecked(true);
}

void ToolSet::changeEyedropperButton()
{
    deselectAllTools();
    eyedropperButton->setChecked(true);
}

void ToolSet::changeColouringButton()
{
    deselectAllTools();
    colouringButton->setChecked(true);
}

void ToolSet::changeSmudgeButton()
{
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

