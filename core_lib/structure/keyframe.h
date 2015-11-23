#ifndef KeyFrame_H
#define KeyFrame_H

#include <cstdint>
#include <vector>
#include <memory>
#include <QString>
#include "pencilerror.h"
class KeyFrameEventListener;


class KeyFrame
{
public:
    KeyFrame();
    virtual ~KeyFrame();

    int  pos() { return mFrame; }
    void setPos( int position ) { mFrame = position; }
    int  length() { return mLength; }
    void setModified( bool b ) { mIsModified = b; }

    QString fileName() { return mAttachedFileName; }
    void    setFileName( QString strFileName ) { mAttachedFileName = strFileName; }

    void addEventListener( KeyFrameEventListener* );
    void removeEventListner( KeyFrameEventListener* );

private:
    int mFrame       = -1;
    int mLength      =  1;
    bool mIsModified = false;
    QString mAttachedFileName;

    std::vector< KeyFrameEventListener* > mEventListeners;
};

typedef std::shared_ptr< KeyFrame > KeyFramePtr;



class KeyFrameEventListener
{
public:
    virtual void onKeyFrameDestroy( KeyFrame* ) = 0;
};



class NullKeyFrame : public KeyFrame
{
public:
    static NullKeyFrame* get();
private:
    NullKeyFrame() {}
    NullKeyFrame( const NullKeyFrame& ) {}
    void operator=( const NullKeyFrame& ) {}
};

#endif // KeyFrame_H
