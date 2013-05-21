#include "eyedroppertool.h"

EyedropperTool::EyedropperTool(QObject *parent) :
    BaseTool(parent)
{
}

ToolType EyedropperTool::type()
{
    return EYEDROPPER;
}

void EyedropperTool::loadSettings()
{

}
