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

void ColorSpinBoxGroup::setColor(const QColor &newColor)
{
    if (newColor == m_color)
    {
        return;
    }
    noColorUpdate = true;

    if(isRgbColors)
    {
        ui->RedspinBox->setValue(newColor.red());
        ui->GreenspinBox->setValue(newColor.green());
        ui->BluespinBox->setValue(newColor.blue());
    }
    else
    {
        ui->RedspinBox->setValue( qBound(0.0, newColor.hsvHueF() * 359, 359.0) );
        ui->GreenspinBox->setValue( qBound(0.0, newColor.hsvSaturationF() * 100, 100.0) );
        ui->BluespinBox->setValue( qBound(0.0, newColor.valueF() * 100, 100.0) );
    }
    m_color = newColor;

    QPalette p = ui->label->palette();
    p.setColor(QPalette::Background, m_color);
    ui->label->setPalette(p);
    noColorUpdate = false;
}

QColor ColorSpinBoxGroup::color()
{
    return m_color;
}

void ColorSpinBoxGroup::onModeChanged()
{
    bool newValue = ui->rgb->isChecked();
    if (isRgbColors == newValue)
    {
        return;
    }

    isRgbColors = newValue;
    noColorUpdate = true;

    if (!isRgbColors)
    {
        ui->red->setText(tr("Hue"));
        ui->green->setText(tr("Saturation"));
        ui->blue->setText(tr("Value"));

        ui->RedspinBox->setRange(0,359);
        ui->GreenspinBox->setRange(0,100);
        ui->GreenspinBox->setSuffix("%");
        ui->BluespinBox->setRange(0,100);
        ui->BluespinBox->setSuffix("%");

        m_color = m_color.toHsv();
        ui->RedspinBox->setValue( qBound(0.0, m_color.hsvHueF()*359, 359.0) );
        ui->GreenspinBox->setValue( qBound(0.0, m_color.hsvSaturationF()*100, 100.0) );
        ui->BluespinBox->setValue( qBound(0.0, m_color.valueF()*100, 100.0) );
    }
    else
    {
        ui->red->setText(tr("Red"));
        ui->green->setText(tr("Green"));
        ui->blue->setText(tr("Blue"));

        ui->RedspinBox->setRange(0,255);
        ui->GreenspinBox->setRange(0,255);
        ui->GreenspinBox->setSuffix("");
        ui->BluespinBox->setRange(0,255);
        ui->BluespinBox->setSuffix("");
        m_color = m_color.toRgb();
        ui->RedspinBox->setValue(m_color.red());
        ui->GreenspinBox->setValue(m_color.green());
        ui->BluespinBox->setValue(m_color.blue());
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

    m_color = c;
    emit colorChanged(c);
}
