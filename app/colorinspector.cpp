/*

Pencil - Traditional Animation Software
Copyright (C) 2012-2017 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/
#include "colorinspector.h"
#include "ui_colorinspector.h"

#include <QDebug>

ColorInspector::ColorInspector(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ColorInspector),
    isRgbColors(true),
    noColorUpdate(false)
{
    ui->setupUi(this);

    //We constrain the widget, as the layout should have a stretch limit.
    parent->setMaximumSize(500,height());

    connect(ui->RedspinBox, SIGNAL(valueChanged(int)),
        this, SLOT(onColorChanged()));
    connect(ui->GreenspinBox, SIGNAL(valueChanged(int)),
        this, SLOT(onColorChanged()));
    connect(ui->BluespinBox, SIGNAL(valueChanged(int)),
        this, SLOT(onColorChanged()));
    connect(ui->AlphaspinBox, SIGNAL(valueChanged(int)),
        this, SLOT(onColorChanged()));
    connect(ui->rgb, SIGNAL(toggled(bool)),
        this, SLOT(onModeChanged()));
}

ColorInspector::~ColorInspector()
{
    delete ui;
}

void ColorInspector::setColor(const QColor &newColor)
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
        ui->AlphaspinBox->setValue(newColor.alpha());
    }
    else
    {
        ui->RedspinBox->setValue( qBound(0.0, newColor.hsvHueF() * 359, 359.0) );
        ui->GreenspinBox->setValue( qBound(0.0, newColor.hsvSaturationF() * 100, 100.0) );
        ui->BluespinBox->setValue( qBound(0.0, newColor.valueF() * 100, 100.0) );
        ui->AlphaspinBox->setValue( qBound(0.0, newColor.alphaF() * 100, 100.0) );
    }
    m_color = newColor;

    QPalette p1 = ui->colorWrapper->palette(), p2 = ui->color->palette();
    p1.setBrush(QPalette::Background, QBrush(QImage(":/background/checkerboard.png")));
    p2.setColor(QPalette::Background, m_color);
    ui->colorWrapper->setPalette(p1);
    ui->color->setPalette(p2);
    //ui->color->setFixedSize(30,30);
    noColorUpdate = false;
}

QColor ColorInspector::color()
{
    return m_color;
}

void ColorInspector::onModeChanged()
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
        ui->alpha->setText(tr("Alpha"));

        ui->RedspinBox->setRange(0,359);
        ui->GreenspinBox->setRange(0,100);
        ui->GreenspinBox->setSuffix("%");
        ui->BluespinBox->setRange(0,100);
        ui->BluespinBox->setSuffix("%");
        ui->AlphaspinBox->setRange(0,100);
        ui->AlphaspinBox->setSuffix("%");

        m_color = m_color.toHsv();
        ui->RedspinBox->setValue( qBound(0.0, m_color.hsvHueF()*359, 359.0) );
        ui->GreenspinBox->setValue( qBound(0.0, m_color.hsvSaturationF()*100, 100.0) );
        ui->BluespinBox->setValue( qBound(0.0, m_color.valueF()*100, 100.0) );
        ui->AlphaspinBox->setValue( qBound(0.0, m_color.alphaF()*100, 100.0) );
    }
    else
    {
        ui->red->setText(tr("Red"));
        ui->green->setText(tr("Green"));
        ui->blue->setText(tr("Blue"));
        ui->alpha->setText(tr("Alpha"));

        ui->RedspinBox->setRange(0,255);
        ui->GreenspinBox->setRange(0,255);
        ui->GreenspinBox->setSuffix("");
        ui->BluespinBox->setRange(0,255);
        ui->BluespinBox->setSuffix("");
        ui->AlphaspinBox->setRange(0,255);
        ui->AlphaspinBox->setSuffix("");
        m_color = m_color.toRgb();
        ui->RedspinBox->setValue(m_color.red());
        ui->GreenspinBox->setValue(m_color.green());
        ui->BluespinBox->setValue(m_color.blue());
        ui->AlphaspinBox->setValue(m_color.alpha());
    }
    noColorUpdate = false;
    emit modeChange(isRgbColors);
}

void ColorInspector::onColorChanged()
{
    if(noColorUpdate) return;

    QColor c;
    if(isRgbColors){
        c = QColor::fromRgb(ui->RedspinBox->value(),
            ui->GreenspinBox->value(),
            ui->BluespinBox->value(),
            ui->AlphaspinBox->value()
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
            1.0),
            qBound(0.0,
            ui->AlphaspinBox->value()/100.0,
            1.0)
            );
    }

    setColor(c);
    emit colorChanged(c);
}
