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
    lMgr = lm;
    ui->setupUi(this);
    init();
}

CopyMultiplekeyframesDialog::~CopyMultiplekeyframesDialog()
{
    delete ui;
}

void CopyMultiplekeyframesDialog::init()
{
    // SET values in spinBoxes
    ui->sBoxFirstFrame->setValue(mFirstFrame);
    ui->sBoxLastFrame->setValue(mLastFrame);
    ui->sBoxStartFrame->setValue(mLastFrame + 1);
    ui->sBoxMove->setValue(mLastFrame + 1);
    ui->sBoxStartReverse->setValue(mLastFrame + 1);

    // SET member variables values
    mFirstFrame = ui->sBoxFirstFrame->value();
    mLastFrame = ui->sBoxLastFrame->value();
    mNumLoops = ui->sBoxNumLoops->value();
    mCopyStart = ui->sBoxStartFrame->value();
    mMoveStart = ui->sBoxMove->value();
    mReverseStart = ui->sBoxStartReverse->value();
    mFromLayer = ui->cBoxFromLayer->currentText();
    mCopyToLayer = ui->cBoxCopyToLayer->currentText();
    mMoveToLayer = ui->cBoxMoveToLayer->currentText();

    // SET values in ComboBoxes
    int lType = lMgr->currentLayer()->type();
    for (int i = 1; i < lMgr->count(); i++)
    {
        if (lMgr->getLayer(i)->type() == lType)
        {
            ui->cBoxFromLayer->addItem(lMgr->getLayer(i)->name());
            ui->cBoxCopyToLayer->addItem(lMgr->getLayer(i)->name());
            ui->cBoxMoveToLayer->addItem(lMgr->getLayer(i)->name());
            ui->cBoxDeleteFrames->addItem(lMgr->getLayer(i)->name());
        }
    }
    ui->cBoxFromLayer->setCurrentText(lMgr->currentLayer()->name());
    ui->cBoxCopyToLayer->setCurrentText(lMgr->currentLayer()->name());
    ui->cBoxMoveToLayer->setCurrentText(lMgr->currentLayer()->name());
    ui->cBoxDeleteFrames->setCurrentText(lMgr->currentLayer()->name());

    // SET text on labWarning and infolabels
    ui->labWarning->setText("");
    ui->labInfoAction->setText(getRadioChecked().toUpper());
    ui->labInfoFromLayer->setText(tr("From: ") + ui->cBoxFromLayer->currentText());
    ui->labInfoToLayer->setText(tr("To: ") + ui->cBoxCopyToLayer->currentText());

    // SET connections
    connect(ui->sBoxFirstFrame, SIGNAL(valueChanged(int)), this, SLOT(setFirstFrame(int)));
    connect(ui->sBoxLastFrame, SIGNAL(valueChanged(int)), this, SLOT(setLastFrame(int)));

    connect(ui->rBtnCopy, SIGNAL(clicked(bool)), this, SLOT(setMethodPicked()));
    connect(ui->sBoxNumLoops, SIGNAL(valueChanged(int)), this, SLOT(setNumLoops(int)));
    connect(ui->sBoxStartFrame, SIGNAL(valueChanged(int)), this, SLOT(setStartFrame(int)));

    connect(ui->rBtnMove, SIGNAL(clicked(bool)), this, SLOT(setMethodPicked()));
    connect(ui->sBoxMove, SIGNAL(valueChanged(int)), this, SLOT(setMoveStartFrame(int)));

    connect(ui->rBtnReverse, SIGNAL(clicked(bool)), this, SLOT(setMethodPicked()));
    connect(ui->sBoxStartReverse, SIGNAL(valueChanged(int)), this, SLOT(setReverseFrom(int)));

    connect(ui->rBtnDelete, SIGNAL(clicked(bool)), this, SLOT(setMethodPicked()));

    connect(ui->cBoxFromLayer, SIGNAL(currentTextChanged(QString)), this, SLOT(setFromLayer(QString)));
    connect(ui->cBoxCopyToLayer, SIGNAL(currentTextChanged(QString)), this, SLOT(setCopyToLayer(QString)));
    connect(ui->cBoxMoveToLayer, SIGNAL(currentTextChanged(QString)), this, SLOT(setMoveToLayer(QString)));
}

int CopyMultiplekeyframesDialog::getFirstFrame()
{
    return ui->sBoxFirstFrame->value();
}

int CopyMultiplekeyframesDialog::getLastFrame()
{
    return ui->sBoxLastFrame->value();
}

QString CopyMultiplekeyframesDialog::getFromLayer()
{
    return ui->cBoxFromLayer->currentText();
}

QString CopyMultiplekeyframesDialog::getCopyToLayer()
{
    return ui->cBoxCopyToLayer->currentText();
}

QString CopyMultiplekeyframesDialog::getMoveToLayer()
{
    return ui->cBoxMoveToLayer->currentText();
}

QString CopyMultiplekeyframesDialog::getDeleteOnLayer()
{
    return ui->cBoxDeleteFrames->currentText();
}

int CopyMultiplekeyframesDialog::getNumLoops()
{
    return ui->sBoxNumLoops->value();
}

void CopyMultiplekeyframesDialog::setNumLoopsMax(int numLoopsMax)
{
    ui->sBoxNumLoops->setMaximum(numLoopsMax);
}

