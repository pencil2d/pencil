#ifndef EDITORSTATE_H
#define EDITORSTATE_H

#include <QColor>
#include <QTransform>


class EditorState
{
public:
    EditorState();

    // editor
    int mCurrentFrame = 0;

    // color manager
    QColor mCurrentColor{ 0, 0, 0, 255 };

    // layer manager
    int mCurrentLayer = 0;
    
    // view manager
    QTransform mCurrentView;
    
    // playback manager
    int  mFps    = 12;
    bool mIsLoop = false;
    bool mIsRangedPlayback = false;
    int  mMarkInFrame  = 1;
    int  mMarkOutFrame = 10;

};

#endif // EDITORSTATE_H
