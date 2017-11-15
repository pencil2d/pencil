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
#include "colorpalettewidget.h"

#include <QDebug>
#include <QListWidget>
#include <QListWidgetItem>
#include <QInputDialog>
#include <QColorDialog>
#include "ui_colorpalette.h"

#include "colordictionary.h"
#include "colourref.h"
#include "object.h"
#include "editor.h"
#include "colorbox.h"
#include "scribblearea.h"
#include "colormanager.h"


ColorPaletteWidget::ColorPaletteWidget(QWidget* parent) : BaseDockWidget(parent)
{
    setWindowTitle(tr("Color Palette", "Window title of color palette."));

    QWidget* pWidget = new QWidget(this);
    ui = new Ui::ColorPalette;
    ui->setupUi(pWidget);
    setWidget(pWidget);

    connect(ui->colorListWidget, &QListWidget::currentItemChanged,
            this, &ColorPaletteWidget::colorListCurrentItemChanged);

    connect(ui->colorListWidget, &QListWidget::itemClicked, this, &ColorPaletteWidget::clickColorListItem);

    connect(ui->colorListWidget, &QListWidget::itemDoubleClicked, this, &ColorPaletteWidget::changeColourName);
    connect(ui->colorListWidget, &QListWidget::currentTextChanged, this, &ColorPaletteWidget::onActiveColorNameChange);

    connect(ui->addColorButton, &QPushButton::clicked, this, &ColorPaletteWidget::clickAddColorButton);
    connect(ui->removeColorButton, &QPushButton::clicked, this, &ColorPaletteWidget::clickRemoveColorButton);
    connect(ui->palettePref, &QToolButton::clicked, this, &ColorPaletteWidget::palettePreferences);
}

void ColorPaletteWidget::initUI()
{
    // "Remove color" feature is disabled because
    // vector strokes that are linked to palette
    // colors don't handle color removal from palette
    //
    mIconSize = QSize(34, 34);
    ui->removeColorButton->hide();
    updateUI();
    palettePreferences();
}

void ColorPaletteWidget::updateUI()
{
    refreshColorList();
    updateGridUI();
}

void ColorPaletteWidget::setColor(QColor newColor, int colorIndex)
{
    QSignalBlocker b(ui->colorListWidget);
    ui->colorListWidget->setCurrentRow(colorIndex);

    if (colorIndex > 0)
    {
        updateItemColor(colorIndex, newColor);
        emit colorChanged(newColor);
    }
}

void ColorPaletteWidget::selectColorNumber(int colorNumber)
{
    ui->colorListWidget->setCurrentRow(colorNumber);
}

int ColorPaletteWidget::currentColourNumber()
{
    if (ui->colorListWidget->currentRow() < 0)
    {
        ui->colorListWidget->setCurrentRow(0);
    }
    return ui->colorListWidget->currentRow();
}

void ColorPaletteWidget::refreshColorList()
{
    QSignalBlocker b(ui->colorListWidget);

    if (ui->colorListWidget->count() > 0)
    {
        ui->colorListWidget->clear();
    }

    QPixmap originalColourSwatch(mIconSize);
    QPainter swatchPainter(&originalColourSwatch);
    swatchPainter.drawTiledPixmap(0, 0, mIconSize.width(), mIconSize.height(), QPixmap(":/background/checkerboard.png"));
    swatchPainter.end();
    QPixmap colourSwatch;

    QListWidgetItem* colourItem;
    ColourRef colourRef;
    for (int i = 0; i < editor()->object()->getColourCount(); i++)
    {
        colourRef = editor()->object()->getColour(i);

        colourItem = new QListWidgetItem(ui->colorListWidget);

        if (ui->colorListWidget->viewMode() != QListView::IconMode)
        {
            colourItem->setText(colourRef.name);
        }
        else
        {
            colourItem->setToolTip(colourRef.name);
        }
        colourSwatch = originalColourSwatch;
        swatchPainter.begin(&colourSwatch);
        swatchPainter.fillRect(0, 0, mIconSize.width(), mIconSize.height(), colourRef.colour);
        swatchPainter.end();
        colourItem->setIcon(colourSwatch);
        colourItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);

        ui->colorListWidget->addItem(colourItem);
    }
    update();
}

