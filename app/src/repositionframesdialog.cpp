#include "repositionframesdialog.h"
#include "ui_repositionframesdialog.h"

#include <QList>
#include <QMessageBox>

#include "selectionmanager.h"
#include "selectionpainter.h"
#include "viewmanager.h"
#include "toolmanager.h"
#include "layer.h"
#include "layermanager.h"
#include "layerbitmap.h"
#include "scribblearea.h"
#include <QDebug>

RepositionFramesDialog::RepositionFramesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RepositionFramesDialog)
{
    ui->setupUi(this);
}

RepositionFramesDialog::~RepositionFramesDialog()
{
    delete ui;
}

void RepositionFramesDialog::setCore(Editor *editor)
{
    mEditor = editor;
    if (mEditor->layers()->currentLayer()->keyExists(mEditor->currentFrame()))
        mRepositionFrame = mEditor->currentFrame();
    else
        mRepositionFrame = mEditor->layers()->currentLayer()->getSelectedFramesList().at(0);
    mEditor->layers()->prepareRepositionSelectedFrames(mRepositionFrame);
    connect(ui->btnReposition, &QPushButton::clicked, this, &RepositionFramesDialog::repositionFrames);
    connect(ui->btnCancel, &QPushButton::clicked, this, &RepositionFramesDialog::closeClicked);
    connect(this, &QDialog::finished, this, &RepositionFramesDialog::closeClicked);
    connect(mEditor->getScribbleArea(), &ScribbleArea::selectionUpdated, this, &RepositionFramesDialog::updateDialogText);
    connect(mEditor->select(), &SelectionManager::selectionReset, this, &RepositionFramesDialog::closeClicked);
    QMessageBox::information(this, nullptr,
                             tr("Please move selection to desired destination"),
                             QMessageBox::Ok);
    mEndPoint = mStartPoint;
    mOriginalPolygonF = mEditor->select()->currentSelectionPolygonF();
    updateDialogSelectedFrames();
    updateDialogText();
}

void RepositionFramesDialog::updateDialogText()
{
    if (mOriginalPolygonF.isEmpty())
        mOriginalPolygonF = mEditor->select()->currentSelectionPolygonF();
    mCurrentPolygonF = mEditor->select()->currentSelectionPolygonF();
    QPoint point = getRepositionPoint();
    ui->labRepositioned->setText(tr("Repositioned: ( %1, %2 )").arg(point.x()).arg(point.y()));
}

void RepositionFramesDialog::updateDialogSelectedFrames()
{
    QList<int> framelist = mEditor->layers()->currentLayer()->getSelectedFramesList();
    ui->labRepositionFrames->setText(tr("Selected on Layer: %1").arg(mEditor->layers()->currentLayer()->name()));
    ui->listSelectedFrames->clear();
    for (int i = 0; i < framelist.count(); i++)
    {
        ui->listSelectedFrames->addItem(QString::number(framelist.at(i)));
    }
}

void RepositionFramesDialog::repositionFrames()
{
    if (mStartPoint == mEndPoint) { return; }

    QList<int> frames = mEditor->layers()->currentLayer()->getSelectedFramesList();
    for (int i = 0; i < frames.size(); i++)
    {
        mEditor->layers()->repositionFrame(mEndPoint, frames.at(i));
    }
    // The next four lines show the worst hack I've EVER done!
    if (mRepositionFrame != frames.first())
        mEditor->scrubTo(frames.first());
    else
        mEditor->scrubTo(frames.last());
    mEditor->getScribbleArea()->applySelectionChanges();
    mEditor->select()->resetSelectionProperties();
    mEditor->scrubTo(mRepositionFrame);
    closeClicked();
}

void RepositionFramesDialog::closeClicked()
{
    mEndPoint = mStartPoint;
    emit closeDialog();
    close();
}

QPoint RepositionFramesDialog::getRepositionPoint()
{
    int x, y;
    x = static_cast<int>(mCurrentPolygonF.boundingRect().x() - mOriginalPolygonF.boundingRect().x());
    y = static_cast<int>(mCurrentPolygonF.boundingRect().y() - mOriginalPolygonF.boundingRect().y());
    mEndPoint = QPoint(x, y);
    return mEndPoint;
}

