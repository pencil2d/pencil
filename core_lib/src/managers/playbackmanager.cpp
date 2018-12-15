/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2012-2018 Matthew Chiawen Chang

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
#include <QElapsedTimer>
#include <QDebug>
#include <QSettings>
#include "object.h"
#include "editor.h"
#include "layersound.h"
#include "layermanager.h"
#include "soundclip.h"


PlaybackManager::PlaybackManager(Editor* editor) : BaseManager(editor)
{
}

PlaybackManager::~PlaybackManager()
{
    delete mElapsedTimer;
}

bool PlaybackManager::init()
{
    mTimer = new QTimer(this);
    mTimer->setTimerType(Qt::PreciseTimer);
    QSettings settings (PENCIL2D, PENCIL2D);
    mFps = settings.value(SETTING_FPS).toInt();

    mElapsedTimer = new QElapsedTimer;
    connect(mTimer, &QTimer::timeout, this, &PlaybackManager::timerTick);
    return true;
}

Status PlaybackManager::load(Object* o)
{
    const ObjectData* e = o->data();

    mIsLooping = e->isLooping();
    mIsRangedPlayback = e->isRangedPlayback();
    mMarkInFrame = e->getMarkInFrameNumber();
    mMarkOutFrame = e->getMarkOutFrameNumber();
    mFps = e->getFrameRate();

    updateStartFrame();
    updateEndFrame();

    return Status::OK;
}

Status PlaybackManager::save(Object* o)
{
    ObjectData* data = o->data();
    data->setLooping(mIsLooping);
    data->setRangedPlayback(mIsRangedPlayback);
    data->setMarkInFrameNumber(mMarkInFrame);
    data->setMarkOutFrameNumber(mMarkOutFrame);
    data->setFrameRate(mFps);
    return Status::OK;
}

bool PlaybackManager::isPlaying()
{
    return mTimer->isActive();
}

