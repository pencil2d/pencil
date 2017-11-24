/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2013-2017 Matt Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/

#include "exportimagedialog.h"
#include "ui_exportimageoptions.h"

ExportImageDialog::ExportImageDialog(QWidget *parent, bool seq) :
    ImportExportDialog(parent),
    ui(new Ui::ExportImageOptions),
    mFileType(seq ? FileType::IMAGE_SEQUENCE : FileType::IMAGE)
{
    ui->setupUi( getOptionsGroupBox() );
    init();

    if (seq)
    {
        setWindowTitle( tr( "Export image sequence" ) );
    }
    else
    {
        setWindowTitle( tr( "Export image" ) );
    }

    connect( ui->formatComboBox, &QComboBox::currentTextChanged, this, &ExportImageDialog::formatChanged );
    formatChanged( getExportFormat() ); // Make sure file extension matches format combobox
}

ExportImageDialog::~ExportImageDialog()
{
    delete ui;
}

void ExportImageDialog::setCamerasInfo(const std::vector<std::pair<QString, QSize>> cameraInfo)
{
	Q_ASSERT(ui->cameraCombo);

	ui->cameraCombo->clear();
	for (const std::pair<QString, QSize>& it : cameraInfo)
	{
		ui->cameraCombo->addItem(it.first, it.second);
	}

	auto indexChanged = static_cast<void(QComboBox::*)(int i)>(&QComboBox::currentIndexChanged);
	connect(ui->cameraCombo, indexChanged, this, &ExportImageDialog::cameraComboChanged);

	cameraComboChanged(0);
}

void ExportImageDialog::setExportSize(QSize size)
{
    ui->imgWidthSpinBox->setValue( size.width() );
    ui->imgHeightSpinBox->setValue( size.height() );
}

QSize ExportImageDialog::getExportSize()
{
    return QSize( ui->imgWidthSpinBox->value(), ui->imgHeightSpinBox->value() );
}

bool ExportImageDialog::getTransparency()
{
    return ui->cbTransparency->checkState() == Qt::Checked;
}

QString ExportImageDialog::getExportFormat()
{
    return ui->formatComboBox->currentText();
}

QString ExportImageDialog::getCameraLayerName()
{
	return ui->cameraCombo->currentText();
}

ImportExportDialog::Mode ExportImageDialog::getMode()
{
    return ImportExportDialog::Export;
}

FileType ExportImageDialog::getFileType()
{
    return mFileType;
}

void ExportImageDialog::formatChanged(QString format)
{
    setFileExtension( format.toLower() );
}

void ExportImageDialog::cameraComboChanged(int index)
{
	QSize cameraSize = ui->cameraCombo->itemData(index).toSize();

	ui->imgWidthSpinBox->setValue(cameraSize.width());
	ui->imgHeightSpinBox->setValue(cameraSize.height());
}
