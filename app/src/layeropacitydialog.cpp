#include "layeropacitydialog.h"
#include "ui_layeropacitydialog.h"

#include "layermanager.h"
#include "playbackmanager.h"
#include "layer.h"
#include "layerbitmap.h"
#include "bitmapimage.h"
#include "layervector.h"
#include "vectorimage.h"
#include "scribblearea.h"
#include "canvaspainter.h"
#include "util.h"
#include <QTimer>
#include <QDebug>


LayerOpacityDialog::LayerOpacityDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LayerOpacityDialog)
{
    ui->setupUi(this);
}

LayerOpacityDialog::~LayerOpacityDialog()
{
    delete ui;
}

void LayerOpacityDialog::setCore(Editor *editor)
{
    mEditor = editor;
    mLayerManager = mEditor->layers();
    mPlayBack = mEditor->playback();
}

void LayerOpacityDialog::init()
{
    connect(ui->chooseOpacitySlider, &QSlider::valueChanged, this, &LayerOpacityDialog::opacitySliderChanged);
    connect(ui->chooseOpacitySpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &LayerOpacityDialog::opacitySpinboxChanged);
    connect(ui->btnFadeIn, &QPushButton::pressed, this, &LayerOpacityDialog::fadeInPressed);
    connect(ui->btnFadeOut, &QPushButton::pressed, this, &LayerOpacityDialog::fadeOutPressed);
    connect(ui->btnClose, &QPushButton::pressed, this, &LayerOpacityDialog::closeClicked);

    connect(mEditor, &Editor::objectLoaded, this, &LayerOpacityDialog::newFileLoaded);
    connect(mPlayBack, &PlaybackManager::playStateChanged, this, &LayerOpacityDialog::playStateChanged);
    connect(mLayerManager, &LayerManager::currentLayerChanged, this, &LayerOpacityDialog::currentLayerChanged);
    connect(mEditor, &Editor::currentFrameChanged, this, &LayerOpacityDialog::currentFrameChanged);
    connect(mLayerManager->currentLayer(), &Layer::selectedFramesChanged, this, &LayerOpacityDialog::selectedFramesChanged);
    connect(this, &QDialog::finished, this, &LayerOpacityDialog::closeClicked);

    if (mLayerManager->currentLayer()->type() == Layer::BITMAP)
        initBitmap();
    else
        initVector();
    currentLayerChanged(mLayerManager->currentLayerIndex());
}

void LayerOpacityDialog::initBitmap()
{
    LayerBitmap* layer = static_cast<LayerBitmap*>(mLayerManager->currentLayer());
    BitmapImage* bitmap = layer->getBitmapImageAtFrame(mEditor->currentFrame());
    ui->labLayerInfo->setText(tr("Layer: %1").arg(layer->name()));
    if (layer->keyExists(mEditor->currentFrame()))
        ui->chooseOpacitySlider->setValue(static_cast<int>(bitmap->getOpacity() * 500));
    else
        ui->chooseOpacitySlider->setValue(500.0);
}

void LayerOpacityDialog::initVector()
{
    LayerVector* layer = static_cast<LayerVector*>(mLayerManager->currentLayer());
    VectorImage* vector = layer->getVectorImageAtFrame(mEditor->currentFrame());
    ui->labLayerInfo->setText(tr("Layer: %1").arg(layer->name()));
    if (layer->keyExists(mEditor->currentFrame()))
        ui->chooseOpacitySlider->setValue(static_cast<int>(vector->getOpacity() * 500));
    else
        ui->chooseOpacitySlider->setValue(500.0);
}


void LayerOpacityDialog::opacitySliderChanged(int value)
{
    ui->chooseOpacitySpinBox->setValue(value * 0.2);
    opacityValueChanged();
}

void LayerOpacityDialog::opacitySpinboxChanged(double value)
{
    ui->chooseOpacitySlider->setValue(static_cast<int>(value * 5.0));
    opacityValueChanged();
}

