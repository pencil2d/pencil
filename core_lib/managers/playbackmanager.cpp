/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2012-2017 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/

#include "playbackmanager.h"

#include <QTimer>
#include "object.h"
#include "editor.h"
#include "layersound.h"
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

Status PlaybackManager::load( Object* o )
{
    const ObjectData* e = o->data();
    
    mIsLooping        = e->isLooping();
    mIsRangedPlayback = e->isRangedPlayback();
    mMarkInFrame      = e->getMarkInFrameNumber();
    mMarkOutFrame     = e->getMarkOutFrameNumber();
    mFps              = e->getFrameRate();

    return Status::OK;
}

Status PlaybackManager::save( Object* o )
{
	ObjectData* data = o->data();
	data->setLooping( mIsLooping );
	data->setRangedPlayback( mIsRangedPlayback );
	data->setMarkInFrameNumber( mMarkInFrame );
	data->setMarkOutFrameNumber( mMarkOutFrame );
	data->setFrameRate( mFps );
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

    if ( ( editor()->currentFrame() >= mEndFrame ) ||
         ( editor()->currentFrame() >= mEndFrame && mIsRangedPlayback ) )
    {
        editor()->scrubTo( mStartFrame );
    }

    // TODO: make proper timer for counting the timeline.. Qtimer is not accurate for such a task
    mTimer->setInterval( 1000.0f / mFps );
    mTimer->start();

    // Check for any sounds we should start playing part-way through.
    mCheckForSoundsHalfway = true;

    emit playStateChanged(true);
}

void PlaybackManager::stop()
{
    mTimer->stop();
    stopSounds();
    emit playStateChanged(false);
}

void PlaybackManager::setFps( int fps )
{
    if ( mFps != fps )
    {
        mFps = fps;
        emit fpsChanged( mFps );

        // Update key-frame lengths of sound layers,
        // since the length depends on fps.
        for ( int i = 0; i < object()->getLayerCount(); ++i )
        {
            Layer* layer = object()->getLayer( i );
            if ( layer->type() == Layer::SOUND )
            {
                auto soundLayer = dynamic_cast<LayerSound *>(layer);
                soundLayer->updateFrameLengths(mFps);
            }
        }
    }
}

void PlaybackManager::playSounds( int frame )
{
    // If sound is turned off, don't play anything.
    if(!mIsPlaySound)
    {
        return;
    }

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
        if (mCheckForSoundsHalfway)
        {
            // Check for sounds which we should start playing from part-way through.
            if ( layer->keyExistsWhichCovers( frame ) )
            {
                KeyFrame* key = layer->getKeyFrameWhichCovers( frame );
                SoundClip* clip = static_cast< SoundClip* >( key );

                clip->playFromPosition(frame, mFps);
            }

            // Set flag to false, since this check should only be done when
            // starting play-back, or when looping.
            mCheckForSoundsHalfway = false;
        }
        else if ( layer->keyExists( frame ) )
        {
            KeyFrame* key = layer->getKeyFrameAt( frame );
            SoundClip* clip = static_cast< SoundClip* >( key );

            clip->play();

            // save the position of our active sound frame
            mActiveSoundFrame = frame;
        }

        if ( frame >= mEndFrame )
        {
            if (layer->keyExists(mActiveSoundFrame)){
                KeyFrame* key = layer->getKeyFrameWhichCovers( mActiveSoundFrame );
                SoundClip* clip = static_cast< SoundClip* >( key );
                clip->stop();
            }
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
    int currentFrame = editor()->currentFrame();
    playSounds( currentFrame );

    if ( ( currentFrame >= mEndFrame ) ||
         ( currentFrame >= mEndFrame && mIsRangedPlayback ) )
    {
        if ( mIsLooping )
        {
            // make sure to stop all sounds before scrubbing to start again
            stopSounds();
            editor()->scrubTo( mStartFrame );
            mCheckForSoundsHalfway = true;
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

void PlaybackManager::enableSound(bool b)
{
    mIsPlaySound = b;

    if(!mIsPlaySound)
    {
        stopSounds();

        // If, during playback, the sound is turned on again,
        // check for sounds partway through.
        mCheckForSoundsHalfway = true;
    }
}


