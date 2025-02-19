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
#ifndef IMPORTPOSITIONDIALOG_H
#define IMPORTPOSITIONDIALOG_H

#include <QDialog>

#include "importpositiontype.h"

namespace Ui {
class ImportPositionDialog;
}

class Editor;

class ImportPositionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ImportPositionDialog(Editor* editor, QWidget *parent = nullptr);
    ~ImportPositionDialog();

    ImportPositionType importOption() const { return mImportOption; }

private slots:
    void didChangeComboBoxIndex(const int index);
    void changeImportView();

private:
    static ImportPositionType getTypeFromIndex(int index) {
        switch (index) {
        case 0:
            return ImportPositionType::CenterOfView;
        case 1:
            return ImportPositionType::CenterOfCanvas;
        case 2:
            return ImportPositionType::CenterOfCamera;
        case 3:
            return ImportPositionType::CenterOfCameraFollowed;
        default:
            return ImportPositionType::None;
        }
    }

    Ui::ImportPositionDialog *ui;

    ImportPositionType mImportOption = ImportPositionType::None;
    Editor* mEditor = nullptr;
};

#endif // IMPORTPOSITIONDIALOG_H
