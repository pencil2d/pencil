
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
