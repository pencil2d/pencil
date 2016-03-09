#include "soundclip.h"

#include <QFile>
#include <QMediaPlayer>
#include "soundplayer.h"

SoundClip::SoundClip()
{
}

SoundClip::~SoundClip()
{
    //QFile::remove( fileName() );
}

Status SoundClip::init( const QString& strSoundFile )
{
    if ( !fileName().isEmpty() )
    {
        return Status::FAIL;
    }

    setFileName( strSoundFile );
    return Status::OK;
}

bool SoundClip::isValid()
{
    if ( fileName().isEmpty() )
    {
        return false;
    }

    if ( mPlayer == nullptr )
    {
        return false;
    }
    
    return true;
}

void SoundClip::attachPlayer( SoundPlayer* player )
{
    Q_ASSERT( player != nullptr );
    mPlayer.reset( player );
}

void SoundClip::detachPlayer()
{
    mPlayer.reset();
}

