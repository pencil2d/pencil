/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2012-2018 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/
#ifndef PENCIL_FILE_FORMAT_H
#define PENCIL_FILE_FORMAT_H

#include <QString>

//Pencil File Format
//PFF - acronym for "Pencil File Format"
#define PFF_OLD_EXTENSION				".pcl"
#define PFF_OLD_BIG_LETTER_EXTENSION	"PCL"
#define PFF_EXTENSION				    ".pclx"
#define PFF_BIG_LETTER_EXTENSION	    "PCLX"

#define PFF_PROJECT_EXT_FILTER \
    QObject::tr("Supported Pencil formats") + " (*.pclx *.pcl);;" + QObject::tr("Pencil Project") + " (*.pclx);;" + QObject::tr("Legacy Pencil Project") + " (*.pcl)"

#define PFF_MOVIE_EXT \
    QObject::tr("Supported Movie formats") + " AVI (*.avi);;MPEG(*.mpg);;MOV(*.mov);;MP4(*.mp4);;SWF(*.swf);;FLV(*.flv);;WMV(*.wmv)"

#define PFF_IMAGE_FILTER \
   QObject::tr( "Supported Image formats") + " (*.png *.jpg *.jpeg *.bmp *.tif *.tiff);;PNG (*.png);;JPG(*.jpg *.jpeg);;BMP(*.bmp);; TIFF(*.tif *.tiff)"

#define PFF_IMAGE_SEQ_FILTER \
    QObject::tr( "Supported Image formats") + " (*.png *.jpg *.jpeg *.bmp *.tif *.tiff);;PNG (*.png);;JPG(*.jpg *.jpeg);;BMP(*.bmp);; TIFF(*.tif *.tiff)"

#define PFF_PALETTE_EXT_FILTER \
    QObject::tr("Supported Palette formats") + " (*.xml *.gpl);;" + QObject::tr("Pencil Palette") + " (*.xml);;" + QObject::tr("GIMP Palette") + " (*.gpl)"

#define PFF_GIF_EXT_FILTER \
    QObject::tr("Animated GIF") + " (*.gif)"

#define PFF_SOUND_EXT_FILTER \
    QObject::tr("Supported Sound formats") + " (*.wav *.mp3);;WAV (*.wav);;MP3 (*.mp3)"


#define PFF_OLD_DATA_DIR 		"data"
#define PFF_DATA_DIR            "data"
#define PFF_XML_FILE_NAME 		"main.xml"
#define PFF_TMP_DECOMPRESS_EXT 	"Y2xD"
#define PFF_PALETTE_FILE        "palette.xml"

bool removePFFTmpDirectory (const QString& dirName);
QString uniqueString(int len);


#endif
