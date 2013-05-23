
#include <QPixmap>
#include "handtool.h"


HandTool::HandTool()
{

}

ToolType HandTool::type()
{
    return HAND;
}

void HandTool::loadSettings()
{

}

QCursor HandTool::cursor()
{
    return QPixmap(":icons/hand.png");
}