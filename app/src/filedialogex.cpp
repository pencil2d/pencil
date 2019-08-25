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

#include "filedialogex.h"

#include <QSettings>
#include <QFileInfo>
#include <QFileDialog>
#include <QDebug>

#include "fileformat.h"
#include "pencildef.h"

FileDialog::FileDialog( QWidget* parent ) : QObject( parent )
{
    mRoot = parent;
}

FileDialog::~FileDialog()
{
}

QString FileDialog::openFile(FileType fileType)
{
    QString strTitle = openDialogTitle( fileType );
    QString strInitialFilePath = getLastOpenPath( fileType );
    QString strFilter = openFileFilters( fileType );

    QString filePath = QFileDialog::getOpenFileName( mRoot,
                                                     strTitle,
                                                     strInitialFilePath,
                                                     strFilter );
    if ( !filePath.isEmpty() )
    {
        setLastOpenPath(fileType, filePath);
    }

    return filePath;
}

QStringList FileDialog::openFiles(FileType fileType)
{
    QString strTitle = openDialogTitle( fileType );
    QString strInitialFilePath = getLastOpenPath( fileType );
    QString strFilter = openFileFilters( fileType );
    QString strSelectedFilter = getFilterForFile(strFilter, strInitialFilePath);

    QStringList filePaths = QFileDialog::getOpenFileNames( mRoot,
                                                           strTitle,
                                                           strInitialFilePath,
                                                           strFilter,
                                                           strSelectedFilter.isNull() ? Q_NULLPTR : &strSelectedFilter );
    if ( !filePaths.isEmpty() && !filePaths.first().isEmpty() )
    {
        setLastOpenPath( fileType, filePaths.first() );
    }

    return filePaths;
}

QString FileDialog::saveFile( FileType fileType )
{
    QString strTitle = saveDialogTitle( fileType );
    QString strInitialFilePath = getLastSavePath( fileType );
    QString strFilter = saveFileFilters( fileType );
    QString strSelectedFilter = getFilterForFile(strFilter, strInitialFilePath);

    QString filePath = QFileDialog::getSaveFileName( mRoot,
                                                     strTitle,
                                                     strInitialFilePath,
                                                     strFilter,
                                                     strSelectedFilter.isNull() ? Q_NULLPTR : &strSelectedFilter );
    if ( !filePath.isEmpty() )
    {
        setLastSavePath( fileType, filePath );
    }

    return filePath;
}

QString FileDialog::getLastOpenPath( FileType fileType )
{
    QSettings setting( PENCIL2D, PENCIL2D );
    setting.beginGroup( "LastOpenPath" );

    return setting.value( toSettingKey( fileType ), QDir::homePath() ).toString();
}

void FileDialog::setLastOpenPath( FileType fileType, QString openPath )
{
    QSettings setting( PENCIL2D, PENCIL2D );
    setting.beginGroup( "LastOpenPath" );

    setting.setValue( toSettingKey( fileType ), QFileInfo( openPath ).absolutePath() );
}

QString FileDialog::getLastSavePath( FileType fileType )
{
    QSettings setting( PENCIL2D, PENCIL2D );
    setting.beginGroup( "LastSavePath" );

    return setting.value( toSettingKey( fileType ),
                          QDir::homePath() + "/" + defaultFileName( fileType) ).toString();
}

void FileDialog::setLastSavePath( FileType fileType, QString savePath )
{
    QSettings setting( PENCIL2D, PENCIL2D );
    setting.beginGroup( "LastSavePath" );

    setting.setValue( toSettingKey( fileType ), savePath );
}

QString FileDialog::openDialogTitle( FileType fileType )
{
    switch ( fileType )
    {
        case FileType::ANIMATION: return tr( "Open animation" );
        case FileType::IMAGE: return tr( "Import image" );
        case FileType::IMAGE_SEQUENCE: return tr( "Import image sequence" );
        case FileType::GIF: return tr( "Import Animated GIF" );
        case FileType::MOVIE: return tr( "Import movie" );
        case FileType::SOUND: return tr( "Import sound" );
        case FileType::PALETTE: return tr( "Import palette" );
        default: Q_ASSERT( false );
    }
    return "";
}

