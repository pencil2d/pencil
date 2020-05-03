#include "layeropacitydialog.h"
#include "ui_layeropacitydialog.h"

#include "layermanager.h"
#include "layer.h"
#include "layerbitmap.h"
#include "bitmapimage.h"
#include "layervector.h"
#include "vectorimage.h"
#include "scribblearea.h"
#include "canvaspainter.h"
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
}

void LayerOpacityDialog::init()
{
    connect(ui->chooseOpacitySlider, &QSlider::valueChanged, this, &LayerOpacityDialog::opacitySliderChanged);
    connect(ui->chooseOpacitySpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &LayerOpacityDialog::opacitySpinboxChanged);
    connect(ui->btnSetOpacityLayer, &QPushButton::pressed, this, &LayerOpacityDialog::allLayerOpacity);
    connect(ui->btnSetOpacitySelection, &QPushButton::pressed, this, &LayerOpacityDialog::selectedKeyframesOpacity);
    connect(ui->btnFadeIn, &QPushButton::pressed, this, &LayerOpacityDialog::fadeInPressed);
    connect(ui->btnFadeOut, &QPushButton::pressed, this, &LayerOpacityDialog::fadeOutPressed);
    connect(ui->btnClose, &QPushButton::pressed, this, &LayerOpacityDialog::close);
    connect(ui->toolBtnPreviousKeyframe, &QToolButton::pressed, this, &LayerOpacityDialog::previousKeyframePressed);
    connect(ui->toolBtnNextKeyframe, &QToolButton::pressed, this, &LayerOpacityDialog::nextKeyframePressed);

    if (mEditor->layers()->currentLayer()->type() == Layer::BITMAP)
        initBitmap();
    else
        initVector();


}

void LayerOpacityDialog::initBitmap()
{
    LayerBitmap* layer = static_cast<LayerBitmap*>(mEditor->layers()->currentLayer());
    BitmapImage* bitmap = layer->getBitmapImageAtFrame(mEditor->currentFrame());
    ui->labLayerInfo->setText(tr("Layer: %1").arg(layer->name()));
    if (layer->getPreviousKeyFramePosition(mEditor->currentFrame()) == mEditor->currentFrame())
        ui->toolBtnPreviousKeyframe->setEnabled(false);
    if (layer->getNextKeyFramePosition(mEditor->currentFrame()) == mEditor->currentFrame())
        ui->toolBtnNextKeyframe->setEnabled(false);
    if (layer->getSelectedFrameList().count() < 3)
    {
        ui->btnFadeIn->setEnabled(false);
        ui->btnFadeOut->setEnabled(false);
    }
    if (layer->getSelectedFrameList().isEmpty())
        ui->btnSetOpacitySelection->setEnabled(false);
    ui->chooseOpacitySlider->setValue(static_cast<int>(bitmap->getOpacity() * 255));
}

void LayerOpacityDialog::initVector()
{
    LayerVector* layer = static_cast<LayerVector*>(mEditor->layers()->currentLayer());
    VectorImage* vector = layer->getVectorImageAtFrame(mEditor->currentFrame());
    ui->labLayerInfo->setText(tr("Layer: %1").arg(layer->name()));
    if (layer->getPreviousKeyFramePosition(mEditor->currentFrame()) == mEditor->currentFrame())
        ui->toolBtnPreviousKeyframe->setEnabled(false);
    if (layer->getNextKeyFramePosition(mEditor->currentFrame()) == mEditor->currentFrame())
        ui->toolBtnNextKeyframe->setEnabled(false);
    if (layer->getSelectedFrameList().count() < 3)
    {
        ui->btnFadeIn->setEnabled(false);
        ui->btnFadeOut->setEnabled(false);
    }
    if (layer->getSelectedFrameList().isEmpty())
        ui->btnSetOpacitySelection->setEnabled(false);
    ui->chooseOpacitySlider->setValue(static_cast<int>(vector->getOpacity() * 255));
}

