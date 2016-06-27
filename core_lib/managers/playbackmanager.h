#ifndef PLAYBACKMANAGER_H
#define PLAYBACKMANAGER_H

#include "basemanager.h"

class QTimer;


class PlaybackManager : public BaseManager
{
    Q_OBJECT
public:
    explicit PlaybackManager( QObject* parent );

    bool init() override;
    Status onObjectLoaded( Object* ) override;

    bool isPlaying();
    bool isLooping() { return mIsLooping; }

    void play();
    void stop();

    int fps() { return mFps; }
    int startFrame() { return mStartFrame; }
    int endFrame() { return mEndFrame; }
    
    int markInFrame() { return mMarkInFrame; }
    int markOutFrame() { return mMarkOutFrame; }
    
    void setFps( int fps );
    void setLooping( bool isLoop );
    void enableRangedPlayback( bool b );
    void setRangedStartFrame( int frame ) { mMarkInFrame = frame; }
    void setRangedEndFrame( int frame ) { mMarkOutFrame = frame; }
    void enbaleSound( bool b ) { mIsPlaySound = b; }

Q_SIGNALS:
    void fpsChanged( int fps );
    void loopStateChanged( bool b );
    void rangedPlaybackStateChanged( bool b );

private:
    void timerTick();
    
    void playSounds( int frame );
    void stopSounds();

    int mStartFrame = 1;
    int mEndFrame = 60;

    bool mIsLooping = false;
    bool mIsPlaySound = false;
    
    bool mIsRangedPlayback = false;
    int mMarkInFrame = 1;
    int mMarkOutFrame = 10;

    int mFps = 12;

    QTimer* mTimer = nullptr;
};

#endif // PLAYBACKMANAGER_H
