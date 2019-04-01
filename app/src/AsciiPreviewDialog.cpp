#include "AsciiPreviewDialog.h"
#include "ui_AsciiPreviewDialog.h"

AsciiPreviewDialog::AsciiPreviewDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AsciiPreviewDialog)
{
    ui->setupUi(this);
}

AsciiPreviewDialog::~AsciiPreviewDialog()
{
    delete ui;
}
