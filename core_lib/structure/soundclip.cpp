/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2012-2017 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation;

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
    int kk = 0;
}

SoundClip::~SoundClip()
{
    //QFile::remove( fileName() );
}

Status SoundClip::init( const QString& strSoundFile )
{
    if ( !fileName().isEmpty() )
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
    int framesIntoSound = frameNumber - pos();
    int msPerFrame = 1000/fps;
    int msIntoSound = framesIntoSound * msPerFrame;

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
