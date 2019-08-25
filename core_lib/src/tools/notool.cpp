#include "notool.h"

NoTool::NoTool(QObject* parent) : BaseTool(parent)
{
}

void NoTool::loadSettings()
{
}

ToolType NoTool::type()
{
    return INVALID_TOOL;
}

void NoTool::pointerPressEvent(PointerEvent *)
{
}

void NoTool::pointerMoveEvent(PointerEvent *)
{
}


void NoTool::pointerReleaseEvent(PointerEvent *)
{
}
