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

#include "pencilerror.h"
#include <map>
#include <QObject>
#include <QSysInfo>
#include "pencildef.h"

Status::Status(Status::ErrorCode eCode, QStringList detailsList, QString title, QString description)
    : mCode( eCode )
    , mTitle( title )
    , mDescription( description )
    , mDetails( detailsList )
{
}

QString Status::msg()
{
    static std::map<ErrorCode, QString> msgMap =
    {
        // error messages.
        { OK,                    QObject::tr( "Everything ok." ) },
        { FAIL,                  QObject::tr( "Ooops, Something went wrong." ) },
        { FILE_NOT_FOUND,        QObject::tr( "File doesn't exist." ) },
        { ERROR_FILE_CANNOT_OPEN,    QObject::tr( "Cannot open file." ) },
        { ERROR_INVALID_XML_FILE,    QObject::tr( "The file is not a valid xml document." ) },
        { ERROR_INVALID_PENCIL_FILE, QObject::tr( "The file is not valid pencil document." ) },
    };

    auto it = msgMap.find( mCode );
    if ( it == msgMap.end() )
    {
        return msgMap[ FAIL ];
    }
    return msgMap[ mCode ];
}

QString Status::details()
{
    QString details = mDetails.join("<br>");
    details.append("<br><br>");
    details.append( QString(
                        "Error Display<br>"
                        "Title: %1<br>"
                        "Description: %2"
                        ).arg( mTitle,
                               mDescription )
                    );
    details.append("<br><br>");
#if QT_VERSION >= 0x050400
    details.append( QString(
                        "System Info<br>"
                        "Pencil version: %1<br>"
                        "Build ABI: %2<br>"
                        "Kernel: %3 %4<br>"
                        "Product name: %5"
                        ).arg( APP_VERSION,
                               QSysInfo::buildAbi(),
                               QSysInfo::kernelType(),
                               QSysInfo::kernelVersion(),
                               QSysInfo::prettyProductName() )
                    );
#endif
    return details;
}

bool Status::operator==( Status::ErrorCode code ) const
{
    return ( mCode == code );
}
