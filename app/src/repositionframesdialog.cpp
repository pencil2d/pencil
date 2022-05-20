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


RepositionFramesDialog::RepositionFramesDialog(QWidget* parent) :
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
}

void RepositionFramesDialog::initUI()
{
    if (mEditor->layers()->currentLayer()->keyExists(mEditor->currentFrame()))
    {
        mRepositionFrame = mEditor->currentFrame();
    }
    else
    {
        mRepositionFrame = mEditor->layers()->currentLayer()->getSelectedFramesByPos().at(0);
    }

    prepareRepositionSelectedImages(mRepositionFrame);

    updateRadioButtons();

    ui->rbAllKeyframes->setChecked(true);
    connect(ui->cbOtherLayers, &QCheckBox::stateChanged, this, &RepositionFramesDialog::checkboxStateChanged);
    connect(ui->rbAllKeyframes, &QRadioButton::clicked, this, &RepositionFramesDialog::updateLayersBox);
    connect(ui->rbSameKeyframes, &QRadioButton::clicked, this, &RepositionFramesDialog::updateLayersBox);
    connect(ui->btnReposition, &QPushButton::clicked, this, &RepositionFramesDialog::repositionFrames);
    connect(ui->btnCancel, &QPushButton::clicked, this, &RepositionFramesDialog::closeClicked);
    connect(mEditor->getScribbleArea(), &ScribbleArea::selectionUpdated, this, &RepositionFramesDialog::updateDialogText);
    connect(mEditor->select(), &SelectionManager::selectionReset, this, &RepositionFramesDialog::closeClicked);
    mEndPoint = mStartPoint = QPoint(0,0);
    mOriginalPolygonF = mEditor->select()->currentSelectionPolygonF();
    updateDialogSelectedFrames();
    updateDialogText();
}

void RepositionFramesDialog::updateDialogText()
{
    if (mOriginalPolygonF.boundingRect().isEmpty())
    {
        mOriginalPolygonF = mEditor->select()->currentSelectionPolygonF();
    }
    mCurrentPolygonF = mEditor->select()->currentSelectionPolygonF();
    QPoint point = getRepositionPoint();
    ui->labRepositioned->setText(tr("Repositioned: ( %1, %2 )").arg(point.x()).arg(point.y()));
}

void RepositionFramesDialog::updateDialogSelectedFrames()
{
    QList<int> framelist = mEditor->layers()->currentLayer()->getSelectedFramesByPos();
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
    QList<int> frames = mEditor->layers()->currentLayer()->getSelectedFramesByPos();
    for (int i = 0; i < frames.size(); i++)
    {
        mEditor->repositionImage(mEndPoint, frames.at(i));
    }

    if (!ui->listSelectedLayers->selectedItems().isEmpty())
    {
        auto layerManager = mEditor->layers();

        // if only selected keyframe-numbers should be repositioned
        if (ui->rbSameKeyframes->isChecked())
        {
            int currLayer = mEditor->currentLayerIndex();
            for (int j = 0; j < mLayerIndexes.size(); j++)
            {
                QListWidgetItem* item = ui->listSelectedLayers->item(j);
                if (item->isSelected())
                {
                    layerManager->setCurrentLayer(mLayerIndexes.at(j));
                    for (int i = 0; i < frames.size(); i++)
                    {       // only move frame if it exists
                        if (layerManager->currentLayer()->keyExists(frames.at(i)))
                        {
                            mEditor->repositionImage(mEndPoint, frames.at(i));
                        }
                    }
                }
            }
            layerManager->setCurrentLayer(currLayer);
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
                    layerManager->setCurrentLayer(mLayerIndexes.at(i));
                    int keyframe = layerManager->currentLayer()->firstKeyFramePosition();
                    do {
                        mEditor->repositionImage(mEndPoint, keyframe);
                        keyframe = layerManager->currentLayer()->getNextKeyFramePosition(keyframe);
                    }
                    while (mEditor->currentFrame() != layerManager->currentLayer()->getMaxKeyFramePosition());
                }
            }
            layerManager->setCurrentLayer(currLayer);
        }
    }
    mEditor->getScribbleArea()->applySelectionChanges();
    mEditor->select()->resetSelectionProperties();
    mEditor->scrubTo(mRepositionFrame);
    
    accept();
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
        updateLayersToSelect();
        break;
    default:
        ui->listSelectedLayers->clear();
        break;
    }
    updateRadioButtons();
}

void RepositionFramesDialog::updateLayersBox()
{
    updateLayersToSelect();
}

void RepositionFramesDialog::closeClicked()
{
    rejected();
}

void RepositionFramesDialog::updateLayersToSelect()
{
    ui->listSelectedLayers->clear();

    mLayerIndexes.clear();
    auto layerManager = mEditor->layers();
    if (ui->rbAllKeyframes->isChecked())
    {
        for (int i = layerManager->count() - 1; i >= 0; i--)
        {
            if (layerManager->getLayer(i)->type() == Layer::BITMAP &&
                    i != layerManager->currentLayerIndex())
            {
                ui->listSelectedLayers->addItem(layerManager->getLayer(i)->name());
                mLayerIndexes.append(i);
            }
        }
    }
    else
    {
        QList<int> frames = layerManager->currentLayer()->getSelectedFramesByPos();
        bool ok = false;
        for (int i = layerManager->count() - 1; i >= 0; i--)
        {
            if (layerManager->getLayer(i)->type() == Layer::BITMAP &&
                    i != layerManager->currentLayerIndex())
            {
                Layer* layer = layerManager->getLayer(i);
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
    int x = static_cast<int>(mCurrentPolygonF.boundingRect().x() - mOriginalPolygonF.boundingRect().x());
    int y = static_cast<int>(mCurrentPolygonF.boundingRect().y() - mOriginalPolygonF.boundingRect().y());
    mEndPoint = QPoint(x, y);
    return mEndPoint;
}

void RepositionFramesDialog::prepareRepositionSelectedImages(int repositionFrame)
{
    auto select = mEditor->select();
    auto layers = mEditor->layers();

    if (select->somethingSelected()) { return; }

    if (layers->currentLayer()->type() == Layer::BITMAP)
    {
        mEditor->scrubTo(repositionFrame);
        LayerBitmap* layer = static_cast<LayerBitmap*>(layers->currentLayer());
        QRect reposRect = layer->getFrameBounds(repositionFrame);
        select->setSelection(reposRect);
    }
}

