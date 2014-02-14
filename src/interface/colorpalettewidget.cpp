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


ColorPaletteWidget::ColorPaletteWidget(Editor* editor) : QDockWidget(editor, Qt::Tool)
{
    this->m_pEditor = editor;

    setWindowTitle(tr("Colors"));

    m_addButton = new QToolButton(this);
    m_addButton->setIcon(QIcon(":icons/add.png"));
    m_addButton->setToolTip(tr("Add Color"));
    m_addButton->setFixedSize(30, 30);

    m_removeButton = new QToolButton(this);
    m_removeButton->setIcon(QIcon(":icons/remove.png"));
    m_removeButton->setToolTip(tr("Remove Color"));
    m_removeButton->setFixedSize(30, 30);

    QLabel* spacer = new QLabel();
    spacer->setFixedWidth(10);

    QToolBar* buttons = new QToolBar(this);
    buttons->addWidget(spacer);
    buttons->addWidget(m_addButton);
    buttons->addWidget(m_removeButton);

    m_colorListView = new QListWidget(this);
    m_colorListView->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    m_colorListView->setLineWidth(1);
    m_colorListView->setFocusPolicy(Qt::NoFocus);
    m_colorListView->setCurrentRow( 0 );

    QVBoxLayout* pLayout = new QVBoxLayout();
    pLayout->addWidget(buttons);
    pLayout->addWidget(m_colorListView);
    pLayout->setMargin(0);

    QWidget* paletteContent = new QWidget(this);
    paletteContent->setLayout(pLayout);

    setWidget(paletteContent);

    setWindowFlags(Qt::WindowStaysOnTopHint);
    setFloating(true);

    connect(m_colorListView, SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)),
        this, SLOT(colorListCurrentItemChanged(QListWidgetItem*, QListWidgetItem*)));

    connect(m_colorListView, SIGNAL(itemClicked(QListWidgetItem*)),
        this, SLOT(clickColorListItem( QListWidgetItem*)));

    connect(m_colorListView, SIGNAL(itemDoubleClicked ( QListWidgetItem*)), this,
        SLOT(changeColourName( QListWidgetItem*)));

    connect(m_addButton, SIGNAL(clicked()), this, SLOT(clickAddColorButton()));
    connect(m_removeButton, SIGNAL(clicked()), this, SLOT(clickRemoveColorButton()));
}

void ColorPaletteWidget::setColor(QColor newColor)
{
    int colorIndex = currentColourNumber();
    updateItemColor(colorIndex, newColor);

    emit colorChanged( newColor );
}

void ColorPaletteWidget::selectColorNumber(int colorNumber)
{
    m_colorListView->setCurrentRow(colorNumber);
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

    for (int i = 0; i < m_pEditor->object()->getColourCount(); i++)
    {
        ColourRef colourRef = m_pEditor->object()->getColour(i);

        QListWidgetItem* colourItem = new QListWidgetItem(m_colorListView);
        colourItem->setText( colourRef.name );

        QPixmap colourSwatch(32, 32);
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
        ColourRef colorRef = m_pEditor->object()->getColour(currentColourNumber());
        QRgb qrgba = QColorDialog::getRgba( colorRef.colour.rgba(), &ok, this );

        if ( ok )
        {
            QColor newColor = QColor::fromRgba(qrgba);
            int colorIndex = currentColourNumber();
            m_pEditor->object()->setColour(colorIndex, newColor);
            m_pEditor->setFrontColour(colorIndex, newColor );

            refreshColorList();
            selectColorNumber(colorIndex);
            setColor(newColor);
        }
    }
}

void ColorPaletteWidget::colorListCurrentItemChanged(QListWidgetItem* current, QListWidgetItem* previous)
{
    if (!current)
    {
        current = previous;
    }
    emit colorNumberChanged( m_colorListView->row(current) );
}

void ColorPaletteWidget::clickColorListItem(QListWidgetItem* currentItem)
{
    int colorIndex = m_colorListView->row(currentItem);
    //m_pEditor->selectAndApplyColour( colorIndex );

    emit colorNumberChanged( colorIndex );
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
            m_pEditor->m_pObject->getColour(colorNumber).name,
            &ok );
        if (ok && !text.isEmpty())
        {
            m_pEditor->m_pObject->renameColour(colorNumber, text);
            refreshColorList();
        }
    }
}

void ColorPaletteWidget::clickAddColorButton()
{
    QColor prevColor = Qt::white;

    if ( currentColourNumber() > -1 )
    {
        prevColor = m_pEditor->m_pObject->getColour(currentColourNumber()).colour;
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
            m_pEditor->object()->addColour(ref);
            refreshColorList();
            m_pEditor->colorManager()->pickColor( m_pEditor->object()->getColourCount() - 1 );
        }
    }
}

void ColorPaletteWidget::clickRemoveColorButton()
{
    int colorNumber = m_colorListView->currentRow();
    m_pEditor->object()->removeColour(colorNumber);

    refreshColorList();
}

void ColorPaletteWidget::updateItemColor( int itemIndex, QColor newColor )
{
    QPixmap colourSwatch(32, 32);
    colourSwatch.fill( newColor );
    m_colorListView->item( itemIndex )->setIcon( colourSwatch );
}