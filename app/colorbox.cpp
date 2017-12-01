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

#include "colorbox.h"
#include "ui_colorbox.h"

ColorBox::ColorBox( QWidget* parent ) :
    BaseDockWidget( parent ),
    ui( new Ui::ColorBox )
{
    ui->setupUi( this );

    QColor defaultColor;
    defaultColor.setHsv(0, 0, 0);
    ui->colorWheel->setColor(defaultColor);
    ui->colorInspector->setColor(defaultColor);
}

ColorBox::~ColorBox()
{
    delete ui;
}

void ColorBox::initUI()
{

}

void ColorBox::updateUI()
{

}

QColor ColorBox::color()
{
    return ui->colorWheel->color();
}

void ColorBox::setColor(const QColor& newColor)
{
    if ( newColor.toHsv() != ui->colorWheel->color() )
    {
        ui->colorWheel->setColor(newColor);
        ui->colorInspector->setColor(newColor);

        emit colorChanged(newColor);
    }
}

void ColorBox::onSpinboxChange(const QColor& color)
{
    if ( ui->colorWheel->color() != color.toHsv() )
    {
        ui->colorWheel->setColor(color);
        emit colorChanged(color);
    }
}

void ColorBox::onWheelMove(const QColor& color)
{
    if ( ui->colorInspector->color() != color )
    {
        ui->colorInspector->setColor(color);
    }
}

void ColorBox::onWheelRelease(const QColor& color)
{
     ui->colorInspector->setColor(color);
     emit colorChanged(color);
}
