
#include "playbackmanager.h"

#include <QTimer>
#include "object.h"
#include "editor.h"
#include "layersound.h"
#include "editorstate.h"
#include "layermanager.h"
#include "soundmanager.h"
#include "soundclip.h"
#include "soundplayer.h"

PlaybackManager::PlaybackManager( QObject* parent ) : BaseManager( parent )
{
}

bool PlaybackManager::init()
{
    mTimer = new QTimer( this );
    connect( mTimer, &QTimer::timeout, this, &PlaybackManager::timerTick );
    return true;
}

Status PlaybackManager::onObjectLoaded( Object* o )
{
    const EditorState* e = o->editorState();
    
    mIsLooping        = e->mIsLoop;
    mIsRangedPlayback = e->mIsRangedPlayback;
    mMarkInFrame      = e->mMarkInFrame;
    mMarkOutFrame     = e->mMarkOutFrame;
    mFps              = e->mFps;

    return Status::OK;
}

bool PlaybackManager::isPlaying()
{
    return mTimer->isActive();
}

void PlaybackManager::play()
{
    int projectLength = editor()->layers()->projectLength();

    mStartFrame = ( mIsRangedPlayback ) ? mMarkInFrame : 1;
    mEndFrame = ( mIsRangedPlayback ) ? mMarkOutFrame : projectLength;

    if ( editor()->currentFrame() >= mEndFrame )
    {
        editor()->scrubTo( mStartFrame );
    }

    mTimer->setInterval( 1000.0f / mFps );
    mTimer->start();
}

void PlaybackManager::stop()
{
    mTimer->stop();
    stopSounds();
}

void PlaybackManager::setFps( int fps )
{
    if ( mFps != fps )
    {
        mFps = fps;
        emit fpsChanged( mFps );
    }
}

void PlaybackManager::playSounds( int frame )
{
    std::vector< LayerSound* > kSoundLayers;
    for ( int i = 0; i < object()->getLayerCount(); ++i )
    {
        Layer* layer = object()->getLayer( i );
        if ( layer->type() == Layer::SOUND )
        {
            kSoundLayers.push_back( static_cast< LayerSound* >( layer ) );
        }
    }

    for ( LayerSound* layer : kSoundLayers )
    {
        if ( layer->keyExists( frame ) )
        {
            KeyFrame* key = layer->getKeyFrameAt( frame );
            SoundClip* clip = static_cast< SoundClip* >( key );

            clip->play();
        }
    }
}

void PlaybackManager::stopSounds()
{
    std::vector< LayerSound* > kSoundLayers;
    for ( int i = 0; i < object()->getLayerCount(); ++i )
    {
        Layer* layer = object()->getLayer( i );
        if ( layer->type() == Layer::SOUND )
        {
            kSoundLayers.push_back( static_cast< LayerSound* >( layer ) );
        }
    }

    for ( LayerSound* layer : kSoundLayers )
    {
        layer->foreachKeyFrame( []( KeyFrame* key )
        {
            SoundClip* clip = static_cast< SoundClip* >( key );
            clip->stop();
        } );
    }
}

void PlaybackManager::timerTick()
{
    playSounds( editor()->currentFrame() );

    if ( editor()->currentFrame() >= mEndFrame )
    {
        if ( mIsLooping )
        {
            editor()->scrubTo( mStartFrame );
        }
        else
        {
            stop();
        }
    }
    else
    {
        editor()->scrubForward();
    }
}

void PlaybackManager::setLooping( bool isLoop )
{
    if ( mIsLooping != isLoop )
    {
        mIsLooping = isLoop;
        emit loopStateChanged( mIsLooping );
    }
}

void PlaybackManager::enableRangedPlayback( bool b )
{
    if ( mIsRangedPlayback != b )
    {
        mIsRangedPlayback = b;
        emit rangedPlaybackStateChanged( mIsRangedPlayback );
    }
}


