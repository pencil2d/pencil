#include "soundmanager.h"
#include "soundplayer.h"
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
    mSoundPlayer = new SoundPlayer( this );
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
    
    KeyFrame* key = soundLayer->getKeyFrameAt( frameNumber );
    if ( key == nullptr )
    {
        key = new SoundClip;
    }
    
    if ( !key->fileName().isEmpty() )
    {
        return Status::FAIL;
    }
    
    SoundClip* soundClip = dynamic_cast< SoundClip* >( key );
    soundClip->init( strSoundFile );
    
    Status st = mSoundPlayer->addSound( soundClip );

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
