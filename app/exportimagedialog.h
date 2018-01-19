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

#ifndef EXPORTIMAGEDIALOG_H
#define EXPORTIMAGEDIALOG_H

#include "importexportdialog.h"

namespace Ui {
class ExportImageOptions;
}

class ExportImageDialog : public ImportExportDialog
{
    Q_OBJECT

public:
    explicit ExportImageDialog(QWidget* parent, FileType eType);
    ~ExportImageDialog();

	void setCamerasInfo(const std::vector<std::pair<QString, QSize>>);

    void  setExportSize( QSize size );
    QSize getExportSize();
    bool getTransparency();
    QString getExportFormat();
	QString getCameraLayerName();

private slots:
    void formatChanged(QString format);
	void cameraComboChanged(int index);

private:
    Ui::ExportImageOptions* ui = nullptr;
};

#endif // EXPORTIMAGEDIALOG_H
