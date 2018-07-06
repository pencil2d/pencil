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

#include "importexportdialog.h"
#include "ui_importexportdialog.h"
#include <QFileInfo>

ImportExportDialog::ImportExportDialog(QWidget* parent, Mode eMode, FileType eFileType) : QDialog(parent)
{
    mMode = eMode;
    mFileType = eFileType;

    ui = new Ui::ImportExportDialog;
    ui->setupUi(this);
    m_fileDialog = new FileDialog(this);

    connect(ui->browseButton, &QPushButton::clicked, this, &ImportExportDialog::browse);

    Qt::WindowFlags eFlags = Qt::Dialog | Qt::WindowTitleHint | Qt::WindowCloseButtonHint;
    setWindowFlags(eFlags);
}

ImportExportDialog::~ImportExportDialog()
{
    delete ui;
}

QString ImportExportDialog::getFilePath()
{
    return m_filePaths.isEmpty() ? QString() : m_filePaths.first();
}

QStringList ImportExportDialog::getFilePaths()
{
    return m_filePaths;
}

QString ImportExportDialog::getAbsolutePath()
{
    QFileInfo info(m_filePaths.first());
    return info.absolutePath();
}

void ImportExportDialog::init()
{
    switch (mMode)
    {
        case Import:
            m_filePaths = QStringList(m_fileDialog->getLastOpenPath(mFileType));
            break;
        case Export:
            m_filePaths = QStringList(m_fileDialog->getLastSavePath(mFileType));
            break;
        default:
            Q_ASSERT(false);
    }
    ui->fileEdit->setText("\"" + m_filePaths.first() + "\"");

    emit filePathsChanged(m_filePaths);
}

QGroupBox* ImportExportDialog::getOptionsGroupBox()
{
    return ui->optionsGroupBox;
}

void ImportExportDialog::setFileExtension(QString extension)
{
    for (int i = 0; i < m_filePaths.size(); i++)
    {
        QFileInfo info(m_filePaths.at(i));
        m_filePaths.replace(i, info.path() + "/" + info.baseName() + "." + extension);
    }
    ui->fileEdit->setText("\"" + m_filePaths.join("\" \"") + "\"");

    emit filePathsChanged(m_filePaths);
}

void ImportExportDialog::browse()
{
    QStringList filePaths;
    switch (mMode)
    {
        case Import:
            if (mFileType == FileType::IMAGE_SEQUENCE)
            {
                filePaths = m_fileDialog->openFiles( FileType::IMAGE_SEQUENCE );
                break;
            }
            filePaths = QStringList(m_fileDialog->openFile(mFileType));
            break;
        case Export:
            filePaths = QStringList(m_fileDialog->saveFile(mFileType));
            break;
        default:
            Q_ASSERT(false);
    }

    if (filePaths.isEmpty() || filePaths.first().isEmpty())
    {
        return;
    }

    m_filePaths = filePaths;
    ui->fileEdit->setText("\"" + filePaths.join("\" \"") + "\"");

    emit filePathsChanged(m_filePaths);
}
