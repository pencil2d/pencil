#include "copymultiplekeyframesdialog.h"
#include "ui_copymultiplekeyframesdialog.h"
#include "timeline.h"

CopyMultiplekeyframesDialog::CopyMultiplekeyframesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CopyMultiplekeyframesDialog)
{
    ui->setupUi(this);
    init();
}

CopyMultiplekeyframesDialog::~CopyMultiplekeyframesDialog()
{
    delete ui;
}

void CopyMultiplekeyframesDialog::init()
{
    connect(ui->sBoxStartLoop, SIGNAL(valueChanged(int)), this, SLOT(setStartLoop(int)));
    connect(ui->sBoxStopLoop, SIGNAL(valueChanged(int)), this, SLOT(setStopLoop(int)));
    connect(ui->sBoxNumLoops, SIGNAL(valueChanged(int)), this, SLOT(setNumLoops(int)));
    connect(ui->sBoxStartFrame, SIGNAL(valueChanged(int)), this, SLOT(setStartFrame(int)));
    mStartLoop = ui->sBoxStartLoop->value();
    mStopLoop = ui->sBoxStopLoop->value();
    mNumLoops = ui->sBoxNumLoops->value();
    mStartFrame = ui->sBoxStartFrame->value();
}

int CopyMultiplekeyframesDialog::getStartLoop()
{
    return ui->sBoxStartLoop->value();
}

void CopyMultiplekeyframesDialog::setStartLoop(int i)
{
    mStartLoop = i;
    mStopLoop = mStartLoop + 1;
    ui->sBoxStopLoop->setMinimum(mStopLoop);
    checkValidity();
}

int CopyMultiplekeyframesDialog::getStopLoop()
{
    return ui->sBoxStopLoop->value();
}

void CopyMultiplekeyframesDialog::setStopLoop(int i)
{
    mStopLoop = i;
    mStartFrame = mStopLoop + 1;
    ui->sBoxStartFrame->setValue(mStartFrame);
    checkValidity();
}

int CopyMultiplekeyframesDialog::getNumLoops()
{
    return ui->sBoxNumLoops->value();
}

void CopyMultiplekeyframesDialog::setNumLoops(int i)
{
    mNumLoops = i;
    checkValidity();
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
    mStartFrame = i;
    checkValidity();
}

void CopyMultiplekeyframesDialog::checkValidity()
{
    if ((mStopLoop + 1 - mStartLoop) * mNumLoops + mStartFrame > 500)
    {
        ui->buttonBox->setEnabled(false);
        ui->labWarning->setText(tr("Values exceed scene length!"));
    }
    else if (ui->labWarning->text() != "")
    {
        ui->buttonBox->setEnabled(true);
        ui->labWarning->setText("");
    }
}
