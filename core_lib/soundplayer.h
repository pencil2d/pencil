/*

Pencil - Traditional Animation Software
Copyright (C) 2012-2018 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/

#ifndef SOUNDPLAYER_H
#define SOUNDPLAYER_H

#include <memory>
#include <QObject>
#include "pencilerror.h"
#include "keyframe.h"

class SoundClip;
class QMediaPlayer;

class SoundPlayer : public QObject, public KeyFrameEventListener
{
    Q_OBJECT
public:
    SoundPlayer();
    ~SoundPlayer();

    void init( SoundClip* );
    void onKeyFrameDestroy( KeyFrame* ) override;
    bool isValid();

    void play();
    void stop();
    
    int64_t duration();
    SoundClip* clip() { return mSoundClip; }

    void setMediaPlayerPosition( qint64 pos );

Q_SIGNALS:
    void corruptedSoundFile( SoundClip* );
    void durationChanged( SoundPlayer*, int64_t duration );

private:
    void makeConnections();

    SoundClip* mSoundClip = nullptr;
    QMediaPlayer* mMediaPlayer = nullptr;
};

#endif // SOUNDPLAYER_H
