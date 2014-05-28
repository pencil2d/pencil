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
#include <QListWidget>
#include <QListWidgetItem>
#include <QVBoxLayout>
#include <QInputDialog>
#include <QColorDialog>

#include "colourref.h"
#include "object.h"
#include "editor.h"
#include "colorbox.h"
#include "scribblearea.h"
#include "colormanager.h"
#include "colorpalettewidget.h"
#include "ui_colorpalette.h"


ColorPaletteWidget::ColorPaletteWidget( QString strTitle, QWidget* pParent ) 
    : BaseDockWidget( strTitle, pParent, Qt::Tool )
    , ui( new Ui::ColorPalette )
{
    QWidget* pWidget = new QWidget( this );
    ui->setupUi( pWidget );
    setWidget( pWidget );

    connect( ui->colorListWidget, &QListWidget::currentItemChanged, 
             this, &ColorPaletteWidget::colorListCurrentItemChanged );

    connect( ui->colorListWidget, SIGNAL(itemClicked(QListWidgetItem*)),
        this, SLOT(clickColorListItem( QListWidgetItem*)));

    connect( ui->colorListWidget, SIGNAL(itemDoubleClicked ( QListWidgetItem*)), this,
        SLOT(changeColourName( QListWidgetItem*)));

    connect( ui->addColorButton, &QPushButton::clicked, this, &ColorPaletteWidget::clickAddColorButton );
    connect( ui->removeColorButton, &QPushButton::clicked, this, &ColorPaletteWidget::clickRemoveColorButton );
}

void ColorPaletteWidget::initUI()
{
    updateUI();
}

void ColorPaletteWidget::updateUI()
{
    refreshColorList();
}

void ColorPaletteWidget::setColor(QColor newColor)
{
    int colorIndex = currentColourNumber();
    updateItemColor(colorIndex, newColor);

    emit colorChanged( newColor );
}

void ColorPaletteWidget::selectColorNumber(int colorNumber)
{
    ui->colorListWidget->setCurrentRow(colorNumber);
}

int ColorPaletteWidget::currentColourNumber()
{
    if ( ui->colorListWidget->currentRow() < 0 )
    {
        ui->colorListWidget->setCurrentRow( 0 );
    }
    return ui->colorListWidget->currentRow();
}

void ColorPaletteWidget::refreshColorList()
{
    if ( ui->colorListWidget->count() > 0)
    {
        ui->colorListWidget->clear();
    }
    
    for (int i = 0; i < core()->object()->getColourCount(); i++)
    {
        ColourRef colourRef = core()->object()->getColour(i);

        QListWidgetItem* colourItem = new QListWidgetItem( ui->colorListWidget );
        colourItem->setText( colourRef.name );

        QPixmap colourSwatch(32, 32);
        colourSwatch.fill( colourRef.colour );
        colourItem->setIcon( colourSwatch );

        ui->colorListWidget->addItem( colourItem );
    }
    update();
}

void ColorPaletteWidget::colorListCurrentItemChanged(QListWidgetItem* current, QListWidgetItem* previous)
{
    if (!current)
    {
        current = previous;
    }
    emit colorNumberChanged( ui->colorListWidget->row(current) );
}

void ColorPaletteWidget::clickColorListItem(QListWidgetItem* currentItem)
{
    int colorIndex = ui->colorListWidget->row( currentItem );
    //m_pEditor->selectAndApplyColour( colorIndex );

    emit colorNumberChanged( colorIndex );
}

void ColorPaletteWidget::changeColourName( QListWidgetItem* item )
{
    Q_ASSERT( item != NULL );

    int colorNumber = ui->colorListWidget->row( item );
    if (colorNumber > -1)
    {
        bool ok;
        QString text = QInputDialog::getText(this,
            tr("Colour name"),
            tr("Colour name:"),
            QLineEdit::Normal,
            core()->object()->getColour(colorNumber).name,
            &ok );
        if (ok && !text.isEmpty())
        {
            core()->object()->renameColour(colorNumber, text);
            refreshColorList();
        }
    }
}

void ColorPaletteWidget::clickAddColorButton()
{
    QColor prevColor = Qt::white;

    if ( currentColourNumber() > -1 )
    {
        prevColor = core()->object()->getColour(currentColourNumber()).colour;
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
            QString(tr("Colour %1")).arg( ui->colorListWidget->count()),
            &ok );
        if (ok)
        {
            ref.name = text;
            core()->object()->addColour(ref);
            refreshColorList();
            core()->color()->setColor( core()->object()->getColourCount() - 1 );
        }
    }
}

void ColorPaletteWidget::clickRemoveColorButton()
{
    int colorNumber = ui->colorListWidget->currentRow();
    core()->object()->removeColour(colorNumber);

    refreshColorList();
}

void ColorPaletteWidget::updateItemColor( int itemIndex, QColor newColor )
{
    QPixmap colourSwatch(32, 32);
    colourSwatch.fill( newColor );
    ui->colorListWidget->item( itemIndex )->setIcon( colourSwatch );
}
