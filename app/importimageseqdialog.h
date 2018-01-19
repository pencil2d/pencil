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

#ifndef IMPORTIMAGESEQDIALOG_H
#define IMPORTIMAGESEQDIALOG_H

#include "importexportdialog.h"

namespace Ui {
class ImportImageSeqOptions;
}

class ImportImageSeqDialog : public ImportExportDialog
{
    Q_OBJECT

public:
    explicit ImportImageSeqDialog(QWidget *parent = 0);
    ~ImportImageSeqDialog();

    int getSpace();

protected:
    Mode getMode();
    FileType getFileType();

private slots:
    void setSpace(int number);

private:
    Ui::ImportImageSeqOptions *ui;
};

#endif // IMPORTIMAGESEQDIALOG_H
