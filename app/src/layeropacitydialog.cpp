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
    mTimer = new QTimer();
    mTimer->setTimerType(Qt::PreciseTimer);
    connect(mTimer, &QTimer::timeout, this, &LayerOpacityDialog::setOpacityCurrentKeyframe);
    connect(ui->chooseOpacitySlider, &QSlider::valueChanged, this, &LayerOpacityDialog::opacitySliderChanged);
    connect(ui->chooseOpacitySpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &LayerOpacityDialog::opacitySpinboxChanged);
    connect(ui->btnClose, &QPushButton::pressed, this, &LayerOpacityDialog::close);
    connect(ui->btnSetOpacityLayer, &QPushButton::pressed, this, &LayerOpacityDialog::allLayerOpacity);
    connect(ui->toolBtnPreviousKeyframe, &QToolButton::pressed, this, &LayerOpacityDialog::previousKeyframePressed);
    connect(ui->toolBtnNextKeyframe, &QToolButton::pressed, this, &LayerOpacityDialog::nextKeyframePressed);
    LayerBitmap* layer = static_cast<LayerBitmap*>(mEditor->layers()->currentLayer());
    BitmapImage* bitmap = layer->getBitmapImageAtFrame(mEditor->currentFrame());
    if (layer->getPreviousKeyFramePosition(mEditor->currentFrame()) == mEditor->currentFrame())
        ui->toolBtnPreviousKeyframe->setEnabled(false);
    if (layer->getNextKeyFramePosition(mEditor->currentFrame()) == mEditor->currentFrame())
        ui->toolBtnNextKeyframe->setEnabled(false);
    if (layer->getSelectedFrameList().count() < 2)
    {
        ui->btnFadeIn->setEnabled(false);
        ui->btnFadeOut->setEnabled(false);
    }
    ui->chooseOpacitySlider->setValue(bitmap->getOpacity() * 255);
}

void LayerOpacityDialog::previousKeyframePressed()
{
    LayerBitmap* layer = static_cast<LayerBitmap*>(mEditor->layers()->currentLayer());
    mEditor->scrubPreviousKeyFrame();

    BitmapImage* bitmap = layer->getBitmapImageAtFrame(mEditor->currentFrame());
    ui->chooseOpacitySlider->setValue(bitmap->getOpacity() * 255);

    if (layer->getPreviousKeyFramePosition(mEditor->currentFrame()) == mEditor->currentFrame())
        ui->toolBtnPreviousKeyframe->setEnabled(false);

    ui->toolBtnNextKeyframe->setEnabled(true);
}

void LayerOpacityDialog::nextKeyframePressed()
{
    LayerBitmap* layer = static_cast<LayerBitmap*>(mEditor->layers()->currentLayer());
    mEditor->scrubNextKeyFrame();

    BitmapImage* bitmap = layer->getBitmapImageAtFrame(mEditor->currentFrame());
    ui->chooseOpacitySlider->setValue(bitmap->getOpacity() * 255);

    if (layer->getNextKeyFramePosition(mEditor->currentFrame()) == mEditor->currentFrame())
        ui->toolBtnNextKeyframe->setEnabled(false);

    ui->toolBtnPreviousKeyframe->setEnabled(true);
}

void LayerOpacityDialog::opacitySliderChanged(int value)
{
    mTimer->stop();
    ui->chooseOpacitySpinBox->setValue(value);
    mTimer->start(mTimeOut);
}

void LayerOpacityDialog::opacitySpinboxChanged(int value)
{
    mTimer->stop();
    ui->chooseOpacitySlider->setValue(value);
    mTimer->start(mTimeOut);
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

void LayerOpacityDialog::fadeInPressed()
{

}

void LayerOpacityDialog::fadeOutPressed()
{

}

void LayerOpacityDialog::setOpacityCurrentKeyframe()
{
    mTimer->stop();
    LayerBitmap* layer = static_cast<LayerBitmap*>(mEditor->layers()->currentLayer());
    BitmapImage* bitmap = layer->getBitmapImageAtFrame(mEditor->currentFrame());

    bitmap->setOpacity(static_cast<qreal>(ui->chooseOpacitySlider->value() / 255.0));
    mEditor->updateCurrentFrame();
}
