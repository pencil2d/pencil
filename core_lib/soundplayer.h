#ifndef SOUNDPLAYER_H
#define SOUNDPLAYER_H

#include <memory>
#include <QObject>
#include <QMediaPlayer>
#include "pencilerror.h"
class SoundClip;


class SoundPlayer : public QObject
{
    Q_OBJECT
public:
    SoundPlayer( QObject* parent );
    ~SoundPlayer();

    Status addSound( SoundClip* );

Q_SIGNALS:
    void corruptedSoundFile( SoundClip* );

private:
    std::vector< SoundClip* > mSoundClips;
    
};

#endif // SOUNDPLAYER_H
