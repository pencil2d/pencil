#ifndef SOUNDMANAGER_H
#define SOUNDMANAGER_H


#include "basemanager.h"
#include <QMediaPlayer>
class SoundPlayer;
class Layer;

class SoundManager : public BaseManager
{
    Q_OBJECT
public:
    SoundManager( QObject* parent );
    ~SoundManager();
    bool init() override;

    Status loadSound( Layer* soundLayer, int frameNumber, QString strSoundFile );

    void errorHandler( QMediaPlayer::Error );

private:
    SoundPlayer* mSoundPlayer = nullptr;
};

#endif // SOUNDMANAGER_H
