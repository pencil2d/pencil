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

    QMediaPlayer* mediaPlayer = new QMediaPlayer( this );
    mediaPlayer->setMedia( QUrl::fromLocalFile( clip->fileName() ) );

    qDebug() << mediaPlayer->mediaStatus();

    clip->attachPlayer( this );

    mMediaPlayer = mediaPlayer;
    mMediaPlayer->play();
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

void SoundPlayer::makeConnections()
{   
    QObject::connect( mMediaPlayer, &QMediaPlayer::mediaStatusChanged, this, [ this ]( QMediaPlayer::MediaStatus s )
    {
        // WARNING :
        // This call is not supported in QT 5.3. Is it necessary?
        //
        QMediaPlayer* mediaPlayer = ( QMediaPlayer* )QObject::sender();
        qDebug() << "MediaStatus: " << s;
        qDebug() << "Duration:" << mediaPlayer->duration();

        switch ( s )
        {
            case QMediaPlayer::BufferedMedia:
                break;
        }
    } );

    auto errorSignal = static_cast< void ( QMediaPlayer::* )( QMediaPlayer::Error ) >( &QMediaPlayer::error );
    connect( mMediaPlayer, errorSignal, this, [ this ]( QMediaPlayer::Error error )
    {
        qDebug() << "MediaPlayer Error: " << error;
    } );
}
