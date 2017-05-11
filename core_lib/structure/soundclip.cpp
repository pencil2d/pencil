#include "soundclip.h"

#include <QFile>
#include <QMediaPlayer>
#include "soundplayer.h"

SoundClip::SoundClip() :
    mDuration(0)
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

void SoundClip::play()
{
    if ( mPlayer )
    {
        mPlayer->play();
    }
}

void SoundClip::playFromPosition(int frameNumber, int fps)
{
    int framesIntoSound = frameNumber - pos();
    int msPerFrame = 1000/fps;
    int msIntoSound = framesIntoSound * msPerFrame;

    if ( mPlayer )
    {
        mPlayer->setMediaPlayerPosition(msIntoSound);
        mPlayer->play();
    }
}

void SoundClip::stop()
{
    if ( mPlayer )
    {
        mPlayer->stop();
    }
}

int64_t SoundClip::duration() const
{
    return mDuration;
}

void SoundClip::setDuration(const int64_t &duration)
{
    mDuration = duration;
}

void SoundClip::updateLength(int fps)
{
    setLength(mDuration * fps / 1000.0);
}
