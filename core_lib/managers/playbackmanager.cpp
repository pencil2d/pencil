
#include "playbackmanager.h"

#include <QTimer>
#include "object.h"
#include "layersound.h"
#include "editorstate.h"
#include "editor.h"
#include "layermanager.h"



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


