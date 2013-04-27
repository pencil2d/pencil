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

#include "toolset.h"



// ----------------------------------------------------------------------------------

ToolSet::ToolSet()
{
    drawPalette = new QDockWidget(tr("Tools"));

    QFrame* drawGroup = new QFrame();
    drawPalette->setWidget(drawGroup);
    QGridLayout* drawLay = new QGridLayout();

    displayPalette = createDisplayPalette();
    optionPalette = createOptionPalette();

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

    connect(thinLinesButton, SIGNAL(clicked()), this, SIGNAL(thinLinesClick()));
    connect(outlinesButton, SIGNAL(clicked()), this, SIGNAL(outlinesClick()));

    connect(usePressureBox, SIGNAL(clicked(bool)), this, SLOT(pressureClick(bool)));
    connect(makeInvisibleBox, SIGNAL(clicked(bool)), this, SLOT(invisibleClick(bool)));
    connect(preserveAlphaBox, SIGNAL(clicked(bool)), this, SLOT(preserveAlphaClick(bool)));
    connect(followContourBox, SIGNAL(clicked(bool)), this, SLOT(followContourClick(bool)));
    connect(sizeSlider, SIGNAL(valueChanged(qreal)), this, SIGNAL(widthClick(qreal)));
    connect(featherSlider, SIGNAL(valueChanged(qreal)), this, SIGNAL(featherClick(qreal)));
    connect(opacitySlider, SIGNAL(valueChanged(qreal)), this, SIGNAL(opacityClick(qreal)));

    // -- mj
    connect(onionPrev, SIGNAL(clicked(bool)), this, SIGNAL(togglePrev(bool)));
    connect(onionNext, SIGNAL(clicked(bool)), this, SIGNAL(toggleNext(bool)));
    connect(choseColour, SIGNAL(clicked()), this, SIGNAL(colourClick()));
    connect(clearButton, SIGNAL(clicked()), this, SIGNAL(clearClick()));
    connect(mirrorButton, SIGNAL(clicked()), this, SIGNAL(mirrorClick()));
    connect(mirrorButtonV, SIGNAL(clicked()), this, SIGNAL(mirrorVClick()));

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

QDockWidget* ToolSet::createOptionPalette()
{
    QFrame* optionGroup = new QFrame();
    QGridLayout* optionLay = new QGridLayout();
    optionLay->setMargin(8);
    optionLay->setSpacing(8);

    QSettings settings("Pencil","Pencil");

    QLabel* colourLabel = new QLabel();
    colourLabel->setText(tr("Color:"));
    colourLabel->setFont( QFont("Helvetica", 10) );

    QPixmap colourSwatch(30,30);
    colourSwatch.fill(Qt::black);
    choseColour = new QToolButton(this);
    choseColour->setIcon(colourSwatch);
    choseColour->setToolTip("Display Colors");

    sizeSlider = new SpinSlider("Size", "log", "real", 0.2, 200.0, this);
    sizeSlider->setValue(settings.value("pencilWidth").toDouble());
    sizeSlider->setToolTip("Set Pen Width");

    featherSlider = new SpinSlider("Feather", "log", "real", 0.2, 200.0, this);
    featherSlider->setValue(settings.value("pencilFeather").toDouble());

    opacitySlider = new SpinSlider("Opacity", "linear", "real", 0.0, 1.0, this);
    opacitySlider->setValue(settings.value("pencilOpacity").toDouble());

    usePressureBox = new QCheckBox("Pressure");
    usePressureBox->setToolTip("Size with pressure");
    usePressureBox->setFont( QFont("Helvetica", 10) );
    usePressureBox->setChecked(true);

    makeInvisibleBox = new QCheckBox("Invisible");
    makeInvisibleBox->setToolTip("Make invisible");
    makeInvisibleBox->setFont( QFont("Helvetica", 10) );
    makeInvisibleBox->setChecked(false);

    preserveAlphaBox = new QCheckBox("Alpha");
    preserveAlphaBox->setToolTip("Preserve Alpha");
    preserveAlphaBox->setFont( QFont("Helvetica", 10) );
    preserveAlphaBox->setChecked(false);

    followContourBox = new QCheckBox("Contours");
    followContourBox->setToolTip("Stop at contours");
    followContourBox->setFont( QFont("Helvetica", 10) );
    followContourBox->setChecked(false);

    optionLay->addWidget(colourLabel,6,0);
    optionLay->addWidget(choseColour,6,1);

    optionLay->addWidget(sizeSlider,8,0,1,2);
    optionLay->addWidget(featherSlider,9,0,1,2);

    optionLay->addWidget(usePressureBox,11,0,1,2);
    optionLay->addWidget(preserveAlphaBox,12,0,1,2);
    optionLay->addWidget(followContourBox,13,0,1,2);
    optionLay->addWidget(makeInvisibleBox,14,0,1,2);
    optionLay->setRowStretch(15,1);

    optionGroup->setLayout(optionLay);

    QDockWidget* dockWidget = new QDockWidget(tr("Options"));
    dockWidget->setWidget(optionGroup);
    return dockWidget;
}

QDockWidget* ToolSet::createDisplayPalette()
{
    // Create Display Option Tool Buttons
    QFrame* displayGroup = new QFrame();

    thinLinesButton = new QToolButton(displayGroup);
    thinLinesButton->setIcon(QIcon(":icons/thinlines5.png"));
    thinLinesButton->setToolTip("Show invisible lines");
    thinLinesButton->setIconSize( QSize(21,21) );

    outlinesButton = new QToolButton(displayGroup);
    outlinesButton->setIcon(QIcon(":icons/outlines5.png"));
    outlinesButton->setToolTip("Show outlines only");
    outlinesButton->setIconSize( QSize(21,21) );

    mirrorButton = new QToolButton(displayGroup);
    mirrorButton->setIcon(QIcon(":icons/mirror.png"));
    mirrorButton->setToolTip("Horizontal flip");
    mirrorButton->setIconSize( QSize(21,21) );

    mirrorButtonV = new QToolButton(displayGroup);
    mirrorButtonV->setIcon(QIcon(":icons/mirrorV.png"));
    mirrorButtonV->setToolTip("Vertical flip");
    mirrorButtonV->setIconSize( QSize(21,21) );

    onionPrev = new QToolButton(displayGroup);
    onionPrev->setIcon(QIcon(":icons/onionPrev.png"));
    onionPrev->setToolTip("Onion skin previous frame");
    onionPrev->setIconSize( QSize(21,21) );

    onionNext = new QToolButton(displayGroup);
    onionNext->setIcon(QIcon(":icons/onionNext.png"));
    onionNext->setToolTip("Onion skin next frame");
    onionNext->setIconSize( QSize(21,21) );

    thinLinesButton->setCheckable(true);
    thinLinesButton->setChecked(false);
    outlinesButton->setCheckable(true);
    outlinesButton->setChecked(false);
    mirrorButton->setCheckable(true);
    mirrorButton->setChecked(false);
    mirrorButtonV->setCheckable(true);
    mirrorButtonV->setChecked(false);
    onionPrev->setCheckable(true);
    onionPrev->setChecked(true);
    onionNext->setCheckable(true);
    onionNext->setChecked(false);

    QGridLayout* displayLay = new QGridLayout();
    displayLay->setMargin(4);
    displayLay->setSpacing(0);
    displayLay->addWidget(mirrorButton,0,0);
    displayLay->addWidget(thinLinesButton,0,1);
    displayLay->addWidget(outlinesButton,1,1);
    displayLay->addWidget(mirrorButtonV,1,0);
    displayLay->addWidget(onionPrev,0,2);
    displayLay->addWidget(onionNext,1,2);

    displayGroup->setLayout(displayLay);

    QDockWidget* dockWidget = new QDockWidget(tr("Display Options"));
    dockWidget->setWidget(displayGroup);
    dockWidget->setMaximumHeight(60);
    return dockWidget;
}

void ToolSet::newToolButton(QToolButton*& toolButton)
{
    toolButton = new QToolButton(this);
    toolButton->setAutoRaise(true);
    toolButton->setIconSize( QSize(24,24) );
    toolButton->setFixedSize(32,32);
}

void ToolSet::setCounter(int x)
{
    //framecounter->setText(QString::number(x));
}

void ToolSet::setWidth(qreal x)
{
    if(x < 0)
    {
        sizeSlider->setEnabled(false);
    }
    else
    {
        sizeSlider->setEnabled(true);
        sizeSlider->setValue(x);
    }
}

void ToolSet::setFeather(qreal x)
{
    if(x < 0)
    {
        featherSlider->setEnabled(false);
    }
    else
    {
        featherSlider->setEnabled(true);
        featherSlider->setValue(x);
    }
}

void ToolSet::setOpacity(qreal x)
{
    if(x < 0)
    {
        opacitySlider->setEnabled(false);
    }
    else
    {
        opacitySlider->setEnabled(true);
        opacitySlider->setValue(x);
    }
}

void ToolSet::setPressure(int x)   // x = -1, 0, 1
{
    if(x<0)
    {
        usePressureBox->setEnabled(false);
    }
    else
    {
        usePressureBox->setEnabled(true);
        usePressureBox->setChecked(x>0);
    }
}

void ToolSet::pressureClick(bool x)
{
    int y = 0;
    if(x) y = 1;
    emit pressureClick(y);
}

void ToolSet::setInvisibility(int x)   // x = -1, 0, 1
{
    if(x<0)
    {
        makeInvisibleBox->setEnabled(false);
    }
    else
    {
        makeInvisibleBox->setEnabled(true);
        makeInvisibleBox->setChecked(x>0);
    }
}

void ToolSet::invisibleClick(bool x)
{
    int y = 0;
    if(x) y = 1;
    emit invisibleClick(y);
}

void ToolSet::setPreserveAlpha(int x)   // x = -1, 0, 1
{
    if(x<0)
    {
        preserveAlphaBox->setEnabled(false);
    }
    else
    {
        preserveAlphaBox->setEnabled(true);
        preserveAlphaBox->setChecked(x>0);
    }
}

void ToolSet::preserveAlphaClick(bool x)
{
    int y = 0;
    if(x) y = 1;
    emit preserveAlphaClick(y);
}

void ToolSet::setFollowContour(int x)   // x = -1, 0, 1
{
    if(x<0)
    {
        followContourBox->setEnabled(false);
    }
    else
    {
        followContourBox->setEnabled(true);
        followContourBox->setChecked(x>0);
    }
}

void ToolSet::followContourClick(bool x)
{
    int y = 0;
    if(x) y = 1;
    emit followContourClick(y);
}

void ToolSet::setColour(QColor x)
{
    QPixmap colourSwatch(30,30);
    colourSwatch.fill(x);
    choseColour->setIcon(colourSwatch);
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

void ToolSet::changeOutlinesButton(bool trueOrFalse)
{
    outlinesButton->setChecked(trueOrFalse);
}


void ToolSet::changeThinLinesButton(bool trueOrFalse)
{
    thinLinesButton->setChecked(trueOrFalse);
}


void ToolSet::resetMirror()
{
    mirrorButton->setChecked(false);
}

void ToolSet::resetMirrorV()
{
    mirrorButtonV->setChecked(false);
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

void ToolSet::onionPrevChanged(bool checked)
{
    onionPrev->setChecked(checked);
}

void ToolSet::onionNextChanged(bool checked)
{
    onionNext->setChecked(checked);
}
