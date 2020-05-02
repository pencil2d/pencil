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
    connect(mTimer, &QTimer::timeout, this, &LayerOpacityDialog::setBitmapOpacity);
    connect(ui->chooseOpacitySlider, &QSlider::valueChanged, this, &LayerOpacityDialog::opacitySliderChanged);
    connect(ui->chooseOpacitySpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &LayerOpacityDialog::opacitySpinboxChanged);
    connect(ui->btnClose, &QPushButton::pressed, this, &LayerOpacityDialog::close);
    LayerBitmap* layer = static_cast<LayerBitmap*>(mEditor->layers()->currentLayer());
    BitmapImage* bitmap = layer->getBitmapImageAtFrame(mEditor->currentFrame());
    ui->chooseOpacitySlider->setValue(bitmap->getOpacity() * 255);
}

void LayerOpacityDialog::opacitySliderChanged(int value)
{
    mTimer->stop();
    ui->chooseOpacitySpinBox->setValue(value);
    mTimer->start(300);
}

void LayerOpacityDialog::opacitySpinboxChanged(int value)
{
    mTimer->stop();
    ui->chooseOpacitySlider->setValue(value);
    mTimer->start(300);
}

void LayerOpacityDialog::fadeInPressed()
{

}

void LayerOpacityDialog::fadeOutPressed()
{

}

void LayerOpacityDialog::setBitmapOpacity()
{
    mTimer->stop();
    if (mEditor->layers()->currentLayer()->type() == Layer::BITMAP &&
            mEditor->layers()->currentLayer()->keyExists(mEditor->currentFrame()))
    {
        LayerBitmap* layer = static_cast<LayerBitmap*>(mEditor->layers()->currentLayer());
        BitmapImage* bitmap = layer->getBitmapImageAtFrame(mEditor->currentFrame());

        bitmap->setOpacity(static_cast<qreal>(ui->chooseOpacitySlider->value() / 255.0));
        mEditor->updateCurrentFrame();
    }
}
