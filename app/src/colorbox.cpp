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
#include "qsettings.h"
#include "pencildef.h"

ColorBox::ColorBox( QWidget* parent ) : BaseDockWidget( parent )
{
    setWindowTitle(tr("Color Box", "Color Box window title"));

    mColorWheel = new ColorWheel(this);

    QVBoxLayout* layout = new QVBoxLayout();
    layout->setContentsMargins(5, 5, 5, 5);
    layout->addWidget(mColorWheel);
    layout->setStretch(0, 1);
    layout->setStretch(1, 0);
    QWidget* mainWidget = new QWidget;
    mainWidget->setLayout(layout);
    setWidget(mainWidget);

    connect(mColorWheel, &ColorWheel::colorChanged, this, &ColorBox::onWheelMove);
    connect(mColorWheel, &ColorWheel::colorSelected, this, &ColorBox::onWheelRelease);
//    connect(this, &ColorBox::colorChanged, mColorWheel, &ColorWheel::setColor);
}

ColorBox::~ColorBox()
{
}

void ColorBox::initUI()
{
    QSettings settings(PENCIL2D, PENCIL2D);

    QColor savedColor;
    savedColor.setRgba(settings.value("colorOfSliders").toUInt());
    setColor(savedColor);
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
    if ( newColor != mColorWheel->color() )
    {
        mColorWheel->setColor(newColor);
    }
}

void ColorBox::onWheelMove(const QColor& color)
{
    emit colorChanged(color);
}

void ColorBox::onWheelRelease(const QColor& color)
{
     emit colorChanged(color);
}
