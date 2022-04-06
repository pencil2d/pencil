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

	void setCamerasInfo(const std::vector<std::pair<QString, QSize>>& camInfo);
    void setDefaultRange( int startFrame, int endFrame, int endFrameWithSounds );

    void  setExportSize( QSize size );
    QSize getExportSize() const;
    bool getTransparency() const;
    bool getExportKeyframesOnly() const;
    QString getExportFormat() const;
	QString getCameraLayerName() const;

    int getStartFrame() const;
    int getEndFrame() const;

private slots:
    void frameCheckboxClicked(bool checked);
    void formatChanged(const QString& format);
	void cameraComboChanged(int index);


private:
    Ui::ExportImageOptions* ui = nullptr;

    void setTransparencyOptionVisibility(const QString& format);
    int mEndFrameWithSounds = 0;
    int mEndFrame = 0;
};

#endif // EXPORTIMAGEDIALOG_H
