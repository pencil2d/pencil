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

#include "camerapropertiesdialog.h"
#include "QtCore/qdebug.h"
#include "ui_camerapropertiesdialog.h"

CameraPropertiesDialog::CameraPropertiesDialog(const QString& name, int width, int height, double aperture) :
    QDialog(),
    ui(new Ui::CameraPropertiesDialog)
{
    ui->setupUi(this);

    ui->nameBox->setText(name);
    ui->widthBox->setValue(width);
    ui->heightBox->setValue(height);
    initComboBox(aperture);
    setAperture(aperture);
    connect(ui->cbAperture, &QComboBox::currentTextChanged, this, &CameraPropertiesDialog::updateAperture);
}

CameraPropertiesDialog::~CameraPropertiesDialog()
{
    delete ui;
}

QString CameraPropertiesDialog::getName()
{
    return ui->nameBox->text();
}

void CameraPropertiesDialog::setName(const QString& name)
{
    ui->nameBox->setText(name);
}

int CameraPropertiesDialog::getWidth()
{
    return ui->widthBox->value();
}

void CameraPropertiesDialog::setWidth(int width)
{
    ui->widthBox->setValue(width);
}

int CameraPropertiesDialog::getHeight()
{
    return ui->heightBox->value();
}

void CameraPropertiesDialog::setHeight(int height)
{
    ui->heightBox->setValue(height);
}

void CameraPropertiesDialog::updateAperture()
{
    switch (ui->cbAperture->currentIndex()) {
    case 0: setAperture(1.4); break;
    case 1: setAperture(2.0); break;
    case 2: setAperture(2.8); break;
    case 3: setAperture(4.0); break;
    case 4: setAperture(5.6); break;
    case 5: setAperture(8.0); break;
    case 6: setAperture(11.0); break;
    case 7: setAperture(16.0); break;
    case 8: setAperture(22.0); break;
    default: setAperture(8.0); break;
    }
}

void CameraPropertiesDialog::initComboBox(qreal aperture)
{
    for (int i = 0; i < ui->cbAperture->count(); i++)
    {
        if (ui->cbAperture->itemText(i).toDouble() == aperture)
        {
            ui->cbAperture->setCurrentIndex(i);
            return;
        }
    }
}

