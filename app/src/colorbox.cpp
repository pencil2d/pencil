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
#include "colorbox.h"
#include "editor.h"
#include "colormanager.h"


ColorBox::ColorBox( QWidget* parent ) : BaseDockWidget( parent )
{
    setWindowTitle(tr("Color Box", "Color Box window title"));
}

ColorBox::~ColorBox()
{
}

void ColorBox::initUI()
{
    mColorWheel = new ColorWheel(this);

    QVBoxLayout* layout = new QVBoxLayout;
    layout->setContentsMargins(5, 5, 5, 5);
    layout->addWidget(mColorWheel);
    layout->setStretch(0, 1);
    layout->setStretch(1, 0);
    QWidget* mainWidget = new QWidget;
    mainWidget->setLayout(layout);
    setWidget(mainWidget);

    connect(mColorWheel, &ColorWheel::colorChanged, this, &ColorBox::onWheelMove);
    connect(mColorWheel, &ColorWheel::colorSelected, this, &ColorBox::onWheelRelease);

    connect(editor(), &Editor::objectLoaded, this, &ColorBox::updateUI);
}

void ColorBox::updateUI()
{
    QColor newColor = editor()->color()->frontColor();
    setColor(newColor);
}

QColor ColorBox::color()
{
    return mColorWheel->color();
}

void ColorBox::setColor(QColor newColor)
{
    newColor = newColor.toHsv();

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
