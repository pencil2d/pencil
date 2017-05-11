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

    int64_t duration() const;
    void setDuration(const int64_t &duration);

    void updateLength(int fps);

private:
    std::shared_ptr< SoundPlayer > mPlayer;

    // Duration in seconds. This is stored to update the length of the
    // frame when the FPS changes.
    int64_t mDuration;
};

#endif // SOUNDCLIP_H
