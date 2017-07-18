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

    connect( ui->colorListWidget, SIGNAL( itemDoubleClicked( QListWidgetItem* ) ), this,
             SLOT( changeColourName( QListWidgetItem* ) ) );

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

QString ColorPaletteWidget::getDefaultColorName(QColor c)
{
    // Initialize NBS/ISCC Color dictionary, John Foster version (http://tx4.us/nbs-iscc.htm), converted to CIE L*u*v with D65 white point and rounded.
    const int dictSize = 267;
    static const float colorDict[dictSize][3] = {{129.44, 17.49, 4.51}, {121.33, 22.84, 5.93}, {113.18, 33.56, 8.5}, {133.35, 9.63, 4.25}, {124.33, 13.9, 5.56}, {114.29, 18.26, 6.35}, {134.81, 3.79, 1.72}, {124.74, 5.33, 2.33}, {136.75, 1.26, 1.15}, {126.18, 2.3, 2.04}, {72.24, 105.85, -3.37}, {96.15, 46.92, 11.81}, {76.45, 57.74, 6.63}, {60.36, 67.58, -6.78}, {99.25, 33.26, 10.64}, {82.82, 33.5, 6.35}, {63.94, 34.51, -7.72}, {114.86, 9.93, 5.6}, {101.47, 17.71, 6.68}, {85.01, 11.4, 2.58}, {64.08, 14.21, 0.16}, {111.19, 3.94, 2.43}, {92.79, 4.76, 2.41}, {64.84, 6.75, -0.05}, {129.42, 14.32, 12.7}, {121.57, 22.27, 21.12}, {108.94, 27.15, 63.12}, {131.27, 11.24, 4.1}, {124.34, 12.97, 3.49}, {115.02, 16.75, 11.37}, {134.12, 3.17, 7.08}, {124.53, 5.15, 6.1}, {123.87, 3.0, 9.3}, {104.98, 34.69, 57.26}, {106.95, 31.37, 37.98}, {90.95, 39.91, 43.0}, {109.61, 23.82, 26.21}, {95.16, 28.7, 28.57}, {109.96, 15.89, 18.56}, {83.32, 36.66, 41.32}, {56.25, 63.71, 23.86}, {111.34, 10.81, 12.04}, {91.06, 19.99, 14.9}, {66.59, 22.89, 7.83}, {111.0, 5.37, 8.26}, {92.3, 10.08, 7.38}, {77.22, 9.99, 5.68}, {115.62, 11.46, 112.43}, {120.85, 15.44, 53.69}, {116.85, 14.84, 62.0}, {105.26, 15.4, 69.99}, {128.12, 9.48, 27.39}, {118.39, 11.64, 34.12}, {105.93, 13.96, 38.36}, {91.34, 14.66, 46.17}, {80.29, 12.55, 33.67}, {110.55, 7.93, 21.01}, {93.17, 8.49, 20.95}, {71.03, 13.95, 22.13}, {111.03, 3.81, 9.84}, {93.5, 5.19, 9.05}, {77.35, 4.95, 7.72}, {111.22, 2.2, 5.43}, {92.69, 4.35, 2.27}, {64.49, 4.44, 6.94}, {122.72, -2.14, 117.46}, {129.65, 0.32, 54.79}, {121.9, 0.89, 78.29}, {113.1, -0.81, 109.4}, {131.67, 2.41, 32.31}, {123.71, 4.02, 41.99}, {114.84, 3.18, 46.57}, {134.44, 2.17, 19.81}, {102.38, 3.33, 67.17}, {88.58, 5.99, 34.17}, {119.36, 3.77, 24.66}, {102.09, 3.5, 23.67}, {80.15, 5.21, 23.45}, {118.89, 1.55, 12.34}, {104.19, 2.36, 12.21}, {82.82, 3.37, 10.09}, {127.96, -13.63, 121.11}, {133.92, -7.58, 51.81}, {123.58, -6.91, 62.29}, {113.59, -9.28, 82.32}, {135.01, -5.5, 37.33}, {123.49, -4.13, 38.02}, {115.29, -4.34, 42.05}, {136.63, -3.61, 20.49}, {124.53, -2.67, 21.45}, {114.82, -2.04, 24.04}, {137.97, -0.97, 6.67}, {126.17, -0.77, 7.59}, {105.31, -3.69, 68.07}, {93.97, -1.94, 45.65}, {76.05, 0.87, 17.48}, {129.59, -25.16, 121.74}, {134.5, -16.57, 60.21}, {123.68, -17.24, 68.88}, {113.4, -22.12, 108.22}, {135.55, -11.88, 40.29}, {123.65, -11.25, 40.34}, {114.58, -12.42, 45.05}, {136.62, -7.14, 23.18}, {124.53, -6.55, 22.8}, {108.12, -9.94, 42.28}, {96.15, -9.87, 48.28}, {81.63, -6.28, 25.25}, {111.81, -3.68, 15.7}, {94.76, -3.55, 14.36}, {77.4, -3.7, 12.7}, {112.02, -1.84, 8.07}, {93.79, -1.15, 5.83}, {66.3, -2.01, 8.14}, {119.72, -39.76, 112.48}, {130.73, -23.61, 50.9}, {114.94, -28.34, 57.27}, {99.48, -32.3, 41.01}, {132.54, -14.53, 28.59}, {115.61, -14.55, 27.77}, {134.65, -5.8, 12.91}, {115.94, -6.91, 12.22}, {87.05, -28.42, 85.51}, {70.22, -26.55, 71.38}, {93.79, -21.48, 39.46}, {79.34, -17.22, 16.27}, {93.6, -6.75, 11.13}, {77.37, -6.28, 8.5}, {111.41, -59.72, 30.07}, {127.11, -32.11, 25.97}, {109.17, -40.81, 28.62}, {88.61, -66.57, 21.62}, {67.11, -51.92, 15.14}, {133.74, -17.21, 14.43}, {125.81, -19.66, 14.74}, {111.81, -20.79, 15.35}, {92.91, -26.4, 16.57}, {73.43, -28.56, 14.26}, {101.33, -67.13, 9.23}, {117.32, -42.38, 5.92}, {97.21, -60.67, 1.44}, {84.21, -54.03, 1.85}, {128.3, -20.57, 5.18}, {118.27, -23.04, 5.38}, {102.04, -27.55, 4.29}, {84.63, -30.51, 2.03}, {74.26, -18.71, 1.71}, {135.4, -8.55, 2.36}, {118.99, -7.54, 1.85}, {103.18, -8.07, 1.21}, {87.53, -9.04, -0.09}, {64.69, -9.07, 0.63}, {137.93, -3.51, 0.49}, {126.86, -4.11, 2.16}, {111.91, -4.48, 0.92}, {93.86, -4.18, -0.11}, {64.8, -4.67, 0.87}, {102.51, -54.86, -13.01}, {110.38, -61.37, -9.23}, {98.28, -53.09, -12.25}, {77.98, -44.39, -8.89}, {131.54, -18.53, -1.48}, {118.89, -21.45, -3.22}, {101.73, -27.88, -6.17}, {81.24, -44.47, -11.53}, {64.16, -36.59, -9.62}, {102.55, -45.67, -27.1}, {111.84, -35.01, -22.98}, {98.26, -43.75, -26.51}, {105.51, -27.26, -17.03}, {129.79, -11.21, -7.82}, {118.77, -16.43, -12.92}, {101.89, -19.64, -16.38}, {81.05, -37.53, -21.97}, {67.39, -30.39, -21.58}, {110.32, -48.87, -28.57}, {113.31, -14.53, -28.76}, {94.13, -30.59, -41.72}, {78.54, -25.56, -36.75}, {129.19, -4.81, -15.68}, {118.28, -8.15, -19.57}, {100.59, -6.54, -24.28}, {69.44, -23.22, -33.04}, {132.12, -3.28, -7.57}, {119.71, -3.01, -6.59}, {100.27, -4.63, -10.64}, {85.27, -4.74, -9.08}, {68.66, -2.61, -9.35}, {137.85, 0.44, -1.2}, {126.81, -1.44, -2.07}, {111.89, -1.18, -2.37}, {94.71, -1.55, -4.41}, {66.09, -1.39, -5.11}, {73.67, 31.16, -52.78}, {108.21, 6.51, -27.19}, {97.75, 13.05, -36.38}, {69.75, 19.62, -38.92}, {127.38, 2.69, -13.31}, {115.65, 4.11, -18.51}, {93.34, 8.95, -25.16}, {68.19, 10.8, -23.95}, {130.18, 1.22, -8.74}, {115.78, 2.22, -10.83}, {92.56, 4.13, -16.33}, {106.38, 30.27, -36.66}, {108.06, 14.95, -27.31}, {94.78, 21.95, -33.81}, {63.87, 38.8, -40.39}, {133.47, 8.11, -13.45}, {112.48, 11.09, -19.75}, {94.1, 17.79, -24.35}, {68.18, 20.2, -23.83}, {129.44, 3.46, -8.45}, {115.96, 5.74, -10.07}, {91.69, 10.22, -15.3}, {100.42, 42.77, -34.71}, {122.69, 24.51, -21.48}, {100.29, 28.08, -22.83}, {82.78, 37.71, -29.84}, {67.79, 40.85, -33.37}, {128.71, 10.38, -8.75}, {119.43, 14.74, -12.95}, {102.79, 21.08, -17.21}, {85.93, 19.65, -16.2}, {63.21, 27.3, -21.07}, {131.58, 4.99, -5.19}, {118.84, 7.29, -4.88}, {103.3, 8.74, -5.81}, {86.18, 10.63, -6.05}, {64.27, 12.52, -8.71}, {136.72, 1.28, -0.23}, {126.8, 1.97, -0.89}, {112.07, 2.45, -1.1}, {94.8, 4.48, -2.34}, {64.85, 3.45, -2.45}, {67.34, 106.1, -58.7}, {99.97, 37.46, -18.41}, {81.99, 47.16, -26.18}, {70.44, 50.69, -30.48}, {115.41, 19.33, -9.02}, {102.31, 24.91, -12.59}, {85.3, 23.1, -11.92}, {62.54, 31.44, -19.19}, {115.67, 11.95, -4.94}, {103.05, 14.36, -6.6}, {133.18, 13.63, -0.19}, {120.68, 26.04, -2.96}, {113.21, 38.21, -9.47}, {129.98, 13.9, -1.45}, {121.31, 18.92, -2.97}, {114.27, 21.7, -1.13}, {132.72, 7.5, -1.35}, {123.24, 8.79, -1.44}, {100.92, 52.89, -7.53}, {97.83, 46.17, -6.94}, {74.84, 63.03, -20.4}, {66.3, 54.43, -22.03}, {100.61, 34.89, -4.04}, {82.95, 30.99, -6.26}, {61.74, 35.0, -15.37}, {114.78, 12.83, 0.84}, {102.51, 19.54, -0.93}, {139.92, -0.1, -0.36}, {126.14, -0.15, 1.16}, {111.2, -0.33, 0.9}, {93.88, -0.0, 0.0}, {64.96, -0.0, 0.0}};
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