int CopyMultiplekeyframesDialog::getCopyStartFrame()
{
    return ui->sBoxStartFrame->value();
}

int CopyMultiplekeyframesDialog::getMoveStartFrame()
{
    return ui->sBoxMove->value();
}

int CopyMultiplekeyframesDialog::getReverseStartFrame()
{
    return ui->sBoxStartReverse->value();
}

QString CopyMultiplekeyframesDialog::getRadioChecked()
{
    if (ui->rBtnCopy->isChecked())
        return "copy";
    if (ui->rBtnMove->isChecked())
        return "move";
    if (ui->rBtnReverse->isChecked())
    {
        ui->labInfoToLayer->setText("");
        return "reverse";
    }
    if (ui->rBtnDelete->isChecked())
        return "delete";
    return "";
}

// SLOTs
void CopyMultiplekeyframesDialog::setFirstFrame(int firstFrame)
{
    mFirstFrame = firstFrame;
    checkValidity();
}

void CopyMultiplekeyframesDialog::setLastFrame(int lastFrame)
{
    mLastFrame = lastFrame;
    checkValidity();
}

void CopyMultiplekeyframesDialog::setFromLayer(QString fromLayer)
{
    mFromLayer = fromLayer;
    if (ui->rBtnCopy->isChecked() || ui->rBtnMove->isChecked())
        ui->labInfoFromLayer->setText(tr("From: ") + fromLayer);
    else
        ui->labInfoFromLayer->setText(tr("On: ") + fromLayer);
}

void CopyMultiplekeyframesDialog::setCopyToLayer(QString copyToLayer)
{
    mCopyToLayer = copyToLayer;
    if (ui->rBtnCopy->isChecked())
    {
        ui->labInfoToLayer->setText(tr("To: ") + copyToLayer);
    }
}

void CopyMultiplekeyframesDialog::setNumLoops(int numLoops)
{
    mNumLoops = numLoops;
    checkValidity();
}

void CopyMultiplekeyframesDialog::setStartFrame(int startFrame)
{
    mCopyStart = startFrame;
    checkValidity();
}

void CopyMultiplekeyframesDialog::setMoveStartFrame(int startFrame)
{
    mMoveStart = startFrame;
}

void CopyMultiplekeyframesDialog::setMoveToLayer(QString moveToLayer)
{
    mMoveToLayer = moveToLayer;
    if (ui->rBtnMove->isChecked())
    {
        ui->labInfoFromLayer->setText(tr("From: ") + ui->cBoxFromLayer->currentText());
        ui->labInfoToLayer->setText(tr("To: ") + moveToLayer);
    }
}

void CopyMultiplekeyframesDialog::setReverseFrom(int reverseFrom)
{
    mReverseStart = reverseFrom;
    if (ui->rBtnReverse->isChecked())
    {
        ui->labInfoFromLayer->setText(tr("On: ") + ui->cBoxFromLayer->currentText());
        ui->labInfoToLayer->setText("");
    }
}

void CopyMultiplekeyframesDialog::setDeleteOnLayer()
{
    mDeleteOnLayer = ui->cBoxDeleteFrames->currentText();
    if (ui->rBtnDelete->isChecked())
    {
        ui->labInfoFromLayer->setText(tr("On: ") + ui->cBoxDeleteFrames->currentText());
        ui->labInfoToLayer->setText("");
    }

}

void CopyMultiplekeyframesDialog::setMethodPicked()
{
    if (ui->rBtnCopy->isChecked())
    {
        ui->labInfoAction->setText("COPY");
        setCopyToLayer(ui->cBoxCopyToLayer->currentText());
    }
    if (ui->rBtnMove->isChecked())
    {
        ui->labInfoAction->setText("MOVE");
        setMoveToLayer(ui->cBoxMoveToLayer->currentText());
    }
    if (ui->rBtnReverse->isChecked())
    {
        ui->labInfoAction->setText("REVERSE");
        setReverseFrom(ui->sBoxStartReverse->value());
    }
    if (ui->rBtnDelete->isChecked())
    {
        ui->labInfoAction->setText("DELETE");
        setDeleteOnLayer();
    }
}

void CopyMultiplekeyframesDialog::checkValidity()
{
    int cop = 1, mov = 1, rev = 1;
    QString msg = "";
    if (ui->rBtnCopy->isChecked())
    {
        cop = (mLastFrame + 1 - mFirstFrame) * (mNumLoops + 1) + mCopyStart - 1;
    }
    if (ui->rBtnMove->isChecked())
    {
        mov = (mLastFrame + 1 - mFirstFrame) + mMoveStart;
    }
    if (ui->rBtnReverse->isChecked())
    {
        rev = (mLastFrame + 1 - mFirstFrame) + mReverseStart;
    }
    if (cop > 9999 || mov > 9999 || rev > 9999) // 9999 frames is maximum timeline length
    {
        ui->btnBoxOkCancel->setEnabled(false);
        msg = tr("Exceeds 9999 frames!");
    }
    else if (mFirstFrame >= mLastFrame)     // Range must be valid
    {
        ui->btnBoxOkCancel->setEnabled(false);
        msg = tr("Range not valid!");
    }
    else
    {
        ui->btnBoxOkCancel->setEnabled(true);
    }
    ui->labWarning->setText(msg);       // writes empty string OR error message
}
