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
#include "palette.h"
#include "colourref.h"
#include "object.h"
#include "editor.h"

Palette::Palette(Editor* editor) : QDockWidget(editor, Qt::Tool)
{
    this->editor = editor;

    QWidget* paletteContent = new QWidget();

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
    colourSwatch->setIcon(QIcon(colourPixmap));

    buttons->addWidget(spacer);
    buttons->addWidget(colourSwatch);
    buttons->addWidget(addButton);
    buttons->addWidget(removeButton);

    listOfColours->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    listOfColours->setLineWidth(1);
    listOfColours->setFocusPolicy(Qt::NoFocus);

    QVBoxLayout* layout = new QVBoxLayout();
    layout->addLayout(sliderLayout);
    layout->addWidget(buttons);
    layout->addWidget(listOfColours);
    layout->setMargin(0);

    paletteContent->setLayout(layout);
    setWidget(paletteContent);

    setWindowFlags(Qt::WindowStaysOnTopHint);
    setFloating(true);
    paletteContent->setFixedWidth(150);  /// otherwise the palette is naturally too wide. Someone please fix this.

    setWindowTitle(tr("Colors"));

    connect(sliderRed, SIGNAL(sliderMoved(int)), this, SLOT(colorSliderMoved()));
    connect(sliderGreen, SIGNAL(sliderMoved(int)), this, SLOT(colorSliderMoved()));
    connect(sliderBlue, SIGNAL(sliderMoved(int)), this, SLOT(colorSliderMoved()));
    connect(sliderAlpha, SIGNAL(sliderMoved(int)), this, SLOT(colorSliderMoved()));

    connect(sliderRed, SIGNAL(sliderReleased()), this, SLOT(colourSliderValueChange()));
    connect(sliderGreen, SIGNAL(sliderReleased()), this, SLOT(colourSliderValueChange()));
    connect(sliderBlue, SIGNAL(sliderReleased()), this, SLOT(colourSliderValueChange()));
    connect(sliderAlpha, SIGNAL(sliderReleased()), this, SLOT(colourSliderValueChange()));

    connect(listOfColours, SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)), this, SLOT(colorListItemChanged(QListWidgetItem*, QListWidgetItem*)));
    connect(listOfColours, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(clickColorListItem( QListWidgetItem*)));
    connect(listOfColours, SIGNAL(itemDoubleClicked ( QListWidgetItem*)), this, SLOT(changeColourName( QListWidgetItem*)));

    connect(addButton, SIGNAL(clicked()), this, SLOT(clickAddColorButton()));
    connect(removeButton, SIGNAL(clicked()), this, SLOT(clickRemoveColorButton()));

    connect(colourSwatch, SIGNAL(clicked()), this, SLOT(colourSwatchClicked()));

    connect(this, SIGNAL(topLevelChanged(bool)), this, SLOT(closeIfDocked(bool)));
}

void Palette::updateList()
{
    while (listOfColours->count() > 0)
    {
        listOfColours->takeItem(0);
    }

    for (int i = 0; i < editor->object->getColourCount(); i++)
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
    if (currentColourNumber() > -1)
    {
        bool ok;
        ColourRef colorRef = editor->object->getColour(currentColourNumber());
        QRgb qrgba = QColorDialog::getRgba( colorRef.colour.rgba(), &ok, this );

        if ( ok )
        {
            QColor newColor = QColor::fromRgba(qrgba);
            int colorIndex = currentColourNumber();
            editor->object->setColour(colorIndex, newColor);
            editor->setFrontColour(colorIndex, newColor );

            updateList();
            selectColorListRow(colorIndex);
            setColour(newColor);
        }
    }
}

void Palette::colorListItemChanged(QListWidgetItem* current, QListWidgetItem* previous)
{
    if (!current) current = previous;
    editor->selectVectorColourNumber(listOfColours->row(current));
}

void Palette::clickColorListItem(QListWidgetItem* current)
{
    editor->selectAndApplyColour(listOfColours->row(current));
}

void Palette::colourSliderValueChange()
{
    QColor newColor = QColor( sliderRed->value(),
                              sliderGreen->value(),
                              sliderBlue->value(),
                              sliderAlpha->value() );

    int colorIndex = currentColourNumber();
    editor->object->setColour(colorIndex, newColor);
    editor->setFrontColour(colorIndex, newColor);

    updateList();
    selectColorListRow(colorIndex);
    setColour(newColor);
}

void Palette::colorSliderMoved()
{
    QColor newColour = QColor( sliderRed->value(),
                               sliderGreen->value(),
                               sliderBlue->value(),
                               sliderAlpha->value() );
    editor->setFrontColour(currentColourNumber(), newColour);
}

void Palette::updateSwatch(QColor colour)
{
    QPixmap colourPixmap(30,30);
    colourPixmap.fill( colour );
    if (colourSwatch != NULL)
    {
        colourSwatch->setIcon(QIcon(colourPixmap));
    }
}

void Palette::changeColourName( QListWidgetItem* item )
{
    if (item == NULL)
    {
        return;
    }
    int colorNumber = listOfColours->row(item);
    if (colorNumber > -1)
    {
        bool ok;
        QString text = QInputDialog::getText(this,
                                             tr("Colour name"),
                                             tr("Colour name:"),
                                             QLineEdit::Normal,
                                             editor->object->getColour(colorNumber).name,
                                             &ok );
        if (ok && !text.isEmpty())
        {
            editor->object->renameColour(colorNumber, text);
            updateList();
        }
    }
}

void Palette::clickAddColorButton()
{
    QColor prevColor = Qt::white;

    if ( currentColourNumber() > -1 )
    {
        prevColor = editor->object->getColour(currentColourNumber()).colour;
    }

    bool ok;
    QRgb qrgba = QColorDialog::getRgba( prevColor.rgba(), &ok, this );
    if ( ok )
    {
        ColourRef ref = ColourRef(QColor::fromRgba(qrgba));
        bool ok;
        QString text = QInputDialog::getText(this,
                                             tr("Colour name"),
                                             tr("Colour name:"),
                                             QLineEdit::Normal,
                                             QString(tr("Colour %1")).arg(listOfColours->count()),
                                             &ok );
        if (ok) {
            ref.name = text;
            editor->object->addColour(ref);
            updateList();
            editor->selectVectorColourNumber(editor->object->getColourCount() - 1);
        }
    }
}

void Palette::clickRemoveColorButton()
{
    int colorNumber = listOfColours->currentRow();
    editor->object->removeColour(colorNumber);
    updateList();
}

void Palette::closeIfDocked(bool)
{
    //if (floating == false) close(); // we don't want to dock the palette in the mainwindow (or do we?)
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
