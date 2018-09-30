#include "copymultiplekeyframesdialog.h"
#include "ui_copymultiplekeyframesdialog.h"
#include "timeline.h"
#include "timecontrols.h"
#include <QSettings>

CopyMultiplekeyframesDialog::CopyMultiplekeyframesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CopyMultiplekeyframesDialog)
{
    ui->setupUi(this);
    init();
}

CopyMultiplekeyframesDialog::CopyMultiplekeyframesDialog(QWidget *parent, int startLoop, int stopLoop) :
    QDialog(parent),
    ui(new Ui::CopyMultiplekeyframesDialog)
{
    mStartLoop = startLoop;
    mStopLoop = stopLoop;
    ui->setupUi(this);
    init();
}

CopyMultiplekeyframesDialog::~CopyMultiplekeyframesDialog()
{
    delete ui;
}

void CopyMultiplekeyframesDialog::init()
{
//    connect(ui->sBoxStartLoop, SIGNAL(valueChanged(int)), this, SLOT(setStartLoop(int)));
//    connect(ui->sBoxStopLoop, SIGNAL(valueChanged(int)), this, SLOT(setStopLoop(int)));
    connect(ui->sBoxNumLoops, SIGNAL(valueChanged(int)), this, SLOT(setNumLoops(int)));
    connect(ui->sBoxStartFrame, SIGNAL(valueChanged(int)), this, SLOT(setStartFrame(int)));
//    mStartLoop = ui->sBoxStartLoop->value();
//    mStopLoop = ui->sBoxStopLoop->value();
    ui->labRangeDefine->setText(tr("Range: ") + QString::number(mStartLoop) +
                                " -> " + QString::number(mStopLoop) + tr(" \(both included\)"));
    mNumLoops = ui->sBoxNumLoops->value();
    mStartFrame = ui->sBoxStartFrame->value();
    QSettings settings ("Pencil", "Pencil");
    mTimelineLength = settings.value("TimelineSize").toInt();
    checkValidity();
}

int CopyMultiplekeyframesDialog::getStartLoop()
{
//    return ui->sBoxStartLoop->value();
    return 0;
}

void CopyMultiplekeyframesDialog::setStartLoop(int i)
{
    mStartLoop = i;
    mStopLoop = mStartLoop + 1;
 //   ui->sBoxStopLoop->setMinimum(mStopLoop);
    checkValidity();
}

int CopyMultiplekeyframesDialog::getStopLoop()
{
 //   return ui->sBoxStopLoop->value();
    return 0;
}

void CopyMultiplekeyframesDialog::setStopLoop(int i)
{
    mStopLoop = i;
    mStartFrame = mStopLoop + 1;
    ui->sBoxStartFrame->setValue(mStartFrame);
    ui->sBoxStartFrame->setMinimum(mStartFrame);
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
    int def = (mStopLoop + 1 - mStartLoop) * mNumLoops + mStartFrame - 1;
    if (def > mTimelineLength)
    {
        ui->btnBoxOkCancel->setEnabled(false);
  //      ui->labWarning->setText(tr("Timeline exceeded!"));
    }
    else
    {
        ui->btnBoxOkCancel->setEnabled(true);
//        ui->labWarning->setText(tr("Ends at frame ") + QString::number(def) + " (" + QString::number(mTimelineLength) + ")");
    }
}
