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
#include "platformhandler.h"

#define MIN(a,b) ((a)>(b)?(b):(a))

namespace PlatformHandler
{
    void configurePlatformSpecificSettings() {}
}

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
