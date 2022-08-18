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

#ifndef IMPORTEXPORTDIALOG_H
#define IMPORTEXPORTDIALOG_H

#include <QDialog>
#include <QGroupBox>
#include "filetype.h"

namespace Ui {
class ImportExportDialog;
}

class QDialogButtonBox;

class ImportExportDialog : public QDialog
{
    Q_OBJECT

public:
    enum Mode { Import, Export };

    explicit ImportExportDialog(QWidget* parent, Mode eMode, FileType eFileType);
    ~ImportExportDialog() override;

    void init();
    QString getFilePath() const;
    QString getAbsolutePath();
    QStringList getFilePaths();
    int getPosIndex() { return mPosIndex; }

signals:
    void filePathsChanged(QStringList filePaths);

protected:
    QGroupBox* getOptionsGroupBox();
    QGroupBox* getPreviewGroupBox();
    QDialogButtonBox* getDialogButtonBox();

    void setFileExtension(const QString& extension);
    void hideOptionsGroupBox(bool hide);
    void hidePreviewGroupBox(bool hide);
    void hideInstructionsLabel(bool hide);

    void setInstructionsLabel(const QString& text);

private slots:
    void browse();
    void setPosIndex(int index) { mPosIndex = index; }

private:
    Ui::ImportExportDialog* ui = nullptr;

    QStringList m_filePaths;

    FileType mFileType = FileType::ANIMATION;
    Mode mMode = Import;
    int mPosIndex = 0;
};

#endif // IMPORTEXPORTDIALOG_H
