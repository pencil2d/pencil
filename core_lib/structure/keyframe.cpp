#include "keyframe.h"


KeyFrame::KeyFrame()
{
}

KeyFrame::~KeyFrame()
{
    for ( KeyFrameEventListener* listener : mEventListeners )
    {
        listener->onKeyFrameDestroy( this );
    }
}

void KeyFrame::addEventListener( KeyFrameEventListener* listener )
{
    auto it = std::find( mEventListeners.begin(), mEventListeners.end(), listener );
    if ( it != mEventListeners.end() )
    {
        mEventListeners.push_back( listener );
    }
}

void KeyFrame::removeEventListner( KeyFrameEventListener* listener )
{
    auto it = std::find( mEventListeners.begin(), mEventListeners.end(), listener );
    if ( it != mEventListeners.end() )
    {
        mEventListeners.erase( it );
    }
}

NullKeyFrame* NullKeyFrame::get()
{
    static NullKeyFrame* pTheOnlyOne = new NullKeyFrame;

    return pTheOnlyOne;
}
