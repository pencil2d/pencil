#include "soundclip.h"
#include <QFile>


SoundClip::SoundClip()
{
}

SoundClip::~SoundClip()
{
    //QFile::remove( fileName() );
}

Status SoundClip::init( QString strSoundFile )
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

    if ( mMediaPlayer == nullptr )
    {
        return false;
    }
    auto status = mediaPlayer()->mediaStatus();
    switch ( status )
    {
        case QMediaPlayer::InvalidMedia:
        case QMediaPlayer::NoMedia:
        case QMediaPlayer::UnknownMediaStatus:
            return false;
    }
    

    return true;
}

void SoundClip::attachPlayer( QMediaPlayer* mediaPlayer )
{
    Q_ASSERT( mediaPlayer != nullptr );
    mMediaPlayer.reset( mediaPlayer );
}

void SoundClip::detachPlayer()
{
    mMediaPlayer.reset();
}

void SoundClip::mediaStatusChanged( QMediaPlayer::MediaStatus eStatus )
{
    if ( eStatus == QMediaPlayer::InvalidMedia )
    {
        detachPlayer();
    }
}

