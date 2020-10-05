/*

Pencil - Traditional Animation Software
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

#ifndef FILEDIALOG_H
#define FILEDIALOG_H

#include <QObject>

#include "filetype.h"

class FileDialog : public QObject
{
    Q_OBJECT
public:
    static QString getOpenFileName( QWidget* parent, FileType fileType, const QString& caption = QString() );
    static QStringList getOpenFileNames( QWidget* parent, FileType fileType, const QString& caption = QString() );
    static QString getSaveFileName( QWidget* parent, FileType fileType, const QString& caption = QString() );

    static QString getLastOpenPath( FileType fileType );
    static void setLastOpenPath( FileType fileType, const QString& openPath );
    static QString getLastSavePath( FileType fileType );
    static void setLastSavePath( FileType fileType, const QString& savePath );

private:
    static QString openDialogCaption( FileType fileType );
    static QString saveDialogCaption( FileType fileType );
    static QString openFileFilters( FileType fileType );
    static QString saveFileFilters( FileType fileType );
    static QString getFilterForFile( const QString& fileType, QString filePath );
    static QString defaultFileName( FileType fileType );

    static QString addDefaultExtensionSuffix(FileType fileType);

    static QString toSettingKey( FileType fileType);
};

#endif // FILEDIALOG_H
