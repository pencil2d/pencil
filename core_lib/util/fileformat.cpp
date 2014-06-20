/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2008-2009 Mj Mendoza IV

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation;

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/

#include <QDir>

#include "fileformat.h"


bool removePFFTmpDirectory (const QString& dirName)
{
	bool result;
    QDir dir( dirName + "/" + PFF_OLD_DATA_DIR );
	
    if ( !dir.exists() )
    {
        return false;
    }

	Q_FOREACH(QFileInfo info, dir.entryInfoList(QDir::NoDotAndDotDot | QDir::Files, QDir::DirsFirst))
	{
		result = QFile::remove(info.absoluteFilePath());

		if (!result)
			return result;
	}
	
	QDir dir2(dirName);
    result = dir2.rmdir( dirName + "/" + PFF_OLD_DATA_DIR );
	if (!result)
		return result;
	
	result = QFile::remove(dirName + "/" + PFF_XML_FILE_NAME);
	if (!result)
		return result;

	result = dir2.cdUp();
	if (!result)
		return result;

	result = dir2.rmdir(dirName);

	return result;
}
