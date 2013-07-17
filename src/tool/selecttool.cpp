
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
    properties.width = -1;
    properties.feather = -1;
}

QCursor SelectTool::cursor()
{
    return Qt::CrossCursor;
}
