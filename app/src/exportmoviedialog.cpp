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

#include "exportmoviedialog.h"
#include "ui_exportmovieoptions.h"
#include "util.h"

ExportMovieDialog::ExportMovieDialog(QWidget *parent, Mode mode, FileType fileType) :
    ImportExportDialog(parent, mode, fileType),
    ui(new Ui::ExportMovieOptions)
{
    ui->setupUi(getOptionsGroupBox());

    if (fileType == FileType::GIF) {
        setWindowTitle(tr("Export Animated GIF"));
        ui->exporterGroupBox->hide();
    } else {
        setWindowTitle(tr("Export Movie"));
    }

    QSizePolicy policy = ui->unevenWidthLabel->sizePolicy();
    policy.setRetainSizeWhenHidden(true);
    ui->unevenWidthLabel->setSizePolicy(policy);
    policy = ui->unevenHeightLabel->sizePolicy();
    policy.setRetainSizeWhenHidden(true);
    ui->unevenHeightLabel->setSizePolicy(policy);

    connect(this, &ExportMovieDialog::filePathsChanged, this, &ExportMovieDialog::onFilePathsChanged);
    connect(ui->widthSpinBox, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &ExportMovieDialog::validateResolution);
    connect(ui->heightSpinBox, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &ExportMovieDialog::validateResolution);
}

ExportMovieDialog::~ExportMovieDialog()
{
    delete ui;
}

void ExportMovieDialog::setCamerasInfo(const std::vector<std::pair<QString, QSize>> camerasInfo)
{
    if ( ui->cameraCombo->count() > 0 )
    {
        ui->cameraCombo->clear();
    }

	for (const std::pair<QString, QSize>& camera : camerasInfo)
    {
        ui->cameraCombo->addItem( camera.first, camera.second );
    }

    auto indexChanged = static_cast< void(QComboBox::*)( int i ) >( &QComboBox::currentIndexChanged );
    connect( ui->cameraCombo, indexChanged, this, &ExportMovieDialog::updateResolutionCombo );

    updateResolutionCombo( 0 );
}

void ExportMovieDialog::updateResolutionCombo( int index )
{
    QSize camSize = ui->cameraCombo->itemData( index ).toSize();

    QSignalBlocker b1( ui->widthSpinBox );
    QSignalBlocker b2( ui->heightSpinBox );

    ui->widthSpinBox->setValue( camSize.width() );
    ui->heightSpinBox->setValue( camSize.height() );
    validateResolution();
}

void ExportMovieDialog::setDefaultRange(int startFrame, int endFrame, int endFrameWithSounds)
{
    mEndFrame = endFrame;
    mEndFrameWithSounds = endFrameWithSounds;

    QSignalBlocker b1( ui->startSpinBox );
    QSignalBlocker b2( ui->endSpinBox );

    ui->startSpinBox->setValue( startFrame );
    ui->endSpinBox->setValue( endFrame );

    connect(ui->frameCheckBox, &QCheckBox::clicked, this, &ExportMovieDialog::frameCheckboxClicked);
}

QString ExportMovieDialog::getSelectedCameraName()
{
    return ui->cameraCombo->currentText();
}

QSize ExportMovieDialog::getExportSize()
{
    return QSize( ui->widthSpinBox->value(), ui->heightSpinBox->value() );
}

bool ExportMovieDialog::getTransparency() const
{
    return ui->transparencyCheckBox->isChecked() && supportsTransparency(getFilePath());
}

int ExportMovieDialog::getStartFrame()
{
    return ui->startSpinBox->value();
}

int ExportMovieDialog::getEndFrame()
{
    return ui->endSpinBox->value();
}

bool ExportMovieDialog::getLoop()
{
    return ui->loopCheckBox->isChecked();
}

void ExportMovieDialog::frameCheckboxClicked(bool checked)
{
    int e = (checked) ? mEndFrameWithSounds : mEndFrame;
    ui->endSpinBox->setValue(e);
}

void ExportMovieDialog::onFilePathsChanged(QStringList filePaths)
{
    QString filePath = filePaths.first().toLower();
    bool canLoop = supportsLooping(filePath);
    ui->loopCheckBox->setEnabled(canLoop);
    if (!canLoop)
    {
        ui->loopCheckBox->setChecked(false);
    }
    ui->transparencyCheckBox->setEnabled(supportsTransparency(filePath));
    validateResolution();
}

bool ExportMovieDialog::supportsLooping(QString filePath) const
{
    return filePath.endsWith(".apng", Qt::CaseInsensitive) ||
           filePath.endsWith(".gif", Qt::CaseInsensitive);
}

bool ExportMovieDialog::supportsTransparency(QString filePath) const
{
    return filePath.endsWith(".apng", Qt::CaseInsensitive) ||
           filePath.endsWith(".webm", Qt::CaseInsensitive);
}

void ExportMovieDialog::validateResolution()
{
    const bool isMp4 = getFilePath().endsWith(".mp4", Qt::CaseInsensitive);
    const bool widthValid = !isMp4 || ui->widthSpinBox->value() % 2 == 0;
    const bool heightValid = !isMp4 || ui->heightSpinBox->value() % 2 == 0;
    ui->unevenWidthLabel->setHidden(widthValid);
    ui->unevenHeightLabel->setHidden(heightValid);
    setOkButtonEnabled(widthValid && heightValid);
}
