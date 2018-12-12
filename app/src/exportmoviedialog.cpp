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
    connect(this, &ExportMovieDialog::filePathsChanged, this, &ExportMovieDialog::onFilePathsChanged);
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

    SignalBlocker b1( ui->widthSpinBox );
    SignalBlocker b2( ui->heightSpinBox );

    ui->widthSpinBox->setValue( camSize.width() );
    ui->heightSpinBox->setValue( camSize.height() );
}

void ExportMovieDialog::setDefaultRange(int startFrame, int endFrame, int endFrameWithSounds)
{
    mEndFrame = endFrame;
    mEndFrameWithSounds = endFrameWithSounds;

    SignalBlocker b1( ui->startSpinBox );
    SignalBlocker b2( ui->endSpinBox );

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
    ui->loopCheckBox->setEnabled(supportsLooping(filePath));
    ui->transparencyCheckBox->setEnabled(supportsTransparency(filePath));
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
