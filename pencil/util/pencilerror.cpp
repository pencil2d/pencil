
#include <QMap>
#include "pencilerror.h"


QString PencilError::msg()
{
    static QMap<PencilErrorCode, QString> errorMsgMap;

    if ( errorMsgMap.count() <= 0 )
    {
        // error messages.
        errorMsgMap.insert( PCL_OK, "Everything ok." );
        errorMsgMap.insert( PCL_ERROR_FILE_NOT_EXIST, "File doesn't exist." );
        errorMsgMap.insert( PCL_ERROR_FILE_CANNOT_OPEN, "Cannot open file." );
        errorMsgMap.insert( PCL_ERROR_INVALID_XML_FILE, "The file is not a valid xml document.");
        errorMsgMap.insert( PCL_ERROR_INVALID_PENCIL_FILE, "The file is not valid pencil document.");
    }

    if ( !errorMsgMap.contains( m_eCode ) )
    {
        return "Oops, Something went wrong.";
    }

    return errorMsgMap[ m_eCode ];
}
