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

#ifndef SOUNDMANAGER_H
#define SOUNDMANAGER_H

#include <cstdint>
#include "basemanager.h"

class Layer;
class SoundClip;
class SoundPlayer;


class SoundManager : public BaseManager
{
    Q_OBJECT
public:

    explicit SoundManager(Editor* editor);
    ~SoundManager() override;

    bool init() override;
    Status load(Object*) override;
    Status save(Object*) override;

    Status loadSound(Layer* soundLayer, int frameNumber, QString strSoundFile);
    Status loadSound(SoundClip* soundClip, QString strSoundFile);
    Status processSound(SoundClip* soundClip);

signals:
    void soundClipDurationChanged();

private:
    void onDurationChanged(SoundPlayer* player, int64_t duration);

    Status createMediaPlayer(SoundClip*);
};

#endif // SOUNDMANAGER_H
