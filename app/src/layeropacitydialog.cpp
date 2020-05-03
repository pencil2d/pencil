#include "layeropacitydialog.h"
#include "ui_layeropacitydialog.h"

#include "layermanager.h"
#include "layer.h"
#include "layerbitmap.h"
#include "bitmapimage.h"
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

void LayerOpacityDialog::previousKeyframePressed()
{
    LayerBitmap* layer = static_cast<LayerBitmap*>(mEditor->layers()->currentLayer());
    mEditor->scrubPreviousKeyFrame();

    BitmapImage* bitmap = layer->getBitmapImageAtFrame(mEditor->currentFrame());
    ui->chooseOpacitySlider->setValue(static_cast<int>(bitmap->getOpacity() * 255));

    if (layer->getPreviousKeyFramePosition(mEditor->currentFrame()) == mEditor->currentFrame())
        ui->toolBtnPreviousKeyframe->setEnabled(false);

    ui->toolBtnNextKeyframe->setEnabled(true);
}

void LayerOpacityDialog::nextKeyframePressed()
{
    LayerBitmap* layer = static_cast<LayerBitmap*>(mEditor->layers()->currentLayer());
    mEditor->scrubNextKeyFrame();

    BitmapImage* bitmap = layer->getBitmapImageAtFrame(mEditor->currentFrame());
    ui->chooseOpacitySlider->setValue(static_cast<int>(bitmap->getOpacity() * 255));

    if (layer->getNextKeyFramePosition(mEditor->currentFrame()) == mEditor->currentFrame())
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
    LayerBitmap* layer = static_cast<LayerBitmap*>(mEditor->layers()->currentLayer());
    for (int i = 0; i <= layer->getMaxKeyFramePosition(); i++)
    {
        if (layer->keyExists(i))
        {
            BitmapImage* bitmap = layer->getBitmapImageAtFrame(i);
            bitmap->setOpacity(static_cast<qreal>(ui->chooseOpacitySlider->value() / 255.0));
        }
    }
    mEditor->updateCurrentFrame();
}

void LayerOpacityDialog::selectedKeyframesOpacity()
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
    mEditor->updateCurrentFrame();
}

void LayerOpacityDialog::fadeInPressed()
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
    mEditor->updateCurrentFrame();
}

void LayerOpacityDialog::fadeOutPressed()
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
    mEditor->updateCurrentFrame();

}

void LayerOpacityDialog::setOpacityCurrentKeyframe()
{
    LayerBitmap* layer = static_cast<LayerBitmap*>(mEditor->layers()->currentLayer());
    BitmapImage* bitmap = layer->getBitmapImageAtFrame(mEditor->currentFrame());

    bitmap->setOpacity(static_cast<qreal>(ui->chooseOpacitySlider->value() / 255.0));
    mEditor->updateCurrentFrame();
}
