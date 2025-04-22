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

#include "importimageconfig.h"

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

    ImportImageConfig importConfig() const { return mImportConfig; }

private slots:
    void didChangeComboBoxIndex(const int index);
    void changeImportView();

private:
    static ImportImageConfig::PositionType getTypeFromIndex(int index) {
        switch (index) {
        case 0:
            return ImportImageConfig::CenterOfView;
        case 1:
            return ImportImageConfig::CenterOfCanvas;
        case 2:
            return ImportImageConfig::CenterOfCamera;
        case 3:
            return ImportImageConfig::CenterOfCameraFollowed;
        default:
            return ImportImageConfig::None;
        }
    }

    Ui::ImportPositionDialog *ui;

    ImportImageConfig mImportConfig;
    Editor* mEditor = nullptr;
};

#endif // IMPORTPOSITIONDIALOG_H
