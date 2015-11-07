#ifndef SOUNDCLIP_H
#define SOUNDCLIP_H

#include "keyframe.h"
#include <cstdint>


class SoundClip : public KeyFrame
{
public:
    SoundClip();
    Status init( QString strSoundFile );

private:
    QString       m_strFilePath;
    uint64_t      m_soundSize = 0;
};

#endif // SOUNDCLIP_H