void ColorPaletteWidget::changeColourName(QListWidgetItem* item)
{
    Q_ASSERT(item != NULL);

    if (ui->colorListWidget->viewMode() == QListView::IconMode)
    {
        int colorNumber = ui->colorListWidget->row(item);
        if (colorNumber > -1)
        {
            bool ok;
            QString text = QInputDialog::getText(this,
                                                 tr("Colour name"),
                                                 tr("Colour name"),
                                                 QLineEdit::Normal,
                                                 editor()->object()->getColour(colorNumber).name,
                                                 &ok);
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
    if (!name.isNull())
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
    emit colorNumberChanged(ui->colorListWidget->row(current));
}

void ColorPaletteWidget::clickColorListItem(QListWidgetItem* currentItem)
{
    int colorIndex = ui->colorListWidget->row(currentItem);

    emit colorNumberChanged(colorIndex);
}

void ColorPaletteWidget::palettePreferences()
{
    mLayoutModeActionGroup = new QActionGroup(this);
    mListModeAction = new QAction(tr("List mode"), this);
    mListModeAction->setStatusTip(tr("Show palette as a list"));
    mListModeAction->setCheckable(true);
    mListModeAction->setChecked(true);
    mLayoutModeActionGroup->addAction(mListModeAction);
    connect(mListModeAction, &QAction::triggered, this, &ColorPaletteWidget::setListMode);

    mGridModeAction = new QAction(tr("Grid mode"), this);
    mGridModeAction->setStatusTip(tr("Show palette as icons"));
    mGridModeAction->setCheckable(true);
    mLayoutModeActionGroup->addAction(mGridModeAction);
    connect(mGridModeAction, &QAction::triggered, this, &ColorPaletteWidget::setGridMode);

    // Swatch size control
    mIconSizesActionGroup = new QActionGroup(this);
    mSmallSwatchAction = new QAction(tr("Small swatch"), this);
    mSmallSwatchAction->setStatusTip(tr("Sets swatch size to: 16x16px"));
    mSmallSwatchAction->setCheckable(true);
    mIconSizesActionGroup->addAction(mSmallSwatchAction);
    connect(mSmallSwatchAction, &QAction::triggered, this, &ColorPaletteWidget::setSwatchSizeSmall);

    mMediumSwatchAction = new QAction(tr("Medium swatch"), this);
    mMediumSwatchAction->setStatusTip(tr("Sets swatch size to: 26x26px"));
    mMediumSwatchAction->setCheckable(true);
    mIconSizesActionGroup->addAction(mMediumSwatchAction);
    connect(mMediumSwatchAction, &QAction::triggered, this, &ColorPaletteWidget::setSwatchSizeMedium);

    mLargeSwatchAction = new QAction(tr("Large swatch"), this);
    mLargeSwatchAction->setCheckable(true);
    mLargeSwatchAction->setStatusTip(tr("Sets swatch size to: 36x36px"));
    mIconSizesActionGroup->addAction(mLargeSwatchAction);
    mLargeSwatchAction->setChecked(true);
    connect(mLargeSwatchAction, &QAction::triggered, this, &ColorPaletteWidget::setSwatchSizeLarge);

    ui->colorListWidget->setMinimumWidth(ui->colorListWidget->sizeHintForColumn(0));

    // Let's pretend this button is a separator
    mSeparator = new QAction(tr(""), this);
    mSeparator->setSeparator(true);

    // Add to UI
    ui->palettePref->addAction(mListModeAction);
    ui->palettePref->addAction(mGridModeAction);
    ui->palettePref->addAction(mSeparator);
    ui->palettePref->addAction(mSmallSwatchAction);
    ui->palettePref->addAction(mMediumSwatchAction);
    ui->palettePref->addAction(mLargeSwatchAction);
}

void ColorPaletteWidget::setListMode()
{
    ui->colorListWidget->setViewMode(QListView::ListMode);
    ui->colorListWidget->setMovement(QListView::Static);
    ui->colorListWidget->setGridSize(QSize(-1, -1));
    updateUI();
}

void ColorPaletteWidget::setGridMode()
{
    ui->colorListWidget->setViewMode(QListView::IconMode);
    ui->colorListWidget->setMovement(QListView::Static); // TODO: update swatch index on move
    ui->colorListWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    ui->colorListWidget->setGridSize(mIconSize);
    updateUI();
}

void ColorPaletteWidget::resizeEvent(QResizeEvent* event)
{
    if (ui->colorListWidget->viewMode() == QListView::IconMode)
    {
        // Find the value to divivde with
        for (int i = 1; i < 75; i++)
        {
            int size = (ui->colorListWidget->width() - 18) / i; // subtract scrollbar width
            if (size >= mIconSize.width() && size <= mIconSize.width() + 8)
            {
                stepper = size;
            }
        }
        QSize tempSize = QSize(stepper, mIconSize.height());

        ui->colorListWidget->setIconSize(QSize(tempSize.width(), mIconSize.height()));
        ui->colorListWidget->setGridSize(QSize(tempSize.width(), mIconSize.height()));
        mIconSize.setWidth(mIconSize.width());
    }
    else
    {
        ui->colorListWidget->setIconSize(mIconSize);
        ui->colorListWidget->setGridSize(QSize(-1, -1));
    }

    //refreshColorList();
    QWidget::resizeEvent(event);
}

void ColorPaletteWidget::setSwatchSizeSmall()
{
    if (mIconSize.width() > 18)
    {
        mIconSize = QSize(14, 14);
    }
    updateUI();
}

void ColorPaletteWidget::setSwatchSizeMedium()
{
    if (mIconSize.width() < 20 || mIconSize.width() > 30)
    {
        mIconSize = QSize(26, 26);
        updateUI();
    }
}

void ColorPaletteWidget::setSwatchSizeLarge()
{
    if (mIconSize.width() < 30)
    {
        mIconSize = QSize(34, 34);
        updateUI();
    }
}

void ColorPaletteWidget::updateGridUI()
{
    if (ui->colorListWidget->viewMode() == QListView::IconMode)
        ui->colorListWidget->setGridSize(QSize(mIconSize.width(), mIconSize.height()));
    else
        ui->colorListWidget->setGridSize(QSize(-1, -1));

    ui->colorListWidget->setIconSize(mIconSize);
}

QString ColorPaletteWidget::getDefaultColorName(QColor c)
{
    // Separate rgb values for convenience
    const int r = c.red();
    const int g = c.green();
    const int b = c.blue();

    // Convert RGB to XYZ with D65 white point
    // (algorithm source: https://www.cs.rit.edu/%7Encs/color/t_convert.html#RGB%20to%20XYZ%20&%20XYZ%20to%20RGB)
    const qreal x = 0.412453*r + 0.357580*g + 0.180423*b;
    const qreal y = 0.212671*r + 0.715160*g + 0.072169*b;
    const qreal z = 0.019334*r + 0.119193*g + 0.950227*b;

    // Convert XYZ to CEI L*u*v
    // (algorithm source: https://www.cs.rit.edu/~ncs/color/t_convert.html#XYZ%20to%20CIE%20L*a*b*%20(CIELAB)%20&%20CIELAB%20to%20XYZ)
    // Helper function for the conversion
    auto f = [](const double a) { return a > 0.008856 ? cbrt(a) : 7.787 * a + 16 / 116; };
    // XYZ tristimulus values for D65 (taken from: https://en.wikipedia.org/wiki/Illuminant_D65#Definition)
    const qreal xn = 95.047,
        yn = 100,
        zn = 108.883;
    const qreal l = y / yn > 0.008856 ? 116 * cbrt(y / yn) - 16 : 903.3*y / yn,
        u = 500 * (f(x / xn) - f(y / yn)),
        v = 200 * (f(y / yn) - f(z / zn));

    // Find closest color match in colorDict to the luv values
    int minLoc = 0;
    if (u < 0.01 && u > -0.01 && v < 0.01 && v > -0.01)
    {
        // The color is grayscale so only compare to gray centroids so there is no 'false hue'
        qreal minDist = pow(colorDict[dictSize - 5][0] - l, 2) + pow(colorDict[dictSize - 5][1] - u, 2) + pow(colorDict[dictSize - 5][2] - v, 2);
        qreal curDist;
        for (int i = dictSize - 4; i < dictSize; i++)
        {
            curDist = pow(colorDict[i][0] - l, 2) + pow(colorDict[i][1] - u, 2) + pow(colorDict[i][2] - v, 2);
            if (curDist < minDist)
            {
                minDist = curDist;
                minLoc = i;
            }
        }
    }
    else
    {
        qreal minDist = pow(colorDict[0][0] - l, 2) + pow(colorDict[0][1] - u, 2) + pow(colorDict[0][2] - v, 2);
        qreal curDist;
        for (int i = 1; i < dictSize; i++)
        {
            curDist = pow(colorDict[i][0] - l, 2) + pow(colorDict[i][1] - u, 2) + pow(colorDict[i][2] - v, 2);
            if (curDist < minDist)
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

    if (currentColourNumber() > -1)
    {
        prevColor = editor()->object()->getColour(currentColourNumber()).colour;
    }

    QColor newColour = QColorDialog::getColor(prevColor.rgba(), this, QString(), QColorDialog::ShowAlphaChannel);
    if (!newColour.isValid())
    {
        // User canceled operation
        return;
    }

    ColourRef ref(newColour);
    ref.name = getDefaultColorName(newColour);

    editor()->object()->addColour(ref);
    refreshColorList();

    int colorIndex = editor()->object()->getColourCount() - 1;

    editor()->color()->setColorNumber(colorIndex);
    editor()->color()->setColor(ref.colour);
}

void ColorPaletteWidget::clickRemoveColorButton()
{
    int colorNumber = ui->colorListWidget->currentRow();
    editor()->object()->removeColour(colorNumber);

    refreshColorList();
}

void ColorPaletteWidget::updateItemColor(int itemIndex, QColor newColor)
{
    QPixmap colourSwatch(mIconSize);
    QPainter swatchPainter(&colourSwatch);
    swatchPainter.drawTiledPixmap(0, 0, mIconSize.width(), mIconSize.height(), QPixmap(":/background/checkerboard.png"));
    swatchPainter.fillRect(0, 0, mIconSize.width(), mIconSize.height(), newColor);
    ui->colorListWidget->item(itemIndex)->setIcon(colourSwatch);

    // Make sure to update grid in grid mode
    if (ui->colorListWidget->viewMode() == QListView::IconMode)
    {
        updateGridUI();
    }
}
