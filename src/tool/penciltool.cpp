#include <QSettings>
#include "penciltool.h"

PencilTool::PencilTool(QObject *parent) :
    BaseTool(parent)
{
}

ToolType PencilTool::type()
{

}

void PencilTool::loadSettings()
{
    QSettings settings("pencil", "pencil");

    properties.width = settings.value("pencilWidth").toDouble();
    properties.colour = Qt::black;
    properties.colourNumber = 0;
    properties.feather = -1;
    properties.opacity = 0.8;
    properties.pressure = 1;
    properties.invisibility = 1;
    properties.preserveAlpha = 0;

    if (properties.width == 0)
    {
        properties.width = 1;
        settings.setValue("pencilWidth", properties.width);
    }
}
