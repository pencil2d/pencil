#include "copymultiplekeyframesdialog.h"
#include "ui_copymultiplekeyframesdialog.h"
#include "timeline.h"
#include "timecontrols.h"
#include "layer.h"
#include "layermanager.h"
#include <QDebug>
#include <QSettings>

CopyMultiplekeyframesDialog::CopyMultiplekeyframesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CopyMultiplekeyframesDialog)
{
    ui->setupUi(this);
    init();
}


CopyMultiplekeyframesDialog::CopyMultiplekeyframesDialog(LayerManager *lm, int startLoop, int stopLoop, QWidget *parent):
    QDialog(parent),
    ui(new Ui::CopyMultiplekeyframesDialog)
{
    mFirstFrame = startLoop;
    mLastFrame = stopLoop;
    LayerManager *lMgr = lm;
    qDebug() << mFirstFrame << " - " << mLastFrame;
    qDebug() << lMgr->count();
    ui->setupUi(this);
    init();
}

CopyMultiplekeyframesDialog::~CopyMultiplekeyframesDialog()
{
    delete ui;
}

void CopyMultiplekeyframesDialog::init()
{
    connect(ui->sBoxFirstFrame, SIGNAL(valueChanged(int)), this, SLOT(setFirstFrame(int)));
    connect(ui->sBoxLastFrame, SIGNAL(valueChanged(int)), this, SLOT(setLastFrame(int)));
//    connect(ui->sBoxNumLoops, SIGNAL(valueChanged(int)), this, SLOT(setNumLoops(int)));
//    connect(ui->sBoxStartFrame, SIGNAL(valueChanged(int)), this, SLOT(setStartFrame(int)));
//    mFirstFrame = ui->sBoxStartLoop->value();
//    mLastFrame = ui->sBoxStopLoop->value();
//    ui->labRangeDefine->setText(tr("Range: ") + QString::number(mFirstFrame) +
//                                " -> " + QString::number(mLastFrame));
    ui->sBoxFirstFrame->setValue(mFirstFrame);
    ui->sBoxLastFrame->setValue(mLastFrame);
    ui->sBoxStartFrame->setValue(mLastFrame + 1);
    ui->sBoxMove->setValue(mLastFrame + 1);
    ui->sBoxStartReverse->setValue(mLastFrame + 1);
 /*
    LayerManager* layerMgr = mEditor->layers();
    int i = layerMgr->count();
    qDebug() << i << " lag i alt.";
*/
    mNumLoops = ui->sBoxNumLoops->value();
    mFirstFrame = ui->sBoxStartFrame->value();
//    QSettings settings ("Pencil", "Pencil");
//    mTimelineLength = settings.value("TimelineSize").toInt();
//    checkValidity();
}

int CopyMultiplekeyframesDialog::getFirstFrame()
{
//    return ui->sBoxStartLoop->value();
    return 0;
}

void CopyMultiplekeyframesDialog::setFirstFrame(int i)
{
    mFirstFrame = i;
    checkValidity();
}

int CopyMultiplekeyframesDialog::getLastFrame()
{
 //   return ui->sBoxStopLoop->value();
    return 0;
}

void CopyMultiplekeyframesDialog::setLastFrame(int i)
{
    mLastFrame = i;
    if (mFirstFrame > mLastFrame)
    {
        mFirstFrame = mLastFrame - 1;
        ui->sBoxFirstFrame->setValue(mFirstFrame);
    }
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

QString CopyMultiplekeyframesDialog::getRadioChecked()
{
    if (ui->rBtnCopy->isChecked())
        return "copy";
    if (ui->rBtnMove->isChecked())
        return "move";
    if (ui->rBtnDelete->isChecked())
        return "delete";
    return "";
}

void CopyMultiplekeyframesDialog::setStartFrame(int i)
{
    mStartFrame = i;
    checkValidity();
}

void CopyMultiplekeyframesDialog::checkValidity()
{
    int def = (mLastFrame + 1 - mFirstFrame) * mNumLoops + mStartFrame - 1;
    if (def > 9999) // 9999 frames is maximim timeline length
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
