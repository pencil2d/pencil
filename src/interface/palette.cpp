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

#include "colourref.h"
#include "object.h"
#include "editor.h"
#include "colorbox.h"
#include "palette.h"


Palette::Palette(Editor* editor) : QDockWidget(editor, Qt::Tool)
{
    this->editor = editor;

    QWidget* paletteContent = new QWidget();

    listOfColours = new QListWidget();

    QToolBar* buttons = new QToolBar();
    addButton = new QToolButton();
    removeButton = new QToolButton();
    addButton->setIcon(QIcon(":icons/add.png"));
    addButton->setToolTip("Add Color");
    addButton->setFixedSize(30, 30);
    removeButton->setIcon(QIcon(":icons/remove.png"));
    removeButton->setToolTip("Remove Color");
    removeButton->setFixedSize(30, 30);

    QLabel* spacer = new QLabel();
    spacer->setFixedWidth(10);

    colourSwatch = new QToolButton();
    colourSwatch->setFixedSize( 40, 40 );
    QPixmap colourPixmap(30, 30);
    colourPixmap.fill( Qt::black );
    colourSwatch->setIcon(QIcon(colourPixmap));

    buttons->addWidget(spacer);
    buttons->addWidget(colourSwatch);
    buttons->addWidget(addButton);
    buttons->addWidget(removeButton);

    listOfColours->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    listOfColours->setLineWidth(1);
    listOfColours->setFocusPolicy(Qt::NoFocus);

    m_colorBox = new ColorBox(this);

    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget(m_colorBox);    
    layout->addWidget(buttons);
    layout->addWidget(listOfColours);
    layout->setMargin(0);

    paletteContent->setLayout(layout);
    setWidget(paletteContent);

    setWindowFlags(Qt::WindowStaysOnTopHint);
    setFloating(true);
    paletteContent->setFixedWidth(180);  /// otherwise the palette is naturally too wide. Someone please fix this.

    setWindowTitle(tr("Colors"));

    connect(listOfColours, SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)), this, SLOT(colorListItemChanged(QListWidgetItem*, QListWidgetItem*)));
    connect(listOfColours, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(clickColorListItem( QListWidgetItem*)));
    connect(listOfColours, SIGNAL(itemDoubleClicked ( QListWidgetItem*)), this, SLOT(changeColourName( QListWidgetItem*)));

    connect(addButton, SIGNAL(clicked()), this, SLOT(clickAddColorButton()));
    connect(removeButton, SIGNAL(clicked()), this, SLOT(clickRemoveColorButton()));
    connect(colourSwatch, SIGNAL(clicked()), this, SLOT(colourSwatchClicked()));
    connect(this, SIGNAL(topLevelChanged(bool)), this, SLOT(closeIfDocked(bool)));

    connect(m_colorBox, SIGNAL(colorChanged(QColor)), 
            this, SLOT(colorChanged(QColor)));
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

void Palette::colorChanged(QColor newColor)
{
    int colorIndex = currentColourNumber();
    editor->object->setColour(colorIndex, newColor);
    editor->setFrontColour(colorIndex, newColor);

    updateList();
    selectColorListRow(colorIndex);
    setColour(newColor);
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
        if (ok) 
        {
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

void Palette::setColour(QColor color)
{
    //m_colorBox->setColor(color);
}