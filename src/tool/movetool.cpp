
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

}

QCursor MoveTool::cursor()
{
    return Qt::ArrowCursor;
}
