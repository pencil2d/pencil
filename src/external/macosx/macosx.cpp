/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation;

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/
#include <QString>
#include <QStringList>
#include <QDir>
#include <QProgressDialog>
#include <QProcess>

#include "object.h"
#include "editor.h"
#include "mainwindow2.h"
#include "style.h"

#include <CoreFoundation/CoreFoundation.h>

void initialise()
{
    qApp->setStyle(new AquaStyle());
}

//void Object::exportMovie(int startFrame, int endFrame, QMatrix view, Layer* currentLayer, QSize exportSize, QString filePath, int fps, int exportFps, QString exportFormat);

void Object::exportMovie(int startFrame, int endFrame, QMatrix view, Layer* currentLayer, QSize exportSize, QString filePath, int fps, int exportFps, QString exportFormat)
{
    Q_UNUSED(startFrame);
    Q_UNUSED(endFrame);
    Q_UNUSED(view);
    Q_UNUSED(currentLayer);
    Q_UNUSED(exportSize);
    Q_UNUSED(filePath);
    Q_UNUSED(fps);
    Q_UNUSED(exportFps);
    Q_UNUSED(exportFormat);
    /*
    if(!filePath.endsWith(".mov", Qt::CaseInsensitive))
    {
        filePath = filePath + ".mov";
    }
    qDebug() << "-------QuickTime------" << filePath << QDir::temp().absolutePath();
    // --------- Export all the temporary frames ----------

    QDir::temp().mkdir("pencil");g
    QString tempPath = QDir::tempPath()+"pencil/";
    QProgressDialog progress("Exporting movie...", "Abort", 0, 100, NULL);
    progress.setWindowModality(Qt::WindowModal);
    progress.show();
    //exportFrames1(startFrame, endFrame, view, currentLayer, exportSize, tempPath+"tmp", "png", 100, true, true, 2,&progress,50,fps,exportFps);

    exportFrames1(startFrame, endFrame, view, currentLayer, exportSize, tempPath+"tmp", "jpg", 100, true, true, 2, &progress, 50, fps, exportFps);
    qDebug() << "frames exported in temp directory";

    // --------- Quicktime assemble call ----------

    // --- locate the assembler ---
    CFURLRef pluginRef = CFBundleCopyBundleURL(CFBundleGetMainBundle());
    CFStringRef macPath = CFURLCopyFileSystemPath(pluginRef,
                          kCFURLPOSIXPathStyle);
    const char* pathPtr = CFStringGetCStringPtr(macPath,
                          CFStringGetSystemEncoding());
    CFRelease(pluginRef);
    CFRelease(macPath);
    QString appPath = pathPtr;

    // --- run the assembler ---
    QProcess assemble;
    QStringList args;
    args << QString::number(endFrame+1) << tempPath+"tmp%03d.jpg" << QString::number(fps) << filePath;
    assemble.start(appPath+"/Contents/Resources/assembler2",args);
    assemble.waitForStarted();
    assemble.waitForFinished();
    qDebug() << "exit status " << assemble.exitStatus();
    qDebug() << assemble.readAllStandardOutput();
    qDebug() << assemble.readAllStandardError();

    progress.setValue(100);
    qDebug() << "QuickTime export done";

    // --------- Clean up temp directory ---------

    QDir dir(tempPath);
    QStringList filtername("*.*");
    QStringList entries = dir.entryList(filtername,QDir::Files,QDir::Type);
    for(int i=0; i<entries.size(); i++)
        dir.remove(entries[i]);
        */
}

void Editor::importMovie (QString filePath, int fps)
{

    int i;
    QSettings settings("Pencil","Pencil");

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