void PlaybackManager::play()
{
    updateStartFrame();
    updateEndFrame();

    int frame = editor()->currentFrame();
    if (frame >= mEndFrame)
    {
        editor()->scrubTo(mStartFrame);
    }

    // get keyframe from layer
    KeyFrame* key = nullptr;
    if (!mListOfActiveSoundFrames.isEmpty())
    {
        for (int i = 0; i < object()->getLayerCount(); ++i)
        {
            Layer* layer = object()->getLayer(i);
            if (layer->type() == Layer::SOUND)
            {
                key = layer->getKeyFrameWhichCovers(frame);
            }
        }
    }

    // check list content before playing
    for (int pos = 0; pos < mListOfActiveSoundFrames.count(); pos++)
    {
        if (key != nullptr)
        {
            if (key->pos() + key->length() >= frame)
            {
                mListOfActiveSoundFrames.takeLast();
            }
        }
        else if (frame < mListOfActiveSoundFrames.at(pos))
        {
            mListOfActiveSoundFrames.clear();
        }
    }

    mTimer->setInterval(1000.f / mFps);
    mTimer->start();

    // for error correction, please ref skipFrame()
    mPlayingFrameCounter = 1;
    mElapsedTimer->start();

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

void PlaybackManager::setFps(int fps)
{
    if (mFps != fps)
    {
        mFps = fps;
        QSettings settings (PENCIL2D, PENCIL2D);
        settings.setValue(SETTING_FPS, fps);
        emit fpsChanged(mFps);

        // Update key-frame lengths of sound layers,
        // since the length depends on fps.
        for (int i = 0; i < object()->getLayerCount(); ++i)
        {
            Layer* layer = object()->getLayer(i);
            if (layer->type() == Layer::SOUND)
            {
                auto soundLayer = dynamic_cast<LayerSound *>(layer);
                soundLayer->updateFrameLengths(mFps);
            }
        }
    }
}

void PlaybackManager::playSounds(int frame)
{
    // If sound is turned off, don't play anything.
    if (!mIsPlaySound)
    {
        return;
    }

    std::vector< LayerSound* > kSoundLayers;
    for (int i = 0; i < object()->getLayerCount(); ++i)
    {
        Layer* layer = object()->getLayer(i);
        if (layer->type() == Layer::SOUND)
        {
            kSoundLayers.push_back(static_cast<LayerSound*>(layer));
        }
    }

    for (LayerSound* layer : kSoundLayers)
    {
        KeyFrame* key = layer->getLastKeyFrameAtPosition(frame);

        if (!layer->getVisibility())
        {
            continue;
        }

        if (key != nullptr)
        {
            // add keyframe position to list
            if (key->pos() + key->length() >= frame)
            {
                if (!mListOfActiveSoundFrames.contains(key->pos()))
                {
                    mListOfActiveSoundFrames.append(key->pos());
                }
            }
        }

        if (mCheckForSoundsHalfway)
        {
            // Check for sounds which we should start playing from part-way through.
            for (int i = 0; i < mListOfActiveSoundFrames.count(); i++)
            {
                int listPosition = mListOfActiveSoundFrames.at(i);
                if (layer->keyExistsWhichCovers(listPosition))
                {
                    key = layer->getKeyFrameWhichCovers(listPosition);
                    SoundClip* clip = static_cast<SoundClip*>(key);
                    clip->playFromPosition(frame, mFps);
                }
            }
        }
        else if (layer->keyExists(frame))
        {
            key = layer->getKeyFrameAt(frame);
            SoundClip* clip = static_cast<SoundClip*>(key);

            clip->play();

            // save the position of our active sound frame
            mActiveSoundFrame = frame;
        }

        if (frame >= mEndFrame)
        {
            if (layer->keyExists(mActiveSoundFrame))
            {
                key = layer->getKeyFrameWhichCovers(mActiveSoundFrame);
                SoundClip* clip = static_cast<SoundClip*>(key);
                clip->stop();

                // make sure list is cleared on end
                if (!mListOfActiveSoundFrames.isEmpty())
                    mListOfActiveSoundFrames.clear();
            }
        }
    }

    // Set flag to false, since this check should only be done when
    // starting play-back, or when looping.
    mCheckForSoundsHalfway = false;
}

/**
 * @brief PlaybackManager::skipFrame()
 * Small errors will accumulate while playing animation
 * If the error time is larger than a frame interval, skip a frame.
 */
bool PlaybackManager::skipFrame()
{
    // uncomment these debug output to see what happens
    //float expectedTime = (mPlayingFrameCounter) * (1000.f / mFps);
    //qDebug("Expected:  %.2f ms", expectedTime);
    //qDebug("Actual:    %d   ms", mElapsedTimer->elapsed());
    
    int t = qRound((mPlayingFrameCounter - 1) * (1000.f / mFps));
    if (mElapsedTimer->elapsed() < t)
    {
        qDebug() << "skip";
        return true;
    }
    
    ++mPlayingFrameCounter;
    return false;
}

void PlaybackManager::stopSounds()
{
    std::vector<LayerSound*> kSoundLayers;

    for (int i = 0; i < object()->getLayerCount(); ++i)
    {
        Layer* layer = object()->getLayer(i);
        if (layer->type() == Layer::SOUND)
        {
            kSoundLayers.push_back(static_cast<LayerSound*>(layer));
        }
    }

    for (LayerSound* layer : kSoundLayers)
    {
        layer->foreachKeyFrame([](KeyFrame* key)
        {
            SoundClip* clip = static_cast<SoundClip*>(key);
            clip->stop();
        });
    }
}

void PlaybackManager::timerTick()
{
    int currentFrame = editor()->currentFrame();
    playSounds(currentFrame);

    // reach the end
    if (currentFrame >= mEndFrame)
    {
        if (mIsLooping)
        {
            editor()->scrubTo(mStartFrame);
            mCheckForSoundsHalfway = true;
        }
        else
        {
            stop();
        }
        return;
    }

    if (skipFrame())
        return;

    // keep going 
    editor()->scrubForward();
}

void PlaybackManager::setLooping(bool isLoop)
{
    if (mIsLooping != isLoop)
    {
        mIsLooping = isLoop;
        emit loopStateChanged(mIsLooping);
    }
}

void PlaybackManager::enableRangedPlayback(bool b)
{
    if (mIsRangedPlayback != b)
    {
        mIsRangedPlayback = b;

        updateStartFrame();
        updateEndFrame();

        emit rangedPlaybackStateChanged(mIsRangedPlayback);
    }
}

void PlaybackManager::setRangedStartFrame(int frame)
{
    mMarkInFrame = frame;
    updateStartFrame();
}

void PlaybackManager::setRangedEndFrame(int frame)
{
    mMarkOutFrame = frame;
    updateEndFrame();
}

void PlaybackManager::updateStartFrame()
{
    mStartFrame = (mIsRangedPlayback) ? mMarkInFrame : 1;
}

void PlaybackManager::updateEndFrame()
{
    int projectLength = editor()->layers()->animationLength();
    mEndFrame = (mIsRangedPlayback) ? mMarkOutFrame : projectLength;
}

void PlaybackManager::enableSound(bool b)
{
    mIsPlaySound = b;

    if (!mIsPlaySound)
    {
        stopSounds();

        // If, during playback, the sound is turned on again,
        // check for sounds partway through.
        mCheckForSoundsHalfway = true;
    }
}
