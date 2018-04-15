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

#ifndef LOG_H
#define LOG_H

#if QT_VERSION >= QT_VERSION_CHECK( 5, 2, 0 )
	#include<QLoggingCategory>
    #define ENABLE_DEBUG_LOG( Logger, OnOff ) Logger.setEnabled( QtDebugMsg, OnOff )
#endif

#if QT_VERSION < QT_VERSION_CHECK( 5, 2, 0 )
    #define QLoggingCategory QString
    #define qCDebug( C ) qDebug()
    #define qCWarning( C ) qWarning()
    #define ENABLE_DEBUG_LOG( Logger, OnOff )
#endif

#endif // LOG_H