QString FileDialog::saveDialogTitle( FileType fileType )
{
    switch ( fileType )
    {
        case FileType::ANIMATION: return tr( "Save animation" );
        case FileType::IMAGE: return tr( "Export image" );
        case FileType::IMAGE_SEQUENCE: return tr( "Export image sequence" );
        case FileType::GIF: return tr( "Export Animated GIF" );
        case FileType::MOVIE: return tr( "Export movie" );
        case FileType::SOUND: return tr( "Export sound" );
        case FileType::PALETTE: return tr( "Export palette" );
        default: Q_ASSERT( false );
    }
    return "";
}

QString FileDialog::openFileFilters( FileType fileType )
{
    switch ( fileType )
    {
        case FileType::ANIMATION: return PFF_OPEN_ALL_FILE_FILTER;
        case FileType::IMAGE: return PENCIL_IMAGE_FILTER;
        case FileType::IMAGE_SEQUENCE: return PENCIL_IMAGE_SEQ_FILTER;
        case FileType::GIF: return QString("%1 (*.gif)").arg(tr("Animated GIF"));
        case FileType::MOVIE: { Q_ASSERT(false); return PENCIL_MOVIE_EXT; } // currently not supported
        case FileType::SOUND: return QString("%1 (*.wav *.mp3);;WAV (*.wav);;MP3 (*.mp3)").arg("Sounds");
        case FileType::PALETTE:
            return QString("%1 (*.xml *.gpl);;%2 (*.xml);;%3 (*.gpl)")
                .arg(tr("Palette"))
                .arg(tr("Pencil2D Palette"))
                .arg(tr("GIMP Palette"));
        default: Q_ASSERT( false );
    }
    return "";
}

QString FileDialog::saveFileFilters( FileType fileType )
{
    switch ( fileType )
    {
        case FileType::ANIMATION: return PFF_SAVE_ALL_FILE_FILTER;
        case FileType::IMAGE: return "";
        case FileType::IMAGE_SEQUENCE: return "";
        case FileType::GIF: return QString("%1 (*.gif)").arg(tr("Animated GIF"));
        case FileType::MOVIE: return "MP4 (*.mp4);; AVI (*.avi);; WebM (*.webm);; APNG (*.apng)";
        case FileType::SOUND: return "";
        case FileType::PALETTE:
            return QString("%1 (*.xml *.gpl);;%2 (*.xml);;%3 (*.gpl)")
                .arg(tr("Palette"))
                .arg(tr("Pencil2D Palette"))
                .arg(tr("GIMP Palette"));
        default: Q_ASSERT( false );
    }
    return "";
}

QString FileDialog::getFilterForFile(QString filters, QString filePath)
{
    qDebug() << __FUNCTION__ << filters << filePath;
    if(!filePath.contains("."))
    {
        return QString();
    }
    QString fileExt = filePath.remove(0, filePath.lastIndexOf(".")).prepend("*");

    QStringList filtersSplit = filters.split(";;");
    for(QString filter : filtersSplit)
    {
        int start = filter.indexOf("(");
        int end = filter.indexOf(")");
        if(start < 0 || end < 0)
        {
            continue;
        }
        start++;
        QStringList filterExts = filter.mid(start, end - start).split(" ");
        if(filterExts.contains(fileExt))
        {
            qDebug() << "Found" << filter;
            return filter.trimmed();
        }
    }

    return QString();
}

QString FileDialog::defaultFileName( FileType fileType )
{
    switch ( fileType )
    {
        case FileType::ANIMATION: return tr( "MyAnimation.pclx" );
        case FileType::IMAGE: return "untitled.png";
        case FileType::IMAGE_SEQUENCE: return "untitled.png";
        case FileType::GIF: return "untitled.gif";
        case FileType::MOVIE: return "untitled.mp4";
        case FileType::SOUND: return "untitled.wav";
        case FileType::PALETTE: return "untitled.xml";
        default: Q_ASSERT( false );
    }
    return "";
}

QString FileDialog::toSettingKey( FileType fileType )
{
    switch ( fileType )
    {
        case FileType::ANIMATION: return "Animation";
        case FileType::IMAGE: return "Image";
        case FileType::IMAGE_SEQUENCE: return "ImageSequence";
        case FileType::GIF: return "Animated GIF";
        case FileType::MOVIE: return "Movie";
        case FileType::SOUND: return "Sound";
        case FileType::PALETTE: return "Palette";
        default: Q_ASSERT( false );
    }
    return "";
}
