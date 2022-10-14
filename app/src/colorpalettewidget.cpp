/*

Pencil2D - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2012-2020 Matthew Chiawen Chang

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
#include <QListWidgetItem>
#include <QInputDialog>
#include <QColorDialog>
#include <QMessageBox>
#include <QPushButton>
#include <QSettings>
#include <QMenu>
#include <QtMath>
#include <QScrollBar>
#include <QAbstractItemModel>
#include <QPainter>

// Project
#include "colorref.h"
#include "object.h"
#include "editor.h"
#include "layerbitmap.h"
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
    mFitSwatches = settings.value("FitSwatchSize", false).toBool();
    if (mFitSwatches)
    {
        fitSwatchSize();
    }

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
    connect(ui->colorListWidget->model(), &QAbstractItemModel::rowsMoved, this, &ColorPaletteWidget::onRowsMoved);

    connect(ui->colorListWidget, &QListWidget::itemDoubleClicked, this, &ColorPaletteWidget::changeColorName);
    connect(ui->colorListWidget, &QListWidget::itemChanged, this, &ColorPaletteWidget::onItemChanged);

    connect(ui->addColorButton, &QPushButton::clicked, this, &ColorPaletteWidget::clickAddColorButton);
    connect(ui->colorDialogButton, &QPushButton::clicked, this, &ColorPaletteWidget::clickColorDialogButton);
    connect(ui->removeColorButton, &QPushButton::clicked, this, &ColorPaletteWidget::clickRemoveColorButton);
    connect(ui->colorListWidget, &QListWidget::customContextMenuRequested, this, &ColorPaletteWidget::showContextMenu);

    connect(editor(), &Editor::objectLoaded, this, &ColorPaletteWidget::updateUI);
}

void ColorPaletteWidget::updateUI()
{
    mObject = mEditor->object();
    refreshColorList();
    updateGridUI();
}

void ColorPaletteWidget::setCore(Editor* editor)
{
    mEditor = editor;
    mObject = mEditor->object();
}

void ColorPaletteWidget::showContextMenu(const QPoint& pos)
{
    QPoint globalPos = ui->colorListWidget->mapToGlobal(pos);

    QMenu* menu = new QMenu;
    connect(menu, &QMenu::triggered, menu, &QMenu::deleteLater);

    menu->addAction(tr("Add"), this, &ColorPaletteWidget::addItem, 0);
    menu->addAction(tr("Replace"),  this, &ColorPaletteWidget::replaceItem, 0);
    menu->addAction(tr("Remove"), this, &ColorPaletteWidget::removeItem, 0);

    menu->exec(globalPos);
}

void ColorPaletteWidget::addItem()
{
    QSignalBlocker b(ui->colorListWidget);
    QColor newColor = mEditor->color()->frontColor(false);

    ColorRef ref(newColor);

    const int colorIndex = ui->colorListWidget->count();
    mObject->addColorAtIndex(colorIndex, ref);

    refreshColorList();

    if (mFitSwatches)
    {
        fitSwatchSize();
    }

    QListWidgetItem* item = ui->colorListWidget->item(colorIndex);
    ui->colorListWidget->editItem(item);
    ui->colorListWidget->scrollToItem(item);
}

void ColorPaletteWidget::replaceItem()
{
    QSignalBlocker b(ui->colorListWidget);
    int index = ui->colorListWidget->currentRow();

    QColor newColor = mEditor->color()->frontColor(false);

    if (index < 0 ) { return; }

    updateItemColor(index, newColor);
    emit colorNumberChanged(index);
    ui->colorListWidget->setCurrentRow(index);
}

void ColorPaletteWidget::removeItem()
{
    QSignalBlocker b(ui->colorListWidget);
    clickRemoveColorButton();
}

void ColorPaletteWidget::selectColorNumber(int colorNumber) const
{
    ui->colorListWidget->setCurrentRow(colorNumber);
}

int ColorPaletteWidget::currentColorNumber()
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

    int colorCount = editor()->object()->getColorCount();

    for (int i = 0; i < colorCount; i++)
    {
        addSwatch(i);
    }

    selectColorNumber(editor()->color()->frontColorNumber());
    updateGridUI();
    update();
}

void ColorPaletteWidget::addSwatch(int colorIndex) const
{
    QPixmap originalColorSwatch(mIconSize);
    QPainter painter(&originalColorSwatch);
    painter.drawTiledPixmap(0, 0, mIconSize.width(), mIconSize.height(), QPixmap(":/background/checkerboard.png"));
    painter.end();

    QPen borderShadow(QColor(0, 0, 0, 200), 1, Qt::DotLine, Qt::FlatCap, Qt::MiterJoin);
    QVector<qreal> dashPattern{ 4, 4 };
    borderShadow.setDashPattern(dashPattern);

    QPen borderHighlight(borderShadow);
    borderHighlight.setColor(QColor(255, 255, 255, 200));
    borderHighlight.setDashOffset(4);

    const ColorRef colorRef = mObject->getColor(colorIndex);
    QListWidgetItem* colorItem = new QListWidgetItem(ui->colorListWidget);

    if (ui->colorListWidget->viewMode() != QListView::IconMode)
    {
        colorItem->setText(colorRef.name);
    }
    else
    {
        colorItem->setToolTip(colorRef.name);
    }

    QPixmap colorSwatch = originalColorSwatch;
    QPainter swatchPainter(&colorSwatch);
    swatchPainter.fillRect(0, 0, mIconSize.width(), mIconSize.height(), colorRef.color);

    QIcon swatchIcon;
    swatchIcon.addPixmap(colorSwatch, QIcon::Normal);

    // Draw selection border
    if (ui->colorListWidget->viewMode() == QListView::IconMode)
    {
        swatchPainter.setPen(borderHighlight);
        swatchPainter.drawRect(0, 0, mIconSize.width() - 1, mIconSize.height() - 1);
        swatchPainter.setPen(borderShadow);
        swatchPainter.drawRect(0, 0, mIconSize.width() - 1, mIconSize.height() - 1);
    }
    swatchIcon.addPixmap(colorSwatch, QIcon::Selected);

    colorItem->setIcon(swatchIcon);
    colorItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsDragEnabled);

    ui->colorListWidget->addItem(colorItem);
}

void ColorPaletteWidget::changeColorName(QListWidgetItem* item)
{
    Q_ASSERT(item != nullptr);

    if (ui->colorListWidget->viewMode() == QListView::IconMode)
    {
        int colorNumber = ui->colorListWidget->row(item);
        if (colorNumber > -1)
        {
            bool ok;
            QString text = QInputDialog::getText(this,
                                                 tr("Color name"),
                                                 tr("Color name"),
                                                 QLineEdit::Normal,
                                                 mObject->getColor(colorNumber).name,
                                                 &ok);
            if (ok && !text.isEmpty())
            {
                mObject->renameColor(colorNumber, text);
                refreshColorList();
            }
        }
    }
}

void ColorPaletteWidget::onItemChanged(QListWidgetItem* item)
{
    int index = ui->colorListWidget->row(item);
    QString newColorName = item->text();
    mObject->renameColor(index, newColorName);
}

void ColorPaletteWidget::onRowsMoved(const QModelIndex &parent, int start, int end, const QModelIndex &destination, int row)
{
    Q_UNUSED(parent)
    Q_UNUSED(destination)
    Q_UNUSED(end)

    int startIndex, endIndex;
    if (start < row)
    {
        row -= 1; // TODO: Is this a bug?
        if (start == row) { return; }

        startIndex = start;
        endIndex = row;

        mObject->movePaletteColor(startIndex, endIndex);

        mObject->addColor(mObject->getColor(startIndex));
        mObject->moveVectorColor(startIndex, mObject->getColorCount() - 1);
        for (int i = startIndex; i < endIndex; i++)
        {
            mObject->moveVectorColor(i + 1, i);
        }
        mObject->moveVectorColor(mObject->getColorCount() - 1, endIndex);
    }
    else
    {
        if (start == row) { return; }

        startIndex = start;
        endIndex = row;

        mObject->movePaletteColor(startIndex, endIndex);

        mObject->addColor(mObject->getColor(startIndex));
        mObject->moveVectorColor(startIndex, mObject->getColorCount() - 1);
        for (int i = startIndex; i > endIndex; i--)
        {
            mObject->moveVectorColor(i - 1, i);
        }
        mObject->moveVectorColor(mObject->getColorCount() - 1, endIndex);
    }

    mObject->removeColor(mObject->getColorCount() - 1);

    refreshColorList();
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
    ui->palettePref->addAction(ui->fitSwatchAction);

    if (mFitSwatches) ui->fitSwatchAction->setChecked(true);
    else if (mIconSize.width() > MEDIUM_ICON_SIZE) ui->largeSwatchAction->setChecked(true);
    else if (mIconSize.width() > MIN_ICON_SIZE) ui->mediumSwatchAction->setChecked(true);
    else ui->smallSwatchAction->setChecked(true);

    if (ui->colorListWidget->viewMode() == QListView::ListMode)
        ui->listModeAction->setChecked(true);
    else
        ui->gridModeAction->setChecked(true);

    connect(ui->listModeAction, &QAction::triggered, this, &ColorPaletteWidget::setListMode);
    connect(ui->gridModeAction, &QAction::triggered, this, &ColorPaletteWidget::setGridMode);
    connect(ui->fitSwatchAction, &QAction::triggered, this, &ColorPaletteWidget::fitSwatchSize);
    connect(ui->smallSwatchAction, &QAction::triggered, this, &ColorPaletteWidget::setSwatchSizeSmall);
    connect(ui->mediumSwatchAction, &QAction::triggered, this, &ColorPaletteWidget::setSwatchSizeMedium);
    connect(ui->largeSwatchAction, &QAction::triggered, this, &ColorPaletteWidget::setSwatchSizeLarge);
}

void ColorPaletteWidget::setListMode()
{
    ui->colorListWidget->setViewMode(QListView::ListMode);
    ui->colorListWidget->setDragDropMode(QAbstractItemView::InternalMove);
    ui->colorListWidget->setGridSize(QSize(-1, -1));
    if (mFitSwatches)
    {
        fitSwatchSize();
    }
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
    if (mFitSwatches)
    {
        fitSwatchSize();
    }
    updateUI();

    QSettings settings(PENCIL2D, PENCIL2D);
    settings.setValue("ColorPaletteViewMode", "GridMode");
}

void ColorPaletteWidget::setSwatchSizeSmall()
{
    if (mIconSize.width() > MIN_ICON_SIZE)
    {
        mIconSize = QSize(MIN_ICON_SIZE, MIN_ICON_SIZE);
        updateUI();

        mFitSwatches = false;
        QSettings settings(PENCIL2D, PENCIL2D);
        settings.setValue("PreferredColorGridSize", MIN_ICON_SIZE);
        settings.setValue("FitSwatchSize", false);
    }
}

void ColorPaletteWidget::setSwatchSizeMedium()
{
    if (mIconSize.width() != MEDIUM_ICON_SIZE)
    {
        mIconSize = QSize(MEDIUM_ICON_SIZE, MEDIUM_ICON_SIZE);
        updateUI();

        mFitSwatches = false;
        QSettings settings(PENCIL2D, PENCIL2D);
        settings.setValue("PreferredColorGridSize", MEDIUM_ICON_SIZE);
        settings.setValue("FitSwatchSize", false);
    }
}

void ColorPaletteWidget::setSwatchSizeLarge()
{
    if (mIconSize.width() < MAX_ICON_SIZE)
    {
        mIconSize = QSize(MAX_ICON_SIZE, MAX_ICON_SIZE);
        updateUI();

        mFitSwatches = false;
        QSettings settings(PENCIL2D, PENCIL2D);
        settings.setValue("PreferredColorGridSize", MAX_ICON_SIZE);
        settings.setValue("FitSwatchSize", false);
    }
}

void ColorPaletteWidget::adjustSwatches()
{
    if (mFitSwatches)
        fitSwatchSize();
}

void ColorPaletteWidget::fitSwatchSize()
{
    int height = ui->colorListWidget->height();
    int width = ui->colorListWidget->width();
    int hScrollBar = ui->colorListWidget->horizontalScrollBar()->geometry().height() + 6;
    int vScrollBar = ui->colorListWidget->verticalScrollBar()->geometry().width() * 2;
    int colorCount = editor()->object()->getColorCount();
    int size;

    if (ui->colorListWidget->viewMode() == QListView::ListMode)
    {
        size = qFloor((height - hScrollBar - (4 * colorCount)) / colorCount);
        if (size < MIN_ICON_SIZE) size = MIN_ICON_SIZE;
        if (size > MAX_ICON_SIZE) size = MAX_ICON_SIZE;
    }
    else
    {
        bool proceed = true;
        size = MIN_ICON_SIZE;
        while (proceed)
        {
            int columns = (width - vScrollBar) / size;
            int rows = static_cast<int>(qCeil(colorCount / columns));
            if (height - hScrollBar > rows * (size + 6))
            {
                size++;
                if (size == MAX_ICON_SIZE)
                {
                    proceed = false;
                }
            }
            else
            {
                proceed = false;
            }
        }
    }
    mIconSize = QSize(size, size);

    updateUI();

    mFitSwatches = true;
    QSettings settings(PENCIL2D, PENCIL2D);
    settings.setValue("PreferredColorGridSize", size);
    settings.setValue("FitSwatchSize", true);
}

void ColorPaletteWidget::resizeEvent(QResizeEvent* event)
{
    updateUI();
    if (mFitSwatches)
    {
        fitSwatchSize();
    }
    QWidget::resizeEvent(event);
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

    QColor newColor;

    if (mIsColorDialog)
        newColor = QColorDialog::getColor(prevColor.rgba(), this, QString(), QColorDialog::ShowAlphaChannel);
    else
        newColor = mEditor->color()->frontColor(false);

    if (!newColor.isValid())
    {
        return; // User canceled operation
    }

    int colorIndex = mObject->getColorCount();
    ColorRef ref(newColor);

    mObject->addColor(ref);
    refreshColorList();

    editor()->color()->setColorNumber(colorIndex);
    editor()->color()->setIndexedColor(ref.color);
    if (mFitSwatches)
    {
        fitSwatchSize();
    }
}

void ColorPaletteWidget::clickRemoveColorButton()
{
    for (auto item : ui->colorListWidget->selectedItems())
    {
        int index = ui->colorListWidget->row(item);

        // items are not deleted by qt, it has to be done manually
        // delete should happen before removing the color from from palette
        // as the palette will be one ahead and crash otherwise

        if (mObject->getColorCount() == 1)
        {
            showPaletteReminder();
            break;
        }

        bool colorRemoved = false;
        if (mObject->isColorInUse(index))
        {
            bool accepted = false;
            if (!mMultipleSelected)
                accepted = showPaletteWarning();

            if ((accepted || mMultipleSelected) && mObject->getColorCount() > 1)
            {
                delete item;
                mObject->removeColor(index);
                colorRemoved = true;
            }
        }
        else if (mObject->getColorCount() > 1)
        {
            delete item;
            mObject->removeColor(index);
            colorRemoved = true;
        }

        if (colorRemoved) {
            int newIndex = qBound(0, index-1, mObject->getColorCount() - 1);
            emit colorNumberChanged(newIndex);
        }
    }
    mMultipleSelected = false;
    if (mFitSwatches)
    {
        fitSwatchSize();
    }
    mEditor->updateCurrentFrame();
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
    QPixmap colorSwatch(mIconSize);
    QPainter swatchPainter(&colorSwatch);
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
    swatchIcon.addPixmap(colorSwatch, QIcon::Normal);

    if(ui->colorListWidget->viewMode() == QListView::IconMode)
    {
        // Draw selection border
        swatchPainter.setPen(borderHighlight);
        swatchPainter.drawRect(0, 0, mIconSize.width() - 1, mIconSize.height() - 1);
        swatchPainter.setPen(borderShadow);
        swatchPainter.drawRect(0, 0, mIconSize.width() - 1, mIconSize.height() - 1);
    }
    swatchIcon.addPixmap(colorSwatch, QIcon::Selected);

    ui->colorListWidget->item(itemIndex)->setIcon(swatchIcon);
    editor()->object()->setColor(itemIndex, newColor);

    // Make sure to update grid in grid mode
    if (ui->colorListWidget->viewMode() == QListView::IconMode)
    {
        updateGridUI();
    }
}
