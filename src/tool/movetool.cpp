
#include "movetool.h"

MoveTool::MoveTool()
{

}

ToolType MoveTool::type()
{
    return MOVE;
}

void MoveTool::loadSettings()
{
    properties.width = -1;
    properties.feather = -1;
}

QCursor MoveTool::cursor()
{
    return Qt::ArrowCursor;
}
