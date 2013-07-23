#include "colorspinboxgroup.h"
#include "ui_colorspinboxgroup.h"

#include <QDebug>

ColorSpinBoxGroup::ColorSpinBoxGroup(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ColorSpinBoxGroup),
    isRgbColors(true),
    noColorUpdate(false)
{
    ui->setupUi(this);
    connect(ui->RedspinBox, SIGNAL(valueChanged(int)),
            this, SLOT(onColorChanged()));
    connect(ui->GreenspinBox, SIGNAL(valueChanged(int)),
            this, SLOT(onColorChanged()));
    connect(ui->BluespinBox, SIGNAL(valueChanged(int)),
            this, SLOT(onColorChanged()));
    connect(ui->rgb, SIGNAL(toggled(bool)),
            this, SLOT(onModeChanged()));
}

ColorSpinBoxGroup::~ColorSpinBoxGroup()
{
    delete ui;
}

void ColorSpinBoxGroup::setColor(const QColor &c)
{
    if(c == color_) return;
    noColorUpdate = true;
    if(isRgbColors){
        ui->RedspinBox->setValue(c.red());
        ui->GreenspinBox->setValue(c.green());
        ui->BluespinBox->setValue(c.blue());
    }else{
        ui->RedspinBox->setValue(
                    qBound(0.0, c.hsvHueF()*359, 359.0)
                    );
        ui->GreenspinBox->setValue(
                    qBound(0.0, c.hsvSaturationF()*100, 100.0)
                    );
        ui->BluespinBox->setValue(
                    qBound(0.0, c.valueF()*100, 100.0)
                    );
    }
    color_ = c;

    QPalette p = ui->label->palette();
    p.setColor(QPalette::Background, color_);
    ui->label->setPalette(p);
    noColorUpdate = false;
}

QColor ColorSpinBoxGroup::color()
{
    return color_;
}

void ColorSpinBoxGroup::onModeChanged()
{
    bool newValue = ui->rgb->isChecked();
    if(isRgbColors == newValue) return;
    isRgbColors = newValue;
    noColorUpdate = true;
    if(!isRgbColors){
        ui->red->setText(tr("Hue"));
        ui->green->setText(tr("Saturation"));
        ui->blue->setText(tr("Value"));

        ui->RedspinBox->setRange(0,359);
        ui->GreenspinBox->setRange(0,100);
        ui->GreenspinBox->setSuffix("%");
        ui->BluespinBox->setRange(0,100);
        ui->BluespinBox->setSuffix("%");
        color_ = color_.toHsv();
        ui->RedspinBox->setValue(
                    qBound(0.0, color_.hsvHueF()*359, 359.0)
                    );
        ui->GreenspinBox->setValue(
                    qBound(0.0, color_.hsvSaturationF()*100, 100.0)
                    );
        ui->BluespinBox->setValue(
                    qBound(0.0, color_.valueF()*100, 100.0)
                    );
    }else{
        ui->red->setText(tr("Red"));
        ui->green->setText(tr("Green"));
        ui->blue->setText(tr("Blue"));

        ui->RedspinBox->setRange(0,255);
        ui->GreenspinBox->setRange(0,255);
        ui->GreenspinBox->setSuffix("");
        ui->BluespinBox->setRange(0,255);
        ui->BluespinBox->setSuffix("");
        color_ = color_.toRgb();
        ui->RedspinBox->setValue(color_.red());
        ui->GreenspinBox->setValue(color_.green());
        ui->BluespinBox->setValue(color_.blue());
    }
    noColorUpdate = false;
    emit modeChange(isRgbColors);
}

void ColorSpinBoxGroup::onColorChanged()
{
    if(noColorUpdate) return;

    QColor c;
    if(isRgbColors){
        c = QColor::fromRgb(ui->RedspinBox->value(),
                            ui->GreenspinBox->value(),
                            ui->BluespinBox->value()
                            );

    }else{
        c = QColor::fromHsvF(qBound(0.0,
                                    ui->RedspinBox->value()/359.0,
                                    1.0),
                             qBound(0.0,
                                    ui->GreenspinBox->value()/100.0,
                                    1.0),
                             qBound(0.0,
                                    ui->BluespinBox->value()/100.0,
                                    1.0)
                             );
    }

    color_ = c;
    emit colorChange(c);
}
