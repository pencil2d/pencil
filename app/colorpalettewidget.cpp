/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2013-2017 Matt Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/
#include <QDebug>
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
#include "preferencemanager.h"


ColorPaletteWidget::ColorPaletteWidget( QWidget* parent ) : BaseDockWidget( parent )
{
    setWindowTitle( tr( "Color Palette", "Window title of color palette." ) );

    QWidget* pWidget = new QWidget( this );
    ui = new Ui::ColorPalette;
    ui->setupUi( pWidget );
    setWidget( pWidget );

    connect( ui->colorListWidget, &QListWidget::currentItemChanged, 
             this, &ColorPaletteWidget::colorListCurrentItemChanged );

    connect( ui->colorListWidget, SIGNAL( itemClicked( QListWidgetItem* ) ),
             this, SLOT( clickColorListItem( QListWidgetItem* ) ) );

    connect( ui->colorListWidget, SIGNAL( itemDoubleClicked( QListWidgetItem* ) ), this,
             SLOT( changeColourName( QListWidgetItem* ) ) );

    connect( ui->addColorButton, &QPushButton::clicked, this, &ColorPaletteWidget::clickAddColorButton );
    connect( ui->removeColorButton, &QPushButton::clicked, this, &ColorPaletteWidget::clickRemoveColorButton );
    connect( ui->palettePref, &QToolButton::clicked, this, &ColorPaletteWidget::palettePreferences );
}

void ColorPaletteWidget::init(PreferenceManager *prefs)
{
    mPrefs = prefs;
    connect(mPrefs, &PreferenceManager::optionChanged, this, &ColorPaletteWidget::settingUpdated);
    loadBackgroundStyle();
    update();
}

void ColorPaletteWidget::initUI()
{
    // "Remove color" feature is disabled because
    // vector strokes that are linked to palette
    // colors don't handle color removal from palette
    //
    iconSize = QSize(34,34);
    ui->removeColorButton->hide();
    updateUI();
    palettePreferences();
}

void ColorPaletteWidget::updateUI()
{
    refreshColorList();
    updateGridUI();
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

    QPixmap originalColourSwatch( iconSize );
    QPainter swatchPainter( &originalColourSwatch );
    swatchPainter.drawTiledPixmap( 0, 0, iconSize.width(), iconSize.height(), QPixmap( ":/background/checkerboard.png" ) );
    swatchPainter.end();
    QPixmap colourSwatch;

    QListWidgetItem* colourItem;
    ColourRef colourRef;
    for (int i = 0; i < editor()->object()->getColourCount(); i++)
    {
        colourRef = editor()->object()->getColour(i);

        colourItem = new QListWidgetItem( ui->colorListWidget );

        if ( ui->colorListWidget->viewMode() != QListView::IconMode){
            colourItem->setText( colourRef.name );
        }
        colourSwatch = originalColourSwatch;
        swatchPainter.begin( &colourSwatch );
        swatchPainter.fillRect( 0, 0, iconSize.width(), iconSize.height(), colourRef.colour );
        swatchPainter.end();
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
            editor()->object()->getColour(colorNumber).name,
            &ok );
        if (ok && !text.isEmpty())
        {
            editor()->object()->renameColour(colorNumber, text);
            refreshColorList();
        }
    }
}

void ColorPaletteWidget::palettePreferences()
{
        layoutModes = new QActionGroup(this);
        listMode = new QAction (tr("List mode"), this);
        listMode->setStatusTip(tr("Show palette as a list"));
        listMode->setCheckable(true);
        listMode->setChecked(true);
        layoutModes->addAction(listMode);
        connect(listMode, &QAction::triggered, this, &ColorPaletteWidget::setListMode);

        gridMode = new QAction (tr("Grid mode"), this);
        gridMode->setStatusTip(tr("Show palette as icons"));
        gridMode->setCheckable(true);
        layoutModes->addAction(gridMode);
        connect(gridMode, &QAction::triggered, this, &ColorPaletteWidget::setGridMode);

        // Swatch size control
        iconSizes = new QActionGroup (this);
        smallSwatch = new QAction(tr("Small swatch"), this);
        smallSwatch->setStatusTip(tr("Sets swatch size to: 16x16px"));
        smallSwatch->setCheckable(true);
        iconSizes->addAction(smallSwatch);
        connect(smallSwatch, &QAction::triggered, this, &ColorPaletteWidget::setSwatchSizeSmall);

        mediumSwatch = new QAction(tr("Medium swatch"), this);
        mediumSwatch->setStatusTip(tr("Sets swatch size to: 26x26px"));
        mediumSwatch->setCheckable(true);
        iconSizes->addAction(mediumSwatch);
        connect(mediumSwatch, &QAction::triggered, this, &ColorPaletteWidget::setSwatchSizeMedium);

        largeSwatch = new QAction(tr("Large swatch"), this);
        largeSwatch->setCheckable(true);
        largeSwatch->setStatusTip(tr("Sets swatch size to: 36x36px"));
        iconSizes->addAction(largeSwatch);
        largeSwatch->setChecked(true);
        connect(largeSwatch, &QAction::triggered, this, &ColorPaletteWidget::setSwatchSizeLarge);

        ui->colorListWidget->setMinimumWidth(ui->colorListWidget->sizeHintForColumn(0));

        // Let's pretend this button is a separator
        separator = new QAction(tr(""), this);
        separator->setSeparator(true);

        // Add to UI
        ui->palettePref->addAction(listMode);
        ui->palettePref->addAction(gridMode);
        ui->palettePref->addAction(separator);
        ui->palettePref->addAction(smallSwatch);
        ui->palettePref->addAction(mediumSwatch);
        ui->palettePref->addAction(largeSwatch);
}

