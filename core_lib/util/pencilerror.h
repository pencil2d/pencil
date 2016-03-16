#ifndef PENCILERROR_H
#define PENCILERROR_H

#include <QString>



class Status
{
public:

    enum ErrorCode
    {
        OK = 0,
        SAFE,
        FAIL,
        FILE_NOT_FOUND,
        CORRUPTION,
        NOT_SUPPORTED,
        INVALID_ARGUMENT,
        NOT_IMPLEMENTED_YET,

        // for Object loading
        ERROR_FILE_CANNOT_OPEN,
        ERROR_INVALID_XML_FILE,
        ERROR_INVALID_PENCIL_FILE,

        //
        ERROR_INVALID_LAYER_TYPE,
        ERROR_INVALID_FRAME_NUMBER,
        ERROR_LOAD_IMAGE_FAIL,

        // Sound
        ERROR_LOAD_SOUND_FILE,
    };


    Status() {}
    Status( ErrorCode eCode ) { mCode = eCode; }

    ErrorCode code() { return mCode; }
    bool      ok() const { return mCode == OK; }
    QString   msg();

    bool operator==( ErrorCode code );

private:
    ErrorCode mCode = OK;
};

#endif // PENCILERROR_H
