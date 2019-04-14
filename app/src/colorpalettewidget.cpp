/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2013-2018 Matt Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/
#include "colorpalettewidget.h"
#include "ui_colorpalette.h"

// Standard libraries
#include <cmath>

// Qt
#include <QDebug>
#include <QListWidget>
#include <QListWidgetItem>
#include <QInputDialog>
#include <QColorDialog>
#include <QToolBar>
#include <QMessageBox>
#include <QPushButton>
#include <QSettings>
#include <QMenu>

// Project
#include "colourref.h"
#include "object.h"
#include "editor.h"
#include "colorbox.h"
#include "scribblearea.h"
#include "colormanager.h"


ColorPaletteWidget::ColorPaletteWidget(QWidget* parent) :
    BaseDockWidget(parent),
    ui(new Ui::ColorPalette)
{
    ui->setupUi(this);
}

ColorPaletteWidget::~ColorPaletteWidget()
{
    delete ui;
}

void ColorPaletteWidget::initUI()
{
    QSettings settings(PENCIL2D, PENCIL2D);
    int colorGridSize = settings.value("PreferredColorGridSize", 34).toInt();

    mIconSize = QSize(colorGridSize, colorGridSize);

    ui->colorListWidget->setContextMenuPolicy(Qt::CustomContextMenu);

    QString sViewMode = settings.value("ColorPaletteViewMode", "ListMode").toString();
    if (sViewMode == "ListMode")
        setListMode();
    else
        setGridMode();

    buttonStylesheet = "::menu-indicator{ image: none; }"
                             "QPushButton { border: 0px; }"
                             "QPushButton:pressed { border: 1px solid #ADADAD; border-radius: 2px; background-color: #D5D5D5; }"
                             "QPushButton:checked { border: 1px solid #ADADAD; border-radius: 2px; background-color: #D5D5D5; }";

    ui->addColorButton->setStyleSheet(buttonStylesheet);
    ui->removeColorButton->setStyleSheet(buttonStylesheet);
    ui->colorDialogButton->setStyleSheet(buttonStylesheet);

    palettePreferences();

    connect(ui->colorListWidget, &QListWidget::itemClicked, this, &ColorPaletteWidget::clickColorListItem);

    connect(ui->colorListWidget, &QListWidget::itemDoubleClicked, this, &ColorPaletteWidget::changeColourName);
    connect(ui->colorListWidget, &QListWidget::itemChanged, this, &ColorPaletteWidget::onItemChanged);

    connect(ui->addColorButton, &QPushButton::clicked, this, &ColorPaletteWidget::clickAddColorButton);
    connect(ui->colorDialogButton, &QPushButton::clicked, this, &ColorPaletteWidget::clickColorDialogButton);
    connect(ui->removeColorButton, &QPushButton::clicked, this, &ColorPaletteWidget::clickRemoveColorButton);
    connect(ui->colorListWidget, &QListWidget::customContextMenuRequested, this, &ColorPaletteWidget::showContextMenu);
}

void ColorPaletteWidget::updateUI()
{
    refreshColorList();
    updateGridUI();
}

void ColorPaletteWidget::showContextMenu(const QPoint &pos)
{
    QPoint globalPos = ui->colorListWidget->mapToGlobal(pos);

    QMenu* menu = new QMenu();
    menu->addAction(tr("Add"), this, &ColorPaletteWidget::addItem, 0);
    menu->addAction(tr("Replace"),  this, &ColorPaletteWidget::replaceItem, 0);
    menu->addAction(tr("Remove"), this, &ColorPaletteWidget::removeItem, 0);

    menu->exec(globalPos);
}

void ColorPaletteWidget::addItem()
{
    QSignalBlocker b(ui->colorListWidget);
    QColor newColour = editor()->color()->frontColor();

    // add in front of selected color
    int colorIndex = ui->colorListWidget->currentRow()+1;

    ColourRef ref(newColour);

    editor()->object()->addColourAtIndex(colorIndex, ref);
    refreshColorList();
}

