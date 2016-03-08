#ifndef SOUNDCLIP_H
#define SOUNDCLIP_H

#include <memory>
#include <QMediaPlayer>
#include "keyframe.h"

class SoundClip : public KeyFrame
{
public:
    SoundClip();
    ~SoundClip();

    Status init( QString strSoundFile );
    bool isValid();

    void attachPlayer( QMediaPlayer* mediaPlayer );
    void detachPlayer();
    QMediaPlayer* mediaPlayer() { return mMediaPlayer.get(); }

    void mediaStatusChanged( QMediaPlayer::MediaStatus eStatus );

private:
    std::shared_ptr< QMediaPlayer > mMediaPlayer;
    uint64_t m_soundSize = 0;
};

#endif // SOUNDCLIP_H
