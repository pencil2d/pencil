#include "repositionframesdialog.h"
#include "ui_repositionframesdialog.h"

#include <QList>
#include <QListWidgetItem>
#include <QMessageBox>

#include "selectionmanager.h"
#include "selectionpainter.h"
#include "viewmanager.h"
#include "toolmanager.h"
#include "layer.h"
#include "layermanager.h"
#include "layerbitmap.h"
#include "scribblearea.h"


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
    updateRadioButtons();
    ui->rbAllKeyframes->setChecked(true);
    connect(ui->cbOtherLayers, &QCheckBox::stateChanged, this, &RepositionFramesDialog::checkboxStateChanged);
    connect(ui->rbAllKeyframes, &QRadioButton::clicked, this, &RepositionFramesDialog::updateLayersBox);
    connect(ui->rbSameKeyframes, &QRadioButton::clicked, this, &RepositionFramesDialog::updateLayersBox);
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
    if (mStartPoint == mEndPoint)
    {
        QMessageBox::information(this, nullptr,
                                 tr("Please move selection to desired destination\n"
                                    "or cancel"),
                                 QMessageBox::Ok);
        return;
    }

    mEditor->getScribbleArea()->updateOriginalPolygonF();
    QList<int> frames = mEditor->layers()->currentLayer()->getSelectedFramesList();
    for (int i = 0; i < frames.size(); i++)
    {
        mEditor->layers()->repositionFrame(mEndPoint, frames.at(i));
    }

    if (!ui->listSelectedLayers->selectedItems().isEmpty())
    {
        auto lMgr = mEditor->layers();

        // if only selcted keyframe-numbers should be repositioned
        if (ui->rbSameKeyframes->isChecked())
        {
            int currLayer = mEditor->currentLayerIndex();
            for (int j = 0; j < mLayerIndexes.size(); j++)
            {
                QListWidgetItem* item = ui->listSelectedLayers->item(j);
                if (item->isSelected())
                {
                    lMgr->setCurrentLayer(mLayerIndexes.at(j));
                    for (int i = 0; i < frames.size(); i++)
                    {       // only move frame if it exists
                        if (lMgr->currentLayer()->keyExists(frames.at(i)))
                        {
                            lMgr->repositionFrame(mEndPoint, frames.at(i));
                        }
                    }
                }
            }
            lMgr->setCurrentLayer(currLayer);
        }
        // if all keyframes on layer should be repositioned
        else
        {
            int currLayer = mEditor->currentLayerIndex();
            for (int i= 0; i < mLayerIndexes.size(); i++)
            {
                QListWidgetItem* item = ui->listSelectedLayers->item(i);
                if (item->isSelected())
                {
                    lMgr->setCurrentLayer(mLayerIndexes.at(i));
                    int keyframe = lMgr->currentLayer()->firstKeyFramePosition();
                    do {
                        lMgr->repositionFrame(mEndPoint, keyframe);
                        keyframe = lMgr->currentLayer()->getNextKeyFramePosition(keyframe);
                    } while (mEditor->currentFrame() != lMgr->currentLayer()->getMaxKeyFramePosition());
                }
            }
            lMgr->setCurrentLayer(currLayer);
        }
    }
    mEditor->getScribbleArea()->applySelectionChanges();
    mEditor->select()->resetSelectionProperties();
    mEditor->scrubTo(mRepositionFrame);
    closeClicked();
}

void RepositionFramesDialog::updateRadioButtons()
{
    if (ui->cbOtherLayers->isChecked())
    {
        ui->rbAllKeyframes->setEnabled(true);
        ui->rbSameKeyframes->setEnabled(true);
        ui->listSelectedLayers->setEnabled(true);
    }
    else
    {
        ui->rbAllKeyframes->setEnabled(false);
        ui->rbSameKeyframes->setEnabled(false);
        ui->listSelectedLayers->setEnabled(false);
    }
}

void RepositionFramesDialog::checkboxStateChanged(int i)
{
    switch (i)
    {
    case Qt::Checked:
        updateRadioButtons();
        updateLayersToSelect();
        break;
    default:
        updateRadioButtons();
        ui->listSelectedLayers->clear();
        break;
    }
}

void RepositionFramesDialog::updateLayersBox()
{
    updateLayersToSelect();
}

void RepositionFramesDialog::closeClicked()
{
    mEndPoint = mStartPoint;
    emit closeDialog();
    close();
}

void RepositionFramesDialog::updateLayersToSelect()
{
    ui->listSelectedLayers->clear();

    mLayerIndexes.clear();
    auto layermanager = mEditor->layers();
    if (ui->rbAllKeyframes->isChecked())
    {
        for (int i = layermanager->count() - 1; i >= 0; i--)
        {
            if (layermanager->getLayer(i)->type() == Layer::BITMAP &&
                    i != layermanager->currentLayerIndex())
            {
                ui->listSelectedLayers->addItem(layermanager->getLayer(i)->name());
                mLayerIndexes.append(i);
            }
        }
    }
    else
    {
        QList<int> frames = layermanager->currentLayer()->getSelectedFramesList();
        bool ok = false;
        for (int i = layermanager->count() - 1; i >= 0; i--)
        {
            if (layermanager->getLayer(i)->type() == Layer::BITMAP &&
                    i != layermanager->currentLayerIndex())
            {
                Layer* layer = layermanager->getLayer(i);
                for (int j = 0; j < frames.size(); j++)
                {
                    if (layer->keyExists(frames.at(j)))
                        ok = true;
                }
                if (ok)
                {
                    ui->listSelectedLayers->addItem(layer->name());
                    mLayerIndexes.append(i);
                }
                ok = false;
            }

        }
    }
}

QPoint RepositionFramesDialog::getRepositionPoint()
{
    int x, y;
    x = static_cast<int>(mCurrentPolygonF.boundingRect().x() - mOriginalPolygonF.boundingRect().x());
    y = static_cast<int>(mCurrentPolygonF.boundingRect().y() - mOriginalPolygonF.boundingRect().y());
    mEndPoint = QPoint(x, y);
    return mEndPoint;
}

