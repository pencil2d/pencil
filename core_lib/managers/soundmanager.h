#ifndef SOUNDMANAGER_H
#define SOUNDMANAGER_H


#include "basemanager.h"

class Layer;
class SoundClip;
class SoundPlayer;


class SoundManager : public BaseManager
{
    Q_OBJECT
public:

    SoundManager( QObject* parent );
    ~SoundManager();
    
    bool init() override;
    Status onObjectLoaded( Object* ) override;

    Status loadSound( Layer* soundLayer, int frameNumber, QString strSoundFile );
    Status loadSound( SoundClip* soundClip, QString strSoundFile );

signals:
    void soundClipDurationChanged();

private:
    void onDurationChanged( SoundPlayer* player, int64_t duration );

    Status createMeidaPlayer( SoundClip* );
};

#endif // SOUNDMANAGER_H
