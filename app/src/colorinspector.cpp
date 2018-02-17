/*

Pencil - Traditional Animation Software
Copyright (C) 2012-2018 Matthew Chiawen Chang

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
    ui(new Ui::ColorInspector)
{
    ui->setupUi(this);

    auto spinBoxChanged = static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged);
    connect(ui->RedspinBox, spinBoxChanged, this, &ColorInspector::onColorChanged);
    connect(ui->GreenspinBox, spinBoxChanged, this, &ColorInspector::onColorChanged);
    connect(ui->BluespinBox, spinBoxChanged, this, &ColorInspector::onColorChanged);
    connect(ui->AlphaspinBox, spinBoxChanged, this, &ColorInspector::onColorChanged);
    connect(ui->rgb, &QRadioButton::toggled, this, &ColorInspector::onModeChanged);
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
        ui->RedspinBox->setValue(newColor.hsvHue());
        ui->GreenspinBox->setValue(qRound(newColor.hsvSaturation() / 2.55));
        ui->BluespinBox->setValue(qRound(newColor.value() / 2.55));
        ui->AlphaspinBox->setValue(qRound(newColor.alpha() / 2.55));
    }
    m_color = newColor;

    QPalette p1 = ui->colorWrapper->palette(), p2 = ui->color->palette();
    p1.setBrush(QPalette::Background, QBrush(QImage(":/background/checkerboard.png")));
    p2.setColor(QPalette::Background, m_color);
    ui->colorWrapper->setPalette(p1);
    ui->color->setPalette(p2);
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

    if (isRgbColors)
    {
        ui->red->setText(tr("Red"));
        ui->green->setText(tr("Green"));
        ui->blue->setText(tr("Blue"));
        ui->alpha->setText(tr("Alpha"));

        ui->RedspinBox->setRange(0,255);
        ui->RedspinBox->setSuffix("");
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
    else
    {
        ui->red->setText(tr("Hue"));
        ui->green->setText(tr("Saturation"));
        ui->blue->setText(tr("Value"));
        ui->alpha->setText(tr("Alpha"));

        ui->RedspinBox->setRange(0,359);
        ui->RedspinBox->setSuffix("°");
        ui->GreenspinBox->setRange(0,100);
        ui->GreenspinBox->setSuffix("%");
        ui->BluespinBox->setRange(0,100);
        ui->BluespinBox->setSuffix("%");
        ui->AlphaspinBox->setRange(0,100);
        ui->AlphaspinBox->setSuffix("%");

        m_color = m_color.toHsv();
        ui->RedspinBox->setValue(m_color.hue());
        ui->GreenspinBox->setValue(m_color.saturation());
        ui->BluespinBox->setValue(m_color.value());
        ui->AlphaspinBox->setValue(m_color.alpha());
    }
    noColorUpdate = false;
    emit modeChange(isRgbColors);
}

void ColorInspector::onColorChanged()
{
    if(noColorUpdate) return;

    QColor c;
    if (isRgbColors) {
        c = QColor::fromRgb(
            ui->RedspinBox->value(),
            ui->GreenspinBox->value(),
            ui->BluespinBox->value(),
            ui->AlphaspinBox->value());

    } else {
        c = QColor::fromHsv(
            ui->RedspinBox->value(),
            ui->GreenspinBox->value() * 2.555,
            ui->BluespinBox->value() * 2.555,
            ui->AlphaspinBox->value() * 2.555);
    }

    emit colorChanged(c);
}
