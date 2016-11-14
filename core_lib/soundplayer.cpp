#include "soundplayer.h"
#include <QMediaPlayer>
#include "soundclip.h"

SoundPlayer::SoundPlayer( )
{

}

SoundPlayer::~SoundPlayer()
{
}

void SoundPlayer::init( SoundClip* clip )
{
    Q_ASSERT( clip != nullptr );
    mSoundClip = clip;

    mMediaPlayer = new QMediaPlayer( this );
    mMediaPlayer->setMedia( QUrl::fromLocalFile( clip->fileName() ) );
    makeConnections();

    clip->attachPlayer( this );
    //mMediaPlayer->play();

    qDebug() << "Seekable = " << mMediaPlayer->isSeekable();
}

void SoundPlayer::onKeyFrameDestroy( KeyFrame* keyFrame )
{
}

bool SoundPlayer::isValid()
{
    if ( mMediaPlayer )
    {
        return ( mMediaPlayer->error() == QMediaPlayer::NoError );
    }
    return false;
}

void SoundPlayer::play()
{
    if ( mMediaPlayer )
    {
        mMediaPlayer->play();
    }
}

void SoundPlayer::stop()
{
    if ( mMediaPlayer )
    {
        mMediaPlayer->stop();
    }
}

int64_t SoundPlayer::duration()
{
    if ( mMediaPlayer )
    {
        return mMediaPlayer->duration();
    }
    return 0;
}

void SoundPlayer::makeConnections()
{   
    auto errorSignal = static_cast< void ( QMediaPlayer::* )( QMediaPlayer::Error ) >( &QMediaPlayer::error );
    connect( mMediaPlayer, errorSignal, this, [ this ]( QMediaPlayer::Error err )
    {
        qDebug() << "MediaPlayer Error: " << err;
    } );

    connect( mMediaPlayer, &QMediaPlayer::durationChanged, [ this ]( qint64 duration ) 
    {
        qDebug() << "MediaPlayer durationChanged :" << duration;
        emit durationChanged( this, duration );
    } );
}
