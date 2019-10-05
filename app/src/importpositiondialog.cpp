#include "importpositiondialog.h"
#include "ui_importpositiondialog.h"

ImportPositionDialog::ImportPositionDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ImportPositionDialog)
{
    ui->setupUi(this);

    ui->cbImagePosition->addItem("Current view and position");
    ui->cbImagePosition->addItem("Canvas centre");
    ui->cbImagePosition->addItem("Current camera view");

    connect(ui->cbImagePosition, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ImportPositionDialog::setPosIndex);
}

ImportPositionDialog::~ImportPositionDialog()
{
    delete ui;
}
