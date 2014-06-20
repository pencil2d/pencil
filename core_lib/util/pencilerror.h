#ifndef PENCILERROR_H
#define PENCILERROR_H

#include <QString>


enum Result
{
    PCL_OK = 0,
    PCL_FAIL,
    
    // for Object loading
    PCL_ERROR_FILE_NOT_EXIST,
    PCL_ERROR_FILE_CANNOT_OPEN,
    PCL_ERROR_INVALID_XML_FILE,
    PCL_ERROR_INVALID_PENCIL_FILE
};


class PencilError
{
public:
    PencilError() { m_eCode = PCL_OK; }
    PencilError( Result eCode ) { m_eCode = eCode; }
    Result code() { return m_eCode; }
    QString msg();

private:
    Result m_eCode;
};

#endif // PENCILERROR_H
