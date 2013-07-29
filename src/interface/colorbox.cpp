#include "colorbox.h"
#include "ui_colorbox.h"

ColorBox::ColorBox(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ColorBox)
{
    ui->setupUi(this);
    connect(ui->colorWheel,SIGNAL(colorChanged(QColor)),
            this, SLOT(onWheelChange(QColor)));
    connect(ui->colorSpinBoxGroup,SIGNAL(colorChange(QColor)),
            this, SLOT(onSpinboxChange(QColor)));
}

ColorBox::~ColorBox()
{
    delete ui;
}

QColor ColorBox::color()
{
    return ui->colorWheel->color();
}

void ColorBox::setColor(const QColor &c)
{
    onSpinboxChange(c);
}

void ColorBox::onSpinboxChange(const QColor &c)
{
    if ( ui->colorWheel->color() != c )
    {
        ui->colorWheel->setColor(c);
        emit colorChanged(c);
    }
}

void ColorBox::onWheelChange(const QColor &c)
{
    if ( ui->colorSpinBoxGroup->color() != c )
    {
        ui->colorSpinBoxGroup->setColor(c);
        emit colorChanged(c);
    }
}
