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

#include "soundclip.h"

#include <QFile>
#include <QMediaPlayer>
#include "soundplayer.h"

SoundClip::SoundClip()
{
}

SoundClip::SoundClip(const SoundClip& s2) : KeyFrame(s2)
{
    mOriginalSoundClipName = s2.mOriginalSoundClipName;
}

SoundClip::~SoundClip()
{
    //QFile::remove( fileName() );
}

SoundClip* SoundClip::clone()
{
    return new SoundClip(*this);
}

Status SoundClip::init(const QString& strSoundFile)
{
    if ( strSoundFile.isEmpty() )
    {
        return Status::FAIL;
    }
    setFileName( strSoundFile );
    return Status::OK;
}

bool SoundClip::isValid()
{
    if ( fileName().isEmpty() )
    {
        return false;
    }

    if ( mPlayer == nullptr )
    {
        return false;
    }
    
    return true;
}

void SoundClip::attachPlayer( SoundPlayer* player )
{
    Q_ASSERT( player != nullptr );
    mPlayer.reset( player );
}

void SoundClip::detachPlayer()
{
    mPlayer.reset();
}

void SoundClip::play()
{
    if ( mPlayer )
    {
        mPlayer->play();
    }
}

void SoundClip::playFromPosition(int frameNumber, int fps)
{
    int framesIntoSound = frameNumber;
    if ( pos() > 1 ) {
        framesIntoSound = frameNumber - pos();
    }
    float msPerFrame = ( 1000 / fps );
    float msIntoSound = framesIntoSound * msPerFrame;

    if ( mPlayer )
    {
        mPlayer->setMediaPlayerPosition(msIntoSound);
        mPlayer->play();
    }
}

void SoundClip::stop()
{
    if ( mPlayer )
    {
        mPlayer->stop();
    }
}

int64_t SoundClip::duration() const
{
    return mDuration;
}

void SoundClip::setDuration(const int64_t &duration)
{
    mDuration = duration;
}

void SoundClip::updateLength(int fps)
{
    setLength(mDuration * fps / 1000.0);
}
