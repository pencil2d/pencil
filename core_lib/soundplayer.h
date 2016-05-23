#ifndef SOUNDPLAYER_H
#define SOUNDPLAYER_H

#include <memory>
#include <QObject>
#include "pencilerror.h"
#include "keyframe.h"

class SoundClip;
class QMediaPlayer;

class SoundPlayer : public QObject, public KeyFrameEventListener
{
    Q_OBJECT
public:
    SoundPlayer();
    ~SoundPlayer();

    void init( SoundClip* );
    void onKeyFrameDestroy( KeyFrame* ) override;
    bool isValid();

    void play();
    void stop();

Q_SIGNALS:
    void corruptedSoundFile( SoundClip* );

private:
    void makeConnections();

    SoundClip* mSoundClip = nullptr;
    QMediaPlayer* mMediaPlayer = nullptr;
};

#endif // SOUNDPLAYER_H
