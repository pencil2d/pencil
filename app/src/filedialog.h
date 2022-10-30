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

#ifndef FILEDIALOG_H
#define FILEDIALOG_H

#include <QObject>

#include "filetype.h"

/**
 * A wrapper around QFileDialog which remembers selected paths
 */
class FileDialog : public QObject
{
    Q_OBJECT
public:
    /**
     * Shows a file dialog which allows the user to select a file to open
     *
     * @param parent The parent widget
     * @param fileType The file type to select
     * @param caption The dialog's caption. If not specified, a default caption will be used
     * @return An existing file selected by the user or a null string if the dialog was cancelled
     * @see getOpenFileNames()
     */
    static QString getOpenFileName( QWidget* parent, FileType fileType, const QString& caption = QString() );

    /**
     * Shows a file dialog which allows the user to select multiple files to open
     *
     * @param parent The parent widget
     * @param fileType The file type to select
     * @param caption The dialog's caption. If not specified, a default caption will be used
     * @return One or more existing files selected by the user or an empty list if the dialog was cancelled
     * @see getOpenFileName()
     */
    static QStringList getOpenFileNames( QWidget* parent, FileType fileType, const QString& caption = QString() );

    /**
     * Shows a file dialog which allows the user to select a file save path. The file does not have to exist.
     *
     * @param parent The parent widget
     * @param fileType The file type to select
     * @param caption The dialog's caption. If not specified, a default caption will be used
     * @return An file path selected by the user or a null string if the dialog was cancelled
     */
    static QString getSaveFileName( QWidget* parent, FileType fileType, const QString& caption = QString() );

    /**
     * Retrieves the last opened file of a given type
     *
     * @param fileType The type of file to look up
     * @return The path of the last opened file or the user's home directory if no file of that type has been opened
     *         before
     * @see setLastOpenPath()
     */
    static QString getLastOpenPath( FileType fileType );

    /**
     * Saves the last opened file of a given type
     *
     * @param fileType The opened file's type
     * @param openPath The opened file's path
     * @see getLastOpenPath()
     */
    static void setLastOpenPath( FileType fileType, const QString& openPath );

    /**
     * Retrieves the last used save path for a given file type
     *
     * @param fileType The type of file to look up
     * @return The last used save path or a file with a default name in the user's home directory if no file of that
     *         type has been saved before
     * @see setLastSavePath()
     */
    static QString getLastSavePath( FileType fileType );

    /**
     * Saves the last used save path for a given file type
     *
     * @param fileType The saved file's type
     * @param savePath The saved file's path
     * @see getLastSavePath()
     */
    static void setLastSavePath( FileType fileType, const QString& savePath );

private:
    static QString openDialogCaption( FileType fileType );
    static QString saveDialogCaption( FileType fileType );
    static QString openFileFilters( FileType fileType );
    static QString saveFileFilters( FileType fileType );
    static QString getFilterForFile( const QString& fileType, QString filePath );
    static QString defaultFileName(FileType fileType , QString baseName = QString());

    static QString getDefaultExtensionByFileType(FileType fileType);

    static QString toSettingKey( FileType fileType );
    static void setLastSavePaths(const QString& filePath);
};

#endif // FILEDIALOG_H
