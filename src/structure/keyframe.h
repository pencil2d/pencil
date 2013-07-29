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
        position = frame.position;
        originalPosition = frame.originalPosition;
        filename = frame.filename;
        modified = frame.modified;
    }

public:
    // keep public for now
    int position;
    int originalPosition;
    QString filename;
    bool modified;
};

#endif // KEYFRAME_H
