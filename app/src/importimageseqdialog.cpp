/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2013-2018 Matt Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/

#include "importimageseqdialog.h"
#include "ui_importimageseqoptions.h"
#include "util.h"

ImportImageSeqDialog::ImportImageSeqDialog(QWidget* parent, Mode mode, FileType fileType) :
    ImportExportDialog(parent, mode, fileType)
{
    ui = new Ui::ImportImageSeqOptions;
    ui->setupUi(getOptionsGroupBox());

    if (fileType == FileType::GIF) {
        setWindowTitle(tr("Import Animated GIF"));
    } else {
        setWindowTitle(tr("Import image sequence"));
    }

    connect(ui->spaceSpinBox, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &ImportImageSeqDialog::setSpace);
}

ImportImageSeqDialog::~ImportImageSeqDialog()
{
    delete ui;
}

int ImportImageSeqDialog::getSpace()
{
    return ui->spaceSpinBox->value();
}

ImportExportDialog::Mode ImportImageSeqDialog::getMode()
{
    return ImportExportDialog::Import;
}

FileType ImportImageSeqDialog::getFileType()
{
    return FileType::IMAGE_SEQUENCE;
}

void ImportImageSeqDialog::setSpace(int number)
{
    SignalBlocker b1(ui->spaceSpinBox);
    ui->spaceSpinBox->setValue(number);
}
