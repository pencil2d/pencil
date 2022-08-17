/*

Pencil2D - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2012-2020 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/

#include "toolspage.h"

#include <QtMath>

#include "preferencemanager.h"

#include "ui_toolspage.h"

ToolsPage::ToolsPage() : ui(new Ui::ToolsPage)
{
    ui->setupUi(this);

    connect(ui->useQuickSizingBox, &QCheckBox::stateChanged, this, &ToolsPage::quickSizingChange);
    connect(ui->rotationIncrementSlider, &QSlider::valueChanged, this, &ToolsPage::rotationIncrementChange);
}

ToolsPage::~ToolsPage()
{
    delete ui;
}

void ToolsPage::updateValues()
{
    ui->useQuickSizingBox->setChecked(mManager->isOn(SETTING::QUICK_SIZING));
    setRotationIncrement(mManager->getInt(SETTING::ROTATION_INCREMENT));
}

void ToolsPage::quickSizingChange(int b)
{
    mManager->set(SETTING::QUICK_SIZING, b != Qt::Unchecked);
}

void ToolsPage::setRotationIncrement(int angle)
{
    int value = qSqrt((angle - 1) / 359.0) * 359;
    ui->rotationIncrementSlider->setValue(value);
}

void ToolsPage::rotationIncrementChange(int value)
{
    // Use log scale
    int angle = qPow(value / 359.0, 2) * 359 + 1;
    // Basically round up to the nearest number that is a divisor of 360
    while (360 % angle != 0) {
        angle++;
    }
    ui->rotationIncrementDisplay->setText(tr("%1 degrees").arg(angle)); // don't use tr()'s plural settings, it breaks Transifex.
    mManager->set(SETTING::ROTATION_INCREMENT, angle);
}
