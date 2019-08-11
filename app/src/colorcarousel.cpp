#include "colorcarousel.h"

#include "ui_colorcarousel.h"
#include "colorqueuemodel.h"
#include "colourref.h"

#include <QResizeEvent>

ColorCarousel::ColorCarousel(QWidget* parent) : QWidget(parent)
{
    setWindowTitle(tr("Color Inspector", "Window title of color inspector"));

    ui = new Ui::ColorCarousel;
    ui->setupUi(this);

    mColorQueueModel = new ColorQueueModel(this, QWidget::palette().color(QWidget::backgroundRole()));
    ui->recentColorsList->setViewMode(QListView::IconMode);
    ui->recentColorsList->setStyleSheet("QListView { background: transparent;}");

    ui->recentColorsList->setModel(mColorQueueModel);
    ui->recentColorsList->setMouseTracking(false);

    connect(ui->recentColorsList, &QListView::clicked, this, &ColorCarousel::recentColorChanged);
    connect(ui->recentColorsList->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &ColorCarousel::recentColorSelectionChanged);

    connect(ui->colorLeftButton, &QPushButton::clicked, this, &ColorCarousel::cycleRecentColorsLeft);
    connect(ui->colorRightButton, &QPushButton::clicked, this, &ColorCarousel::cycleRecentColorsRight);
}

ColorCarousel::~ColorCarousel()
{

}

void ColorCarousel::addSwatch(Swatch swatch)
{
    QList<Swatch> swatches = mColorQueueModel->swatches();
     // Ignore same color just applied
     if (!swatches.isEmpty()) {
         if (swatch.color == swatches.first().color) {
             return;
         }
     }

    updateRecentColors(swatch);
    updateRecentColorsSelectedState(0);

    if (ui->recentColorsList->selectionModel()->currentIndex().row() == -1) {
        QModelIndex index = ui->recentColorsList->model()->index(0,0);
        ui->recentColorsList->selectionModel()->setCurrentIndex(index, QItemSelectionModel::Select);
    }
}

void ColorCarousel::recentColorSelectionChanged(const QItemSelection &selected,
                                                const QItemSelection &deselected)
{
    Q_UNUSED(deselected);
    Q_UNUSED(selected);
    int currentRowIndex =  ui->recentColorsList->selectionModel()->currentIndex().row();
    updateRecentColorsSelectedState(currentRowIndex);
    QList<Swatch> swatches = mColorQueueModel->swatches();

    emit recentColorUpdated(swatches[currentRowIndex].color);
}

void ColorCarousel::updateRecentColorsSelectedState(int index)
{
    QList<Swatch> swatches = mColorQueueModel->swatches();
     for (int iSwatch = 0; iSwatch < swatches.count(); iSwatch++) {
        if (index == iSwatch) {
            mColorQueueModel->setSwatchSelected(index, true);
        } else {
            mColorQueueModel->setSwatchSelected(iSwatch, false);
        }
    }
}

void ColorCarousel::updateRecentColors(Swatch& recentSwatch)
{
    recentSwatch.description = ColourRef::getDefaultColorName(recentSwatch.color);
    mColorQueueModel->addColor(recentSwatch);
}

void ColorCarousel::recentColorChanged(const QModelIndex &index)
{
    QList<Swatch> allSwatches = mColorQueueModel->swatches();
    for (int i = 0; i < allSwatches.count(); i++)
    {
        const QColor& swatchColor = allSwatches.at(i).color;
        if (i == index.row()) {
            emit recentColorUpdated(swatchColor);
            return;
        }
    }
}

void ColorCarousel::cycleRecentColorsLeft()
{
    QList<Swatch> swatches = mColorQueueModel->swatches();
    if (swatches.isEmpty()) { return; }
    int currentIndex = ui->recentColorsList->currentIndex().row();

    mColorQueueModel->pushSwatchesLeft();
    // neccesary to update the list since traversed
    swatches = mColorQueueModel->swatches();
    QColor currentColor = swatches.at(currentIndex).color;

    updateRecentColorsSelectedState(currentIndex);
    emit recentColorUpdated(currentColor);
}

void ColorCarousel::cycleRecentColorsRight()
{
    QList<Swatch> swatches = mColorQueueModel->swatches();
    if (swatches.isEmpty()) { return; }
    int currentIndex = ui->recentColorsList->currentIndex().row();

    mColorQueueModel->pushSwatchesRight();

    // neccesary to update the list since traversed
    swatches = mColorQueueModel->swatches();
    QColor currentColor = swatches.at(currentIndex).color;

    updateRecentColorsSelectedState(currentIndex);

    emit recentColorUpdated(currentColor);

}

void ColorCarousel::setLeftButtonShortcut(const QKeySequence &sequence)
{
    ui->colorLeftButton->setShortcut(sequence);
}

void ColorCarousel::setRightButtonShortcut(const QKeySequence &sequence)
{
    ui->colorRightButton->setShortcut(sequence);
}

