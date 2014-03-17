#ifndef KEYFRAME_H
#define KEYFRAME_H

#include <QString>

class Keyframe
{
public:
    Keyframe()
    {
    }

    Keyframe(const Keyframe &frame)
    {
        m_position = frame.m_position;
        m_originalPosition = frame.m_originalPosition;
        m_strFilename = frame.m_strFilename;
        m_isModified = frame.m_isModified;
    }

private:
    int m_position = -1;
    int m_originalPosition = -1;
    QString m_strFilename;
    bool m_isModified = false;
};

#endif // KEYFRAME_H
