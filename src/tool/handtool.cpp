
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
    properties.width = -1;
    properties.feather = -1;
}

QCursor HandTool::cursor()
{
    return QPixmap(":icons/hand.png");
}
