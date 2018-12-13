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

#include "exportimagedialog.h"
#include "ui_exportimageoptions.h"
#include "util.h"

ExportImageDialog::ExportImageDialog(QWidget* parent, FileType eFileType) :
    ImportExportDialog(parent, ImportExportDialog::Export, eFileType),
    ui(new Ui::ExportImageOptions)
{
    ui->setupUi(getOptionsGroupBox());
    if (eFileType == FileType::IMAGE_SEQUENCE)
    {
        setWindowTitle(tr("Export image sequence"));
    }
    else
    {
        setWindowTitle(tr("Export image"));
        ui->frameRangeGroupBox->hide();
    }

    connect(ui->formatComboBox, &QComboBox::currentTextChanged, this, &ExportImageDialog::formatChanged);
    formatChanged(getExportFormat()); // Make sure file extension matches format combobox
}

ExportImageDialog::~ExportImageDialog()
{
    delete ui;
}

void ExportImageDialog::setCamerasInfo(const std::vector<std::pair<QString, QSize>>& cameraInfo)
{
    Q_ASSERT(ui->cameraCombo);

    ui->cameraCombo->clear();
    for (const std::pair<QString, QSize>& it : cameraInfo)
    {
        ui->cameraCombo->addItem(it.first, it.second);
    }

    const auto indexChanged = static_cast<void(QComboBox::*)(int i)>(&QComboBox::currentIndexChanged);
    connect(ui->cameraCombo, indexChanged, this, &ExportImageDialog::cameraComboChanged);

    cameraComboChanged(0);
}

void ExportImageDialog::setDefaultRange(int startFrame, int endFrame, int endFrameWithSounds)
{
    mEndFrame = endFrame;
    mEndFrameWithSounds = endFrameWithSounds;

    SignalBlocker b1( ui->startSpinBox );
    SignalBlocker b2( ui->endSpinBox );

    ui->startSpinBox->setValue( startFrame );
    ui->endSpinBox->setValue( endFrame );

    connect(ui->frameCheckBox, &QCheckBox::clicked, this, &ExportImageDialog::frameCheckboxClicked);
}

int ExportImageDialog::getStartFrame() const
{
    return ui->startSpinBox->value();
}

int ExportImageDialog::getEndFrame() const
{
    return ui->endSpinBox->value();
}

void ExportImageDialog::frameCheckboxClicked(bool checked)
{
    int e = (checked) ? mEndFrameWithSounds : mEndFrame;
    ui->endSpinBox->setValue(e);
}

void ExportImageDialog::setExportSize(QSize size)
{
    ui->imgWidthSpinBox->setValue(size.width());
    ui->imgHeightSpinBox->setValue(size.height());
}

QSize ExportImageDialog::getExportSize() const
{
    return QSize(ui->imgWidthSpinBox->value(), ui->imgHeightSpinBox->value());
}

bool ExportImageDialog::getTransparency() const
{
    return ui->cbTransparency->checkState() == Qt::Checked;
}

bool ExportImageDialog::getExportKeyframesOnly() const
{
    return ui->cbExportKeyframesOnly->checkState() == Qt::Checked;
}

QString ExportImageDialog::getExportFormat() const
{
    return ui->formatComboBox->currentText();
}

QString ExportImageDialog::getCameraLayerName() const
{
    return ui->cameraCombo->currentText();
}

void ExportImageDialog::formatChanged(const QString& format)
{
    setFileExtension(format.toLower());
    setTransparencyOptionVisibility(format);
}

void ExportImageDialog::cameraComboChanged(int index)
{
    const QSize cameraSize = ui->cameraCombo->itemData(index).toSize();

    ui->imgWidthSpinBox->setValue(cameraSize.width());
    ui->imgHeightSpinBox->setValue(cameraSize.height());
}

void ExportImageDialog::setTransparencyOptionVisibility(const QString &format)
{
    if (format == "JPG" || format == "BMP")
        ui->cbTransparency->setDisabled(true);
    else
        ui->cbTransparency->setDisabled(false);
}
