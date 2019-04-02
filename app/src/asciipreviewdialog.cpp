#include "asciipreviewdialog.h"
#include "ui_AsciiPreviewDialog.h"

#include <QDebug>

AsciiPreviewDialog::AsciiPreviewDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AsciiPreviewDialog),
    mMetrics(QFont("Courier New", 13))
{
    ui->setupUi(this);

    ui->verticalLayout->setSizeConstraint(QLayout::SetFixedSize);
}

AsciiPreviewDialog::~AsciiPreviewDialog()
{
    delete ui;
}

void AsciiPreviewDialog::setText(QString s)
{
    ui->textDisplay->setText(s);
    qDebug() << mMetrics.size(0, s);
}
