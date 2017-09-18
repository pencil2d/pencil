/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2012-2017 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/

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
    Status load( Object* ) override;
	Status save( Object* ) override;

    bool isPlaying();
    bool isLooping() { return mIsLooping; }
    void setCheckForSoundsHalfway( bool newCheck ) { mCheckForSoundsHalfway = newCheck; }

    void play();
    void stop();

    int fps() { return mFps; }
    int startFrame() { return mStartFrame; }
    int endFrame() { return mEndFrame; }
    
    bool isRangedPlaybackOn() { return mIsRangedPlayback; }
    int markInFrame() { return mMarkInFrame; }
    int markOutFrame() { return mMarkOutFrame; }
    
    void setFps( int fps );
    void setLooping( bool isLoop );
    void enableRangedPlayback( bool b );
    void setRangedStartFrame( int frame ) { mMarkInFrame = frame; }
    void setRangedEndFrame( int frame ) { mMarkOutFrame = frame; }
    void enableSound( bool b );

    void stopSounds();

Q_SIGNALS:
    void fpsChanged( int fps );
    void loopStateChanged( bool b );
    void rangedPlaybackStateChanged( bool b );
    void playStateChanged( bool isPlaying );

private:
    void timerTick();
    
    void playSounds( int frame );

    int mStartFrame = 1;
    int mEndFrame = 60;

    bool mIsLooping = false;
    bool mIsPlaySound = true;
    
    bool mIsRangedPlayback = false;
    int mMarkInFrame = 1;
    int mMarkOutFrame = 10;
    int mActiveSoundFrame = 0;

    int mFps = 12;

    QTimer* mTimer = nullptr;
    QElapsedTimer* mFrameTimer = nullptr;

    bool mCheckForSoundsHalfway = false;
    QList<int> mListOfActiveSoundFrames;
};

#endif // PLAYBACKMANAGER_H
