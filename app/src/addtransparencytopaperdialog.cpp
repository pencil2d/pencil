#include "addtransparencytopaperdialog.h"
#include "ui_addtransparencytopaperdialog.h"

#include "editor.h"
#include "layermanager.h"
#include "selectionmanager.h"
#include "layerbitmap.h"
#include "bitmapimage.h"

AddTransparencyToPaperDialog::AddTransparencyToPaperDialog(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AddTransparencyToPaperDialog)
{
    ui->setupUi(this);

    connect(ui->btnCancel, &QPushButton::clicked, this, &AddTransparencyToPaperDialog::closeClicked);
    connect(ui->btnTrace, &QPushButton::clicked, this, &AddTransparencyToPaperDialog::traceScannedDrawings);
}

AddTransparencyToPaperDialog::~AddTransparencyToPaperDialog()
{
    delete ui;
}

void AddTransparencyToPaperDialog::setCore(Editor *editor)
{
    mEditor = editor;
    connect(mEditor->layers(), &LayerManager::currentLayerChanged, this, &AddTransparencyToPaperDialog::updateTraceButton);
}

void AddTransparencyToPaperDialog::updateTraceButton()
{
    if (mEditor->layers()->currentLayer()->type() == Layer::BITMAP)
        ui->btnTrace->setEnabled(true);
    else
        ui->btnTrace->setEnabled(false);
}

void AddTransparencyToPaperDialog::traceScannedDrawings()
{
    if (mEditor->layers()->currentLayer()->type() != Layer::BITMAP) { return; }

    LayerBitmap* layer = static_cast<LayerBitmap*>(mEditor->layers()->currentLayer());
    BitmapImage* img = new BitmapImage();
    bool somethingSelected = mEditor->select()->somethingSelected();

    if (ui->rbCurrentKeyframe->isChecked() && layer->keyExists(mEditor->currentFrame()))
    {
        if (somethingSelected)
        {
            mEditor->copy();
            layer->removeKeyFrame(mEditor->currentFrame());
            layer->addNewKeyFrameAt(mEditor->currentFrame());
            mEditor->paste();
        }
        img = layer->getBitmapImageAtFrame(mEditor->currentFrame());
        img->setThreshold(ui->sb_treshold->value());
        img = img->scanToTransparent(img,
                                     ui->cb_Red->isChecked(),
                                     ui->cb_Green->isChecked(),
                                     ui->cb_Blue->isChecked());
    }
    else
    {
        for (int i = layer->firstKeyFramePosition(); i <= layer->getMaxKeyFramePosition(); i++)
        {
            mEditor->scrubTo(i);
            if (layer->keyExists(i))
            {
                if (somethingSelected)
                {
                    mEditor->copy();
                    layer->removeKeyFrame(i);
                    layer->addNewKeyFrameAt(i);
                    mEditor->paste();
                }
                img = layer->getBitmapImageAtFrame(i);
                img->setThreshold(ui->sb_treshold->value());
                img = img->scanToTransparent(img,
                                             ui->cb_Red->isChecked(),
                                             ui->cb_Green->isChecked(),
                                             ui->cb_Blue->isChecked());
            }
        }
    }
}

void AddTransparencyToPaperDialog::closeClicked()
{
    emit closeDialog();
    close();
}
