
#include <QTimer>
#include "editor.h"
#include "layermanager.h"
#include "playbackmanager.h"
#include "object.h"
#include "layer.h"
#include "layersound.h"


PlaybackManager::PlaybackManager( QObject* parent ) : BaseManager( parent )
{
}

bool PlaybackManager::init()
{
    mTimer = new QTimer( this );
    connect( mTimer, &QTimer::timeout, this, &PlaybackManager::timerTick );
    return true;
}

Status PlaybackManager::onObjectLoaded( Object* )
{
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
}

void PlaybackManager::setFps( int fps )
{
    if ( mFps != fps )
    {
        mFps = fps;
        emit fpsChanged( mFps );
    }
}

void PlaybackManager::timerTick()
{
    if ( editor()->currentFrame() > mEndFrame )
    {
        if ( mIsLooping )
        {
            editor()->scrubTo( mStartFrame );
        }
        else
        {
            stop();
        }
        return;
    }

    editor()->scrubTo( editor()->currentFrame() + 1 );

    playSoundIfAny( editor()->currentFrame() );
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

void PlaybackManager::playSoundIfAny(int frame)
{
    for ( int i = 0; i < editor()->object()->getLayerCount(); ++i)
    {
        auto layer = editor()->object()->getLayer( i );
        if ( layer->type() == Layer::SOUND )
        {
            auto soundLayer = static_cast< LayerSound* >( layer );
            soundLayer->playSound( frame );
        }
    }
}


