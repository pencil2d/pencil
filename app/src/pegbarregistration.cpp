#include "pegbarregistration.h"
#include "ui_pegbarregistration.h"

#include <QListWidget>
#include <QListWidgetItem>

PegBarRegistration::PegBarRegistration(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PegBarRegistration)
{
    ui->setupUi(this);

    mLayernames.clear();
}

PegBarRegistration::~PegBarRegistration()
{
    delete ui;
}

void PegBarRegistration::setLayerList(QStringList layerList)
{
     mLayernames = layerList;
     for (int i = 0; i < mLayernames.count(); i++)
     {
         ui->lwLayers->addItem(mLayernames.at(i));
     }
}

QStringList PegBarRegistration::getLayerList()
{
    mLayernames.clear();
    for (int i = 0; i < ui->lwLayers->count(); i++)
    {
        if (ui->lwLayers->item(i)->isSelected())
            mLayernames.append(ui->lwLayers->item(i)->text());
    }
    return mLayernames;
}

void PegBarRegistration::setLabText(QString txt)
{
    ui->labRefKey->setText(txt);
}

