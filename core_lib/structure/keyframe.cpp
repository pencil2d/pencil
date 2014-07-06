#include "keyframe.h"


KeyFrame::KeyFrame()
{
}

KeyFrame::~KeyFrame()
{

}

NullKeyFrame* NullKeyFrame::get()
{
    static NullKeyFrame* pTheOnlyOne = new NullKeyFrame;

    return pTheOnlyOne;
}
