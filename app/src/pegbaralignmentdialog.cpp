#include "pegbaralignmentdialog.h"
#include "ui_pegbarregistration.h"

#include <QListWidget>
#include <QListWidgetItem>

PegBarAlignmentDialog::PegBarAlignmentDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PegBarRegistration)
{
    ui->setupUi(this);

    mLayernames.clear();
}

PegBarAlignmentDialog::~PegBarAlignmentDialog()
{
    delete ui;
}

void PegBarAlignmentDialog::setLayerList(QStringList layerList)
{
     mLayernames = layerList;
     for (int i = 0; i < mLayernames.count(); i++)
     {
         ui->lwLayers->addItem(mLayernames.at(i));
     }
}

QStringList PegBarAlignmentDialog::getLayerList()
{
    mLayernames.clear();
    for (int i = 0; i < ui->lwLayers->count(); i++)
    {
        if (ui->lwLayers->item(i)->isSelected())
            mLayernames.append(ui->lwLayers->item(i)->text());
    }
    return mLayernames;
}

void PegBarAlignmentDialog::setLabText(QString txt)
{
    ui->labRefKey->setText(txt);
}

