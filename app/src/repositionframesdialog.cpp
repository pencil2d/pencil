#include "repositionframesdialog.h"
#include "ui_repositionframesdialog.h"

#include <QList>

#include "selectionmanager.h"
#include "viewmanager.h"
#include "toolmanager.h"
#include "layer.h"
#include "layermanager.h"
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
    mEditor->layers()->prepareRepositionSelectedFrames();
    updateDialogSelectedFrames();
    updateDialogText();
    connect(ui->btnReposition, &QPushButton::clicked, this, &RepositionFramesDialog::repositionFrames);
    connect(ui->btnCancel, &QPushButton::clicked, this, &RepositionFramesDialog::closeClicked);
    connect(this, &QDialog::finished, this, &RepositionFramesDialog::closeClicked);
    connect(mEditor->getScribbleArea(), &ScribbleArea::selectionUpdated, this, &RepositionFramesDialog::updateDialogText);
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
    QPoint point = getRepositionPoint();
    mEditor->layers()->repositionSelectedFrames(point.x(), point.y());
    close();
}

void RepositionFramesDialog::closeClicked()
{

    emit closeDialog();
    close();
}

QPoint RepositionFramesDialog::getRepositionPoint()
{
    int x, y;
    x = static_cast<int>(mCurrentPolygonF.boundingRect().x() - mOriginalPolygonF.boundingRect().x());
    y = static_cast<int>(mCurrentPolygonF.boundingRect().y() - mOriginalPolygonF.boundingRect().y());
    return QPoint(x, y);
}

