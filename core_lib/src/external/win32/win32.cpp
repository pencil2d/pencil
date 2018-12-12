/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2009 Mj Mendoza IV
Copyright (C) 2012-2018 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/

#include <cstdint>
#include <QFile>
#include <QProcess>
#include <QDir>
#include <QString>
#include <QProgressDialog>
#include <QImageReader>
#include <QImageWriter>
#include <QSettings>
#include <QDebug>

#include "object.h"
#include "editor.h"
#include "layersound.h"
#include "platformhandler.h"

namespace PlatformHandler
{
    void configurePlatformSpecificSettings() {}
}

void Editor::importMovie( QString filePath, int fps )
{
    int i = 0;
    QSettings settings( PENCIL2D, PENCIL2D );

    qDebug() << "-------IMPORT VIDEO------" << filePath;

    // --------- Import all the temporary frames ----------
    QDir::temp().mkdir( "pencil" );
    QString tempPath = QDir::temp().absolutePath() + "/pencil/";

    if ( QFile::exists( QDir::current().currentPath() + "/plugins/ffmpeg.exe" ) == true )
    {
        QProgressDialog progress( "Importing movie...", "Abort", 0, 100, NULL );
        progress.setWindowModality( Qt::WindowModal );
        progress.show();
        progress.setValue( 10 );
        QProcess ffmpeg;
        qDebug() << "./plugins/ffmpeg.exe -i \"" << filePath << "\" -r " << QString::number( fps ) << " -f image2 \"" << tempPath << "tmp_import%4d.png\"";
        ffmpeg.start( "./plugins/ffmpeg.exe -i \"" + filePath + "\" -r " + QString::number( fps ) + " -f image2 \"" + tempPath + "tmp_import%4d.png\"" );
        progress.setValue( 20 );
        if ( ffmpeg.waitForStarted() == true )
        {
            if ( ffmpeg.waitForFinished() == true )
            {
                qDebug() << "stdout: " + ffmpeg.readAllStandardOutput();
                qDebug() << "stderr: " + ffmpeg.readAllStandardError();
            }
            else
            {
                qDebug() << "ERROR: FFmpeg did not finish executing.";
            }
        }
        else
        {
            qDebug() << "ERROR: Could not execute FFmpeg.";
        }
        progress.setValue( 50 );
        QDir dir1( tempPath );
        int nFiles = dir1.entryList().count();
        i = 1;
        QString frameNumberString = QString::number( i );
        while ( frameNumberString.length() < 4 ) frameNumberString.prepend( "0" );
        while ( QFile::exists( tempPath + "tmp_import" + frameNumberString + ".png" ) )
        {
            progress.setValue( 50 + i * 50 / nFiles );
            if ( i>1 ) scrubForward();
            importImage( tempPath + "tmp_import" + frameNumberString + ".png" );
            i++;
            frameNumberString = QString::number( i );
            while ( frameNumberString.length() < 4 ) frameNumberString.prepend( "0" );
        }
        progress.setValue( 100 );
        // --------- Clean up temp directory ---------
        QDir dir( tempPath );
        QStringList filtername( "*.*" );
        QStringList entries = dir.entryList( filtername, QDir::Files, QDir::Type );
        for ( int e = 0; e < entries.size(); e++ )
            dir.remove( entries[ e ] );
    }
    else
    {
        qDebug() << "Please place ffmpeg.exe in plugins directory";
    }
}
