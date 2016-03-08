#include "soundmanager.h"

//#include "soundplayer.h"
#include "object.h"
#include "layersound.h"
#include "soundclip.h"


SoundManager::SoundManager( QObject* parnet ) : BaseManager( parnet )
{
}

SoundManager::~SoundManager()
{
}

bool SoundManager::init()
{
    return true;
}

Status SoundManager::onObjectLoaded( Object* )
{
    return Status::OK;
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
        return Status::FILE_NOT_FOUND;
    }
    
    KeyFrame* key = soundLayer->getKeyFrameAt( frameNumber );
    if ( key == nullptr )
    {
        key = new SoundClip;
    }
    
    if ( !key->fileName().isEmpty() )
    {
        return Status::FAIL;
    }

    QString strCopyFile = soundLayer->object()->copyFileToDataFolder( strSoundFile );
    
    SoundClip* soundClip = dynamic_cast< SoundClip* >( key );
    soundClip->init( strSoundFile );

    Status st = createMeidaPlayer( soundClip );
    if ( !st.ok() )
    {
        delete soundClip;
        return st;
    }
    
    bool bAddOK = soundLayer->addKeyFrame( frameNumber, soundClip );
    if ( !bAddOK )
    {
        delete soundClip;
        return Status::FAIL;
    }

    return Status::OK;
}

Status SoundManager::createMeidaPlayer( SoundClip* clip )
{
    QMediaPlayer* mediaPlayer = new QMediaPlayer;
    mediaPlayer->setMedia( QUrl::fromLocalFile( clip->fileName() ) );
    mediaPlayer->play();
    //mediaPlayer->stop();
    
    qDebug() << mediaPlayer->mediaStatus();
    
    connect( mediaPlayer, &QMediaPlayer::mediaStatusChanged, this, []( QMediaPlayer::MediaStatus s )
    {
        qDebug() << "MediaStatus: " << s;
    } );

    return Status::OK;
}
