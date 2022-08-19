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

#include "fileformat.h"
#include <QDir>
#include <QFileInfo>
#include <QMap>

bool removePFFTmpDirectory(const QString& dirName)
{
    if (dirName.isEmpty())
    {
        return false;
    }

    QDir dir(dirName);

    if (!dir.exists())
    {
        Q_ASSERT(false);
        return false;
    }

    bool result = dir.removeRecursively();
    return result;
}

QString retrieveProjectNameFromTempPath(const QString& path)
{
    QFileInfo info(path);
    QString fileName = info.completeBaseName();

    QStringList tokens = fileName.split("_");
    //qDebug() << tokens;
    return tokens[0];
}

QString detectFormatByFileNameExtension(const QString& fileName)
{
    QMap<QString, QString> extensionMapping
        {
            { "png",  "PNG" },
            { "jpg" , "JPG" },
            { "jpeg", "JPG" },
            { "tif",  "TIF" },
            { "tiff", "TIF" },
            { "bmp",  "BMP" },
            { "mp4",  "MP4" },
            { "avi",  "AVI" },
            { "gif",  "GIF" },
            { "webm", "WEBM" },
            { "apng", "APNG" },
        };

    QString extension = fileName.mid(fileName.lastIndexOf(".") + 1).toLower();
    if (!fileName.contains(".") || !extensionMapping.contains(extension))
    {
        return QString();
    }
    return extensionMapping[extension];
}

bool isMovieFormat(const QString& format)
{
    QMap<QString, bool> formatMapping
        {
            { "PNG", false },
            { "JPG", false },
            { "TIF", false },
            { "BMP", false },
            { "MP4",  true },
            { "AVI",  true },
            { "GIF",  true },
            { "WEBM", true },
            { "APNG", true },
        };

    Q_ASSERT(formatMapping.contains(format));
    return formatMapping[format];
}
