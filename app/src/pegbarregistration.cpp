#include "pegbarregistration.h"
#include "ui_pegbarregistration.h"

#include "layermanager.h"
#include "layer.h"
#include <QListWidget>
#include <QListWidgetItem>
#include <QDebug>

PegBarRegistration::PegBarRegistration(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PegBarRegistration)
{
    ui->setupUi(this);

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
            QListWidgetItem* item = new QListWidgetItem(mEditor->layers()->getLayer(i)->name());
            ui->lwLayers->addItem(item);
        }
    }
    ui->labRefKey->setText(mEditor->layers()->currentLayer()->name() +
                           ", " + QString::number(mEditor->currentFrame()));
}

QStringList *PegBarRegistration::getLayerList()
{
    mLayernames->clear();
    for (int i = 0; i < ui->lwLayers->count(); i++)
    {
        if (ui->lwLayers->item(i)->isSelected())
            mLayernames->append(ui->lwLayers->item(i)->text());
    }
    return mLayernames;
}
