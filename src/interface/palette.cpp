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
#include <QtDebug>
#include <QtGui>
#include "palette.h"
#include "colourref.h"
#include "object.h"
#include "editor.h"

Palette::Palette(Editor* editor) : QDockWidget(editor, Qt::Tool)
{
    this->editor = editor;

    QWidget* paletteContent = new QWidget();
    //paletteContent->setWindowFlags(Qt::FramelessWindowHint);

    sliderRed = new QSlider(Qt::Horizontal);
    sliderGreen = new QSlider(Qt::Horizontal);
    sliderBlue = new QSlider(Qt::Horizontal);
    sliderAlpha = new QSlider(Qt::Horizontal);
    sliderRed->setRange(0,255);
    sliderGreen->setRange(0,255);
    sliderBlue->setRange(0,255);
    sliderAlpha->setRange(0,255);
    QLabel* labelRed = new QLabel(tr("Red"));
    QLabel* labelGreen = new QLabel(tr("Green"));
    QLabel* labelBlue = new QLabel(tr("Blue"));
    QLabel* labelAlpha = new QLabel(tr("Alpha"));
    labelRed->setFont( QFont("Helvetica", 10) );
    labelGreen->setFont( QFont("Helvetica", 10) );
    labelBlue->setFont( QFont("Helvetica", 10) );
    labelAlpha->setFont( QFont("Helvetica", 10) );

    QGridLayout* sliderLayout = new QGridLayout();
    sliderLayout->setSpacing(3);
    sliderLayout->addWidget(labelRed, 0, 0);
    sliderLayout->addWidget(sliderRed, 0, 1);
    sliderLayout->addWidget(labelGreen, 1, 0);
    sliderLayout->addWidget(sliderGreen, 1, 1);
    sliderLayout->addWidget(labelBlue, 2, 0);
    sliderLayout->addWidget(sliderBlue, 2, 1);
    sliderLayout->addWidget(labelAlpha, 3, 0);
    sliderLayout->addWidget(sliderAlpha, 3, 1);
    sliderLayout->setMargin(10);
    sliderLayout->setSpacing(2);

    //QWidget* sliders = new QWidget();
    //sliders->setLayout(sliderLayout);
    //sliders->setFixedHeight(60);

    listOfColours = new QListWidget();

    QToolBar* buttons = new QToolBar();
    addButton = new QToolButton();
    removeButton = new QToolButton();
    addButton->setIcon(QIcon(":icons/add.png"));
    addButton->setToolTip("Add Color");
    addButton->setFixedSize(30,30);
    removeButton->setIcon(QIcon(":icons/remove.png"));
    removeButton->setToolTip("Remove Color");
    removeButton->setFixedSize(30,30);

    QLabel* spacer = new QLabel();
    spacer->setFixedWidth(10);

    colourSwatch = new QToolButton(); //QLabel();
    colourSwatch->setFixedSize( 40, 40 );
    QPixmap colourPixmap(30,30);
    colourPixmap.fill( Qt::black );
    colourSwatch->setIcon(QIcon(colourPixmap)); //colourSwatch->setPixmap(colourPixmap);
    /*QFrame* colourSwatchFrame = new QFrame();
    colourSwatchFrame->setFixedSize( 50, 50 );
    //colourSwatchFrame->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    QVBoxLayout *colourSwatchLayout = new QVBoxLayout();
    colourSwatchLayout->addWidget(colourSwatch);
    colourSwatchFrame->setLayout(colourSwatchLayout);*/

    //QGridLayout *buttonLayout = new QGridLayout();
    buttons->addWidget(spacer);
    buttons->addWidget(colourSwatch);
    buttons->addWidget(addButton);
    buttons->addWidget(removeButton);
    //buttons->setFixedSize(100,34);
    //buttons->layout()->setMargin(0);
    //buttons->layout()->setSpacing(0);
    //buttonLayout->setMargin(0);
    //buttonLayout->setSpacing(0);
    //buttons->setLayout(buttonLayout);

    listOfColours->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    listOfColours->setLineWidth(1);
    listOfColours->setFocusPolicy(Qt::NoFocus);
    //listOfColours->setMinimumWidth(100);

    QVBoxLayout* layout = new QVBoxLayout();
    layout->addLayout(sliderLayout);
    layout->addWidget(buttons);
    layout->addWidget(listOfColours);
    layout->setMargin(0);

    paletteContent->setLayout(layout);
    setWidget(paletteContent);

    //setFrameStyle(QFrame::Panel);
    //setWindowFlags(Qt::Tool);
    setWindowFlags(Qt::WindowStaysOnTopHint);
    //setWindowFlags(Qt::SubWindow);
    setFloating(true);
    //setAllowedAreas(Qt::NoDockWidgetArea);
    //setMinimumSize(100, 300);
    paletteContent->setFixedWidth(150);  /// otherwise the palette is naturally too wide. Someone please fix this.
    //setFloating(false);
    //setFixedWidth(130);

    //setGeometry(10,60,100, 300);
    //setFocusPolicy(Qt::NoFocus);
    //setWindowOpacity(0.7);
    setWindowTitle(tr("Colors"));

    connect(sliderRed, SIGNAL(sliderMoved(int)), this, SLOT(updateColour()));
    connect(sliderGreen, SIGNAL(sliderMoved(int)), this, SLOT(updateColour()));
    connect(sliderBlue, SIGNAL(sliderMoved(int)), this, SLOT(updateColour()));
    connect(sliderAlpha, SIGNAL(sliderMoved(int)), this, SLOT(updateColour()));

    connect(sliderRed, SIGNAL(sliderReleased()), this, SLOT(changeColour()));
    connect(sliderGreen, SIGNAL(sliderReleased()), this, SLOT(changeColour()));
    connect(sliderBlue, SIGNAL(sliderReleased()), this, SLOT(changeColour()));
    connect(sliderAlpha, SIGNAL(sliderReleased()), this, SLOT(changeColour()));

    connect(listOfColours, SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)), this, SLOT(selectColour(QListWidgetItem*, QListWidgetItem*)));
    connect(listOfColours, SIGNAL(itemClicked ( QListWidgetItem*)), this, SLOT(selectAndApplyColour( QListWidgetItem*)));
    //connect(listOfColours, SIGNAL(itemDoubleClicked ( QListWidgetItem *)), this, SLOT(changeColour( QListWidgetItem *)));
    connect(listOfColours, SIGNAL(itemDoubleClicked ( QListWidgetItem*)), this, SLOT(changeColourName( QListWidgetItem*)));

    connect(addButton, SIGNAL(clicked()), this, SLOT(addClick()));
    connect(removeButton, SIGNAL(clicked()), this, SLOT(rmClick()));

    connect(colourSwatch, SIGNAL(clicked()), this, SLOT(colourSwatchClicked()));

    connect(this, SIGNAL(topLevelChanged(bool)), this, SLOT(closeIfDocked(bool)));
}

