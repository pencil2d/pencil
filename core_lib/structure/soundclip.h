#ifndef SOUNDCLIP_H
#define SOUNDCLIP_H

#include <memory>
#include "keyframe.h"

class SoundPlayer;


class SoundClip : public KeyFrame
{
public:
    SoundClip();
    ~SoundClip();

    Status init( const QString& strSoundFile );
    bool isValid();

    void attachPlayer( SoundPlayer* player );
    void detachPlayer();
    SoundPlayer* player() { return mPlayer.get(); }

    void play();
    void stop();

private:
    std::shared_ptr< SoundPlayer > mPlayer;
    uint64_t m_soundSize = 0;
};

#endif // SOUNDCLIP_H
