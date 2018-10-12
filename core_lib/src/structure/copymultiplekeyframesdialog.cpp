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
    ///< SET values in spinBoxes
    ui->sBoxFirstFrame->setValue(mFirstFrame);
    ui->sBoxLastFrame->setValue(mLastFrame);
    ui->sBoxStartFrame->setValue(mLastFrame + 1);
    ui->sBoxMove->setValue(mLastFrame + 1);
    ui->sBoxStartReverse->setValue(mLastFrame + 1);

    ///< SET values in ComboBoxes
    int lType = lMgr->currentLayer()->type(); // find layer-type
    for (int i = 1; i < lMgr->count(); i++)
    {
        if (lMgr->getLayer(i)->type() == lType)
        {
            ui->cBoxFromLayer->addItem(lMgr->getLayer(i)->name());
            ui->cBoxCopyToLayer->addItem(lMgr->getLayer(i)->name());
            ui->cBoxMoveToLayer->addItem(lMgr->getLayer(i)->name());
        }
    }
    ui->cBoxFromLayer->setCurrentText(lMgr->currentLayer()->name());
    ui->cBoxCopyToLayer->setCurrentText(lMgr->currentLayer()->name());
    ui->cBoxMoveToLayer->setCurrentText(lMgr->currentLayer()->name());

    ///< SET member variables values
    mNumLoops = ui->sBoxNumLoops->value();
    mCopyStart = ui->sBoxStartFrame->value();
    mMoveStart = ui->sBoxMove->value();
    mReverseStart = ui->sBoxStartReverse->value();
    mManiStartAt = mFirstFrame;
    mManiEndAt = mLastFrame;
    mFromLayer = ui->cBoxFromLayer->currentText();
    mCopyToLayer = ui->cBoxCopyToLayer->currentText();
    mMoveToLayer = ui->cBoxMoveToLayer->currentText();
    mCurrentTab = 0; // starts with COPY selected
    mValidAction = true;

    ///< SET text on dialog
    ui->labHeader->setText(tr("Manipulate Range of Frames"));
    ui->labRangeStart->setText(tr("First Frame"));
    ui->labRangeEnd->setText(tr("Last Frame"));
    ui->labFromLayer->setText(tr("From Layer"));
    ui->labNumLoops->setText(tr("Number of Loops"));
    ui->labCopyFromFrame->setText(tr("Insert Frame at"));
    ui->labCopyToLayer->setText(tr("To Layer"));
    ui->labMoveFromFrame->setText(tr("Insert Frame at"));
    ui->labMoveToLayer->setText(tr("To Layer"));
    ui->labReverseFromFrame->setText(tr("Insert Frame at"));
    ui->labDeleteOnLayer->setText(tr("On Layer ") + mFromLayer );

    ///< SET text on labWarning and infolabels
    ui->labWarning->setText("");
    ui->labInfoAction->setText(getActiveTab().toUpper());
    ui->labInfoToFromLayer->setText(tr("From: %1 To: %2").arg(ui->cBoxFromLayer->currentText()).arg(ui->cBoxFromLayer->currentText()));
    ///< SET connections HEADER
    connect(ui->sBoxFirstFrame, SIGNAL(valueChanged(int)), this, SLOT(setFirstFrame(int)));
    connect(ui->sBoxLastFrame, SIGNAL(valueChanged(int)), this, SLOT(setLastFrame(int)));
    connect(ui->cBoxFromLayer, SIGNAL(currentTextChanged(QString)), this, SLOT(setFromLayer(QString)));
    connect(ui->tabWidget, SIGNAL(currentChanged(int)), this, SLOT(setMethodPicked(int)));
    ///< SET connections COPY
    connect(ui->sBoxNumLoops, SIGNAL(valueChanged(int)), this, SLOT(setNumLoops(int)));
    connect(ui->sBoxStartFrame, SIGNAL(valueChanged(int)), this, SLOT(setStartFrame(int)));
    connect(ui->cBoxCopyToLayer, SIGNAL(currentTextChanged(QString)), this, SLOT(setCopyToLayer(QString)));
    ///< SET connections MOVE
    connect(ui->sBoxMove, SIGNAL(valueChanged(int)), this, SLOT(setMoveStartFrame(int)));
    connect(ui->cBoxMoveToLayer, SIGNAL(currentTextChanged(QString)), this, SLOT(setMoveToLayer(QString)));
    ///< SET connections REVERSE
    connect(ui->sBoxStartReverse, SIGNAL(valueChanged(int)), this, SLOT(setReverseFrom(int)));
    ///< SET connections DELETE (none)

    setStartEnd(0);
    checkValidity();
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

QString CopyMultiplekeyframesDialog::getActiveTab()
{
    int actTab = ui->tabWidget->currentIndex();
    switch (actTab) {
    case 0:
        return tr("copy");
        break;
    case 1:
        return tr("move");
        break;
    case 2:
        ui->labInfoToFromLayer->setText(tr("On layer: %1").arg(ui->cBoxFromLayer->currentText()));
        return tr("reverse");
        break;
    case 3:
        return tr("delete");
        break;
    default:
        Q_ASSERT(false);
        return "";
        break;
    }
}

