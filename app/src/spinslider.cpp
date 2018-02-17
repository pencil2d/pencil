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

#include <cmath>
#include <QLabel>
#include <QSlider>
#include <QGridLayout>
#include <QLocale>
#include <QDebug>
#include <QStyle>


SpinSlider::SpinSlider(QWidget* parent) : QWidget(parent)
{}

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

    mLabel = new QLabel(text + ": ");

    mSlider = new QSlider(Qt::Horizontal, this);
    mSlider->setMinimum(0);
    mSlider->setMaximum(100);
    mSlider->setMaximumWidth(500);

    QGridLayout* layout = new QGridLayout();
    layout->setMargin(2);
    layout->setSpacing(2);

    layout->addWidget(mLabel, 0, 0, 1, 1);
    layout->addWidget(mSlider, 1, 0, 1, 2);

    setLayout(layout);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    connect(mSlider, &QSlider::valueChanged, this, &SpinSlider::onSliderValueChanged);
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
        value2 = mMin + v * (mMax - mMin) / mSlider->maximum();
    }
    else if (mGrowthType == LOG)
    {
        value2 = mMin * std::exp(v * std::log(mMax / mMin) / mSlider->maximum());
    }
    else if (mGrowthType == EXPONENT) {
        value2 = mMin + std::pow(v, mExp) * (mMax - mMin) / std::pow(mSlider->maximum(), mExp);
    }
    changeValue(value2);
}

void SpinSlider::setLabel(QString newText)
{
    mLabel->setText(newText);
}

void SpinSlider::setValue(qreal v)
{
    int value2 = 0;
    if (mGrowthType == LINEAR)
    {
        value2 = std::round(mSlider->maximum() * (v - mMin) / (mMax - mMin));
    }
    else if (mGrowthType == LOG)
    {
        value2 = std::round(std::log(v / mMin) * mSlider->maximum() / std::log(mMax / mMin));
    }
    else if (mGrowthType == EXPONENT)
    {
        value2 = std::round(std::pow((v - mMin) * std::pow(mSlider->maximum(), mExp) / (mMax - mMin), 1 / mExp));
    }

    changeValue(v);
    mSlider->setSliderPosition(value2);
}

void SpinSlider::setPixelPos(qreal min, qreal max, int val, int space, bool upsideDown)
{
    mSlider->setSliderPosition(QStyle::sliderValueFromPosition(min, max, val, space, upsideDown));
}

void SpinSlider::setExponent(const qreal exp)
{
    mExp = exp;
}
