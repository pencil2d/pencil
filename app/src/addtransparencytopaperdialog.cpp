#include "addtransparencytopaperdialog.h"
#include "ui_addtransparencytopaperdialog.h"

#include <QProgressDialog>
#include <QMessageBox>
#include <QPixmap>
#include <QGraphicsScene>
#include <QDebug>

#include "editor.h"
#include "layermanager.h"
#include "selectionmanager.h"
#include "layerbitmap.h"
#include "bitmapimage.h"


AddTransparencyToPaperDialog::AddTransparencyToPaperDialog(QDialog *parent) :
    QDialog(parent),
    ui(new Ui::AddTransparencyToPaperDialog)
{
    ui->setupUi(this);
    ui->mainLayout->setStretchFactor(ui->optionsLayout, 1);
    ui->mainLayout->setStretchFactor(ui->previewLayout, 20);

    connect(this, &QDialog::finished, this, &AddTransparencyToPaperDialog::closeClicked);
    connect(ui->sb_treshold, QOverload<int>::of(&QSpinBox::valueChanged), this, &AddTransparencyToPaperDialog::SpinboxChanged);
    connect(ui->sliderThreshold, &QSlider::valueChanged, this, &AddTransparencyToPaperDialog::SliderChanged);
    connect(ui->cb_Red, &QCheckBox::stateChanged, this, &AddTransparencyToPaperDialog::updateDrawing);
    connect(ui->cb_Green, &QCheckBox::stateChanged, this, &AddTransparencyToPaperDialog::updateDrawing);
    connect(ui->cb_Blue, &QCheckBox::stateChanged, this, &AddTransparencyToPaperDialog::updateDrawing);
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
    if (mEditor->layers()->currentLayer()->type() != Layer::BITMAP)
        this->setEnabled(false);
    loadDrawing(mEditor->currentFrame());
    connect(mEditor->layers(), &LayerManager::currentLayerChanged, this, &AddTransparencyToPaperDialog::layerChanged);
    connect(mEditor, &Editor::scrubbedTo, this, &AddTransparencyToPaperDialog::updateDrawing);
}

void AddTransparencyToPaperDialog::SpinboxChanged(int value)
{
    mThreshold = value;
    ui->sliderThreshold->setValue(value);
    updateDrawing();
}

void AddTransparencyToPaperDialog::SliderChanged(int value)
{
    mThreshold = value;
    ui->sb_treshold->setValue(value);
    updateDrawing();
}

void AddTransparencyToPaperDialog::loadDrawing(int frame)
{
    if (mEditor->layers()->currentLayer()->type() != Layer::BITMAP) { return; }

    LayerBitmap* layer = static_cast<LayerBitmap*>(mEditor->layers()->currentLayer());

    if (!layer->keyExists(frame))
    {
        if (!layer->keyExistsWhichCovers(frame))
            frame = layer->getPreviousKeyFramePosition(frame);
        else
            frame = layer->getNextKeyFramePosition(frame);
    }

    ui->labShowingFrame->setText(tr("Previewing frame %1").arg(QString::number(frame)));

    mBitmap = nullptr;
    mBitmap = layer->getBitmapImageAtFrame(frame)->clone();
    mBitmap->setThreshold(mThreshold);

    mBitmap = mBitmap->scanToTransparent(mBitmap,
                                         ui->cb_Red->isChecked(),
                                         ui->cb_Green->isChecked(),
                                         ui->cb_Blue->isChecked());

    mLoadedImage = *mBitmap->image();
    mPixmapFromImage = QPixmap::fromImage(mLoadedImage);
    scene.clear();
    scene.setBackgroundBrush(Qt::white);
    scene.addPixmap(mPixmapFromImage);
    ui->preview->items().clear();
    ui->preview->setScene(&scene);
    ui->preview->show();
}

void AddTransparencyToPaperDialog::updateDrawing()
{
    loadDrawing(mEditor->currentFrame());
}

void AddTransparencyToPaperDialog::layerChanged(int index)
{
    if (mEditor->layers()->getLayer(index)->type() == Layer::BITMAP)
    {
        this->setEnabled(true);
        updateDrawing();
    }
    else
    {
        this->setEnabled(false);
    }
}

void AddTransparencyToPaperDialog::traceScannedDrawings()
{
    if (mEditor->layers()->currentLayer()->type() != Layer::BITMAP) { return; }

    LayerBitmap* layer = static_cast<LayerBitmap*>(mEditor->layers()->currentLayer());
    BitmapImage* img = new BitmapImage();
    bool somethingSelected = mEditor->select()->somethingSelected();

    if (ui->rbCurrentKeyframe->isChecked())
    {
        int frame = mEditor->currentFrame();
        if (!layer->keyExists(frame))
        {
            if (!layer->keyExistsWhichCovers(frame))
                frame = layer->getPreviousKeyFramePosition(frame);
            else
                frame = layer->getNextKeyFramePosition(frame);
        }
        mEditor->scrubTo(frame);

        if (somethingSelected)
        {
            mEditor->copy();
            layer->removeKeyFrame(frame);
            layer->addNewKeyFrameAt(frame);
            mEditor->paste();
        }
        img = layer->getBitmapImageAtFrame(frame);
        img->setThreshold(mThreshold);
        img = img->scanToTransparent(img,
                                     ui->cb_Red->isChecked(),
                                     ui->cb_Green->isChecked(),
                                     ui->cb_Blue->isChecked());
        img->modification();
    }
    else
    {
        mEditor->setIsDoingRepeatColoring(true);
        int count = mEditor->getAutoSaveCounter();
        QProgressDialog* mProgress = new QProgressDialog(tr("Thinning lines in bitmaps..."), tr("Abort"), 0, 100, this);
        mProgress->setWindowModality(Qt::WindowModal);
        mProgress->show();
        mProgress->setMaximum(layer->keyFrameCount());
        mProgress->setValue(0);
        int keysThinned = 0;
        QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
        for (int i = layer->firstKeyFramePosition(); i <= layer->getMaxKeyFramePosition(); i++)
        {
            if (layer->keyExists(i))
            {
                mProgress->setValue(keysThinned++);
                mEditor->scrubTo(i);
                count++;
                if (mProgress->wasCanceled())
                {
                    break;
                }
                if (somethingSelected)
                {
                    mEditor->copy();
                    layer->removeKeyFrame(i);
                    layer->addNewKeyFrameAt(i);
                    mEditor->paste();
                }
                img = layer->getBitmapImageAtFrame(i);
                img->setThreshold(mThreshold);
                img = img->scanToTransparent(img,
                                             ui->cb_Red->isChecked(),
                                             ui->cb_Green->isChecked(),
                                             ui->cb_Blue->isChecked());
                img->modification();
            }
        }
        mProgress->close();
        mEditor->setIsDoingRepeatColoring(false);
        mEditor->setAutoSaveCounter(count);
    }
    if (ui->rbAllKeyframes->isChecked())
        closeClicked();
}

void AddTransparencyToPaperDialog::closeClicked()
{
    emit closeDialog();
    close();
}
