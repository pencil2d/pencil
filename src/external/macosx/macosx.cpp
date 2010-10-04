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
#include "mainwindow.h"
#include "style.h"

#include <CoreFoundation/CoreFoundation.h>

void initialise() {
	qApp->setStyle(new AquaStyle());
}

void Object::exportMovie(int startFrame, int endFrame, QMatrix view, Layer* currentLayer, QSize exportSize, QString filePath, int fps) {
	if(!filePath.endsWith(".mov", Qt::CaseInsensitive)) {
		filePath = filePath + ".mov";
	}
	qDebug() << "-------QuickTime------" << filePath << QDir::temp().absolutePath();
	// --------- Export all the temporary frames ----------
	
	QDir::temp().mkdir("pencil");
	QString tempPath = QDir::tempPath()+"pencil/";
	QProgressDialog progress("Exporting movie...", "Abort", 0, 100, NULL);
	progress.setWindowModality(Qt::WindowModal);
	progress.show();
	
  exportFrames(startFrame, endFrame, view, currentLayer, exportSize, tempPath+"tmp", "jpg", 100, true, true, 2);
	qDebug() << "frames exported in temp directory";
	
	// --------- Quicktime assemble call ----------
	
	// --- locate the assembler ---
	CFURLRef pluginRef = CFBundleCopyBundleURL(CFBundleGetMainBundle());
	CFStringRef macPath = CFURLCopyFileSystemPath(pluginRef,
                                           kCFURLPOSIXPathStyle);
	const char *pathPtr = CFStringGetCStringPtr(macPath,
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
	for(int i=0;i<entries.size();i++) 
		dir.remove(entries[i]);
}
