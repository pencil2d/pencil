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

#include <QImage>
#include <QPoint>
#include <QWidget>
#include <QScrollBar>

#include "editor.h"
#include "toolset.h"
#include "timecontrols.h"
#include "timelinecells.h"
#include "timeline.h"

TimeLine::TimeLine(QWidget* parent, Editor* editor) : QDockWidget(parent, Qt::Tool) // DockPalette("")
{
    QWidget* timeLineContent = new QWidget(this);

    list = new TimeLineCells(this, editor, "layers");
    cells = new TimeLineCells(this, editor, "tracks");
    connect(list, SIGNAL(mouseMovedY(int)), list, SLOT(setMouseMoveY(int)));
    connect(list, SIGNAL(mouseMovedY(int)), cells, SLOT(setMouseMoveY(int)));

    numberOfLayers = 0;
    hScrollBar = new QScrollBar(Qt::Horizontal);
    vScrollBar = new QScrollBar(Qt::Vertical);
    vScrollBar->setMinimum(0);
    vScrollBar->setMaximum(1);
    vScrollBar->setPageStep(1);
    updateLength(getFrameLength());

    QWidget* leftWidget = new QWidget();
    leftWidget->setMinimumWidth(120);
    QWidget* rightWidget = new QWidget();

    QWidget* leftToolBar = new QWidget();
    leftToolBar->setFixedHeight(31);
    QWidget* rightToolBar = new QWidget();
    rightToolBar->setFixedHeight(31);

    // --- left widget ---
    // --------- layer buttons ---------
    //QFrame* layerButtons = new QFrame(this);
    QToolBar* layerButtons = new QToolBar(this);
    //QHBoxLayout* layerButtonLayout = new QHBoxLayout();
    QLabel* layerLabel = new QLabel(tr("Layers:"));
    layerLabel->setIndent(5);
    layerLabel->setFont( QFont("Helvetica", 10) );
    QToolButton* addLayerButton = new QToolButton(this);
    addLayerButton->setIcon(QIcon(":icons/add.png"));
    addLayerButton->setToolTip("Add Layer");
    addLayerButton->setFixedSize(24,24);
    QToolButton* removeLayerButton = new QToolButton(this);
    removeLayerButton->setIcon(QIcon(":icons/remove.png"));
    removeLayerButton->setToolTip("Remove Layer");
    removeLayerButton->setFixedSize(24,24);
    /*layerButtonLayout->addWidget(layerLabel);
    layerButtonLayout->addWidget(addLayerButton);
    layerButtonLayout->addWidget(removeLayerButton);
    layerButtonLayout->setMargin(0);
    layerButtonLayout->setSpacing(5);
    layerButtons->setLayout(layerButtonLayout);*/
    layerButtons->addWidget(layerLabel);
    layerButtons->addWidget(addLayerButton);
    layerButtons->addWidget(removeLayerButton);

    QHBoxLayout* leftToolBarLayout = new QHBoxLayout();
    leftToolBarLayout->setAlignment(Qt::AlignLeft);
    leftToolBarLayout->setMargin(0);
    leftToolBarLayout->addWidget(layerButtons);
    leftToolBar->setLayout(leftToolBarLayout);

    QAction* newBitmapLayerAct = new QAction(QIcon(":icons/layer-bitmap.png"), tr("New Bitmap Layer"), this);
    QAction* newVectorLayerAct = new QAction(QIcon(":icons/layer-vector.png"), tr("New Vector Layer"), this);
    QAction* newSoundLayerAct = new QAction(QIcon(":icons/layer-sound.png"), tr("New Sound Layer"), this);
    QAction* newCameraLayerAct = new QAction(QIcon(":icons/layer-camera.png"), tr("New Camera Layer"), this);
    QMenu* layerMenu = new QMenu(tr("&Layer"), this);
    layerMenu->addAction(newBitmapLayerAct);
    layerMenu->addAction(newVectorLayerAct);
    layerMenu->addAction(newSoundLayerAct);
    layerMenu->addAction(newCameraLayerAct);
    addLayerButton->setMenu(layerMenu);
    addLayerButton->setPopupMode(QToolButton::InstantPopup);

    QGridLayout* leftLayout = new QGridLayout();
    leftLayout->addWidget(leftToolBar,0,0);
    leftLayout->addWidget(list,1,0);
    leftLayout->setMargin(0);
    leftLayout->setSpacing(0);
    leftWidget->setLayout(leftLayout);

    // --- right widget ---
    // --------- key buttons ---------
    //QFrame* keyButtons = new QFrame(this);
    QToolBar* keyButtons = new QToolBar(this);
    //keyButtons->setFixedWidth(90);
    //QHBoxLayout* keyButtonsLayout = new QHBoxLayout();
    QLabel* keyLabel = new QLabel(tr("Keys:"));
    keyLabel->setFont( QFont("Helvetica", 10) );
    keyLabel->setIndent(5);
    QToolButton* addKeyButton = new QToolButton(this);
    addKeyButton->setIcon(QIcon(":icons/add.png"));
    addKeyButton->setToolTip("Add Frame");
    addKeyButton->setFixedSize(24,24);
    QToolButton* removeKeyButton = new QToolButton(this);
    removeKeyButton->setIcon(QIcon(":icons/remove.png"));
    removeKeyButton->setToolTip("Remove Frame");
    removeKeyButton->setFixedSize(24,24);

    QToolButton* duplicateKeyButton = new QToolButton(this);
    duplicateKeyButton->setIcon(QIcon(":icons/controls/duplicate.png"));
    duplicateKeyButton->setToolTip("Duplicate Frame");
    duplicateKeyButton->setFixedSize(24,24);
    /*keyButtonsLayout->addWidget(keyLabel);
    keyButtonsLayout->addWidget(addKeyButton);
    keyButtonsLayout->addWidget(removeKeyButton);
    keyButtonsLayout->setMargin(0);
    keyButtonsLayout->setSpacing(0);
    //keyButtonLayout->setSizeConstraint(QLayout::SetMinimumSize);
    keyButtons->setLayout(keyButtonsLayout);*/
    keyButtons->addWidget(keyLabel);
    keyButtons->addWidget(addKeyButton);
    keyButtons->addWidget(removeKeyButton);
    keyButtons->addWidget(duplicateKeyButton);

    /*#	// --------- Onion skin buttons ---------TODO put back onion skin buttons
        //QFrame* onionButtons = new QFrame(this);
        QToolBar* onionButtons = new QToolBar(this);
        //onionButtons->setFixedWidth(90);
        //QHBoxLayout* onionButtonsLayout = new QHBoxLayout();
        QLabel* onionLabel = new QLabel(tr("Onion skin:"));
        onionLabel->setFont( QFont("Helvetica", 10) );
        onionLabel->setIndent(5);
        QToolButton* onionPrevButton = new QToolButton(this);
        onionPrevButton->setIcon(QIcon(":icons/onionPrev.png"));
        onionPrevButton->setToolTip("Show previous frame");
        onionPrevButton->setFixedSize(24,24);
        onionPrevButton->setCheckable(true);
        onionPrevButton->setChecked(true);
        QToolButton* onionNextButton = new QToolButton(this);
        onionNextButton->setIcon(QIcon(":icons/onionNext.png"));
        onionNextButton->setToolTip("Show next frame");
        onionNextButton->setFixedSize(24,24);
        onionNextButton->setCheckable(true);
        onionButtonsLayout->addWidget(onionLabel);
        onionButtonsLayout->addWidget(onionPrevButton);
        onionButtonsLayout->addWidget(onionNextButton);
        onionButtonsLayout->setMargin(0);
        onionButtonsLayout->setSpacing(0);
        //keyButtonLayout->setSizeConstraint(QLayout::SetMinimumSize);
        onionButtons->setLayout(onionButtonsLayout);
        onionButtons->addSeparator();
        onionButtons->addWidget(onionLabel);
        onionButtons->addWidget(onionPrevButton);
        onionButtons->addWidget(onionNextButton);#*/


    // --------- Time controls ---------
    timeControls = new TimeControls(this);

    QHBoxLayout* rightToolBarLayout = new QHBoxLayout();
    //rightToolBarLayout->setAlignment(Qt::AlignLeft);
    rightToolBarLayout->addWidget(keyButtons);
    //rightToolBarLayout->addWidget(onionButtons);
    rightToolBarLayout->addStretch(1);
    rightToolBarLayout->addWidget(timeControls);
    rightToolBarLayout->setMargin(0);
    rightToolBarLayout->setSpacing(0);
    rightToolBar->setLayout(rightToolBarLayout);

    QGridLayout* rightLayout = new QGridLayout();
    rightLayout->addWidget(rightToolBar,0,0);
    rightLayout->addWidget(cells,1,0);
    rightLayout->setMargin(0);
    rightLayout->setSpacing(0);
    rightWidget->setLayout(rightLayout);

    // --- Splitter ---
    QSplitter* splitter = new QSplitter(parent);
    splitter->addWidget(leftWidget);
    splitter->addWidget(rightWidget);
    splitter->setSizes( QList<int>() << 100 << 600 );
    //splitter->addWidget(cells);


    QGridLayout* lay = new QGridLayout();
    //lay->addWidget(cells,0,0);
    //lay->addWidget(toolBar,0,0);
    lay->addWidget(splitter,0,0);
    lay->addWidget(vScrollBar,0,1);
    lay->addWidget(hScrollBar,1,0);
    //lay->addWidget(vScrollBar,1,1);
    lay->setMargin(0);
    lay->setSpacing(0);
    timeLineContent->setLayout(lay);
    //timeLineContent->setBackgroundRole(QPalette::Dark);
    //timeLineContent->setForegroundRole(QPalette::Dark);
    setWidget(timeLineContent);

    setWindowFlags(Qt::WindowStaysOnTopHint);
    setWindowTitle("Timeline");
    //setWindowFlags(Qt::SubWindow);
    setFloating(true);
    //setMinimumSize(100, 300);
    //setGeometry(10,60,100, 300);

    connect(this,SIGNAL(lengthChange(QString)), cells, SLOT(lengthChange(QString)));
    connect(this,SIGNAL(fontSizeChange(int)), cells, SLOT(fontSizeChange(int)));
    connect(this,SIGNAL(frameSizeChange(int)), cells, SLOT(frameSizeChange(int)));
    connect(this,SIGNAL(labelChange(int)), cells, SLOT(labelChange(int)));
    connect(this,SIGNAL(scrubChange(int)), cells, SLOT(scrubChange(int)));

    connect(hScrollBar,SIGNAL(valueChanged(int)), cells, SLOT(hScrollChange(int)));
    connect(vScrollBar,SIGNAL(valueChanged(int)), cells, SLOT(vScrollChange(int)));
    connect(vScrollBar,SIGNAL(valueChanged(int)), list, SLOT(vScrollChange(int)));

    connect(addKeyButton, SIGNAL(clicked()), this, SIGNAL(addKeyClick()));
    connect(removeKeyButton, SIGNAL(clicked()), this, SIGNAL(removeKeyClick()));
    connect(duplicateKeyButton, SIGNAL(clicked()), this, SIGNAL(duplicateKeyClick()));

//#	connect(onionPrevButton, SIGNAL(clicked()), this, SIGNAL(onionPrevClick()));
//#	connect(onionNextButton, SIGNAL(clicked()), this, SIGNAL(onionNextClick()));//TODO It's just a signal

    connect(timeControls, SIGNAL(playClick()), this, SIGNAL(playClick()));
    connect(timeControls, SIGNAL(endClick()), this, SIGNAL(endplayClick()));
    connect(timeControls, SIGNAL(startClick()), this, SIGNAL(startplayClick()));
    connect(timeControls, SIGNAL(loopClick(bool)), this, SIGNAL(loopClick(bool)));
    connect(timeControls, SIGNAL(soundClick()), this, SIGNAL(soundClick()));
    connect(timeControls, SIGNAL(fpsClick(int)), this, SIGNAL(fpsClick(int)));

    //connect(timeControls, SIGNAL(loopClick(bool)), this, SIGNAL(loopToggled(bool)));
    connect(this, SIGNAL(toggleLoop(bool)), timeControls, SLOT(toggleLoop(bool)));

    connect(newBitmapLayerAct, SIGNAL(triggered()), this, SIGNAL(newBitmapLayer()));
    connect(newVectorLayerAct, SIGNAL(triggered()), this, SIGNAL(newVectorLayer()));
    connect(newSoundLayerAct, SIGNAL(triggered()), this, SIGNAL(newSoundLayer()));
    connect(newCameraLayerAct, SIGNAL(triggered()), this, SIGNAL(newCameraLayer()));
    connect(removeLayerButton, SIGNAL(clicked()), this, SIGNAL(deleteCurrentLayer()));

    scrubbing = false;
    //QSettings settings("Pencil","Pencil");
    //layerHeight = (settings.value("layerHeight").toInt());
    //if(layerHeight==0) { layerHeight=20; settings.setValue("layerHeight", layerHeight); }
    //startY = 0;
    //endY = 0;
    //startLayerNumber = -1;
    //offsetY = 15;
}

