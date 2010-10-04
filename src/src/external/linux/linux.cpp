/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2009 Mj Mendoza IV

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation;

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/
#include <QFile>
#include <QProcess>
#include <QDir>
#include <QString>
#include "object.h"
#include "mainwindow.h"

void initialise() {
  // nothing, for now
}

void Object::exportMovie(int startFrame, int endFrame, QMatrix view, Layer* currentLayer, QSize exportSize, QString filePath, int fps) {
	if(!filePath.endsWith(".ogg", Qt::CaseInsensitive)) {
		filePath = filePath + ".ogg";
	}
	qDebug() << "-------OGG------";
	// --------- Export all the temporary frames ----------
	QDir::temp().mkdir("pencil");
	QString tempPath = QDir::temp().absolutePath()+"/pencil/";
	QProgressDialog progress("Exporting movie...", "Abort", 0, 100, NULL);
	progress.setWindowModality(Qt::WindowModal);
	progress.show();
	
	QDir dir2(tempPath);
	if (QFile::exists(filePath) == true) { dir2.remove(filePath); }
		
	exportFrames(startFrame, endFrame, view, currentLayer, exportSize, tempPath+"tmp", "png", 100, true, true, 2);
	// --------- Quicktime assemble call ----------
	
	QProcess ffmpeg;
	
	ffmpeg.start("ffmpeg -i " + tempPath + "tmp%03d.png -v 0  -r " + QString::number(fps) + " -y " + filePath + "");
	if (ffmpeg.waitForStarted() == true) {
		if (ffmpeg.waitForFinished() == true) {
			QByteArray sErr = ffmpeg.readAllStandardError();
			if (sErr == "") {
				qDebug() << "ERROR: Could not execute FFmpeg.";
			} else {
				progress.setValue(100);
				//qDebug() << "----------------";
				//qDebug() << sErr;
				qDebug() << "----------------";
				qDebug() << "OGG export done";
			}
		} else {
				qDebug() << "ERROR: FFmpeg did not finish executing.";
			}
	} else {
		qDebug() << "Please install FFMPEG: sudo apt-get install ffmpeg";
	}
	// --------- Clean up temp directory ---------
	QDir dir(tempPath);
	QStringList filtername("*.*");
	QStringList entries = dir.entryList(filtername,QDir::Files,QDir::Type);
	for(int i=0;i<entries.size();i++) 
		dir.remove(entries[i]);
}
