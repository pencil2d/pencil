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
    QDir dir( dirName );
	
    if ( !dir.exists() )
    {
        Q_ASSERT( false );
        return false;
    }

    bool result = dir.removeRecursively();
	
	return result;
}
