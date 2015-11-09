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

    return Status::OK;
}
