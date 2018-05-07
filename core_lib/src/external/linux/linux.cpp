/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2009 Mj Mendoza IV
Copyright (C) 2013-2017 Matt Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/
#include <QFile>
#include <QProcess>
#include <QDir>
#include <QString>
#include <QImageWriter>
#include <QImageReader>
#include <QProgressDialog>
#include <QDebug>
#include <QSettings>
#include "object.h"
#include "editor.h"
#include "layersound.h"
#include "pencildef.h"

#define MIN(a,b) ((a)>(b)?(b):(a))


qint16 safeSum ( qint16 a, qint16 b)
{
    if (((int)a + (int)b) > 32767)
        return 32767;
    if (((int)a + (int)b) < -32768)
        return -32768;
    return a+b;
}

void initialise()
{
    qDebug() << "Initialize linux: <nothing, for now>";
    // Phonon capabilities

    // QImageReader capabilities
    QList<QByteArray> formats = QImageReader::supportedImageFormats();
    foreach (QString format, formats)
    {qDebug() << "QImageReader capability: " << format;}

    // QImageWriter capabilities
    formats = QImageWriter::supportedImageFormats();
    foreach (QString format, formats)
    {qDebug() << "QImageWriter capability: " << format;}
}

void Editor::importMovie (QString filePath, int fps)
{

    int i;
    QSettings settings( PENCIL2D, PENCIL2D );

    qDebug() << "-------IMPORT VIDEO------" << filePath;

    // --------- Import all the temporary frames ----------
    QDir::temp().mkdir("pencil");
    QString tempPath = QDir::temp().absolutePath()+"/pencil/";

    QProgressDialog progress("Importing movie...", "Abort", 0, 100, NULL);
    progress.setWindowModality(Qt::WindowModal);
    progress.show();
    progress.setValue(10);
    QProcess ffmpeg;
    qDebug() << "ffmpeg -i \"" << filePath << "\" -r " << QString::number(fps) << " -f image2 \"" << tempPath << "tmp_import%4d.png\"";
    ffmpeg.start("ffmpeg -i \"" + filePath + "\" -r " + QString::number(fps) + " -f image2 \"" + tempPath + "tmp_import%4d.png\"");
    progress.setValue(20);
    if (ffmpeg.waitForStarted() == true)
    {
        if (ffmpeg.waitForFinished() == true)
        {
            QByteArray sErr = ffmpeg.readAllStandardError();
            if (sErr == "")
            {qDebug() << "ERROR: Could not execute FFmpeg.";}
            else
            {
                qDebug() << "stderr: " + ffmpeg.readAllStandardOutput();
                qDebug() << "stdout: " << sErr;
            }
        }
        else
        {qDebug() << "ERROR: FFmpeg did not finish executing.";}
    }
    else
    {qDebug() << "Please install FFMPEG: sudo apt-get install ffmpeg";}
    progress.setValue(50);
    QDir dir1(tempPath);
    int nFiles = dir1.entryList().count();
    i=1;
    QString frameNumberString = QString::number(i);
    while( frameNumberString.length() < 4) frameNumberString.prepend("0");
    while (QFile::exists(tempPath+"tmp_import"+frameNumberString+".png"))
    {
        progress.setValue(50+i*50/nFiles);
        if(i>1) scrubForward();
        importImage(tempPath+"tmp_import"+frameNumberString+".png");
        i++;
        frameNumberString = QString::number(i);
        while( frameNumberString.length() < 4) frameNumberString.prepend("0");
    }
    progress.setValue(100);
    // --------- Clean up temp directory ---------
    QDir dir(tempPath);
    QStringList filtername("*.*");
    QStringList entries = dir.entryList(filtername,QDir::Files,QDir::Type);
    for(int i=0; i<entries.size(); i++)
        dir.remove(entries[i]);

}

