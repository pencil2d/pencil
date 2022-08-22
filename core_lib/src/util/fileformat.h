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
#ifndef FILEFORMAT_H
#define FILEFORMAT_H

#include <QCoreApplication>
#include <QString>

//Pencil2D File Format
//PFF - acronym for "Pencil2D File Format"
#define PFF_OLD_EXTENSION				".pcl"
#define PFF_OLD_BIG_LETTER_EXTENSION	"PCL"
#define PFF_EXTENSION				    ".pclx"
#define PFF_BIG_LETTER_EXTENSION	    "PCLX"
#define PFF_BACKUP_IDENTIFIER           "backup"

#define PFF_OPEN_PROJECT_EXT_FILTER \
    QCoreApplication::translate("FileFormat", "Pencil2D formats") + " (*.pclx *.pcl);;" + QCoreApplication::translate("FileFormat", "Pencil2D Project") + " (*.pclx);;" + QCoreApplication::translate("FileFormat", "Legacy Pencil2D Project") + " (*.pcl)"

#define PFF_SAVE_PROJECT_EXT_FILTER \
    QCoreApplication::translate("FileFormat", "Pencil2D Project") + " (*.pclx);;" + QCoreApplication::translate("FileFormat", "Legacy Pencil2D Project") + " (*.pcl)"

#define PFF_MOVIE_EXT \
    QCoreApplication::translate("FileFormat", "Movie formats") + "(*.avi *.mpg *.mpeg *.mov *.mp4 *.mkv *.ogv *.swf *.flv *.webm *.wmv);;" \
        "AVI(*.avi);;MPEG(*.mpg *.mpeg);;MOV(*.mov);;MP4(*.mp4);;MKV(*.mkv);;OGV(*.ogv)" \
        ";;SWF(*.swf);;FLV(*.flv);;WEBM(*.webm);;WMV(*.wmv)"

#define PFF_IMAGE_FILTER \
   QCoreApplication::translate("FileFormat",  "Image formats") + " (*.png *.jpg *.jpeg *.bmp *.tif *.tiff);;PNG (*.png);;JPG(*.jpg *.jpeg);;BMP(*.bmp);;TIFF(*.tif *.tiff)"

#define PFF_IMAGE_SEQ_FILTER \
    QCoreApplication::translate("FileFormat",  "Image formats") + " (*.png *.jpg *.jpeg *.bmp *.tif *.tiff);;PNG (*.png);;JPG(*.jpg *.jpeg);;BMP(*.bmp);;TIFF(*.tif *.tiff)"

#define PFF_PALETTE_EXT_FILTER \
    QCoreApplication::translate("FileFormat", "Palette formats") + " (*.xml *.gpl);;" + QCoreApplication::translate("FileFormat", "Pencil2D Palette") + " (*.xml);;" + QCoreApplication::translate("FileFormat", "GIMP Palette") + " (*.gpl)"

#define PFF_GIF_EXT_FILTER \
    QCoreApplication::translate("FileFormat", "Animated GIF") + " (*.gif)"

#define PFF_SOUND_EXT_FILTER \
    QCoreApplication::translate("FileFormat", "Sound formats") + " (*.wav *.mp3 *.wma *.ogg *.flac *.opus *.aiff *.aac *.caf);;WAV (*.wav);;MP3 (*.mp3);;WMA (*.wma);;OGG (*.ogg);;FLAC (*.flac);;Opus (*.opus);;AIFF (*.aiff);;AAC (*.aac);;CAF (*.caf)"


#define PFF_DEFAULT_PROJECT_EXT \
    QString(".pclx")

#define PFF_DEFAULT_IMAGE_EXT \
   QString(".png")

#define PFF_DEFAULT_IMAGE_SEQ_EXT \
    QString(".png")

#define PFF_DEFAULT_ANIMATED_EXT \
    QString(".gif")

#define PFF_DEFAULT_PALETTE_EXT \
    QString(".xml")

#define PFF_OLD_DATA_DIR 		"data"
#define PFF_DATA_DIR            "data"
#define PFF_XML_FILE_NAME 		"main.xml"
#define PFF_TMP_DECOMPRESS_EXT 	"Y2xD"
#define PFF_PALETTE_FILE        "palette.xml"

bool removePFFTmpDirectory(const QString& dirName);
QString retrieveProjectNameFromTempPath(const QString& path);

QString detectFormatByFileNameExtension(const QString& fileName);
bool isMovieFormat(const QString& format);

#endif // FILEFORMAT_H
