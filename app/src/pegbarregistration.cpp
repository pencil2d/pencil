#include "pegbarregistration.h"
#include "ui_pegbarregistration.h"

#include "layermanager.h"
#include "layer.h"
#include <QListWidget>
#include <QListWidgetItem>

PegBarRegistration::PegBarRegistration(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PegBarRegistration)
{
    ui->setupUi(this);

    connect(ui->rbActiveLayer, &QRadioButton::toggled, this, &PegBarRegistration::layerSelectionChanged);
    connect(ui->rbSelectedLayers, &QRadioButton::toggled, this, &PegBarRegistration::layerSelectionChanged);
    mLayernames = new QStringList();
    mLayernames->clear();
}

PegBarRegistration::~PegBarRegistration()
{
    delete ui;
}

void PegBarRegistration::initLayerList(Editor *editor)
{
    mEditor = editor;
    for (int i = 0; i < mEditor->layers()->count();i++)
    {
        if (mEditor->layers()->getLayer(i)->type() == Layer::BITMAP)
        {
            mLayernames->append(mEditor->layers()->getLayer(i)->name());
        }
    }
    ui->labRefKey->setText(mEditor->layers()->currentLayer()->name() +
                           ", " + QString::number(mEditor->currentFrame()));
}

QStringList *PegBarRegistration::getLayerList()
{
    mLayernames->clear();
    if (ui->rbActiveLayer->isChecked())
    {
        mLayernames->append(mEditor->layers()->currentLayer()->name());
        return mLayernames;
    }

    for (int i = 0; i < ui->lwLayers->count(); i++)
    {
        if (ui->lwLayers->item(i)->isSelected())
            mLayernames->append(ui->lwLayers->item(i)->text());
    }
    return mLayernames;
}

void PegBarRegistration::layerSelectionChanged()
{
    if (ui->rbActiveLayer->isChecked())
    {
        ui->lwLayers->clear();
        ui->lwLayers->setEnabled(false);
    }
    else
    {
        ui->lwLayers->clear();
        ui->lwLayers->setEnabled(true);
        for (int i = 0; i < mLayernames->count() ; i++)
        {
            QListWidgetItem* item = new QListWidgetItem(mLayernames->at(i));
            ui->lwLayers->addItem(item);
        }
    }
}