void LayerOpacityDialog::allLayerOpacity()
{
    if (mLayerManager->currentLayer()->type() == Layer::BITMAP)
    {
        LayerBitmap* layer = static_cast<LayerBitmap*>(mLayerManager->currentLayer());
        for (int i = 0; i <= layer->getMaxKeyFramePosition(); i++)
        {
            if (layer->keyExists(i))
            {
                BitmapImage* bitmap = layer->getBitmapImageAtFrame(i);
                bitmap->setOpacity(static_cast<qreal>(ui->chooseOpacitySlider->value()) / 500.0);
            }
        }
    }
    else if (mLayerManager->currentLayer()->type() == Layer::VECTOR)
    {
        LayerVector* layer = static_cast<LayerVector*>(mLayerManager->currentLayer());
        for (int i = 0; i <= layer->getMaxKeyFramePosition(); i++)
        {
            if (layer->keyExists(i))
            {
                VectorImage* vector = layer->getVectorImageAtFrame(i);
                vector->setOpacity(static_cast<qreal>(ui->chooseOpacitySlider->value()) / 500.0);
            }
        }
    }

    mEditor->updateCurrentFrame();
}

void LayerOpacityDialog::selectedKeyframesOpacity()
{
    SignalBlocker b1(ui->chooseOpacitySlider);
    SignalBlocker b2(ui->chooseOpacitySpinBox);
    if (mLayerManager->currentLayer()->type() == Layer::BITMAP)
    {
        LayerBitmap* layer = static_cast<LayerBitmap*>(mLayerManager->currentLayer());
        QList<int> selectedKeys = layer->getSelectedFrameList();
        for (int i = 0; i < selectedKeys.count(); i++)
        {
            BitmapImage* bitmap = layer->getBitmapImageAtFrame(selectedKeys.at(i));
            bitmap->setOpacity(static_cast<qreal>(ui->chooseOpacitySlider->value()) / 500.0);
        }
        int newOpacity = static_cast<int>(layer->getBitmapImageAtFrame(mEditor->currentFrame())->getOpacity() * 500);
        ui->chooseOpacitySlider->setValue(newOpacity);
        ui->chooseOpacitySpinBox->setValue(newOpacity * 0.2);
    }
    else if (mLayerManager->currentLayer()->type() == Layer::VECTOR)
    {
        LayerVector* layer = static_cast<LayerVector*>(mLayerManager->currentLayer());
        QList<int> selectedKeys = layer->getSelectedFrameList();
        for (int i = 0; i < selectedKeys.count(); i++)
        {
            VectorImage* vector = layer->getVectorImageAtFrame(selectedKeys.at(i));
            vector->setOpacity(static_cast<qreal>(ui->chooseOpacitySlider->value()) / 500.0);
        }
        int newOpacity = static_cast<int>(layer->getVectorImageAtFrame(mEditor->currentFrame())->getOpacity() * 500);
        ui->chooseOpacitySlider->setValue(newOpacity);
        ui->chooseOpacitySpinBox->setValue(newOpacity * 0.2);
    }
    mEditor->updateCurrentFrame();
}

