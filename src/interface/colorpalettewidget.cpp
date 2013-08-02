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
#include "scribblearea.h"
#include "colormanager.h"
#include "colorpalettewidget.h"


ColorPaletteWidget::ColorPaletteWidget(Editor* editor) : QDockWidget(editor, Qt::Tool)
{
    this->editor = editor;
    
    setWindowTitle(tr("Colors"));

    addButton = new QToolButton(this);
    addButton->setIcon(QIcon(":icons/add.png"));
    addButton->setToolTip("Add Color");
    addButton->setFixedSize(30, 30);

    removeButton = new QToolButton(this);
    removeButton->setIcon(QIcon(":icons/remove.png"));
    removeButton->setToolTip("Remove Color");
    removeButton->setFixedSize(30, 30);

    QLabel* spacer = new QLabel();
    spacer->setFixedWidth(10);
    
    QToolBar* buttons = new QToolBar(this);    
    buttons->addWidget(spacer);
    buttons->addWidget(addButton);
    buttons->addWidget(removeButton);

    m_colorListView = new QListWidget(this);
    m_colorListView->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    m_colorListView->setLineWidth(1);
    m_colorListView->setFocusPolicy(Qt::NoFocus);
	m_colorListView->setCurrentRow( 0 );

    m_colorBox = new ColorBox(this);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(m_colorBox);
    layout->addWidget(buttons);
    layout->addWidget(m_colorListView);
    layout->setMargin(0);

    QWidget* paletteContent = new QWidget(this);
    paletteContent->setLayout(layout);
    paletteContent->setFixedWidth(180);  /// otherwise the palette is naturally too wide. Someone please fix this.

    setWidget(paletteContent);

    setWindowFlags(Qt::WindowStaysOnTopHint);
    setFloating(true);
    
    connect(m_colorListView, SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)), 
			this, SLOT(colorListCurrentItemChanged(QListWidgetItem*, QListWidgetItem*)));
    
    connect(m_colorListView, SIGNAL(itemClicked(QListWidgetItem*)),
			this, SLOT(clickColorListItem( QListWidgetItem*)));

    connect(m_colorListView, SIGNAL(itemDoubleClicked ( QListWidgetItem*)), this, 
			SLOT(changeColourName( QListWidgetItem*)));
    
    connect(addButton, SIGNAL(clicked()), this, SLOT(clickAddColorButton()));
    connect(removeButton, SIGNAL(clicked()), this, SLOT(clickRemoveColorButton()));
    
	connect(this, SIGNAL(topLevelChanged(bool)), this, SLOT(closeIfDocked(bool)));
	
    connect(m_colorBox, SIGNAL(colorChanged(QColor)), 
            this, SLOT(colorWheelChanged(QColor)));
}

void ColorPaletteWidget::selectColorListRow(int row)
{
	m_colorListView->setCurrentRow(row);
}

int ColorPaletteWidget::currentColourNumber()
{    	
	if ( m_colorListView->currentRow() < 0 )
	{        
		m_colorListView->setCurrentRow( 0 );
	}
	return m_colorListView->currentRow();
}

void ColorPaletteWidget::refreshColorList()
{
    if (m_colorListView->count() > 0)
    {
        m_colorListView->clear();
    }

    for (int i = 0; i < editor->object->getColourCount(); i++)
    {
        ColourRef colourRef = editor->object->getColour(i);

        QListWidgetItem* colourItem = new QListWidgetItem(m_colorListView);
        colourItem->setText( colourRef.name );

        QPixmap colourSwatch(32,32);
        colourSwatch.fill( colourRef.colour );
        colourItem->setIcon( colourSwatch );
    }
    update();
}

void ColorPaletteWidget::colourSwatchClicked()
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

            refreshColorList();
            selectColorListRow(colorIndex);
            setColour(newColor);
        }
    }
}

void ColorPaletteWidget::colorListCurrentItemChanged(QListWidgetItem* current, QListWidgetItem* previous)
{
    if (!current) 
	{
		current = previous;
	}
    editor->selectVectorColourNumber(m_colorListView->row(current));

	emit colorNumberChanged( m_colorListView->row(current));
}

void ColorPaletteWidget::clickColorListItem(QListWidgetItem* currentItem)
{
	int colorIndex = m_colorListView->row(currentItem);

	m_colorBox->setColor( editor->getObject()->getColour(colorIndex).colour );

    editor->selectAndApplyColour( colorIndex );

	emit colorNumberChanged( colorIndex );
}

void ColorPaletteWidget::colorWheelChanged(QColor newColor)
{    
    int colorIndex = currentColourNumber();

    editor->object->setColour(colorIndex, newColor);    
    editor->getScribbleArea()->updateFrame();

    updateItemColor(colorIndex, newColor);

	emit colorChanged( newColor );
}

void ColorPaletteWidget::changeColourName( QListWidgetItem* item )
{
    Q_ASSERT( item != NULL );

    int colorNumber = m_colorListView->row(item);
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
            refreshColorList();
        }
    }
}

void ColorPaletteWidget::clickAddColorButton()
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
                                             QString(tr("Colour %1")).arg(m_colorListView->count()),
                                             &ok );
        if (ok) 
        {
            ref.name = text;
            editor->object->addColour(ref);
            refreshColorList();
            editor->selectVectorColourNumber(editor->object->getColourCount() - 1);
        }
    }
}

void ColorPaletteWidget::clickRemoveColorButton()
{
    int colorNumber = m_colorListView->currentRow();
    editor->object->removeColour(colorNumber);

    refreshColorList();
}

void ColorPaletteWidget::setColour(QColor color)
{
    m_colorBox->setColor(color);
}

void ColorPaletteWidget::updateItemColor( int itemIndex, QColor newColor )
{
    QPixmap colourSwatch(32, 32);
    colourSwatch.fill( newColor );
    m_colorListView->item( itemIndex )->setIcon( colourSwatch );
}
