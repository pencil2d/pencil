/*

Pencil2D - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2012-2020 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/
#include "pegbaralignmentdialog.h"
#include "ui_pegbaralignmentdialog.h"

#include <QListWidget>
#include <QListWidgetItem>
#include <QMessageBox>

#include "keyframe.h"
#include "layermanager.h"
#include "selectionmanager.h"
#include "toolmanager.h"

#include <pegbaraligner.h>

PegBarAlignmentDialog::PegBarAlignmentDialog(Editor *editor, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PegBarAlignmentDialog), mEditor(editor)
{
    ui->setupUi(this);
    connect(ui->btnAlign, &QPushButton::clicked, this, &PegBarAlignmentDialog::alignPegs);
    connect(ui->btnCancel, &QPushButton::clicked, this, &PegBarAlignmentDialog::closeClicked);
    connect(ui->lwLayers, &QListWidget::clicked, this, &PegBarAlignmentDialog::updatePegRegDialog);

    connect(mEditor->layers(), &LayerManager::layerCountChanged, this, &PegBarAlignmentDialog::updatePegRegLayers);
    connect(mEditor->select(), &SelectionManager::selectionChanged, this, &PegBarAlignmentDialog::updatePegRegDialog);
    connect(mEditor, &Editor::scrubbed, this, &PegBarAlignmentDialog::updatePegRegDialog);
    connect(mEditor->layers(), &LayerManager::currentLayerChanged, this, &PegBarAlignmentDialog::updatePegRegDialog);

    updatePegRegLayers();

    ui->btnAlign->setEnabled(false);
    mLayernames.clear();

    mEditor->tools()->setCurrentTool(SELECT);

    if (!mEditor->select()->somethingSelected()) {
        mEditor->select()->setSelection(QRect(0,0,200,100));
    }
}

PegBarAlignmentDialog::~PegBarAlignmentDialog()
{
    delete ui;
    mEditor->select()->resetSelectionProperties();
}

void PegBarAlignmentDialog::setLayerList(QStringList layerList)
{
    ui->lwLayers->clear();
    mLayernames = layerList;
    for (int i = 0; i < mLayernames.count(); i++)
    {
        ui->lwLayers->addItem(mLayernames.at(i));
    }

    // Select the first layer.
    if (ui->lwLayers->count() > 0) {
        ui->lwLayers->item(0)->setSelected(true);
    }
}

QStringList PegBarAlignmentDialog::getLayerList()
{
    QStringList selectedLayers;
    selectedLayers.clear();
    for (int i = 0; i < ui->lwLayers->count(); i++)
    {
        if (!ui->lwLayers->item(i)->isSelected()) { continue; }

        selectedLayers.append(ui->lwLayers->item(i)->text());
    }
    return selectedLayers;
}

void PegBarAlignmentDialog::updateRefKeyLabel(QString text)
{
    ui->labRefKey->setText(text);
}

void PegBarAlignmentDialog::setAreaSelected(bool b)
{
    mAreaSelected = b;
    updateAlignButton();
}

void PegBarAlignmentDialog::setReferenceSelected(bool b)
{
    mReferenceSelected = b;
    updateAlignButton();
}

void PegBarAlignmentDialog::setLayerSelected(bool b)
{
    mLayerSelected = b;
    updateAlignButton();
}

void PegBarAlignmentDialog::updatePegRegLayers()
{
    QStringList bitmaplayers;
    auto layerMan = mEditor->layers();
    for (int i = 0; i < layerMan->count(); i++)
    {
        const Layer* layer = layerMan->getLayer(i);
        if (layer->type() != Layer::BITMAP) { continue; }

        bitmaplayers.append(layer->name());
    }
    setLayerList(bitmaplayers);
    updatePegRegDialog();
}

void PegBarAlignmentDialog::updatePegRegDialog()
{
    // is something selected in scribblearea?
    setAreaSelected(mEditor->select()->somethingSelected());

    const Layer* currentLayer = mEditor->layers()->currentLayer();
    // is the reference key valid?
    KeyFrame* key = currentLayer->getLastKeyFrameAtPosition(mEditor->currentFrame());

    bool isReferenceSelected = (currentLayer->type() == Layer::BITMAP &&
                                currentLayer->getLastKeyFrameAtPosition(mEditor->currentFrame()));
    setReferenceSelected(isReferenceSelected);

    // has minimum one layer been selected?
    const QStringList bitmaplayers = getLayerList();

    setLayerSelected(!bitmaplayers.isEmpty());
    updateRefKeyLabel(QString::number(key->pos()));
    updateAlignButton();
}

void PegBarAlignmentDialog::alignPegs()
{
    const QStringList bitmaplayers = getLayerList();
    if (bitmaplayers.isEmpty())
    {
        QMessageBox::information(this, "Pencil2D",
                                 tr("No layers selected!", "PegBar Dialog error message"),
                                 QMessageBox::Ok);
        return;
    }

    Status result = PegBarAligner(mEditor).align(bitmaplayers);
    if (!result.ok())
    {
        QMessageBox::information(this, "Pencil2D",
                                 result.description(),
                                 QMessageBox::Ok);
        return;
    }
    done(QDialog::Accepted);
}

void PegBarAlignmentDialog::updateAlignButton()
{
    if (mAreaSelected && mReferenceSelected && mLayerSelected)
        ui->btnAlign->setEnabled(true);
    else
        ui->btnAlign->setEnabled(false);
}

void PegBarAlignmentDialog::closeClicked()
{
    done(QDialog::Accepted);
}