/*QSize Palette::sizeHint() {
	return QSize(150,600);
}*/

void Palette::updateList()
{

    //listOfColours->clear(); // for some reason, this creates an bus error when one removes the last element
    while(listOfColours->count() > 0) listOfColours->takeItem(0);
    for(int i=0; i <  editor->object->getColourCount(); i++)
    {
        ColourRef colourRef = editor->object->getColour(i);
        QListWidgetItem* colourItem = new QListWidgetItem(listOfColours);
        colourItem->setText( colourRef.name );
        QPixmap colourSwatch(32,32);
        colourSwatch.fill( colourRef.colour );
        QPainter painter(&colourSwatch);
        painter.setPen( QColor(0,0,0,128) );
        painter.drawRect( QRect(0,-1,31,31) );
        colourItem->setIcon( colourSwatch );
    }
    update();
}

void Palette::colourSwatchClicked()
{
    editor->changeColour(currentColour());
}

void Palette::selectColour(QListWidgetItem* current, QListWidgetItem* previous)
{
    if (!current) current = previous;
    editor->selectColour(listOfColours->row(current));
}

void Palette::selectAndApplyColour(QListWidgetItem* current)
{
    editor->selectAndApplyColour(listOfColours->row(current));
}

void Palette::changeColour()
{
    QColor newColour = QColor( sliderRed->value(), sliderGreen->value(), sliderBlue->value(), sliderAlpha->value() );
    editor->changeColour(currentColour(), newColour);
}

void Palette::updateColour()
{
    QColor newColour = QColor( sliderRed->value(), sliderGreen->value(), sliderBlue->value(), sliderAlpha->value() );
    editor->updateColour(currentColour(), newColour);
}

void Palette::updateSwatch(QColor colour)
{
    QPixmap colourPixmap(30,30);
    colourPixmap.fill( colour );
    if(colourSwatch != NULL) colourSwatch->setIcon(QIcon(colourPixmap)); //colourSwatch->setPixmap(colourPixmap);
}

void Palette::changeColour( QListWidgetItem* item )
{
    if(item != NULL) editor->changeColour(listOfColours->row(item));
}

void Palette::changeColourName( QListWidgetItem* item )
{
    if(item != NULL) editor->changeColourName(listOfColours->row(item));
}

void Palette::addClick()
{
    editor->addColour();
}

void Palette::rmClick()
{
    editor->removeColour(listOfColours->currentRow());
}

void Palette::closeIfDocked(bool floating)
{
    //if(floating == false) close(); // we don't want to dock the palette in the mainwindow (or do we?)
}

void Palette::setColour(QColor colour)
{
    setColour(colour.red(), colour.green(), colour.blue(), colour.alpha());
    updateSwatch(colour);
}

void Palette::setColour(int r, int g, int b, int a)
{
    sliderRed->setValue(r);
    sliderGreen->setValue(g);
    sliderBlue->setValue(b);
    sliderAlpha->setValue(a);
}
