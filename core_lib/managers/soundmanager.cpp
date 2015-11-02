#include "soundmanager.h"
#include "soundplayer.h"
#include "layersound.h"

#include <QMediaPlayer>


SoundManager::SoundManager( QObject* parnet ) : BaseManager( parnet )
{
}

SoundManager::~SoundManager()
{
}

bool SoundManager::init()
{
    mSoundPlayer = new SoundPlayer;
    return true;
}

Status SoundManager::loadSound( Layer* soundLayer, int frameNumber, QString strSoundFile )
{
    Q_ASSERT( soundLayer );
    if ( soundLayer->type() != Layer::SOUND )
    {
        return Status::ERROR_INVALID_LAYER_TYPE;
    }

    if ( frameNumber < 0 )
    {
        return Status::ERROR_INVALID_FRAME_NUMBER;
    }

    if ( !QFile::exists( strSoundFile ) )
    {
        return Status::ERROR_FILE_NOT_EXIST;
    }


    QMediaPlayer* mediaPlayer = new QMediaPlayer;
    mediaPlayer->setMedia( QUrl::fromLocalFile( strSoundFile ) );
    mediaPlayer->play();

    //QMediaPlayer::Error eErrorCode = mediaPlayer->error();

    connect( mediaPlayer, SIGNAL( error( QMediaPlayer::Error ) ), this, SLOT( errorHandler( QMediaPlayer::Error ) ) );

    return Status::OK;
}

void SoundManager::errorHandler( QMediaPlayer::Error errorCode )
{
    qDebug() << "kerker";
    qDebug() << errorCode;
}
