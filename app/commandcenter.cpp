#include "commandcenter.h"
#include "editor.h"
#include "viewmanager.h"
#include "layermanager.h"
#include "util.h"

#include "layerbitmap.h"
#include "layervector.h"
#include "bitmapimage.h"
#include "vectorimage.h"


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

void CommandCenter::flipX()
{
    auto view = mEditor->view();

    bool b = view->isFlipHorizontal();
    view->flipHorizontal( !b );
}

void CommandCenter::flipY()
{
    auto view = mEditor->view();

    bool b = view->isFlipVertical();
    view->flipVertical( !b );
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


