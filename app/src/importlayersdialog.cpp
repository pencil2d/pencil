#include "importlayersdialog.h"
#include "ui_importlayersdialog.h"

ImportLayersDialog::ImportLayersDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ImportLayersDialog)
{
    ui->setupUi(this);
}

ImportLayersDialog::~ImportLayersDialog()
{
    delete ui;
}
