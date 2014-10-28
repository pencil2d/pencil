
#include "pencilerror.h"
#include <map>
#include <QObject>


QString Error::msg()
{
    static std::map<ErrorCode, QString> msgMap =
    {
        // error messages.
        { PCL_OK,                        QObject::tr( "Everything ok." ) },
        { PCL_FAIL,                      QObject::tr( "Ooops, Something went wrong." ) },
        { ERROR_FILE_NOT_EXIST,      QObject::tr( "File doesn't exist." ) },
        { ERROR_FILE_CANNOT_OPEN,    QObject::tr( "Cannot open file." ) },
        { ERROR_INVALID_XML_FILE,    QObject::tr( "The file is not a valid xml document." ) },
        { ERROR_INVALID_PENCIL_FILE, QObject::tr( "The file is not valid pencil document." ) },
    };

    auto it = msgMap.find( mCode );
    if ( it == msgMap.end() )
    {
        return msgMap[ PCL_FAIL ];
    }
    return msgMap[ mCode ];
}