bool CopyMultiplekeyframesDialog::getValidity()
{
    return mValidAction;
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
    int index = ui->tabWidget->currentIndex();
    switch (index) {
    case 0:
        ui->labInfoToFromLayer->setText(tr("From: %1 To: %2").arg(ui->cBoxFromLayer->currentText()).arg(ui->cBoxCopyToLayer->currentText()));
        break;
    case 1:
        ui->labInfoToFromLayer->setText(tr("From: %1 To: %2").arg(ui->cBoxFromLayer->currentText()).arg(ui->cBoxMoveToLayer->currentText()));
        break;
    case 2:
        ui->labInfoToFromLayer->setText(tr("On: %1").arg(mFromLayer));
        break;
    case 3:
        ui->labInfoToFromLayer->setText(tr("On: %1").arg(mFromLayer));
        ui->labDeleteOnLayer->setText(tr("On Layer %1").arg(mFromLayer));
        break;
    default:
        Q_ASSERT(false);
        break;
    }
}

void CopyMultiplekeyframesDialog::setCopyToLayer(QString copyToLayer)
{
    mCopyToLayer = copyToLayer;
    if (ui->tabWidget->currentIndex() == 0)
    {
        ui->labInfoToFromLayer->setText(tr("From: %1 To: %2").arg(ui->cBoxFromLayer->currentText()).arg(ui->cBoxCopyToLayer->currentText()));
    }
    checkValidity();
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
    checkValidity();
}

void CopyMultiplekeyframesDialog::setMoveToLayer(QString moveToLayer)
{
    mMoveToLayer = moveToLayer;
    if (ui->tabWidget->currentIndex() == 1)
    {
        ui->labInfoToFromLayer->setText(tr("From: %1 To: %2").arg(ui->cBoxFromLayer->currentText()).arg(ui->cBoxMoveToLayer->currentText()));
    }
    checkValidity();
}

void CopyMultiplekeyframesDialog::setReverseFrom(int reverseFrom)
{
    mReverseStart = reverseFrom;
    if (ui->tabWidget->currentIndex() == 2)
    {
        ui->labInfoToFromLayer->setText(tr("On: %1").arg(ui->cBoxFromLayer->currentText()));
    }
    checkValidity();
}

void CopyMultiplekeyframesDialog::setDeleteOnLayer(QString deleteFromLayer)
{
    mDeleteOnLayer = deleteFromLayer;
    if (ui->tabWidget->currentIndex() == 3)
    {
        ui->labInfoToFromLayer->setText(tr("On: %1").arg(mDeleteOnLayer));
        ui->labDeleteOnLayer->setText(tr("On Layer %1").arg(mDeleteOnLayer));
    }
    checkValidity();
}

void CopyMultiplekeyframesDialog::setMethodPicked(int tabIndex)
{
    ui->labInfoAction->setText(getActiveTab().toUpper());
    switch (tabIndex) {
    case 0:
        setCopyToLayer(ui->cBoxCopyToLayer->currentText());
        break;
    case 1:
        setMoveToLayer(ui->cBoxMoveToLayer->currentText());
        break;
    case 2:
        setReverseFrom(ui->sBoxStartReverse->value());
        break;
    case 3:
        setDeleteOnLayer(ui->cBoxFromLayer->currentText());
        break;
    default:
        Q_ASSERT(false);
        break;
    }
    checkValidity();
}


void CopyMultiplekeyframesDialog::setStartEnd(int methodChosen)
{
    switch (methodChosen) {
    case 0: // copy
        mManiStartAt = ui->sBoxStartFrame->value();
        mManiEndAt = mManiStartAt - 1 + (mLastFrame + 1 - mFirstFrame) * mNumLoops;
        break;
    case 1: // move
        mManiStartAt = ui->sBoxMove->value();
        mManiEndAt = mManiStartAt + mLastFrame - mFirstFrame;
        break;
    case 2: // reverse
        mManiStartAt = ui->sBoxStartReverse->value();
        mManiEndAt = mManiStartAt + mLastFrame - mFirstFrame;
        break;
    case 3:
        mManiStartAt = ui->sBoxFirstFrame->value();
        mManiEndAt = ui->sBoxLastFrame->value();
        break;
    default:
        Q_ASSERT(false);
        break;
    }
}

void CopyMultiplekeyframesDialog::checkValidity()
{
    setStartEnd(ui->tabWidget->currentIndex());
    QString msg = "";
    bool testValidity = true;
    // 9999 frames is maximum timeline length
    if (mManiEndAt > 9999)
    {
        msg = tr("Exceeds 9999 frames!");
        testValidity = false;
    }               // Range must be valid
    else if (mFirstFrame >= mLastFrame)
    {
        msg = tr("Range not valid!");
        testValidity = false;
    }
    /*
    else if (ui->tabWidget->currentIndex() == 0 && ui->cBoxFromLayer->currentText() == mCopyToLayer)
    {
        if (ui->sBoxLastFrame->value() >= ui->sBoxStartFrame->value())
        {
            msg = tr("Beware! Originals may be damaged!");
        }
    }
    else if (ui->tabWidget->currentIndex() == 1 &&
             (ui->sBoxFirstFrame->value() == ui->sBoxMove->value()))
    {
            msg = tr("Alert! Originals will be erased!");
    }
    else if (ui->tabWidget->currentIndex() == 2)
    {
        if (ui->sBoxLastFrame->value() >= ui->sBoxStartReverse->value())
        {
            msg = tr("Beware! Originals may be damaged!");
        }
    }
    */
    if (msg == "")
    {
        ui->labWarning->setText(tr("Affects Frames %1 %2 %3").arg(QString::number(mManiStartAt)).arg(QChar(0x2192)).arg(QString::number(mManiEndAt)));
    }
    else
    {
        ui->labWarning->setText(msg);
    }
    mValidAction = testValidity;
}