void LayerOpacityDialog::previousKeyframePressed()
{
    if (mEditor->layers()->currentLayer()->type() == Layer::BITMAP)
    {
        LayerBitmap* layer = static_cast<LayerBitmap*>(mEditor->layers()->currentLayer());
        mEditor->scrubPreviousKeyFrame();

        BitmapImage* bitmap = layer->getBitmapImageAtFrame(mEditor->currentFrame());
        ui->chooseOpacitySlider->setValue(static_cast<int>(bitmap->getOpacity() * 255));
    }
    else if (mEditor->layers()->currentLayer()->type() == Layer::VECTOR)
    {
        LayerVector* layer = static_cast<LayerVector*>(mEditor->layers()->currentLayer());
        mEditor->scrubPreviousKeyFrame();

        VectorImage* vector = layer->getVectorImageAtFrame(mEditor->currentFrame());
        ui->chooseOpacitySlider->setValue(static_cast<int>(vector->getOpacity() * 255));
    }

    if (mEditor->layers()->currentLayer()->getPreviousKeyFramePosition(mEditor->currentFrame()) == mEditor->currentFrame())
        ui->toolBtnPreviousKeyframe->setEnabled(false);

    ui->toolBtnNextKeyframe->setEnabled(true);
}

void LayerOpacityDialog::nextKeyframePressed()
{
    if (mEditor->layers()->currentLayer()->type() == Layer::BITMAP)
    {
        LayerBitmap* layer = static_cast<LayerBitmap*>(mEditor->layers()->currentLayer());
        mEditor->scrubNextKeyFrame();

        BitmapImage* bitmap = layer->getBitmapImageAtFrame(mEditor->currentFrame());
        ui->chooseOpacitySlider->setValue(static_cast<int>(bitmap->getOpacity() * 255));
    }
    else if (mEditor->layers()->currentLayer()->type() == Layer::VECTOR)
    {
        LayerVector* layer = static_cast<LayerVector*>(mEditor->layers()->currentLayer());
        mEditor->scrubNextKeyFrame();

        VectorImage* vector = layer->getVectorImageAtFrame(mEditor->currentFrame());
        ui->chooseOpacitySlider->setValue(static_cast<int>(vector->getOpacity() * 255));
    }

    if (mEditor->layers()->currentLayer()->getNextKeyFramePosition(mEditor->currentFrame()) == mEditor->currentFrame())
        ui->toolBtnNextKeyframe->setEnabled(false);

    ui->toolBtnPreviousKeyframe->setEnabled(true);
}

void LayerOpacityDialog::opacitySliderChanged(int value)
{
    ui->chooseOpacitySpinBox->setValue(value);
    setOpacityCurrentKeyframe();
}

void LayerOpacityDialog::opacitySpinboxChanged(int value)
{
    ui->chooseOpacitySlider->setValue(value);
    setOpacityCurrentKeyframe();
}

void LayerOpacityDialog::allLayerOpacity()
{
    if (mEditor->layers()->currentLayer()->type() == Layer::BITMAP)
    {
        LayerBitmap* layer = static_cast<LayerBitmap*>(mEditor->layers()->currentLayer());
        for (int i = 0; i <= layer->getMaxKeyFramePosition(); i++)
        {
            if (layer->keyExists(i))
            {
                BitmapImage* bitmap = layer->getBitmapImageAtFrame(i);
                bitmap->setOpacity(static_cast<qreal>(ui->chooseOpacitySlider->value() / 255.0));
            }
        }
    }
    else if (mEditor->layers()->currentLayer()->type() == Layer::VECTOR)
    {
        LayerVector* layer = static_cast<LayerVector*>(mEditor->layers()->currentLayer());
        for (int i = 0; i <= layer->getMaxKeyFramePosition(); i++)
        {
            if (layer->keyExists(i))
            {
                VectorImage* vector = layer->getVectorImageAtFrame(i);
                vector->setOpacity(static_cast<qreal>(ui->chooseOpacitySlider->value() / 255.0));
            }
        }
    }

    mEditor->updateCurrentFrame();
}

