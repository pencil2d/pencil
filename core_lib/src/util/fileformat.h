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

#define PFF_OPEN_ALL_FILE_FILTER	QObject::tr( "All Pencil Files PCLX & PCL(*.pclx *.pcl);;Pencil Animation File PCLX(*.pclx);;Old Pencil Animation File PCL(*.pcl);;Any files (*)" )
#define PFF_SAVE_ALL_FILE_FILTER	QObject::tr( "Pencil Animation File PCLX(*.pclx);;Old Pencil Animation File PCL(*.pcl)" )


#define PFF_OLD_DATA_DIR 		"data"
#define PFF_DATA_DIR            "data"
#define PFF_XML_FILE_NAME 		"main.xml"
#define PFF_TMP_DECOMPRESS_EXT 	"Y2xD"
#define PFF_PALETTE_FILE        "palette.xml"


bool removePFFTmpDirectory (const QString& dirName);
QString uniqueString(int len);


#endif
