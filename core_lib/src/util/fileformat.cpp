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

QString uniqueString(int len)
{
    static const char alphanum[] = "0123456789abcdefghijklmnopqrstuvwxyz";
    const int alphanum_len = sizeof(alphanum);

    if (len > 128) len = 128;

    char s[128 + 1];
    for (int i = 0; i < len; ++i)
    {
        s[i] = alphanum[rand() % (alphanum_len - 1)];
    }
    s[len] = 0;
    return QString::fromUtf8(s);
}
