
#include <QSettings>
#include "brushtool.h"

BrushTool::BrushTool(QObject *parent) :
    BaseTool(parent)
{
}

ToolType BrushTool::type()
{
    return BRUSH;
}

void BrushTool::loadSettings()
{
    QSettings settings("pencil", "pencil");

    properties.width = settings.value("brushWidth").toDouble();
    if (properties.width == 0)
    {
        properties.width = 48;
        settings.setValue("brushWidth", properties.width);
    }
    properties.colour = Qt::blue;
    properties.colourNumber = 1;
    properties.feather = settings.value("brushFeather").toDouble();
    if (properties.feather == 0)
    {
        properties.feather = 70;
        settings.setValue("brushFeather", properties.feather);
    }
    properties.opacity = 0.5;
    properties.pressure = ON;
    properties.invisibility = DISABLED;
    properties.preserveAlpha = OFF;
}
