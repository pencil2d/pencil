#include "soundplayer.h"
#include <QMediaPlayer>
#include "soundclip.h"

SoundPlayer::SoundPlayer( QObject* parent ) : QObject( parent )
{
}

SoundPlayer::~SoundPlayer()
{
}

Status SoundPlayer::addSound( SoundClip* clip )
{
    Q_ASSERT( clip != nullptr );

    qDebug() << "Add sound clip " << clip->fileName();

    QMediaPlayer* mediaPlayer = new QMediaPlayer;
    mediaPlayer->setMedia( QUrl::fromLocalFile( clip->fileName() ) );
    mediaPlayer->play();
    mediaPlayer->stop();

    connect( mediaPlayer, &QMediaPlayer::mediaStatusChanged, this, [ = ]( QMediaPlayer::MediaStatus mediaStatus )
    {
        if ( mediaStatus == QMediaPlayer::InvalidMedia )
        {
            clip->detachPlayer();
            Q_EMIT corruptedSoundFile( clip );
        }
    } );

    clip->addEventListener( this );
    mSoundClips.push_back( clip );
    
    
    // TODO: calc the sound length.

    return Status::OK;
}

void SoundPlayer::onKeyFrameDestroy( KeyFrame* keyFrame )
{
    auto it = std::find( mSoundClips.begin(), mSoundClips.end(), keyFrame );

    if ( it != mSoundClips.end() )
    {
        mSoundClips.erase( it );
    }
}
