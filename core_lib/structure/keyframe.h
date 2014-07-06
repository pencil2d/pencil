#ifndef KeyFrame_H
#define KeyFrame_H

#include <QString>

enum class KeyFrameType
{
    NULLTYPE,
    BITMAP_TYPE,
    VECTOR_TYPE,
    CAMERA_TYPE,
    SOUND_TYPE,
};

class KeyFrame
{
public:
    KeyFrame();
    virtual ~KeyFrame();

    int pos() { return mFrame; }
    void setPos( int position ) { mFrame = position; }
    void setModified( bool b ) { mIsModified = b; }

private:
    int mFrame = -1;
	int mLength = 1;
    QString mstrFilename;
    bool mIsModified = false;
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
