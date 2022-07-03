/*

Pencil2D - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2012-2020 Matthew Chiawen Chang

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
#include <QFileInfo>
#include "editor.h"
#include "object.h"
#include "layersound.h"
#include "soundclip.h"
#include "soundplayer.h"
#include "layermanager.h"

SoundManager::SoundManager(Editor* editor) : BaseManager(editor, __FUNCTION__)
{
}

SoundManager::~SoundManager()
{
}

bool SoundManager::init()
{
    return true;
}

Status SoundManager::load(Object* obj)
{
    int count = obj->getLayerCount();
    for (int i = 0; i < count; ++i)
    {
        Layer* layer = obj->getLayer(i);
        if (layer->type() != Layer::SOUND)
        {
            continue;
        }

        LayerSound* soundLayer = static_cast<LayerSound*>(layer);

        soundLayer->foreachKeyFrame([this](KeyFrame* key)
        {
            SoundClip* clip = dynamic_cast<SoundClip*>(key);
            Q_ASSERT(clip);

            createMediaPlayer(clip);
        });
    }
    return Status::OK;
}

Status SoundManager::save(Object*)
{
    return Status::OK;
}

Status SoundManager::loadSound(Layer* soundLayer, int frameNumber, QString soundFilePath)
{
    Q_ASSERT(soundLayer);
    if (soundLayer->type() != Layer::SOUND)
    {
        return Status::ERROR_INVALID_LAYER_TYPE;
    }

    if (frameNumber < 0)
    {
        return Status::ERROR_INVALID_FRAME_NUMBER;
    }

    if (!QFile::exists(soundFilePath))
    {
        return Status::FILE_NOT_FOUND;
    }

    KeyFrame* key = soundLayer->getKeyFrameAt(frameNumber);
    if (key == nullptr)
    {
        key = new SoundClip;
        soundLayer->addKeyFrame(frameNumber, key);
    }

    if (!key->fileName().isEmpty())
    {
        // file path should be empty.
        // we can only load a audio clip to an empty key!
        return Status::FAIL;
    }

    QString strCopyFile = soundLayer->object()->copyFileToDataFolder(soundFilePath);
    Q_ASSERT(!strCopyFile.isEmpty());

    QString sOriginalName = QFileInfo(soundFilePath).fileName();

    SoundClip* soundClip = dynamic_cast<SoundClip*>(key);
    soundClip->init(strCopyFile);
    soundClip->setSoundClipName(sOriginalName);

    Status st = createMediaPlayer(soundClip);
    if (!st.ok())
    {
        delete soundClip;
        return st;
    }

    return Status::OK;
}

Status SoundManager::loadSound(SoundClip* soundClip, QString strSoundFile)
{
    Q_ASSERT(soundClip);

    if (!QFile::exists(strSoundFile))
    {
        return Status::FILE_NOT_FOUND;
    }

    if (strSoundFile.isEmpty())
    {
        return Status::FAIL;
    }

    QString strCopyFile = editor()->object()->copyFileToDataFolder(strSoundFile);
    Q_ASSERT(!strCopyFile.isEmpty());

    soundClip->init(strCopyFile);
    if (soundClip->soundClipName().isEmpty())
    {
        soundClip->setSoundClipName(QFileInfo(strSoundFile).fileName());
    }

    Status st = createMediaPlayer(soundClip);
    if (!st.ok())
    {
        delete soundClip;
        return st;
    }

    editor()->layers()->notifyAnimationLengthChanged();

    return Status::OK;
}

Status SoundManager::processSound(SoundClip* soundClip)
{
    Q_ASSERT(soundClip);

    if (!QFile::exists(soundClip->fileName()))
    {
        return Status::FILE_NOT_FOUND;
    }
    soundClip->init(soundClip->fileName());

    Status st = createMediaPlayer(soundClip);
    if (!st.ok())
    {
        return st;
    }
    return Status::OK;
}

int SoundManager::soundClipCount() const
{
    LayerManager *layerManager = editor()->layers();
    int totalCount = 0;


    for (int i = 0; i < layerManager->count(); ++i)
    {
        Layer* layer = layerManager->getLayer(i);
        if (layer->type() != Layer::SOUND)
        {
            continue;
        }

        totalCount += layer->keyFrameCount();
    }
    return totalCount;
}

void SoundManager::onDurationChanged(SoundPlayer* player, int64_t duration)
{
    SoundClip* clip = player->clip();

    double fps = static_cast<double>(editor()->fps());

    double frameLength = duration * fps / 1000.0;
    clip->setLength(static_cast<int>(frameLength));
    clip->setDuration(duration);

    editor()->layers()->notifyAnimationLengthChanged();

    emit soundClipDurationChanged();
}

Status SoundManager::createMediaPlayer(SoundClip* clip)
{
    SoundPlayer* newPlayer = new SoundPlayer();
    newPlayer->init(clip);

    connect(newPlayer, &SoundPlayer::durationChanged, this, &SoundManager::onDurationChanged);

    return Status::OK;
}
