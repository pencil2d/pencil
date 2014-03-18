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

    int pos() { return m_position; }
    void setPos( int position ) { m_position = position; }

private:
    int m_position;
    int m_originalPosition;
    QString m_strFilename;
    bool m_isModified;
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
