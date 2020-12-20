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
#ifndef IMPORTLAYERSDIALOG_H
#define IMPORTLAYERSDIALOG_H

#include <QDialog>
#include "object.h"
#include "editor.h"

namespace Ui {
class ImportLayersDialog;
}

class ImportLayersDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ImportLayersDialog(QWidget *parent = nullptr);
    ~ImportLayersDialog();

    void setCore(Editor *editor);

public slots:
    void getFileName();
    void listWidgetChanged();
    void importLayers();
    void cancel();

private:
    Ui::ImportLayersDialog *ui;

    void getLayers();

    std::unique_ptr<Object> mImportObject;
    Layer* mImportLayer = nullptr;
    Editor* mEditor = nullptr;
    QString mFileName;
    QList<int> mItemsSelected;
    void loadKeyFrames(Layer* importedLayer);
};

#endif // IMPORTLAYERSDIALOG_H
