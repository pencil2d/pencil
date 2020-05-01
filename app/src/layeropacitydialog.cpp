#include "layeropacitydialog.h"
#include "ui_layeropacitydialog.h"

#include "layermanager.h"
#include "layer.h"

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
    for (int i = mEditor->layers()->count() -1; i >= 0; i--)
    {
        if (mEditor->layers()->getLayer(i)->type() == Layer::BITMAP)
        {
            ui->layerComboBox->addItem(mEditor->layers()->getLayer(i)->name());
        }
    }
    connect(ui->chooseOpacitySlider, &QSlider::valueChanged, this, &LayerOpacityDialog::opacitySliderChanged);
    connect(ui->chooseOpacitySpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &LayerOpacityDialog::opacitySpinboxChanged);
    connect(ui->btnClose, &QPushButton::pressed, this, &LayerOpacityDialog::close);
}

void LayerOpacityDialog::opacitySliderChanged(int value)
{
    ui->chooseOpacitySpinBox->setValue(value);
}

void LayerOpacityDialog::opacitySpinboxChanged(int value)
{
    ui->chooseOpacitySlider->setValue(value);
}
