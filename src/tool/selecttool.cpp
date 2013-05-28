
#include "selecttool.h"

SelectTool::SelectTool()
{

}

ToolType SelectTool::type()
{
    return SELECT;
}

void SelectTool::loadSettings()
{

}

QCursor SelectTool::cursor()
{
    return Qt::CrossCursor;
}
