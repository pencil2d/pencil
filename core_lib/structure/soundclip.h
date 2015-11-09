#ifndef SOUNDCLIP_H
#define SOUNDCLIP_H

#include <memory>
#include "keyframe.h"
class QMediaPlayer;


class SoundClip : public KeyFrame
{
public:
    SoundClip();
    ~SoundClip();

    Status init( QString strSoundFile );

    void attachPlayer( std::shared_ptr< QMediaPlayer > spPlayer ) { mMediaPlayer = spPlayer; }
    void detachPlayer() { mMediaPlayer.reset(); }
    QMediaPlayer* mediaPlayer() { return mMediaPlayer.get(); }

private:
    std::shared_ptr< QMediaPlayer > mMediaPlayer;
    uint64_t      m_soundSize = 0;
};

#endif // SOUNDCLIP_H
