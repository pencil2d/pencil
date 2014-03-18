#include "KeyFrame.h"


KeyFrame::KeyFrame()
{
    m_position = -1;
    m_originalPosition = -1;
    m_isModified = false;
}

KeyFrame::~KeyFrame()
{

}

NullKeyFrame* NullKeyFrame::get()
{
    static NullKeyFrame* pTheOnlyOne = new NullKeyFrame;

    return pTheOnlyOne;
}
