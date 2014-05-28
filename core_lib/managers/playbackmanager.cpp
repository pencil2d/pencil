
#include <QTimer>
#include "editor.h"
#include "layermanager.h"
#include "playbackmanager.h"

PlaybackManager::PlaybackManager(QObject* parent) :
    BaseManager(parent)
{
}

bool PlaybackManager::init()
{
    m_pTimer = new QTimer( this );
    connect( m_pTimer, &QTimer::timeout, [ = ] { timerTick(); } );
    return true;
}

void PlaybackManager::play()
{
    //updateMaxFrame();

    int projectLength = editor()->layers()->projectLength();

    m_startFrame = ( m_isRangedPlayback ) ? m_markInFrame : 1;
    m_endFrame = ( m_isRangedPlayback ) ? m_markOutFrame : projectLength;

    int currentFrame = editor()->layers()->currentFramePosition();
    if ( currentFrame >= m_endFrame )
    {
        editor()->layers()->setCurrentKeyFrame( m_startFrame );
    }

    m_pTimer->setInterval( 1000.0f / m_fps );
    m_pTimer->start();
}

void PlaybackManager::stop()
{
    m_pTimer->stop();
}

void PlaybackManager::setFps( int fps )
{
    if ( m_fps != fps )
    {
        m_fps = fps;
        emit fpsChanged( m_fps );
    }
}

void PlaybackManager::timerTick()
{
    int currentFrame = editor()->layers()->currentFramePosition();
    if ( currentFrame > m_endFrame )
    {
        if ( m_isLooping )
        {
            editor()->layers()->setCurrentKeyFrame( m_startFrame );
        }
        else
        {
            stop();
        }
        return;
    }

    editor()->layers()->setCurrentKeyFrame( currentFrame + 1 );

    // TODO: play sound if any
}

void PlaybackManager::setLoop( bool isLoop )
{
    if ( m_isLooping != isLoop )
    {
        m_isLooping = isLoop;
        emit loopStateChanged( m_isLooping );
    }
}

void PlaybackManager::enableRangedPlayback( bool b )
{
    if ( m_isRangedPlayback != b )
    {
        m_isRangedPlayback = b;
        emit rangedPlaybackStateChanged( m_isRangedPlayback );
    }
}