void LayerOpacityDialog::fadeInPressed()
{
    SignalBlocker b1(ui->chooseOpacitySlider);
    SignalBlocker b2(ui->chooseOpacitySpinBox);
    if (mLayerManager->currentLayer()->type() == Layer::BITMAP)
    {
        LayerBitmap* layer = static_cast<LayerBitmap*>(mLayerManager->currentLayer());
        QList<int> selectedKeys = layer->getSelectedFrameList();
        if (selectedKeys.count() < 3) { return; }

        BitmapImage* image = layer->getBitmapImageAtFrame(selectedKeys.last());
        if (image == nullptr) { return; }

        qreal endOpacity = image->getOpacity();
        qreal imageCount = static_cast<qreal>(selectedKeys.count());
        for (int i = 0; i < selectedKeys.count() - 1; i++)
        {
            image = layer->getBitmapImageAtFrame(selectedKeys.at(i));
            qreal newOpacity = static_cast<qreal>((i + 1) / imageCount) * endOpacity;
            image->setOpacity(newOpacity);
        }
        int newOpacity = static_cast<int>(layer->getBitmapImageAtFrame(mEditor->currentFrame())->getOpacity() * 500);
        ui->chooseOpacitySlider->setValue(newOpacity);
        ui->chooseOpacitySpinBox->setValue(newOpacity * 0.2);
    }
    else
    {
        LayerVector* layer = static_cast<LayerVector*>(mLayerManager->currentLayer());
        QList<int> selectedKeys = layer->getSelectedFrameList();
        if (selectedKeys.count() < 3) { return; }

        VectorImage* image = layer->getVectorImageAtFrame(selectedKeys.last());
        if (image == nullptr) { return; }

        qreal endOpacity = image->getOpacity();
        qreal imageCount = static_cast<qreal>(selectedKeys.count());
        for (int i = 0; i < selectedKeys.count() - 1; i++)
        {
            image = layer->getVectorImageAtFrame(selectedKeys.at(i));
            qreal newOpacity = static_cast<qreal>((i + 1) / imageCount) * endOpacity;
            image->setOpacity(newOpacity);
        }
        int newOpacity = static_cast<int>(layer->getVectorImageAtFrame(mEditor->currentFrame())->getOpacity() * 500);
        ui->chooseOpacitySlider->setValue(newOpacity);
        ui->chooseOpacitySpinBox->setValue(newOpacity * 0.2);
    }
    mEditor->updateCurrentFrame();
}

void LayerOpacityDialog::fadeOutPressed()
{
    SignalBlocker b1(ui->chooseOpacitySlider);
    SignalBlocker b2(ui->chooseOpacitySpinBox);
    if (mLayerManager->currentLayer()->type() == Layer::BITMAP)
    {
        LayerBitmap* layer = static_cast<LayerBitmap*>(mLayerManager->currentLayer());
        QList<int> selectedKeys = layer->getSelectedFrameList();
        if (selectedKeys.count() < 3) { return; }

        BitmapImage* image = layer->getBitmapImageAtFrame(selectedKeys.first());
        if (image == nullptr) { return; }

        qreal startOpacity = image->getOpacity();
        qreal imageCount = static_cast<qreal>(selectedKeys.count() - 1);
        for (int i = 1; i < selectedKeys.count(); i++)
        {
            image = layer->getBitmapImageAtFrame(selectedKeys.at(i));
            qreal newOpacity = static_cast<qreal>(startOpacity - (i / imageCount) * startOpacity);
            image->setOpacity(newOpacity);
        }
        int newOpacity = static_cast<int>(layer->getBitmapImageAtFrame(mEditor->currentFrame())->getOpacity() * 500);
        ui->chooseOpacitySlider->setValue(newOpacity);
        ui->chooseOpacitySpinBox->setValue(newOpacity * 0.2);
    }
    else if (mLayerManager->currentLayer()->type() == Layer::VECTOR)
    {
        LayerVector* layer = static_cast<LayerVector*>(mLayerManager->currentLayer());
        QList<int> selectedKeys = layer->getSelectedFrameList();
        if (selectedKeys.count() < 3) { return; }

        VectorImage* image = layer->getVectorImageAtFrame(selectedKeys.first());
        if (image == nullptr) { return; }

        qreal startOpacity = image->getOpacity();
        qreal imageCount = static_cast<qreal>(selectedKeys.count() - 1);
        for (int i = 1; i < selectedKeys.count(); i++)
        {
            image = layer->getVectorImageAtFrame(selectedKeys.at(i));
            qreal newOpacity = static_cast<qreal>(startOpacity - (i / imageCount) * startOpacity);
            image->setOpacity(newOpacity);
        }
        int newOpacity = static_cast<int>(layer->getVectorImageAtFrame(mEditor->currentFrame())->getOpacity() * 500);
        ui->chooseOpacitySlider->setValue(newOpacity);
        ui->chooseOpacitySpinBox->setValue(newOpacity * 0.2);
    }

    mEditor->updateCurrentFrame();

}

