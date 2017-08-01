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

    connect( ui->colorListWidget, &QListWidget::itemDoubleClicked, this, &ColorPaletteWidget::changeColourName );
    connect( ui->colorListWidget, &QListWidget::currentTextChanged, this, &ColorPaletteWidget::onActiveColorNameChange );

    connect( ui->addColorButton, &QPushButton::clicked, this, &ColorPaletteWidget::clickAddColorButton );
    connect( ui->removeColorButton, &QPushButton::clicked, this, &ColorPaletteWidget::clickRemoveColorButton );
    connect( ui->palettePref, &QToolButton::clicked, this, &ColorPaletteWidget::palettePreferences );
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
        else {
            colourItem->setToolTip( colourRef.name );
        }
        colourSwatch = originalColourSwatch;
        swatchPainter.begin( &colourSwatch );
        swatchPainter.fillRect( 0, 0, iconSize.width(), iconSize.height(), colourRef.colour );
        swatchPainter.end();
        colourItem->setIcon( colourSwatch );
        colourItem->setFlags( Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable );

        ui->colorListWidget->addItem( colourItem );
    }
    update();
}

void ColorPaletteWidget::changeColourName( QListWidgetItem* item )
{
    Q_ASSERT( item != NULL );

    if( ui->colorListWidget->viewMode() == QListView::IconMode )
    {
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
}

void ColorPaletteWidget::onActiveColorNameChange(QString name)
{
    if ( !name.isNull() )
    {
        editor()->object()->renameColour(ui->colorListWidget->currentRow(), name);
    }
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
    if ( ui->colorListWidget->viewMode() == QListView::IconMode )
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
    }
    else {
        ui->colorListWidget->setIconSize(iconSize);
        ui->colorListWidget->setGridSize( QSize( -1, -1 ) );
    }

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

QString ColorPaletteWidget::getDefaultColorName(QColor c)
{
    // Initialize NBS/ISCC Color dictionary, John Foster version (http://tx4.us/nbs-iscc.htm), converted to CIE L*u*v with D65 white point and rounded.
    const int dictSize = 267;
    static const float colorDict[dictSize][3] = {{129.44f, 17.49f, 4.51f}, {121.33f, 22.84f, 5.93f}, {113.18f, 33.56f, 8.5f}, {133.35f, 9.63f, 4.25f}, {124.33f, 13.9f, 5.56f}, {114.29f, 18.26f, 6.35f}, {134.81f, 3.79f, 1.72f}, {124.74f, 5.33f, 2.33f}, {136.75f, 1.26f, 1.15f}, {126.18f, 2.3f, 2.04f}, {72.24f, 105.85f, -3.37f}, {96.15f, 46.92f, 11.81f}, {76.45f, 57.74f, 6.63f}, {60.36f, 67.58f, -6.78f}, {99.25f, 33.26f, 10.64f}, {82.82f, 33.5f, 6.35f}, {63.94f, 34.51f, -7.72f}, {114.86f, 9.93f, 5.6f}, {101.47f, 17.71f, 6.68f}, {85.01f, 11.4f, 2.58f}, {64.08f, 14.21f, 0.16f}, {111.19f, 3.94f, 2.43f}, {92.79f, 4.76f, 2.41f}, {64.84f, 6.75f, -0.05f}, {129.42f, 14.32f, 12.7f}, {121.57f, 22.27f, 21.12f}, {108.94f, 27.15f, 63.12f}, {131.27f, 11.24f, 4.1f}, {124.34f, 12.97f, 3.49f}, {115.02f, 16.75f, 11.37f}, {134.12f, 3.17f, 7.08f}, {124.53f, 5.15f, 6.1f}, {123.87f, 3.0f, 9.3f}, {104.98f, 34.69f, 57.26f}, {106.95f, 31.37f, 37.98f}, {90.95f, 39.91f, 43.0f}, {109.61f, 23.82f, 26.21f}, {95.16f, 28.7f, 28.57f}, {109.96f, 15.89f, 18.56f}, {83.32f, 36.66f, 41.32f}, {56.25f, 63.71f, 23.86f}, {111.34f, 10.81f, 12.04f}, {91.06f, 19.99f, 14.9f}, {66.59f, 22.89f, 7.83f}, {111.0f, 5.37f, 8.26f}, {92.3f, 10.08f, 7.38f}, {77.22f, 9.99f, 5.68f}, {115.62f, 11.46f, 112.43f}, {120.85f, 15.44f, 53.69f}, {116.85f, 14.84f, 62.0f}, {105.26f, 15.4f, 69.99f}, {128.12f, 9.48f, 27.39f}, {118.39f, 11.64f, 34.12f}, {105.93f, 13.96f, 38.36f}, {91.34f, 14.66f, 46.17f}, {80.29f, 12.55f, 33.67f}, {110.55f, 7.93f, 21.01f}, {93.17f, 8.49f, 20.95f}, {71.03f, 13.95f, 22.13f}, {111.03f, 3.81f, 9.84f}, {93.5f, 5.19f, 9.05f}, {77.35f, 4.95f, 7.72f}, {111.22f, 2.2f, 5.43f}, {92.69f, 4.35f, 2.27f}, {64.49f, 4.44f, 6.94f}, {122.72f, -2.14f, 117.46f}, {129.65f, 0.32f, 54.79f}, {121.9f, 0.89f, 78.29f}, {113.1f, -0.81f, 109.4f}, {131.67f, 2.41f, 32.31f}, {123.71f, 4.02f, 41.99f}, {114.84f, 3.18f, 46.57f}, {134.44f, 2.17f, 19.81f}, {102.38f, 3.33f, 67.17f}, {88.58f, 5.99f, 34.17f}, {119.36f, 3.77f, 24.66f}, {102.09f, 3.5f, 23.67f}, {80.15f, 5.21f, 23.45f}, {118.89f, 1.55f, 12.34f}, {104.19f, 2.36f, 12.21f}, {82.82f, 3.37f, 10.09f}, {127.96f, -13.63f, 121.11f}, {133.92f, -7.58f, 51.81f}, {123.58f, -6.91f, 62.29f}, {113.59f, -9.28f, 82.32f}, {135.01f, -5.5f, 37.33f}, {123.49f, -4.13f, 38.02f}, {115.29f, -4.34f, 42.05f}, {136.63f, -3.61f, 20.49f}, {124.53f, -2.67f, 21.45f}, {114.82f, -2.04f, 24.04f}, {137.97f, -0.97f, 6.67f}, {126.17f, -0.77f, 7.59f}, {105.31f, -3.69f, 68.07f}, {93.97f, -1.94f, 45.65f}, {76.05f, 0.87f, 17.48f}, {129.59f, -25.16f, 121.74f}, {134.5f, -16.57f, 60.21f}, {123.68f, -17.24f, 68.88f}, {113.4f, -22.12f, 108.22f}, {135.55f, -11.88f, 40.29f}, {123.65f, -11.25f, 40.34f}, {114.58f, -12.42f, 45.05f}, {136.62f, -7.14f, 23.18f}, {124.53f, -6.55f, 22.8f}, {108.12f, -9.94f, 42.28f}, {96.15f, -9.87f, 48.28f}, {81.63f, -6.28f, 25.25f}, {111.81f, -3.68f, 15.7f}, {94.76f, -3.55f, 14.36f}, {77.4f, -3.7f, 12.7f}, {112.02f, -1.84f, 8.07f}, {93.79f, -1.15f, 5.83f}, {66.3f, -2.01f, 8.14f}, {119.72f, -39.76f, 112.48f}, {130.73f, -23.61f, 50.9f}, {114.94f, -28.34f, 57.27f}, {99.48f, -32.3f, 41.01f}, {132.54f, -14.53f, 28.59f}, {115.61f, -14.55f, 27.77f}, {134.65f, -5.8f, 12.91f}, {115.94f, -6.91f, 12.22f}, {87.05f, -28.42f, 85.51f}, {70.22f, -26.55f, 71.38f}, {93.79f, -21.48f, 39.46f}, {79.34f, -17.22f, 16.27f}, {93.6f, -6.75f, 11.13f}, {77.37f, -6.28f, 8.5f}, {111.41f, -59.72f, 30.07f}, {127.11f, -32.11f, 25.97f}, {109.17f, -40.81f, 28.62f}, {88.61f, -66.57f, 21.62f}, {67.11f, -51.92f, 15.14f}, {133.74f, -17.21f, 14.43f}, {125.81f, -19.66f, 14.74f}, {111.81f, -20.79f, 15.35f}, {92.91f, -26.4f, 16.57f}, {73.43f, -28.56f, 14.26f}, {101.33f, -67.13f, 9.23f}, {117.32f, -42.38f, 5.92f}, {97.21f, -60.67f, 1.44f}, {84.21f, -54.03f, 1.85f}, {128.3f, -20.57f, 5.18f}, {118.27f, -23.04f, 5.38f}, {102.04f, -27.55f, 4.29f}, {84.63f, -30.51f, 2.03f}, {74.26f, -18.71f, 1.71f}, {135.4f, -8.55f, 2.36f}, {118.99f, -7.54f, 1.85f}, {103.18f, -8.07f, 1.21f}, {87.53f, -9.04f, -0.09f}, {64.69f, -9.07f, 0.63f}, {137.93f, -3.51f, 0.49f}, {126.86f, -4.11f, 2.16f}, {111.91f, -4.48f, 0.92f}, {93.86f, -4.18f, -0.11f}, {64.8f, -4.67f, 0.87f}, {102.51f, -54.86f, -13.01f}, {110.38f, -61.37f, -9.23f}, {98.28f, -53.09f, -12.25f}, {77.98f, -44.39f, -8.89f}, {131.54f, -18.53f, -1.48f}, {118.89f, -21.45f, -3.22f}, {101.73f, -27.88f, -6.17f}, {81.24f, -44.47f, -11.53f}, {64.16f, -36.59f, -9.62f}, {102.55f, -45.67f, -27.1f}, {111.84f, -35.01f, -22.98f}, {98.26f, -43.75f, -26.51f}, {105.51f, -27.26f, -17.03f}, {129.79f, -11.21f, -7.82f}, {118.77f, -16.43f, -12.92f}, {101.89f, -19.64f, -16.38f}, {81.05f, -37.53f, -21.97f}, {67.39f, -30.39f, -21.58f}, {110.32f, -48.87f, -28.57f}, {113.31f, -14.53f, -28.76f}, {94.13f, -30.59f, -41.72f}, {78.54f, -25.56f, -36.75f}, {129.19f, -4.81f, -15.68f}, {118.28f, -8.15f, -19.57f}, {100.59f, -6.54f, -24.28f}, {69.44f, -23.22f, -33.04f}, {132.12f, -3.28f, -7.57f}, {119.71f, -3.01f, -6.59f}, {100.27f, -4.63f, -10.64f}, {85.27f, -4.74f, -9.08f}, {68.66f, -2.61f, -9.35f}, {137.85f, 0.44f, -1.2f}, {126.81f, -1.44f, -2.07f}, {111.89f, -1.18f, -2.37f}, {94.71f, -1.55f, -4.41f}, {66.09f, -1.39f, -5.11f}, {73.67f, 31.16f, -52.78f}, {108.21f, 6.51f, -27.19f}, {97.75f, 13.05f, -36.38f}, {69.75f, 19.62f, -38.92f}, {127.38f, 2.69f, -13.31f}, {115.65f, 4.11f, -18.51f}, {93.34f, 8.95f, -25.16f}, {68.19f, 10.8f, -23.95f}, {130.18f, 1.22f, -8.74f}, {115.78f, 2.22f, -10.83f}, {92.56f, 4.13f, -16.33f}, {106.38f, 30.27f, -36.66f}, {108.06f, 14.95f, -27.31f}, {94.78f, 21.95f, -33.81f}, {63.87f, 38.8f, -40.39f}, {133.47f, 8.11f, -13.45f}, {112.48f, 11.09f, -19.75f}, {94.1f, 17.79f, -24.35f}, {68.18f, 20.2f, -23.83f}, {129.44f, 3.46f, -8.45f}, {115.96f, 5.74f, -10.07f}, {91.69f, 10.22f, -15.3f}, {100.42f, 42.77f, -34.71f}, {122.69f, 24.51f, -21.48f}, {100.29f, 28.08f, -22.83f}, {82.78f, 37.71f, -29.84f}, {67.79f, 40.85f, -33.37f}, {128.71f, 10.38f, -8.75f}, {119.43f, 14.74f, -12.95f}, {102.79f, 21.08f, -17.21f}, {85.93f, 19.65f, -16.2f}, {63.21f, 27.3f, -21.07f}, {131.58f, 4.99f, -5.19f}, {118.84f, 7.29f, -4.88f}, {103.3f, 8.74f, -5.81f}, {86.18f, 10.63f, -6.05f}, {64.27f, 12.52f, -8.71f}, {136.72f, 1.28f, -0.23f}, {126.8f, 1.97f, -0.89f}, {112.07f, 2.45f, -1.1f}, {94.8f, 4.48f, -2.34f}, {64.85f, 3.45f, -2.45f}, {67.34f, 106.1f, -58.7f}, {99.97f, 37.46f, -18.41f}, {81.99f, 47.16f, -26.18f}, {70.44f, 50.69f, -30.48f}, {115.41f, 19.33f, -9.02f}, {102.31f, 24.91f, -12.59f}, {85.3f, 23.1f, -11.92f}, {62.54f, 31.44f, -19.19f}, {115.67f, 11.95f, -4.94f}, {103.05f, 14.36f, -6.6f}, {133.18f, 13.63f, -0.19f}, {120.68f, 26.04f, -2.96f}, {113.21f, 38.21f, -9.47f}, {129.98f, 13.9f, -1.45f}, {121.31f, 18.92f, -2.97f}, {114.27f, 21.7f, -1.13f}, {132.72f, 7.5f, -1.35f}, {123.24f, 8.79f, -1.44f}, {100.92f, 52.89f, -7.53f}, {97.83f, 46.17f, -6.94f}, {74.84f, 63.03f, -20.4f}, {66.3f, 54.43f, -22.03f}, {100.61f, 34.89f, -4.04f}, {82.95f, 30.99f, -6.26f}, {61.74f, 35.0f, -15.37f}, {114.78f, 12.83f, 0.84f}, {102.51f, 19.54f, -0.93f}, {139.92f, -0.1f, -0.36f}, {126.14f, -0.15f, 1.16f}, {111.2f, -0.33f, 0.9f}, {93.88f, -0.0f, 0.0f}, {64.96f, -0.0f, 0.0f}};
    static const QString nameDict[dictSize] = {tr("Vivid Pink"), tr("Strong Pink"), tr("Deep Pink"), tr("Light Pink"), tr("Moderate Pink"), tr("Dark Pink"), tr("Pale Pink"), tr("Grayish Pink"), tr("Pinkish White"), tr("Pinkish Gray"), tr("Vivid Red"), tr("Strong Red"), tr("Deep Red"), tr("Very Deep Red"), tr("Moderate Red"), tr("Dark Red"), tr("Very Dark Red"), tr("Light Grayish Red"), tr("Grayish Red"), tr("Dark Grayish Red"), tr("Blackish Red"), tr("Reddish Gray"), tr("Dark Reddish Gray"), tr("Reddish Black"), tr("Vivid Yellowish Pink"), tr("Strong Yellowish Pink"), tr("Deep Yellowish Pink"), tr("Light Yellowish Pink"), tr("Moderate Yellowish Pink"), tr("Dark Yellowish Pink"), tr("Pale Yellowish Pink"), tr("Grayish Yellowish Pink"), tr("Brownish Pink"), tr("Vivid Reddish Orange"), tr("Strong Reddish Orange"), tr("Deep Reddish Orange"), tr("Moderate Reddish Orange"), tr("Dark Reddish Orange"), tr("Grayish Reddish Orange"), tr("Strong Reddish Brown"), tr("Deep Reddish Brown"), tr("Light Reddish Brown"), tr("Moderate Reddish Brown"), tr("Dark Reddish Brown"), tr("Light Grayish Reddish Brown"), tr("Grayish Reddish Brown"), tr("Dark Grayish Reddish Brown"), tr("Vivid Orange"), tr("Brilliant Orange"), tr("Strong Orange"), tr("Deep Orange"), tr("Light Orange"), tr("Moderate Orange"), tr("Brownish Orange"), tr("Strong Brown"), tr("Deep Brown"), tr("Light Brown"), tr("Moderate Brown"), tr("Dark Brown"), tr("Light Grayish Brown"), tr("Grayish Brown"), tr("Dark Grayish Brown"), tr("Light Brownish Gray"), tr("Brownish Gray"), tr("Brownish Black"), tr("Vivid Orange Yellow"), tr("Brilliant Orange Yellow"), tr("Strong Orange Yellow"), tr("Deep Orange Yellow"), tr("Light Orange Yellow"), tr("Moderate Orange Yellow"), tr("Dark Orange Yellow"), tr("Pale Orange Yellow"), tr("Strong Yellowish Brown"), tr("Deep Yellowish Brown"), tr("Light Yellowish Brown"), tr("Moderate Yellowish Brown"), tr("Dark Yellowish Brown"), tr("Light Grayish Yellowish Brown"), tr("Grayish Yellowish Brown"), tr("Dark Grayish Yellowish Brown"), tr("Vivid Yellow"), tr("Brilliant Yellow"), tr("Strong Yellow"), tr("Deep Yellow"), tr("Light Yellow"), tr("Moderate Yellow"), tr("Dark Yellow"), tr("Pale Yellow"), tr("Grayish Yellow"), tr("Dark Grayish Yellow"), tr("Yellowish White"), tr("Yellowish Gray"), tr("Light Olive Brown"), tr("Moderate Olive Brown"), tr("Dark Olive Brown"), tr("Vivid Greenish Yellow"), tr("Brilliant Greenish Yellow"), tr("Strong Greenish Yellow"), tr("Deep Greenish Yellow"), tr("Light Greenish Yellow"), tr("Moderate Greenish Yellow"), tr("Dark Greenish Yellow"), tr("Pale Greenish Yellow"), tr("Grayish Greenish Yellow"), tr("Light Olive"), tr("Moderate Olive"), tr("Dark Olive"), tr("Light Grayish Olive"), tr("Grayish Olive"), tr("Dark Grayish Olive"), tr("Light Olive Gray"), tr("Olive Gray"), tr("Olive Black"), tr("Vivid Yellow Green"), tr("Brilliant Yellow Green"), tr("Strong Yellow Green"), tr("Deep Yellow Green"), tr("Light Yellow Green"), tr("Moderate Yellow Green"), tr("Pale Yellow Green"), tr("Grayish Yellow Green"), tr("Strong Olive Green"), tr("Deep Olive Green"), tr("Moderate Olive Green"), tr("Dark Olive Green"), tr("Grayish Olive Green"), tr("Dark Grayish Olive Green"), tr("Vivid Yellowish Green"), tr("Brilliant Yellowish Green"), tr("Strong Yellowish Green"), tr("Deep Yellowish Green"), tr("Very Deep Yellowish Green"), tr("Very Light Yellowish Green"), tr("Light Yellowish Green"), tr("Moderate Yellowish Green"), tr("Dark Yellowish Green"), tr("Very Dark Yellowish Green"), tr("Vivid Green"), tr("Brilliant Green"), tr("Strong Green"), tr("Deep Green"), tr("Very Light Green"), tr("Light Green"), tr("Moderate Green"), tr("Dark Green"), tr("Very Dark Green"), tr("Very Pale Green"), tr("Pale Green"), tr("Grayish Green"), tr("Dark Grayish Green"), tr("Blackish Green"), tr("Greenish White"), tr("Light Greenish Gray"), tr("Greenish Gray"), tr("Dark Greenish Gray"), tr("Greenish Black"), tr("Vivid Bluish Green"), tr("Brilliant Bluish Green"), tr("Strong Bluish Green"), tr("Deep Bluish Green"), tr("Very Light Bluish Green"), tr("Light Bluish Green"), tr("Moderate Bluish Green"), tr("Dark Bluish Green"), tr("Very Dark Bluish Green"), tr("Vivid Greenish Blue"), tr("Brilliant Greenish Blue"), tr("Strong Greenish Blue"), tr("Deep Greenish Blue"), tr("Very Light Greenish Blue"), tr("Light Greenish Blue"), tr("Moderate Greenish Blue"), tr("Dark Greenish Blue"), tr("Very Dark Greenish Blue"), tr("Vivid Blue"), tr("Brilliant Blue"), tr("Strong Blue"), tr("Deep Blue"), tr("Very Light Blue"), tr("Light Blue"), tr("Moderate Blue"), tr("Dark Blue"), tr("Very Pale Blue"), tr("Pale Blue"), tr("Grayish Blue"), tr("Dark Grayish Blue"), tr("Blackish Blue"), tr("Bluish White"), tr("Light Bluish Gray"), tr("Bluish Gray"), tr("Dark Bluish Gray"), tr("Bluish Black"), tr("Vivid Purplish Blue"), tr("Brilliant Purplish Blue"), tr("Strong Purplish Blue"), tr("Deep Purplish Blue"), tr("Very Light Purplish Blue"), tr("Light Purplish Blue"), tr("Moderate Purplish Blue"), tr("Dark Purplish Blue"), tr("Very Pale Purplish Blue"), tr("Pale Purplish Blue"), tr("Grayish Purplish Blue"), tr("Vivid Violet"), tr("Brilliant Violet"), tr("Strong Violet"), tr("Deep Violet"), tr("Very Light Violet"), tr("Light Violet"), tr("Moderate Violet"), tr("Dark Violet"), tr("Very Pale Violet"), tr("Pale Violet"), tr("Grayish Violet"), tr("Vivid Purple"), tr("Brilliant Purple"), tr("Strong Purple"), tr("Deep Purple"), tr("Very Deep Purple"), tr("Very Light Purple"), tr("Light Purple"), tr("Moderate Purple"), tr("Dark Purple"), tr("Very Dark Purple"), tr("Very Pale Purple"), tr("Pale Purple"), tr("Grayish Purple"), tr("Dark Grayish Purple"), tr("Blackish Purple"), tr("Purplish White"), tr("Light Purplish Gray"), tr("Purplish Gray"), tr("Dark Purplish Gray"), tr("Purplish Black"), tr("Vivid Reddish Purple"), tr("Strong Reddish Purple"), tr("Deep Reddish Purple"), tr("Very Deep Reddish Purple"), tr("Light Reddish Purple"), tr("Moderate Reddish Purple"), tr("Dark Reddish Purple"), tr("Very Dark Reddish Purple"), tr("Pale Reddish Purple"), tr("Grayish Reddish Purple"), tr("Brilliant Purplish Pink"), tr("Strong Purplish Pink"), tr("Deep Purplish Pink"), tr("Light Purplish Pink"), tr("Moderate Purplish Pink"), tr("Dark Purplish Pink"), tr("Pale Purplish Pink"), tr("Grayish Purplish Pink"), tr("Vivid Purplish Red"), tr("Strong Purplish Red"), tr("Deep Purplish Red"), tr("Very Deep Purplish Red"), tr("Moderate Purplish Red"), tr("Dark Purplish Red"), tr("Very Dark Purplish Red"), tr("Light Grayish Purplish Red"), tr("Grayish Purplish Red"), tr("White"), tr("Light Gray"), tr("Medium Gray"), tr("Dark Gray"), tr("Black")};

    // Separate rgb values for convenience
    const int r = c.red(),
                g = c.green(),
                b = c.blue();

    // Convert RGB to XYZ with D65 white point (algorithm source: https://www.cs.rit.edu/%7Encs/color/t_convert.html#RGB%20to%20XYZ%20&%20XYZ%20to%20RGB)
    const qreal x = 0.412453*r + 0.357580*g + 0.180423*b,
                y = 0.212671*r + 0.715160*g + 0.072169*b,
                z = 0.019334*r + 0.119193*g + 0.950227*b;

    // Convert XYZ to CEI L*u*v (algorithm source: https://www.cs.rit.edu/~ncs/color/t_convert.html#XYZ%20to%20CIE%20L*a*b*%20(CIELAB)%20&%20CIELAB%20to%20XYZ)
    // Helper function for the conversion
    auto f = [] (const double a) { return a > 0.008856 ? cbrt(a) : 7.787*a + 16/116; };
    // XYZ tristimulus values for D65 (taken from: https://en.wikipedia.org/wiki/Illuminant_D65#Definition)
    const qreal xn = 95.047,
                yn = 100,
                zn = 108.883;
    const qreal l = y/yn > 0.008856 ? 116*cbrt( y/yn ) - 16 : 903.3*y/yn,
                u = 500 * ( f( x/xn ) - f( y/yn ) ),
                v = 200 * ( f( y/yn ) - f( z/zn ) );

    // Find closest color match in colorDict to the luv values
    int minLoc = 0;
    if(u < 0.01 && u > -0.01 && v < 0.01 && v > -0.01) {
        // The color is grayscale so only compare to gray centroids so there is no 'false hue'
        qreal minDist = pow(colorDict[dictSize-5][0] - l, 2) + pow(colorDict[dictSize-5][1] - u, 2) + pow(colorDict[dictSize-5][2] - v, 2);
        qreal curDist;
        for ( int i = dictSize-4; i < dictSize; i++)
        {
            curDist = pow(colorDict[i][0] - l, 2) + pow(colorDict[i][1] - u, 2) + pow(colorDict[i][2] - v, 2);
            if ( curDist < minDist )
            {
                minDist = curDist;
                minLoc = i;
            }
        }
    }
    else {
        qreal minDist = pow(colorDict[0][0] - l, 2) + pow(colorDict[0][1] - u, 2) + pow(colorDict[0][2] - v, 2);
        qreal curDist;
        for ( int i = 1; i < dictSize; i++)
        {
            curDist = pow(colorDict[i][0] - l, 2) + pow(colorDict[i][1] - u, 2) + pow(colorDict[i][2] - v, 2);
            if ( curDist < minDist )
            {
                minDist = curDist;
                minLoc = i;
            }
        }
    }
    return nameDict[minLoc];
}

void ColorPaletteWidget::clickAddColorButton()
{
    QColor prevColor = Qt::white;

    if ( currentColourNumber() > -1 )
    {
        prevColor = editor()->object()->getColour(currentColourNumber()).colour;
    }

    QColor newColour = QColorDialog::getColor( prevColor.rgba(), this, QString(), QColorDialog::ShowAlphaChannel );
    if ( !newColour.isValid() )
    {
        // User cancelled operation
        return;
    }

    ColourRef ref(newColour);
    ref.name = getDefaultColorName(newColour);

    editor()->object()->addColour(ref);
    refreshColorList();

    int colorIndex = editor()->object()->getColourCount() - 1;

    editor()->color()->setColorNumber(colorIndex);
    editor()->color()->setColor( ref.colour );
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