void ColorPaletteWidget::replaceItem()
{
    QSignalBlocker b(ui->colorListWidget);
    int index = ui->colorListWidget->currentRow();

    QColor newColour = editor()->color()->frontColor();

    if (index >= 0)
    {
        updateItemColor(index, newColour);
        emit colorChanged(newColour);
        ui->colorListWidget->setCurrentRow(index);
    }
}

void ColorPaletteWidget::removeItem()
{
    QSignalBlocker b(ui->colorListWidget);
    clickRemoveColorButton();
}

void ColorPaletteWidget::setColor(QColor newColor, int colorIndex)
{
    QSignalBlocker b(ui->colorListWidget);
    ui->colorListWidget->setCurrentRow(colorIndex);

    if (colorIndex >= 0)
    {
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
    QPen borderShadow(QColor(0, 0, 0, 200), 1, Qt::DotLine, Qt::FlatCap, Qt::MiterJoin);
    QVector<qreal> dashPattern;
    dashPattern << 4 << 4;
    borderShadow.setDashPattern(dashPattern);
    QPen borderHighlight(borderShadow);
    borderHighlight.setColor(QColor(255, 255, 255, 200));
    borderHighlight.setDashOffset(4);

    int colourCount = editor()->object()->getColourCount();

    for (int i = 0; i < colourCount; i++)
    {
        const ColourRef colourRef = editor()->object()->getColour(i);
        QListWidgetItem* colourItem = new QListWidgetItem(ui->colorListWidget);

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

        QIcon swatchIcon;
        swatchIcon.addPixmap(colourSwatch, QIcon::Normal);

        // Draw selection border
        if(ui->colorListWidget->viewMode() == QListView::IconMode) {
            swatchPainter.setPen(borderHighlight);
            swatchPainter.drawRect(0, 0, mIconSize.width() - 1, mIconSize.height() - 1);
            swatchPainter.setPen(borderShadow);
            swatchPainter.drawRect(0, 0, mIconSize.width() - 1, mIconSize.height() - 1);
        }
        swatchIcon.addPixmap(colourSwatch, QIcon::Selected);

        colourItem->setIcon(swatchIcon);
        swatchPainter.end();
        colourItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);

        ui->colorListWidget->addItem(colourItem);
    }
    updateGridUI();
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

void ColorPaletteWidget::onItemChanged(QListWidgetItem* item)
{
    int index = ui->colorListWidget->row(item);
    QString newColorName = item->text();
    editor()->object()->renameColour(index, newColorName);
}

void ColorPaletteWidget::clickColorListItem(QListWidgetItem* currentItem)
{
    auto modifiers = qApp->keyboardModifiers();

    // to avoid conflicts with multiple selections
    // ie. will be seen as selected twice and cause problems
    if (modifiers & Qt::ShiftModifier || modifiers & Qt::ControlModifier) { return; }

    int colorIndex = ui->colorListWidget->row(currentItem);

    emit colorNumberChanged(colorIndex);
}

void ColorPaletteWidget::palettePreferences()
{
    ui->colorListWidget->setMinimumWidth(ui->colorListWidget->sizeHintForColumn(0));

    // Let's pretend this button is a separator
    mSeparator = new QAction("", this);
    mSeparator->setSeparator(true);

    buttonStylesheet = "::menu-indicator{ image: none; }"
        "QToolButton { border: 0px; }"
        "QToolButton:pressed { border: 1px solid #ADADAD; border-radius: 2px; background-color: #D5D5D5; }"
        "QToolButton:checked { border: 1px solid #ADADAD; border-radius: 2px; background-color: #D5D5D5; }";


    // Add to UI
    ui->palettePref->setIcon(QIcon(":/app/icons/new/svg/more_options.svg"));
    ui->palettePref->setIconSize(QSize(15,15));
    ui->palettePref->setArrowType(Qt::ArrowType::NoArrow);
    ui->palettePref->setStyleSheet(buttonStylesheet);
    ui->palettePref->addAction(ui->listModeAction);
    ui->palettePref->addAction(ui->gridModeAction);
    ui->palettePref->addAction(mSeparator);
    ui->palettePref->addAction(ui->smallSwatchAction);
    ui->palettePref->addAction(ui->mediumSwatchAction);
    ui->palettePref->addAction(ui->largeSwatchAction);

    if (mIconSize.width() > 30) ui->largeSwatchAction->setChecked(true);
    else if (mIconSize.width() > 20) ui->mediumSwatchAction->setChecked(true);
    else ui->smallSwatchAction->setChecked(true);

    if (ui->colorListWidget->viewMode() == QListView::ListMode)
        ui->listModeAction->setChecked(true);
    else
        ui->gridModeAction->setChecked(true);

    connect(ui->listModeAction, &QAction::triggered, this, &ColorPaletteWidget::setListMode);
    connect(ui->gridModeAction, &QAction::triggered, this, &ColorPaletteWidget::setGridMode);
    connect(ui->smallSwatchAction, &QAction::triggered, this, &ColorPaletteWidget::setSwatchSizeSmall);
    connect(ui->mediumSwatchAction, &QAction::triggered, this, &ColorPaletteWidget::setSwatchSizeMedium);
    connect(ui->largeSwatchAction, &QAction::triggered, this, &ColorPaletteWidget::setSwatchSizeLarge);
}

void ColorPaletteWidget::setListMode()
{
    ui->colorListWidget->setViewMode(QListView::ListMode);
    ui->colorListWidget->setMovement(QListView::Static);
    ui->colorListWidget->setGridSize(QSize(-1, -1));
    updateUI();

    QSettings settings(PENCIL2D, PENCIL2D);
    settings.setValue("ColorPaletteViewMode", "ListMode");
}

void ColorPaletteWidget::setGridMode()
{
    ui->colorListWidget->setViewMode(QListView::IconMode);
    ui->colorListWidget->setMovement(QListView::Static); // TODO: update swatch index on move
    ui->colorListWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    ui->colorListWidget->setGridSize(QSize(mIconSize.width() + 1, mIconSize.height() + 1));

    updateUI();

    QSettings settings(PENCIL2D, PENCIL2D);
    settings.setValue("ColorPaletteViewMode", "GridMode");
}

void ColorPaletteWidget::resizeEvent(QResizeEvent* event)
{
    updateUI();
    QWidget::resizeEvent(event);
}

void ColorPaletteWidget::setSwatchSizeSmall()
{
    if (mIconSize.width() > 18)
    {
        mIconSize = QSize(14, 14);
        updateUI();

        QSettings settings(PENCIL2D, PENCIL2D);
        settings.setValue("PreferredColorGridSize", 14);
    }
}

void ColorPaletteWidget::setSwatchSizeMedium()
{
    if (mIconSize.width() < 20 || mIconSize.width() > 30)
    {
        mIconSize = QSize(26, 26);
        updateUI();

        QSettings settings(PENCIL2D, PENCIL2D);
        settings.setValue("PreferredColorGridSize", 26);
    }
}

void ColorPaletteWidget::setSwatchSizeLarge()
{
    if (mIconSize.width() < 30)
    {
        mIconSize = QSize(34, 34);
        updateUI();

        QSettings settings(PENCIL2D, PENCIL2D);
        settings.setValue("PreferredColorGridSize", 34);
    }
}

void ColorPaletteWidget::updateGridUI()
{
    if (ui->colorListWidget->viewMode() == QListView::IconMode) {
        // Find the value to divide with
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
        ui->colorListWidget->setGridSize(QSize(tempSize.width(), mIconSize.height() + 2));
        mIconSize.setWidth(mIconSize.width());
    }
    else
    {
        ui->colorListWidget->setIconSize(mIconSize);
        ui->colorListWidget->setGridSize(QSize(-1, -1));
    }
}

void ColorPaletteWidget::clickColorDialogButton()
{
    mIsColorDialog = true;
    clickAddColorButton();
    mIsColorDialog = false;
}

void ColorPaletteWidget::clickAddColorButton()
{
    QColor prevColor = Qt::white;

    QColor newColour;

    if (mIsColorDialog)
        newColour = QColorDialog::getColor(prevColor.rgba(), this, QString(), QColorDialog::ShowAlphaChannel);
    else 
        newColour = editor()->color()->frontColor();

    if (!newColour.isValid())
    {
        return; // User canceled operation
    }

    int colorIndex = editor()->object()->getColourCount();
    ColourRef ref(newColour);

    editor()->object()->addColour(ref);
    refreshColorList();

    editor()->color()->setColorNumber(colorIndex);
    editor()->color()->setColor(ref.colour);
}

void ColorPaletteWidget::clickRemoveColorButton()
{
    for (auto item : ui->colorListWidget->selectedItems())
    {
        int index = ui->colorListWidget->row(item);

        // items are not deleted by qt, it has to be done manually
        // delete should happen before removing the color from from palette
        // as the palette will be one ahead and crash otherwise
        if (editor()->object()->isColourInUse(index))
        {
            bool accepted = false;
            if (!mMultipleSelected)
                accepted = showPaletteWarning();

            if ((accepted || mMultipleSelected) && editor()->object()->getColourCount() > 1)
            {
                delete item;
                editor()->object()->removeColour(index);
            }
        }
        else if (editor()->object()->getColourCount() > 1)
        {
            delete item;
            editor()->object()->removeColour(index);
        }
        else if (editor()->object()->getColourCount() == 1)
        {
            showPaletteReminder();
        }
        editor()->updateCurrentFrame();
    }
    mMultipleSelected = false;
}

bool ColorPaletteWidget::showPaletteWarning()
{
    QMessageBox msgBox;
    msgBox.setText(tr("The color(s) you are about to delete are currently being used by one or multiple strokes."));
    msgBox.addButton(tr("Cancel"), QMessageBox::RejectRole);
    QPushButton* removeButton = msgBox.addButton(tr("Delete"), QMessageBox::AcceptRole);

    msgBox.exec();
    if (msgBox.clickedButton() == removeButton)
    {
        if (ui->colorListWidget->selectedItems().size() > 1)
        {
            mMultipleSelected = true;
        }
        return true;
    }
    return false;
}

void ColorPaletteWidget::showPaletteReminder()
{
    QMessageBox::warning(nullptr, tr("Palette Restriction"),
                                  tr("The palette requires at least one swatch to remain functional"));
}

void ColorPaletteWidget::updateItemColor(int itemIndex, QColor newColor)
{
    QPixmap colourSwatch(mIconSize);
    QPainter swatchPainter(&colourSwatch);
    swatchPainter.drawTiledPixmap(0, 0, mIconSize.width(), mIconSize.height(), QPixmap(":/background/checkerboard.png"));
    swatchPainter.fillRect(0, 0, mIconSize.width(), mIconSize.height(), newColor);

    QPen borderShadow(QColor(0, 0, 0, 200), 1, Qt::DotLine, Qt::FlatCap, Qt::MiterJoin);
    QVector<qreal> dashPattern;
    dashPattern << 4 << 4;
    borderShadow.setDashPattern(dashPattern);
    QPen borderHighlight(borderShadow);
    borderHighlight.setColor(QColor(255, 255, 255, 200));
    borderHighlight.setDashOffset(4);

    QIcon swatchIcon;
    swatchIcon.addPixmap(colourSwatch, QIcon::Normal);

    if(ui->colorListWidget->viewMode() == QListView::IconMode)
    {
        // Draw selection border
        swatchPainter.setPen(borderHighlight);
        swatchPainter.drawRect(0, 0, mIconSize.width() - 1, mIconSize.height() - 1);
        swatchPainter.setPen(borderShadow);
        swatchPainter.drawRect(0, 0, mIconSize.width() - 1, mIconSize.height() - 1);
    }
    swatchIcon.addPixmap(colourSwatch, QIcon::Selected);

    ui->colorListWidget->item(itemIndex)->setIcon(swatchIcon);

    // Make sure to update grid in grid mode
    if (ui->colorListWidget->viewMode() == QListView::IconMode)
    {
        updateGridUI();
    }
}