int TimeLine::getFrameLength()
{
    return cells->getFrameLength();
}

void TimeLine::resizeEvent(QResizeEvent* event)
{
    //QWidget::resizeEvent(event);
    updateLayerView();
    //event->accept();
}

void TimeLine::updateFrame(int frameNumber)
{
    if(cells) cells->updateFrame(frameNumber);
}

void TimeLine::updateLayerView()
{
    vScrollBar->setPageStep( (height()-cells->getOffsetY()-hScrollBar->height())/cells->getLayerHeight() -2 );
    vScrollBar->setMinimum( 0 );
    vScrollBar->setMaximum( qMax(0, numberOfLayers - vScrollBar->pageStep()) );
    update();
    updateContent();
}

void TimeLine::updateLayerNumber(int numberOfLayers)
{
    this->numberOfLayers = numberOfLayers;
    updateLayerView();
}

void TimeLine::updateLength(int frameLength)
{
    hScrollBar->setMaximum( frameLength );
}

void TimeLine::updateContent()
{
    list->updateContent();
    cells->updateContent();
    update();
}

/*void TimeLine::mousePressEvent(QMouseEvent *event) {
    //int frameNumber = getFrameNumber(event->pos().x());
    int layerNumber = getLayerNumber(event->pos().y());
    startY = event->pos().y();
    startLayerNumber = layerNumber;
    endY = event->pos().y();

    //if(frameNumber == editor->currentFrame) {
    //	scrubbing = true;
    //} else {
        if( (layerNumber != -1) && layerNumber < editor->object->getLayerCount()) {
            //editor->object->getLayer(layerNumber)->mousePress(event, frameNumber);
            editor->setCurrentLayer(layerNumber);
        //} else {
            //editor->scrubTo(frameNumber);
            //scrubbing = true;
        }
    //}
}

void TimeLine::mouseMoveEvent(QMouseEvent *event) {
    endY = event->pos().y();
    //int frameNumber = getFrameNumber(event->pos().x());
    int layerNumber = getLayerNumber(event->pos().y());
    //if(scrubbing) {
    //	editor->scrubTo(frameNumber);
    //} else {
        //if(layerNumber != -1 && layerNumber < editor->object->getLayerCount()) {
        //	editor->object->getLayer(layerNumber)->mouseMove(event, frameNumber);
        //}
    //}
    update();
}

void TimeLine::mouseReleaseEvent(QMouseEvent *event) {
    endY = startY;
    //scrubbing = false;
    //int frameNumber = getFrameNumber(event->pos().x());
    int layerNumber = getLayerNumber(event->pos().y());
    //if(layerNumber != -1 && layerNumber < editor->object->getLayerCount() ) {
    //	editor->object->getLayer(layerNumber)->mouseRelease(event, frameNumber);
    //}
    if(layerNumber != startLayerNumber && startLayerNumber != -1 && layerNumber != -1) {
        editor->moveLayer(startLayerNumber, layerNumber);
    }
    update();
}

void TimeLine::mouseDoubleClickEvent(QMouseEvent *event) {
    int layerNumber = getLayerNumber(event->pos().y());
    if(layerNumber != -1 && layerNumber < editor->object->getLayerCount() ) {
        editor->switchVisibilityOfLayer(layerNumber);
    }
}*/

void TimeLine::setFps ( int value )
{
    timeControls->setFps(value);
}

void TimeLine::forceUpdateLength(QString newLength)
{
    bool ok;
    int dec = newLength.toInt(&ok, 10);

    if ( dec > getFrameLength())
    {
        updateLength(dec);
        updateContent();
        QSettings settings("Pencil","Pencil");
        settings.setValue("length", dec);
    }
}


