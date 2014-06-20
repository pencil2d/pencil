
#include <unordered_map>
#include "pencilerror.h"


QString PencilError::msg()
{
    static std::unordered_map<PencilErrorCode, QString> errorMsgMap = 
    {
        // error messages.
        { PCL_OK, QObject::tr(  "Everything ok." ) },
        { PCL_FAIL, QObject::tr( "Ooops, Something went wrong." ) },
        { PCL_ERROR_FILE_NOT_EXIST, QObject::tr( "File doesn't exist." ) },
        { PCL_ERROR_FILE_CANNOT_OPEN, QObject::tr( "Cannot open file." ) },
        { PCL_ERROR_INVALID_XML_FILE, QObject::tr( "The file is not a valid xml document." ) },
        { PCL_ERROR_INVALID_PENCIL_FILE, QObject::tr( "The file is not valid pencil document." ) },
    };

    auto it = errorMsgMap.find( m_eCode );
    if ( it == errorMsgMap.end() )
    {
        return errorMsgMap[ PCL_FAIL ];
    }
    return errorMsgMap[ m_eCode ];
}
