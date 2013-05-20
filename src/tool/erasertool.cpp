
#include <QSettings>
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
    QSettings settings("pencil", "pencil");
    properties.width = settings.value("eraserWidth").toDouble();
    if (properties.width == 0)
    {
        properties.width = 24;
        settings.setValue("eraserWidth", properties.width);
    }
    properties.feather = 0;
    properties.opacity = 0.5;
    properties.pressure = 1;
    properties.invisibility = -1;
    properties.preserveAlpha = 0;
}
