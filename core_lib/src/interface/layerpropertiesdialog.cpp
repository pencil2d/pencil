#include "layerpropertiesdialog.h"
#include "ui_layerpropertiesdialog.h"

LayerPropertiesDialog::LayerPropertiesDialog(QString name, int distance) :
    QDialog(),
    ui(new Ui::LayerPropertiesDialog)
{
    ui->setupUi(this);

    ui->leName->setText(name);
    qreal dist = distance/1000.0;
    ui->doubleSpinBoxDistance->setValue(dist);
}

LayerPropertiesDialog::~LayerPropertiesDialog()
{
    delete ui;
}

QString LayerPropertiesDialog::getName()
{
    return ui->leName->text();
}

int LayerPropertiesDialog::getDistance()
{
    return ui->doubleSpinBoxDistance->value()*1000;
}
