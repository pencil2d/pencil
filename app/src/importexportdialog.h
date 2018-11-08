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

#ifndef IMPORTEXPORTDIALOG_H
#define IMPORTEXPORTDIALOG_H

#include <QDialog>
#include <QGroupBox>
#include "filedialogex.h"

namespace Ui {
class ImportExportDialog;
}

class ImportExportDialog : public QDialog
{
    Q_OBJECT

public:
    enum Mode { Import, Export };

    explicit ImportExportDialog(QWidget* parent, Mode eMode, FileType eFileType);
    ~ImportExportDialog();

    void init();
    QString getFilePath() const;
    QString getAbsolutePath();
    QStringList getFilePaths();

signals:
    void filePathsChanged(QStringList filePaths);

protected:
    QGroupBox* getOptionsGroupBox();
    void setFileExtension(QString extension);

private slots:
    void browse();

private:
    Ui::ImportExportDialog* ui = nullptr;

    FileDialog* m_fileDialog = nullptr;
    QStringList m_filePaths;

    FileType mFileType = FileType::ANIMATION;
    Mode mMode = Import;
};

#endif // IMPORTEXPORTDIALOG_H
