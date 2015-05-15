#include "commandcenter.h"
#include "editor.h"
#include "viewmanager.h"


CommandCenter::CommandCenter(QObject *parent) : QObject(parent)
{
}

CommandCenter::~CommandCenter()
{
}

void CommandCenter::ZoomIn()
{
    Q_ASSERT( mEditor );
    float newScaleValue = mEditor->view()->scaling() * 1.2;
    mEditor->view()->scale( newScaleValue );
}

void CommandCenter::ZoomOut()
{
    Q_ASSERT( mEditor );
    float newScaleValue = mEditor->view()->scaling() * 0.8;
    mEditor->view()->scale( newScaleValue );
}

void CommandCenter::GotoNextFrame()
{

}

void CommandCenter::GotoPrevFrame()
{

}

void CommandCenter::GotoNextKeyFrame()
{

}

void CommandCenter::GotoPrevKeyFrame()
{

}


