#include "copymultiplekeyframesdialog.h"
#include "ui_copymultiplekeyframesdialog.h"

CopyMultiplekeyframesDialog::CopyMultiplekeyframesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CopyMultiplekeyframesDialog)
{
    ui->setupUi(this);
}

CopyMultiplekeyframesDialog::~CopyMultiplekeyframesDialog()
{
    delete ui;
}

int CopyMultiplekeyframesDialog::getStartLoop()
{
    return ui->sBoxStartLoop->value();
}

void CopyMultiplekeyframesDialog::setStartLoop(int i)
{
    ui->sBoxStartLoop->setValue(i);
}

int CopyMultiplekeyframesDialog::getStopLoop()
{
    return ui->sBoxStopLoop->value();
}

void CopyMultiplekeyframesDialog::setStopLoop(int i)
{
    ui->sBoxStopLoop->setValue(i);
}

int CopyMultiplekeyframesDialog::getNumLoops()
{
    return ui->sBoxNumLoops->value();
}

void CopyMultiplekeyframesDialog::setNumLoops(int i)
{
    ui->sBoxNumLoops->setValue(i);
}

void CopyMultiplekeyframesDialog::setNumLoopsMax(int i)
{
    ui->sBoxNumLoops->setMaximum(i);
}

int CopyMultiplekeyframesDialog::getStartFrame()
{
    return ui->sBoxStartFrame->value();
}

void CopyMultiplekeyframesDialog::setStartFrame(int i)
{
    ui->sBoxStartFrame->setValue(i);
}
