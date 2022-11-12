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

namespace Ui {
class ImportPositionDialog;
}

class Editor;

class ImportPositionDialog : public QDialog
{
    Q_OBJECT

    struct ImportPosition {

        enum Type {
            CenterOfView,
            CenterOfCanvas,
            CenterOfCamera,
            CenterOfCameraFollowed,
            None
        };

        static Type getTypeFromIndex(int index) {
            switch (index) {
            case 0:
                return CenterOfView;
            case 1:
                return CenterOfCanvas;
            case 2:
                return CenterOfCamera;
            case 3:
                return CenterOfCameraFollowed;
            default:
                return None;
            }
        }
    };

public:
    explicit ImportPositionDialog(Editor* editor, QWidget *parent = nullptr);
    ~ImportPositionDialog();

private slots:
    void didChangeComboBoxIndex(const int index);
    void changeImportView();

private:
    Ui::ImportPositionDialog *ui;

    ImportPosition::Type mImportOption = ImportPosition::None;
    Editor* mEditor = nullptr;
};

#endif // IMPORTPOSITIONDIALOG_H
