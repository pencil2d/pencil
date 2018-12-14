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

#include "doubleprogressdialog.h"
#include "ui_doubleprogressdialog.h"

#include <QtMath>

DoubleProgressDialog::DoubleProgressDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DoubleProgressDialog)
{
    ui->setupUi(this);

    major = new ProgressBarControl(ui->majorProgressBar);
    minor = new ProgressBarControl(ui->minorProgressBar);

    connect(ui->cancelButton, &QPushButton::pressed, this, &DoubleProgressDialog::canceled);
}

DoubleProgressDialog::~DoubleProgressDialog()
{
    delete ui;
}

QString DoubleProgressDialog::getStatus() {
    return ui->progressStatus->text();
}

void DoubleProgressDialog::setStatus(QString msg)
{
    ui->progressStatus->setText(msg);
}

DoubleProgressDialog::ProgressBarControl::ProgressBarControl(QProgressBar *b)
{
    bar = b;
}

void DoubleProgressDialog::ProgressBarControl::setMin(float minimum)
{
    min = minimum;
    if(max < min) setMax(min);
    bar->setMinimum(convertUnits(min));
}

void DoubleProgressDialog::ProgressBarControl::setMax(float maximum)
{
    max = maximum;
    if(min > max) setMin(max);
    bar->setMaximum(convertUnits(max));
}

void DoubleProgressDialog::ProgressBarControl::setValue(float value)
{
    val = qBound(min, value, max);
    bar->setValue(convertUnits(val));
}

int DoubleProgressDialog::ProgressBarControl::getPrecision()
{
    return static_cast<int>(qLn(unitFactor) / qLn(10));
}

void DoubleProgressDialog::ProgressBarControl::setPrecision(int e)
{
    int oldFactor = unitFactor;
    unitFactor = static_cast<int>(qPow(10, e));

    min *= unitFactor / static_cast<float>(oldFactor);
    max *= unitFactor / static_cast<float>(oldFactor);
    val *= unitFactor / static_cast<float>(oldFactor);
}

int DoubleProgressDialog::ProgressBarControl::convertUnits(float value)
{
    return qRound(value * unitFactor);
}