void ColorPaletteWidget::setListMode()
{
    ui->colorListWidget->setViewMode(QListView::ListMode);
    ui->colorListWidget->setMovement(QListView::Static);
    ui->colorListWidget->setGridSize(QSize(-1,-1));
    updateUI();
}

void ColorPaletteWidget::setGridMode()
{
    ui->colorListWidget->setViewMode(QListView::IconMode);
    ui->colorListWidget->setMovement(QListView::Static); // TODO: update swatch index on move
    ui->colorListWidget->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOn );
    ui->colorListWidget->setGridSize(iconSize);
    updateUI();
}

void ColorPaletteWidget::resizeEvent(QResizeEvent *event)
{
    // Find the value to divivde with
    for (int i = 1; i < 75; i++) {
        int size = (ui->colorListWidget->width()-18) / i; // subtract scrollbar width
            if (size >= iconSize.width() && size <= iconSize.width() + 8){
                stepper = size;
            }
    }
    QSize tempSize = QSize(stepper,iconSize.height());

    ui->colorListWidget->setIconSize(QSize(tempSize.width(),iconSize.height()));
    ui->colorListWidget->setGridSize(QSize(tempSize.width(),iconSize.height()));
    iconSize.setWidth(iconSize.width());

    refreshColorList();
    QWidget::resizeEvent(event);
}

void ColorPaletteWidget::setSwatchSizeSmall()
{
    if (iconSize.width() > 18) {
        iconSize = QSize(14,14);
    }
    updateUI();
}

void ColorPaletteWidget::setSwatchSizeMedium()
{
    if (iconSize.width() < 20 || iconSize.width() > 30) {
        iconSize = QSize(26,26);
        updateUI();
    }
}

void ColorPaletteWidget::setSwatchSizeLarge()
{
    if (iconSize.width() < 30) {
        iconSize = QSize(34,34);
        updateUI();
    }
}

void ColorPaletteWidget::updateGridUI()
{
    if (ui->colorListWidget->viewMode() == QListView::IconMode)
    {
        ui->colorListWidget->setGridSize(QSize(iconSize.width(),iconSize.height()));
    }
    else {

        ui->colorListWidget->setGridSize(QSize(-1,-1));
    }
    ui->colorListWidget->setIconSize(iconSize);
}

void ColorPaletteWidget::settingUpdated(SETTING setting)
{
    switch ( setting )
    {
    case SETTING::COLOR_PALETTE_BACKGROUND_STYLE:
        loadBackgroundStyle();
        update();
        break;

    default:
        break;
    }
}

void ColorPaletteWidget::loadBackgroundStyle()
{
    QString bgName = mPrefs->getString(SETTING::COLOR_PALETTE_BACKGROUND_STYLE);

    if ( bgName == "white" )
    {
        ui->colorListWidget->setStyleSheet(QString("QListWidget { background-color:white; } "));
    }
    else if ( bgName == "checkerboard" )
    {
        ui->colorListWidget->setStyleSheet(QString("QListWidget { background-image: url(:/background/checkerboard.png); } "));
    }
}

void ColorPaletteWidget::clickAddColorButton()
{
    QColor prevColor = Qt::white;

    if ( currentColourNumber() > -1 )
    {
        prevColor = editor()->object()->getColour(currentColourNumber()).colour;
    }

    bool ok;
    QRgb qrgba = QColorDialog::getRgba( prevColor.rgba(), &ok, this );
    ColourRef ref;
    QString text;
    if (ui->colorListWidget->viewMode() != QListView::IconMode){
        if ( ok )
        {
            ref = ColourRef(QColor::fromRgba(qrgba));
            bool ok;
            text = QInputDialog::getText(this,
                tr("Colour name"),
                tr("Colour name:"),
                QLineEdit::Normal,
                QString(tr("Colour %1")).arg( ui->colorListWidget->count()),
                &ok );
            if (ok)
            {
                ref.name = text;
                editor()->object()->addColour(ref);
                refreshColorList();

                int colorIndex = editor()->object()->getColourCount() - 1;

                editor()->color()->setColorNumber(colorIndex);
                editor()->color()->setColor( ref.colour );

                // This is done through editor()->color()->setColorNumber()
                //
                //emit colorNumberChanged( colorIndex );
            }
        }
    }
    else{
        ref = ColourRef(QColor::fromRgba(qrgba));
        editor()->object()->addColour(ref);
        refreshColorList();

        int colorIndex = editor()->object()->getColourCount() - 1;

        editor()->color()->setColorNumber(colorIndex);
        editor()->color()->setColor( ref.colour );
    }
}

void ColorPaletteWidget::clickRemoveColorButton()
{
    int colorNumber = ui->colorListWidget->currentRow();
    editor()->object()->removeColour(colorNumber);

    refreshColorList();
}

void ColorPaletteWidget::updateItemColor( int itemIndex, QColor newColor )
{
    QPixmap colourSwatch( iconSize );
    QPainter swatchPainter( &colourSwatch );
    swatchPainter.drawTiledPixmap( 0, 0, iconSize.width(), iconSize.height(), QPixmap( ":/background/checkerboard.png" ) );
    swatchPainter.fillRect( 0, 0, iconSize.width(), iconSize.height(), newColor );
    ui->colorListWidget->item( itemIndex )->setIcon( colourSwatch );

    // Make sure to update grid in grid mode
    if (ui->colorListWidget->viewMode() == QListView::IconMode)
    {
        updateGridUI();
    }
}
