#ifndef SOUNDMANAGER_H
#define SOUNDMANAGER_H


#include "basemanager.h"
#include <memory>
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

    //void errorHandler( QMediaPlayer::Error );

private:
    std::unique_ptr< SoundPlayer > mSoundPlayer;
};

#endif // SOUNDMANAGER_H
