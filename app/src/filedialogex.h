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

#ifndef FILEDIALOGEX_H
#define FILEDIALOGEX_H

#include <QObject>

enum class FileType
{
    ANIMATION,
    IMAGE,
    IMAGE_SEQUENCE,
    MOVIE,
    SOUND,
    PALETTE
};

class FileDialog : public QObject
{
    Q_OBJECT
public:
    FileDialog( QWidget* parent );
    ~FileDialog();

    QString openFile( FileType fileType );
    QStringList openFiles( FileType fileType );
    QString saveFile( FileType fileType );

    QString getLastOpenPath( FileType fileType );
    void setLastOpenPath( FileType fileType, QString openPath );
    QString getLastSavePath( FileType fileType );
    void setLastSavePath( FileType fileType, QString savePath );

private:
    QString openDialogTitle( FileType fileType );
    QString saveDialogTitle( FileType fileType );
    QString openFileFilters( FileType fileType );
    QString saveFileFilters( FileType fileType );
    QString getFilterForFile( QString fileType, QString filePath );
    QString defaultFileName( FileType fileType );

    QString toSettingKey( FileType fileType);

    QWidget* mRoot = nullptr;
};

#endif // FILEDIALOGEX_H
