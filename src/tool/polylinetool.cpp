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
    properties.width = 1;
    properties.feather = -1;
}

QCursor PolylineTool::cursor()
{
    return Qt::CrossCursor;
}