void LayerOpacityDialog::newFileLoaded()
{
    init();
}

void LayerOpacityDialog::currentLayerChanged(int index)
{
    ui->labLayerInfo->setText(tr("Layer: %1").arg(mLayerManager->getLayer(index)->name()));

    if (mLayerManager->getLayer(index)->type() == Layer::BITMAP ||
            mLayerManager->getLayer(index)->type() == Layer::VECTOR)
    {
        enableDialog();
        currentFrameChanged(mEditor->currentFrame());
    }
    else
    {
        disableDialog();
    }
}

void LayerOpacityDialog::currentFrameChanged(int frame)
{
    if (mPlayerIsPlaying) { return; }

    if (mLayerManager->currentLayer()->type() == Layer::BITMAP ||
            mLayerManager->currentLayer()->type() == Layer::VECTOR)
    {
        // if on a keyframe
        if (mLayerManager->currentLayer()->keyExists(frame))
        {
            enableDialog();
            updateSlider();
            selectedFramesChanged();
        }
        else
            disableDialog();
    }
}

void LayerOpacityDialog::selectedFramesChanged()
{
    QList<int> frames = mLayerManager->currentLayer()->getSelectedFrameList();
    if (!frames.isEmpty() && mEditor->layers()->currentLayer()->keyExists(mEditor->currentFrame()))
        ui->rbSelectedKeyframes->setEnabled(true);
    else
        ui->rbSelectedKeyframes->setEnabled(false);

    if (frames.count() > 2 && mEditor->layers()->currentLayer()->keyExists(mEditor->currentFrame()))
        ui->groupBoxFade->setEnabled(true);
    else
        ui->groupBoxFade->setEnabled(false);
}

void LayerOpacityDialog::playStateChanged(bool isPlaying)
{
     mPlayerIsPlaying = isPlaying;
    if (!mPlayerIsPlaying)
    {
        enableDialog();
        currentFrameChanged(mEditor->currentFrame());
    }
    else
    {
        disableDialog();
    }
}

void LayerOpacityDialog::updateSlider()
{
    if (!mLayerManager->currentLayer()->keyExists(mEditor->currentFrame())) { return; }

    SignalBlocker b1(ui->chooseOpacitySlider);
    SignalBlocker b2(ui->chooseOpacitySpinBox);
    if (mLayerManager->currentLayer()->type() == Layer::BITMAP)
    {
        LayerBitmap* layer = static_cast<LayerBitmap*>(mLayerManager->currentLayer());
        BitmapImage* bitmap = layer->getBitmapImageAtFrame(mEditor->currentFrame());
        ui->chooseOpacitySlider->setValue(static_cast<int>(bitmap->getOpacity() * 500));
        ui->chooseOpacitySpinBox->setValue(ui->chooseOpacitySlider->value() * 0.2);
    }
    else if (mLayerManager->currentLayer()->type() == Layer::VECTOR)
    {
        LayerVector* layer = static_cast<LayerVector*>(mLayerManager->currentLayer());
        VectorImage* vector = layer->getVectorImageAtFrame(mEditor->currentFrame());
        ui->chooseOpacitySlider->setValue(static_cast<int>(vector->getOpacity() * 500));
        ui->chooseOpacitySpinBox->setValue(ui->chooseOpacitySlider->value() * 0.2);
    }
}

void LayerOpacityDialog::opacityValueChanged()
{
    if (ui->rbActiveKeyframe->isChecked())
        setOpacityCurrentKeyframe();
    else if (ui->rbActiveLayer->isChecked())
        setOpacityLayer();
    else if (ui->rbSelectedKeyframes->isChecked())
        setOpacitySelectedKeyframes();
}

