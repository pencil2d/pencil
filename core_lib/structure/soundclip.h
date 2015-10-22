#ifndef SOUNDCLIP_H
#define SOUNDCLIP_H

#include "keyframe.h"
#include <cstdint>

class QMediaPlayer;

class SoundClip : public KeyFrame
{
public:
    SoundClip();

    QString       m_strFilePath;
    QMediaPlayer* m_pPlayer = nullptr;
    uint64_t      m_soundSize = 0;
};

#endif // SOUNDCLIP_H
