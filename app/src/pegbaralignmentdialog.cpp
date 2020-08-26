#include "pegbaralignmentdialog.h"
#include "ui_pegbaralignmentdialog.h"

#include <QListWidget>
#include <QListWidgetItem>
#include <QMessageBox>
#include "layermanager.h"
#include "selectionmanager.h"

PegBarAlignmentDialog::PegBarAlignmentDialog(Editor *editor, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PegBarAlignmentDialog)
{
    ui->setupUi(this);
    mEditor = editor;
    connect(ui->btnAlign, &QPushButton::clicked, this, &PegBarAlignmentDialog::alignPegs);
    connect(ui->btnCancel, &QPushButton::clicked, this, &PegBarAlignmentDialog::closeClicked);
    connect(ui->lwLayers, &QListWidget::clicked, this, &PegBarAlignmentDialog::updatePegRegDialog);

    connect(mEditor->layers(), &LayerManager::layerCountChanged, this, &PegBarAlignmentDialog::updatePegRegLayers);
    connect(mEditor->select(), &SelectionManager::selectionChanged, this, &PegBarAlignmentDialog::updatePegRegDialog);
    connect(mEditor, &Editor::currentFrameChanged, this, &PegBarAlignmentDialog::updatePegRegDialog);
    connect(mEditor->layers(), &LayerManager::currentLayerChanged, this, &PegBarAlignmentDialog::updatePegRegDialog);

    ui->btnAlign->setEnabled(false);
    mLayernames.clear();
}

PegBarAlignmentDialog::~PegBarAlignmentDialog()
{
    delete ui;
}

void PegBarAlignmentDialog::setLayerList(QStringList layerList)
{
    ui->lwLayers->clear();
    mLayernames = layerList;
    for (int i = 0; i < mLayernames.count(); i++)
    {
        ui->lwLayers->addItem(mLayernames.at(i));
    }
}

QStringList PegBarAlignmentDialog::getLayerList()
{
    QStringList selectedLayers;
    selectedLayers.clear();
    for (int i = 0; i < ui->lwLayers->count(); i++)
    {
        if (ui->lwLayers->item(i)->isSelected())
            selectedLayers.append(ui->lwLayers->item(i)->text());
    }
    return selectedLayers;
}

void PegBarAlignmentDialog::updateRefKeyLabelText()
{
    ui->labRefKey->setText(QStringLiteral("%1 - %2").arg(mRefLayer).arg(mRefkey));
}

void PegBarAlignmentDialog::setAreaSelected(bool b)
{
    mAreaSelected = b;
    setBtnAlignEnabled();
}

void PegBarAlignmentDialog::setReferenceSelected(bool b)
{
    mReferenceSelected = b;
    setBtnAlignEnabled();
}

void PegBarAlignmentDialog::setLayerSelected(bool b)
{
    mLayerSelected = b;
    setBtnAlignEnabled();
}

void PegBarAlignmentDialog::updatePegRegLayers()
{
    QStringList bitmaplayers;
    for (int i = 0; i < mEditor->layers()->count(); i++)
    {
        if (mEditor->layers()->getLayer(i)->type() == Layer::BITMAP)
        {
            bitmaplayers.append(mEditor->layers()->getLayer(i)->name());
        }
    }
    setLayerList(bitmaplayers);
}

void PegBarAlignmentDialog::updatePegRegDialog()
{
    // is something selected in scribblearea?
    setAreaSelected(mEditor->select()->somethingSelected());

    const Layer* currentLayer = mEditor->layers()->currentLayer();
    // is the reference key valid?
    setRefLayer(currentLayer->name());
    setRefKey(mEditor->currentFrame());

    bool isReferenceSelected = (currentLayer->type() == Layer::BITMAP &&
                                currentLayer->keyExists(mEditor->currentFrame()));
    setReferenceSelected(isReferenceSelected);

    // has minimum one layer been selected?
    const QStringList bitmaplayers = getLayerList();

    if (bitmaplayers.isEmpty())
    {
        setLayerSelected(false);
    }
    else
    {
        setRefLayer(currentLayer->name());
        setRefKey(mEditor->currentFrame());
        setLayerSelected(true);
    }

    setBtnAlignEnabled();
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

    Status result = mEditor->pegBarAlignment(bitmaplayers);
    if (!result.ok())
    {
        QMessageBox::information(this, "Pencil2D",
                                 result.description(),
                                 QMessageBox::Ok);
        return;
    }
    done(QDialog::Accepted);
}

void PegBarAlignmentDialog::setBtnAlignEnabled()
{
    if (mAreaSelected && mReferenceSelected && mLayerSelected)
        ui->btnAlign->setEnabled(true);
    else
        ui->btnAlign->setEnabled(false);
}

void PegBarAlignmentDialog::setRefLayer(QString s)
{
    mRefLayer = s;
    updateRefKeyLabelText();
}

void PegBarAlignmentDialog::setRefKey(int i)
{
    mRefkey = i;
    updateRefKeyLabelText();
}

void PegBarAlignmentDialog::closeClicked()
{
    done(QDialog::Accepted);
}
