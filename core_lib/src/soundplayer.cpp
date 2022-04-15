/*

Pencil2D - Traditional Animation Software
Copyright (C) 2012-2020 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/

#include "soundplayer.h"
#include <QMediaPlayer>
#include <QFile>
#include "soundclip.h"

SoundPlayer::SoundPlayer()
{
}

SoundPlayer::~SoundPlayer()
{
}

void SoundPlayer::init(SoundClip* clip)
{
    Q_ASSERT(clip != nullptr);
    mSoundClip = clip;

    mMediaPlayer = new QMediaPlayer(this);

    QFile file(clip->fileName());
    file.open(QIODevice::ReadOnly);

    mBuffer.setData(file.readAll());
    mBuffer.open(QBuffer::ReadOnly);

    mMediaPlayer->setMedia(QUrl::fromLocalFile(clip->fileName()), &mBuffer);
    makeConnections();

    clip->attachPlayer(this);
}

void SoundPlayer::onKeyFrameDestroy(KeyFrame* keyFrame)
{
    Q_UNUSED(keyFrame)
}

bool SoundPlayer::isValid()
{
    if (mMediaPlayer)
    {
        return (mMediaPlayer->error() == QMediaPlayer::NoError);
    }
    return false;
}

void SoundPlayer::play()
{
    if (mMediaPlayer)
    {
        mMediaPlayer->play();
    }
}

void SoundPlayer::pause()
{
    if (mMediaPlayer)
    {
        mMediaPlayer->pause();
    }
}

void SoundPlayer::stop()
{
    if (mMediaPlayer)
    {
        mMediaPlayer->stop();
    }
}

int64_t SoundPlayer::duration()
{
    if (mMediaPlayer)
    {
        return mMediaPlayer->duration();
    }
    return 0;
}

void SoundPlayer::setMediaPlayerPosition(qint64 pos)
{
    if (mMediaPlayer)
    {
        mMediaPlayer->setPosition(pos);
    }
}

void SoundPlayer::makeConnections()
{
    auto errorSignal = static_cast<void (QMediaPlayer::*)(QMediaPlayer::Error)>(&QMediaPlayer::error);
    connect(mMediaPlayer, errorSignal, this, [](QMediaPlayer::Error err)
    {
        qDebug() << "MediaPlayer Error: " << err;
    });

    connect(mMediaPlayer, &QMediaPlayer::durationChanged, [this](qint64 duration)
    {
        qDebug() << "MediaPlayer durationChanged :" << duration;
        emit durationChanged(this, duration);
    });
}
