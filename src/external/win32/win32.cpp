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
  qDebug() << "Initialize: <nothing, for now>";
}

void Object::exportMovie(int startFrame, int endFrame, QMatrix view, Layer* currentLayer, QSize exportSize, QString filePath, int fps) {
/*	if(!filePath.endsWith(".mov", Qt::CaseInsensitive)) {
		filePath = filePath + ".mov";
	}
	if(!filePath.endsWith(".avi", Qt::CaseInsensitive)) {
			filePath = filePath + ".avi";
		}*/
	qDebug() << "-------VIDEO------";
	// --------- Export all the temporary frames ----------
	QDir::temp().mkdir("pencil");
	QString tempPath = QDir::temp().absolutePath()+"/pencil/";
	QProgressDialog progress("Exporting movie...", "Abort", 0, 100, NULL);
	progress.setWindowModality(Qt::WindowModal);
	progress.show();

	QDir dir2(filePath);
	if (QFile::exists(QDir::current().currentPath() +"/plugins/ffmpeg.exe") == true) {
		if (QFile::exists(filePath) == true) { dir2.remove(filePath); }

		exportFrames(startFrame, endFrame, view, currentLayer, exportSize, tempPath+"tmp", "png", 100, true, true, 2);
		// --------- Quicktime assemble call ----------
		QDir sampledir;
		qDebug() << "testmic:" << sampledir.filePath(filePath);
		QProcess ffmpeg;

		qDebug() << "Trying to export VIDEO";

		ffmpeg.start("./plugins/ffmpeg.exe -i " + tempPath + "tmp%03d.png -v 0 -r " + QString::number(fps) + " -y \"" + filePath + "\"");
		if (ffmpeg.waitForStarted() == true) {
			if (ffmpeg.waitForFinished() == true) {
				/*
					qDebug() << ffmpeg.readAllStandardOutput();
					qDebug() << ffmpeg.readAllStandardError();

					qDebug() << "dbg:" << QDir::current().currentPath() +"/plugins/";
					qDebug() << ":" << tempPath + "tmp%03d.png";
					qDebug() << ":\"" + filePath + "\"";
				 */

				qDebug() << "VIDEO export done.";
			} else {
				qDebug() << "ERROR: FFmpeg did not finish executing.";
			}
		} else {
			qDebug() << "ERROR: Could not execute FFmpeg.";
		}
		// --------- Clean up temp directory ---------
		QDir dir(tempPath);
		QStringList filtername("*.*");
		QStringList entries = dir.entryList(filtername,QDir::Files,QDir::Type);
		for(int i=0;i<entries.size();i++)
			dir.remove(entries[i]);
	} else {
		qDebug() << "Please place ffmpeg.exe in plugins directory";
	}
	qDebug() << "-----";
}
