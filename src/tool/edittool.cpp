
#include "edittool.h"

EditTool::EditTool()
{

}

ToolType EditTool::type()
{
    return EDIT;
}

void EditTool::loadSettings()
{
    properties.width = -1;
    properties.feather = -1;
}
