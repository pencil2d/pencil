#include "soundclip.h"
#include <QFile>


SoundClip::SoundClip()
{
}

SoundClip::~SoundClip()
{
    QFile::remove( fileName() );
}

Status SoundClip::init( QString strSoundFile )
{
    if ( !QFile::exists( strSoundFile ) )
    {
        return Status::ERROR_FILE_NOT_EXIST;
    }

    if ( !fileName().isEmpty() )
    {
        return Status::FAIL;
    }

    setFileName( strSoundFile );
    return Status::OK;
}