void LayerOpacityDialog::setOpacityCurrentKeyframe()
{
    if (mLayerManager->currentLayer()->type() == Layer::BITMAP)
    {
        LayerBitmap* layer = static_cast<LayerBitmap*>(mLayerManager->currentLayer());
        BitmapImage* bitmap = layer->getBitmapImageAtFrame(mEditor->currentFrame());
        bitmap->setOpacity(static_cast<qreal>(ui->chooseOpacitySlider->value()) / 500.0);
        mOpacityChanges = true;
    }
    else if (mLayerManager->currentLayer()->type() == Layer::VECTOR)
    {
        LayerVector* layer = static_cast<LayerVector*>(mLayerManager->currentLayer());
        VectorImage* vector = layer->getVectorImageAtFrame(mEditor->currentFrame());
        vector->setOpacity(static_cast<qreal>(ui->chooseOpacitySlider->value()) / 500.0);
        mOpacityChanges = true;
    }
    mEditor->updateCurrentFrame();
}

void LayerOpacityDialog::setOpacitySelectedKeyframes()
{
    QList<int> frames = mLayerManager->currentLayer()->getSelectedFrameList();
    if (frames.isEmpty()) { return; }

    if (mLayerManager->currentLayer()->type() == Layer::BITMAP)
    {
        LayerBitmap* layer = static_cast<LayerBitmap*>(mLayerManager->currentLayer());
        for (int i = 0; i < frames.count(); i++)
        {
            BitmapImage* bitmap = layer->getBitmapImageAtFrame(frames.at(i));
            bitmap->setOpacity(static_cast<qreal>(ui->chooseOpacitySlider->value()) / 500.0);
            mOpacityChanges = true;
        }
    }
    else if (mLayerManager->currentLayer()->type() == Layer::VECTOR)
    {
        LayerVector* layer = static_cast<LayerVector*>(mLayerManager->currentLayer());
        for (int i = 0; i < frames.count(); i++)
        {
            VectorImage* vector = layer->getVectorImageAtFrame(frames.at(i));
            vector->setOpacity(static_cast<qreal>(ui->chooseOpacitySlider->value()) / 500.0);
            mOpacityChanges = true;
        }
    }
    mEditor->updateCurrentFrame();
}

void LayerOpacityDialog::setOpacityLayer()
{
    if (mLayerManager->currentLayer()->type() == Layer::BITMAP)
    {
        LayerBitmap* layer = static_cast<LayerBitmap*>(mLayerManager->currentLayer());
        for (int i = layer->firstKeyFramePosition(); i <= layer->getMaxKeyFramePosition(); i++)
        {
            if (layer->keyExists(i))
            {
                BitmapImage* bitmap = layer->getBitmapImageAtFrame(i);
                bitmap->setOpacity(static_cast<qreal>(ui->chooseOpacitySlider->value()) / 500.0);
                mOpacityChanges = true;
            }
        }
    }
    else if (mLayerManager->currentLayer()->type() == Layer::VECTOR)
    {
        LayerVector* layer = static_cast<LayerVector*>(mLayerManager->currentLayer());
        for (int i = layer->firstKeyFramePosition(); i <= layer->getMaxKeyFramePosition(); i++)
        {
            if (layer->keyExists(i))
            {
                VectorImage* vector = layer->getVectorImageAtFrame(i);
                vector->setOpacity(static_cast<qreal>(ui->chooseOpacitySlider->value()) / 500.0);
                mOpacityChanges = true;
            }
        }
    }
    mEditor->updateCurrentFrame();
}

void LayerOpacityDialog::enableDialog()
{
    ui->groupBoxOpacity->setEnabled(true);
    ui->groupBoxFade->setEnabled(true);
    ui->chooseOpacitySlider->setEnabled(true);
    ui->chooseOpacitySpinBox->setEnabled(true);
    ui->btnClose->setEnabled(true);
}

void LayerOpacityDialog::disableDialog()
{
    ui->groupBoxOpacity->setEnabled(false);
    ui->groupBoxFade->setEnabled(false);
    ui->chooseOpacitySlider->setEnabled(false);
    ui->chooseOpacitySpinBox->setEnabled(false);
    ui->btnClose->setEnabled(false);
}

void LayerOpacityDialog::closeClicked()
{
    emit closedialog();
    close();
}
