/*

Pencil2D - Traditional Animation Software
Copyright (C) 2020 David Lamhauge

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/

#include "addtransparencytopaperdialog.h"
#include "ui_addtransparencytopaperdialog.h"

#include <QGraphicsPixmapItem>
#include <QProgressDialog>
#include <QPushButton>

#include "editor.h"
#include "layermanager.h"
#include "selectionmanager.h"
#include "layerbitmap.h"
#include "bitmapimage.h"


AddTransparencyToPaperDialog::AddTransparencyToPaperDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddTransparencyToPaperDialog)
{
    ui->setupUi(this);

    connect(ui->sb_threshold, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &AddTransparencyToPaperDialog::thresholdSpinboxChanged);
    connect(ui->sliderThreshold, &QSlider::valueChanged, this, &AddTransparencyToPaperDialog::thresholdSliderChanged);
    connect(ui->cb_Red, &QCheckBox::stateChanged, this, &AddTransparencyToPaperDialog::updateDrawing);
    connect(ui->cb_Green, &QCheckBox::stateChanged, this, &AddTransparencyToPaperDialog::updateDrawing);
    connect(ui->cb_Blue, &QCheckBox::stateChanged, this, &AddTransparencyToPaperDialog::updateDrawing);
    connect(ui->buttonBox, &QDialogButtonBox::clicked, this, &AddTransparencyToPaperDialog::buttonClicked);
    connect(ui->testTransparencyCheckbox, &QCheckBox::stateChanged, this, &AddTransparencyToPaperDialog::checkerStateChanged);
    connect(ui->zoomSlider, &QSlider::valueChanged, this, &AddTransparencyToPaperDialog::zoomChanged);
}

AddTransparencyToPaperDialog::~AddTransparencyToPaperDialog()
{
    delete ui;
}

void AddTransparencyToPaperDialog::setCore(Editor *editor)
{
    mEditor = editor;
}

void AddTransparencyToPaperDialog::initUI()
{
    if (mEditor->layers()->currentLayer()->type() != Layer::BITMAP)
        this->setEnabled(false);
    loadDrawing(mEditor->currentFrame());
    connect(mEditor->layers(), &LayerManager::currentLayerChanged, this, &AddTransparencyToPaperDialog::layerChanged);
    connect(mEditor, &Editor::scrubbed, this, &AddTransparencyToPaperDialog::updateDrawing);

    scene.setBackgroundBrush(Qt::white);
    ui->preview->setScene(&scene);
    ui->preview->show();

    if (!mBitmap.bounds().isValid()) {
        ui->buttonBox->button(QDialogButtonBox::Apply)->setEnabled(false);
    }
}

void AddTransparencyToPaperDialog::thresholdSpinboxChanged(int value)
{
    mThreshold = value;
    ui->sliderThreshold->setValue(value);
    updateDrawing();
}

void AddTransparencyToPaperDialog::thresholdSliderChanged(int value)
{
    mThreshold = value;
    ui->sb_threshold->setValue(value);
    updateDrawing();
}

void AddTransparencyToPaperDialog::checkerStateChanged(bool state)
{
    if (state) {
        scene.setBackgroundBrush(QBrush(QImage(":/background/checkerboard.png")));
    } else {
        scene.setBackgroundBrush(Qt::white);
    }
}

void AddTransparencyToPaperDialog::zoomChanged(int zoomLevel)
{
    mZoomLevel = zoomLevel;
    updatePreview();
}

void AddTransparencyToPaperDialog::buttonClicked(QAbstractButton* button)
{
    switch (ui->buttonBox->buttonRole(button)) {
        case QDialogButtonBox::ApplyRole:
            accept();
            return;
        case QDialogButtonBox::RejectRole:
            reject();
            return;
        default:
            Q_UNREACHABLE();
    }
}

void AddTransparencyToPaperDialog::resizeEvent(QResizeEvent*)
{
    updatePreview();
}

void AddTransparencyToPaperDialog::updatePreview()
{
    QImage loadedImage = *mBitmap.image();

    QSize previewSize = ui->preview->size()*mZoomLevel;
    QSize size = mBitmap.size().scaled(previewSize, Qt::KeepAspectRatioByExpanding);
    mPixmapFromImage = QPixmap(size);
    mPixmapFromImage.fill(Qt::transparent);

    QPainter painter(&mPixmapFromImage);

    painter.drawImage(QRect(QPoint(0,0),QSize(size)), loadedImage, loadedImage.rect());
    mPreviewImageItem->setPixmap(mPixmapFromImage);

    scene.setSceneRect(QRect(QPoint(), previewSize));
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

    ui->labShowingFrame->setText(tr("Previewing Frame %1").arg(QString::number(frame)));

    BitmapImage* currentImage = layer->getBitmapImageAtFrame(frame);

    if (!currentImage) { return; }

    mBitmap = currentImage->copy();

    mBitmap = *mBitmap.scanToTransparent(&mBitmap,
                                         mThreshold,
                                         ui->cb_Red->isChecked(),
                                         ui->cb_Green->isChecked(),
                                         ui->cb_Blue->isChecked());

    if (mPreviewImageItem == nullptr) {
        mPreviewImageItem = scene.addPixmap(mPixmapFromImage);
    } else {
        mPreviewImageItem->setPixmap(mPixmapFromImage);
    }

    ui->buttonBox->button(QDialogButtonBox::Apply)->setEnabled(true);

    updatePreview();
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
    const bool somethingSelected = mEditor->select()->somethingSelected();
    const QRect selectionRect = mEditor->select()->mySelectionRect().toRect();

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
            BitmapImage selection = layer->getBitmapImageAtFrame(frame)->copy(selectionRect);
            layer->removeKeyFrame(frame);
            layer->addNewKeyFrameAt(frame);
            layer->getBitmapImageAtFrame(frame)->paste(&selection);
        }
        img = layer->getBitmapImageAtFrame(frame);
        img = img->scanToTransparent(img,
                                     mThreshold,
                                     ui->cb_Red->isChecked(),
                                     ui->cb_Green->isChecked(),
                                     ui->cb_Blue->isChecked());
        img->modification();
    }
    else
    {
        QProgressDialog* mProgress = new QProgressDialog(tr("Tracing scanned drawings..."), tr("Abort"), 0, 100, this);
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
                if (mProgress->wasCanceled())
                {
                    break;
                }
                if (somethingSelected)
                {
                    BitmapImage selection = layer->getBitmapImageAtFrame(i)->copy(selectionRect);
                    layer->removeKeyFrame(i);
                    layer->addNewKeyFrameAt(i);
                    layer->getBitmapImageAtFrame(i)->paste(&selection);
                }
                img = layer->getBitmapImageAtFrame(i);
                img = img->scanToTransparent(img,
                                             mThreshold,
                                             ui->cb_Red->isChecked(),
                                             ui->cb_Green->isChecked(),
                                             ui->cb_Blue->isChecked());
                img->modification();
            }
        }
        mProgress->close();
    }
}