void LayerOpacityDialog::selectedKeyframesOpacity()
{
    if (mEditor->layers()->currentLayer()->type() == Layer::BITMAP)
    {
        LayerBitmap* layer = static_cast<LayerBitmap*>(mEditor->layers()->currentLayer());
        QList<int> selectedKeys = layer->getSelectedFrameList();
        for (int i = 0; i < selectedKeys.count(); i++)
        {
            BitmapImage* bitmap = layer->getBitmapImageAtFrame(selectedKeys.at(i));
            bitmap->setOpacity(static_cast<qreal>(ui->chooseOpacitySlider->value() / 255.0));
        }
        int newOpacity = static_cast<int>(layer->getBitmapImageAtFrame(mEditor->currentFrame())->getOpacity() * 255);
        ui->chooseOpacitySlider->setValue(newOpacity);
    }
    else if (mEditor->layers()->currentLayer()->type() == Layer::VECTOR)
    {
        LayerVector* layer = static_cast<LayerVector*>(mEditor->layers()->currentLayer());
        QList<int> selectedKeys = layer->getSelectedFrameList();
        for (int i = 0; i < selectedKeys.count(); i++)
        {
            VectorImage* vector = layer->getVectorImageAtFrame(selectedKeys.at(i));
            vector->setOpacity(static_cast<qreal>(ui->chooseOpacitySlider->value() / 255.0));
        }
        int newOpacity = static_cast<int>(layer->getVectorImageAtFrame(mEditor->currentFrame())->getOpacity() * 255);
        ui->chooseOpacitySlider->setValue(newOpacity);
    }
    mEditor->updateCurrentFrame();
}

void LayerOpacityDialog::fadeInPressed()
{
    if (mEditor->layers()->currentLayer()->type() == Layer::BITMAP)
    {
        LayerBitmap* layer = static_cast<LayerBitmap*>(mEditor->layers()->currentLayer());
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
            image->modification();
        }
        int newOpacity = static_cast<int>(layer->getBitmapImageAtFrame(mEditor->currentFrame())->getOpacity() * 255);
        ui->chooseOpacitySlider->setValue(newOpacity);
    }
    else
    {
        LayerVector* layer = static_cast<LayerVector*>(mEditor->layers()->currentLayer());
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
            image->modification();
        }
        int newOpacity = static_cast<int>(layer->getVectorImageAtFrame(mEditor->currentFrame())->getOpacity() * 255);
        ui->chooseOpacitySlider->setValue(newOpacity);
    }
    mEditor->updateCurrentFrame();
}

void LayerOpacityDialog::fadeOutPressed()
{
    if (mEditor->layers()->currentLayer()->type() == Layer::BITMAP)
    {
        LayerBitmap* layer = static_cast<LayerBitmap*>(mEditor->layers()->currentLayer());
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
            image->modification();
        }
        int newOpacity = static_cast<int>(layer->getBitmapImageAtFrame(mEditor->currentFrame())->getOpacity() * 255);
        ui->chooseOpacitySlider->setValue(newOpacity);
    }
    else if (mEditor->layers()->currentLayer()->type() == Layer::VECTOR)
    {
        LayerVector* layer = static_cast<LayerVector*>(mEditor->layers()->currentLayer());
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
            image->modification();
        }
        int newOpacity = static_cast<int>(layer->getVectorImageAtFrame(mEditor->currentFrame())->getOpacity() * 255);
        ui->chooseOpacitySlider->setValue(newOpacity);
    }

    mEditor->updateCurrentFrame();

}

void LayerOpacityDialog::setOpacityCurrentKeyframe()
{
    if (mEditor->layers()->currentLayer()->type() == Layer::BITMAP)
    {
        LayerBitmap* layer = static_cast<LayerBitmap*>(mEditor->layers()->currentLayer());
        BitmapImage* bitmap = layer->getBitmapImageAtFrame(mEditor->currentFrame());
        bitmap->setOpacity(static_cast<qreal>(ui->chooseOpacitySlider->value() / 255.0));
    }
    else if (mEditor->layers()->currentLayer()->type() == Layer::VECTOR)
    {
        LayerVector* layer = static_cast<LayerVector*>(mEditor->layers()->currentLayer());
        VectorImage* vector = layer->getVectorImageAtFrame(mEditor->currentFrame());
        vector->setOpacity(static_cast<qreal>(ui->chooseOpacitySlider->value() / 255.0));
    }
    mEditor->updateCurrentFrame();
}
