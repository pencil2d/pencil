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

#include "fileformat.h"
#include <QDir>

bool removePFFTmpDirectory (const QString& dirName)
{
    if ( dirName.isEmpty() )
    {
        return false;
    }

    QDir dir( dirName );
	
    if ( !dir.exists() )
    {
        Q_ASSERT( false );
        return false;
    }

    bool result = dir.removeRecursively();
	
	return result;
}
