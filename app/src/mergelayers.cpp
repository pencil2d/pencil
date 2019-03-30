#include "mergelayers.h"
#include "ui_mergelayers.h"

#include "layerbitmap.h"
#include "layervector.h"

MergeLayers::MergeLayers(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MergeLayers)
{
    ui->setupUi(this);
    connect(ui->cbFromLayer, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MergeLayers::layerSelectionChanged);
    connect(ui->cbToLayer, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MergeLayers::layerSelectionChanged);
    connect(ui->btnMerge, &QPushButton::clicked, this, &MergeLayers::mergeLayers);
    connect(ui->btnClose, &QPushButton::clicked, this, &MergeLayers::close);
}

MergeLayers::~MergeLayers()
{
    delete ui;
}

void MergeLayers::initDialog(Editor *editor)
{
    mEditor = editor;
    mLayerType = mEditor->layers()->currentLayer()->type();
    ui->cbFromLayer->clear();
    ui->cbToLayer->clear();
    for (int i = 0; i < mEditor->layers()->count(); i++)
    {
        if (mEditor->layers()->getLayer(i)->type() == mLayerType)
        {
            ui->cbFromLayer->addItem(mEditor->layers()->getLayer(i)->name());
            ui->cbToLayer->addItem(mEditor->layers()->getLayer(i)->name());
        }
    }
    if (ui->cbFromLayer->currentText() == ui->cbToLayer->currentText())
    {
        ui->btnMerge->setEnabled(false);
    }
}

void MergeLayers::layerSelectionChanged()
{
    if (ui->cbFromLayer->currentText() == ui->cbToLayer->currentText())
    {
        ui->btnMerge->setEnabled(false);
    }
    else {
        ui->btnMerge->setEnabled(true);
    }
}

void MergeLayers::mergeLayers()
{
    if (mLayerType == Layer::BITMAP)
    {
        mFromLayer = static_cast<LayerBitmap*>(mEditor->layers()->findLayerByName(ui->cbFromLayer->currentText()));
        mToLayer   = static_cast<LayerBitmap*>(mEditor->layers()->findLayerByName(ui->cbToLayer->currentText()));
    }
    else
    {
        mFromLayer = static_cast<LayerVector*>(mEditor->layers()->findLayerByName(ui->cbFromLayer->currentText()));
        mToLayer   = static_cast<LayerVector*>(mEditor->layers()->findLayerByName(ui->cbToLayer->currentText()));
    }
    mEditor->layers()->mergeLayers(mFromLayer, mToLayer);
    mEditor->layers()->setCurrentLayer(mFromLayer);
    if (ui->cbDeleteLayer->isChecked())
        mEditor->layers()->deleteLayer(mEditor->layers()->currentLayerIndex());
}

void MergeLayers::closeUi()
{
    close();
}
