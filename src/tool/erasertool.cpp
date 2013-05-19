#include "erasertool.h"

EraserTool::EraserTool(QObject *parent) :
    BaseTool(parent)
{
}

ToolType EraserTool::type()
{
    return ERASER;
}

void EraserTool::loadSettings()
{

}
