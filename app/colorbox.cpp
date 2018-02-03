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

#include <QVBoxLayout>
#include "colorwheel.h"
#include "colorinspector.h"
#include "colorbox.h"

ColorBox::ColorBox( QWidget* parent ) : BaseDockWidget( parent )
{
    setWindowTitle(tr("Color Wheel", "Color Wheel's window title"));

    mColorWheel = new ColorWheel(this);
    mColorInspector = new ColorInspector(this);

    QVBoxLayout* layout = new QVBoxLayout();
    layout->setContentsMargins(5, 5, 5, 5);
    layout->addWidget(mColorWheel);
    layout->addWidget(mColorInspector);
    layout->setStretch(0, 1);
    layout->setStretch(1, 0);
    QWidget* mainWidget = new QWidget;
    mainWidget->setLayout(layout);
    setWidget(mainWidget);

    connect(mColorWheel, &ColorWheel::colorChanged, this, &ColorBox::onWheelMove);
    connect(mColorInspector, &ColorInspector::colorChanged, this, &ColorBox::onSpinboxChange);
    connect(mColorWheel, &ColorWheel::colorSelected, this, &ColorBox::onWheelRelease);

    QColor defaultColor;
    defaultColor.setHsv(0, 0, 0);
    mColorWheel->setColor(defaultColor);
    mColorInspector->setColor(defaultColor);
}

ColorBox::~ColorBox()
{
}

void ColorBox::initUI()
{
}

void ColorBox::updateUI()
{
}

QColor ColorBox::color()
{
    return mColorWheel->color();
}

void ColorBox::setColor(const QColor& newColor)
{
    if ( newColor.toHsv() != mColorWheel->color() )
    {
        mColorWheel->setColor(newColor);
        mColorInspector->setColor(newColor);

        emit colorChanged(newColor);
    }
}

void ColorBox::onSpinboxChange(const QColor& color)
{
    if ( mColorWheel->color() != color.toHsv() )
    {
        mColorWheel->setColor(color);
        emit colorChanged(color);
    }
}

void ColorBox::onWheelMove(const QColor& color)
{
    if ( mColorInspector->color() != color )
    {
        mColorInspector->setColor(color);
    }
}

void ColorBox::onWheelRelease(const QColor& color)
{
     mColorInspector->setColor(color);
     emit colorChanged(color);
}
