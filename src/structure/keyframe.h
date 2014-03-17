#ifndef KEYFRAME_H
#define KEYFRAME_H

#include <QString>

class Keyframe
{
public:
    Keyframe();
    virtual ~Keyframe();

private:
    int m_position;
    int m_originalPosition;
    QString m_strFilename;
    bool m_isModified;
};

#endif // KEYFRAME_H
