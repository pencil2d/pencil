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

QCursor PolylineTool::cursor()
{
    return Qt::CrossCursor;
}
