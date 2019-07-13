#include "pegbaralignmentdialog.h"
#include "ui_pegbaralignmentdialog.h"

#include <QListWidget>
#include <QListWidgetItem>

PegBarAlignmentDialog::PegBarAlignmentDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PegBarAlignmentDialog)
{
    ui->setupUi(this);
    connect(ui->btnAlign, &QPushButton::clicked, this, &PegBarAlignmentDialog::alignClicked);
    connect(ui->btnCancel, &QPushButton::clicked, this, &PegBarAlignmentDialog::cancelClicked);
    connect(ui->lwLayers, &QListWidget::clicked, this, &PegBarAlignmentDialog::layerListUpdate);

    ui->btnAlign->setEnabled(false);
    mLayernames.clear();
}

PegBarAlignmentDialog::~PegBarAlignmentDialog()
{
    delete ui;
}

void PegBarAlignmentDialog::setLayerList(QStringList layerList)
{
    ui->lwLayers->clear();
     mLayernames = layerList;
     for (int i = 0; i < mLayernames.count(); i++)
     {
         ui->lwLayers->addItem(mLayernames.at(i));
     }
}

QStringList PegBarAlignmentDialog::getLayerList()
{
    QStringList selectedLayers;
    selectedLayers.clear();
    for (int i = 0; i < ui->lwLayers->count(); i++)
    {
        if (ui->lwLayers->item(i)->isSelected())
            selectedLayers.append(ui->lwLayers->item(i)->text());
    }
    return selectedLayers;
}

void PegBarAlignmentDialog::setLabRefKey()
{
    ui->labRefKey->setText(refLayer + " - " + QString::number(refkey));
}

void PegBarAlignmentDialog::setAreaSelected(bool b)
{
    areaSelected = b;
    setBtnAlignEnabled();
}

void PegBarAlignmentDialog::setReferenceSelected(bool b)
{
    referenceSelected = b;
    setBtnAlignEnabled();
}

void PegBarAlignmentDialog::setLayerSelected(bool b)
{
    layerSelected = b;
    setBtnAlignEnabled();
}

void PegBarAlignmentDialog::setBtnAlignEnabled()
{
    if (areaSelected && referenceSelected && layerSelected)
        ui->btnAlign->setEnabled(true);
    else
        ui->btnAlign->setEnabled(false);
}

void PegBarAlignmentDialog::setRefLayer(QString s)
{
    refLayer = s;
    setLabRefKey();
}

void PegBarAlignmentDialog::setRefKey(int i)
{
    refkey = i;
    setLabRefKey();
}

void PegBarAlignmentDialog::cancelClicked()
{
    emit cancelPressed();
}

void PegBarAlignmentDialog::alignClicked()
{
    emit alignPressed();
}

void PegBarAlignmentDialog::layerListUpdate()
{
    emit layerListClicked();
}

