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
    explicit ImportExportDialog(QWidget* parent = 0);
    ~ImportExportDialog();

    QString getFilePath();
    QStringList getFilePaths();

protected:
    void init();
    QGroupBox* getOptionsGroupBox();
    void setFileExtension(QString extension);

    enum Mode { Import, Export };
    virtual Mode getMode() = 0;
    virtual FileType getFileType() = 0;

private slots:
    void browse();

private:
    Ui::ImportExportDialog* ui = nullptr;

    FileDialog* m_fileDialog = nullptr;
    QStringList m_filePaths;
};

#endif // IMPORTEXPORTDIALOG_H
