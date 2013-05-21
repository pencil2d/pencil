#include "polylinetool.h"


PolylineTool::PolylineTool(QObject *parent) :
    BaseTool(parent)
{
}

ToolType PolylineTool::type()
{
    return POLYLINE;
}

void PolylineTool::loadSettings()
{

}
