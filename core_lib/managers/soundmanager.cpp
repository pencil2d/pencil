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

#include "soundmanager.h"

#include <QString>
#include "object.h"
#include "layersound.h"
#include "soundclip.h"
#include "soundplayer.h"

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

Status SoundManager::load( Object* obj )
{
    int count = obj->getLayerCount();
    for ( int i = 0; i < count; ++i )
    {
        Layer* layer = obj->getLayer( i );
        if ( layer->type() != Layer::SOUND )
        {
            continue;
        }

        LayerSound* soundLayer = static_cast< LayerSound* >( layer );

        soundLayer->foreachKeyFrame( [ this ]( KeyFrame* key )
        {
            SoundClip* clip = dynamic_cast< SoundClip* >( key );
            Q_ASSERT( clip );

            createMediaPlayer( clip );
        } );
    }
    return Status::OK;
}

Status SoundManager::save( Object* )
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
        soundLayer->addKeyFrame( frameNumber, key );
    }

    if ( !key->fileName().isEmpty() )
    {
        return Status::FAIL;
    }

    QString strCopyFile = soundLayer->object()->copyFileToDataFolder( strSoundFile );
    Q_ASSERT( !strCopyFile.isEmpty() );

    SoundClip* soundClip = dynamic_cast< SoundClip* >( key );
    soundClip->init( strCopyFile );

    Status st = createMediaPlayer( soundClip );
    if ( !st.ok() )
    {
        delete soundClip;
        return st;
    }

    return Status::OK;
}

Status SoundManager::loadSound( SoundClip* soundClip, QString strSoundFile )
{
    Q_ASSERT( soundClip );

    if ( !QFile::exists( strSoundFile ) )
    {
        return Status::FILE_NOT_FOUND;
    }

    if ( !soundClip->fileName().isEmpty() )
    {
        return Status::FAIL;
    }

    QString strCopyFile = editor()->object()->copyFileToDataFolder( strSoundFile );
    Q_ASSERT( !strCopyFile.isEmpty() );

    soundClip->init( strCopyFile );

    Status st = createMediaPlayer( soundClip );
    if ( !st.ok() )
    {
        delete soundClip;
        return st;
    }

    return Status::OK;
}

void SoundManager::onDurationChanged( SoundPlayer* player, int64_t duration )
{
    SoundClip* clip = player->clip();

    double fps = static_cast< double >( editor()->fps() );

    double frameLength = duration * fps / 1000.0;
    clip->setLength( frameLength );

    emit soundClipDurationChanged();
}

Status SoundManager::createMediaPlayer( SoundClip* clip )
{
    SoundPlayer* newPlayer = new SoundPlayer();
    newPlayer->init( clip );

    connect( newPlayer, &SoundPlayer::durationChanged, this, &SoundManager::onDurationChanged );

    return Status::OK;
}
