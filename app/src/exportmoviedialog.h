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

#ifndef EXPORTMOVIEDIALOG_H
#define EXPORTMOVIEDIALOG_H

#include "importexportdialog.h"
#include <QCheckBox>

namespace Ui {
class ExportMovieOptions;
}


class ExportMovieDialog : public ImportExportDialog
{
    Q_OBJECT

public:
    explicit ExportMovieDialog(QWidget* parent = nullptr, Mode mode = ImportExportDialog::Export, FileType fileType = FileType::MOVIE);
    ~ExportMovieDialog();

    void setCamerasInfo(const std::vector<std::pair<QString, QSize>>);
    void updateResolutionCombo( int index );

    void setDefaultRange( int startFrame, int endFrame, int endFrameWithSounds );

    QString getSelectedCameraName();
    QSize getExportSize();
    bool getTransparency() const;

    int getStartFrame();
    int getEndFrame();

    bool getLoop();
    
private:
    void frameCheckboxClicked(bool checked);
    void onFilePathsChanged(QStringList filePaths);

    bool supportsLooping(QString filePath) const;
    bool supportsTransparency(QString filePath) const;
    
    int mEndFrameWithSounds = 0;
    int mEndFrame = 0;

    Ui::ExportMovieOptions* ui = nullptr;
};

#endif // EXPORTMOVIEDIALOG_H
