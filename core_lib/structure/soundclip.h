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
    void playFromPosition(int frameNumber, int fps);
    void stop();

private:
    std::shared_ptr< SoundPlayer > mPlayer;
};

#endif // SOUNDCLIP_H
