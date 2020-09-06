#include "retimedialog.h"
#include "ui_retimedialog.h"

RetimeDialog::RetimeDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RetimeDialog)
{
    ui->setupUi(this);
}

RetimeDialog::~RetimeDialog()
{
    delete ui;
}

void RetimeDialog::setOrigFps(int origFps)
{
    ui->origFps->setText(QString("<span style=' font-weight:600;'>%1 %2</span>").arg(origFps).arg(tr("fps")));
    ui->newFpsBox->setValue(origFps);
}

int RetimeDialog::getNewFps()
{
    return ui->newFpsBox->value();
}

qreal RetimeDialog::getNewSpeed()
{
    return ui->newSpeedBox->value();
}
