#ifndef PLAYBACKMANAGER_H
#define PLAYBACKMANAGER_H

#include "basemanager.h"

class QTimer;


class PlaybackManager : public BaseManager
{
    Q_OBJECT
public:
    explicit PlaybackManager(QObject* parent);

    bool init() override;

    bool isPlaying() { return m_isPlaying; }
    bool isLooping() { return m_isLooping; }

    void play();
    void stop();

    int  fps() { return m_fps; }
    int startFrame() { return mStartFrame; }
    int endFrame() { return mEndFrame; }

    void setFps( int fps );
    void setLooping( bool isLoop );
    void enableRangedPlayback( bool b );
    void setRangedStartFrame( int frame ) { m_markInFrame = frame; }
    void setRangedEndFrame( int frame ) { m_markOutFrame = frame; }
    void enbaleSound( bool b ) { m_isPlaySound = b; }

Q_SIGNALS:
    void fpsChanged( int fps );
    void loopStateChanged( bool b );
    void rangedPlaybackStateChanged( bool b );

private:
    void timerTick();

    bool m_isLooping = false;
    bool m_isPlaying = false;
    bool m_isPlaySound = false;
    int mStartFrame = 1;
    int mEndFrame = 60;

    bool m_isRangedPlayback = false;
    int m_markInFrame = 1;
    int m_markOutFrame = 10;

    int m_fps = 12;

    QTimer* m_pTimer;
};

#endif // PLAYBACKMANAGER_H
