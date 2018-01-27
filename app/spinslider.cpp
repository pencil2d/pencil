/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2012-2018 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/

#include "spinslider.h"
#include "ui_spinslider.h"

#include <cmath>
#include <QLabel>
#include <QSlider>
#include <QGridLayout>
#include <QLocale>
#include <QDebug>
#include <QStyle>


SpinSlider::SpinSlider(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::SpinSlider)
{
    ui->setupUi(this);
}

SpinSlider::~SpinSlider()
{
    delete ui;
}

void SpinSlider::init(QString text, GROWTH_TYPE type, VALUE_TYPE dataType, qreal min, qreal max)
{
    if (type == LOG)
    {
        // important! dividing by zero is not acceptable.
        Q_ASSERT_X(min > 0.f, "SpinSlider", "Real type value must larger than 0!!");
    }

    mValue = 1.0;
    mGrowthType = type;
    mValueType = dataType;
    mMin = min;
    mMax = max;

    ui->label->setText(text + ": ");
}

void SpinSlider::changeValue(qreal value)
{
    mValue = value;
    emit valueChanged(value);
}

void SpinSlider::onSliderValueChanged(int v)
{
    qreal value2 = 0.0;
    if (mGrowthType == LINEAR)
    {
        value2 = mMin + v * (mMax - mMin) / ui->slider->maximum();
    }
    else if (mGrowthType == LOG)
    {
        value2 = mMin * std::exp(v * std::log(mMax / mMin) / ui->slider->maximum());
    }
    else if (mGrowthType == EXPONENT) {
        value2 = mMin + std::pow(v, mExp) * (mMax - mMin) / std::pow(ui->slider->maximum(), mExp);
    }
    changeValue(value2);
}

void SpinSlider::setLabel(QString newText)
{
    ui->label->setText(newText);
}

void SpinSlider::setValue(qreal v)
{
    int value2 = 0;
    if (mGrowthType == LINEAR)
    {
        value2 = std::round(ui->slider->maximum() * (v - mMin) / (mMax - mMin));
    }
    else if (mGrowthType == LOG)
    {
        value2 = std::round(std::log(v / mMin) * ui->slider->maximum() / std::log(mMax / mMin));
    }
    else if (mGrowthType == EXPONENT)
    {
        value2 = std::round(std::pow((v - mMin) * std::pow(ui->slider->maximum(), mExp) / (mMax - mMin), 1 / mExp));
    }

    changeValue(v);
    ui->slider->setSliderPosition(value2);
}

void SpinSlider::setPixelPos(qreal min, qreal max, int val, int space, bool upsideDown)
{
    ui->slider->setSliderPosition(QStyle::sliderValueFromPosition(min, max, val, space, upsideDown));
}

void SpinSlider::setExponent(const qreal exp)
{
    mExp = exp;
}
