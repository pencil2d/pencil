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

#ifndef SOUNDCLIP_H
#define SOUNDCLIP_H

#include <memory>
#include "keyframe.h"

class SoundPlayer;


class SoundClip : public KeyFrame
{
public:
    explicit SoundClip();
    explicit SoundClip(const SoundClip&);
    ~SoundClip();

    SoundClip* clone() override;

    Status init( const QString& strSoundFile );
    bool isValid();

    void setSoundClipName( const QString& sName ) { mOriginalSoundClipName = sName; }
    QString soundClipName() { return mOriginalSoundClipName; }

    void attachPlayer( SoundPlayer* player );
    void detachPlayer();
    SoundPlayer* player() { return mPlayer.get(); }

    void play();
    void playFromPosition(int frameNumber, int fps);
    void stop();

    int64_t duration() const;
    void setDuration(const int64_t &duration);

    void updateLength(int fps);

private:
    std::shared_ptr< SoundPlayer > mPlayer;

    QString mOriginalSoundClipName;

    // Duration in seconds. This is stored to update the length of the
    // frame when the FPS changes.
    int64_t mDuration = 0;
};

#endif // SOUNDCLIP_H
